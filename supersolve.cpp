//треба додати попередню обробку, яка виявляє домінуючі пари, ці пари встановлюються перед початком рекурсивного розвязання
//розвязувач
#include "solvedominosa.h"
#include <iostream>
#include <algorithm>
#include <limits>
#include <set>
#include <queue>
using namespace std;

pair<int, int> minmax(int a, int b) { //створення пари з мінімального і максимального значення для дпопомоги
    return {min(a, b), max(a, b)};
}
dominosa_solve::dominosa_solve(const board& b) : d_board(b) {//розвязувач з дошкою якак передається
    num_max = 0; //тут ми знаходимо максимальне число на нашій дошці для визначення набору доміно
    for (int i = 0; i < d_board.get_r(); ++i) {
        for (int j = 0; j < d_board.get_c(); ++j) {
            if (d_board.get_val(i, j) > num_max) {
                num_max = d_board.get_val(i, j);
            }
        }
    }
    is_cover.assign(d_board.get_r(), vector<bool>(d_board.get_c(), false));
    init_dominosa();
}
void dominosa_solve::init_dominosa() {//ініціалізація набору всіх можливих доміно
    map<pair<int, int>, int> domino_counts;
    int rows = d_board.get_r();
    int cols = d_board.get_c();
    
    if ((rows * cols) % 2 != 0) {//валідність розміру дошки
        cerr << "Invalid board size: odd number of cells." << endl;
        exit(1);
    }
    for (int i = 0; i < rows; ++i) {//підрахунок гориз.доміно
        for (int j = 0; j < cols - 1; ++j) {
            pair<int, int> domino = minmax(d_board.get_val(i, j), d_board.get_val(i, j + 1));
            domino_counts[domino]++;
        }
    }
    for (int i = 0; i < rows - 1; ++i) {//підрахунок вертик. доміно
        for (int j = 0; j < cols; ++j) {
            pair<int, int> domino = minmax(d_board.get_val(i, j), d_board.get_val(i + 1, j));
            domino_counts[domino]++;
        }
    }
    domino_available.clear();
    for (const auto& pair : domino_counts) {
        domino_available[pair.first] = pair.second; //встановимо реальну кількість
    }
}
bool dominosa_solve::placeall_dominoes(const pair<int, int>& domino_type) {//знаходимо всі можливі позиції для розміщення доміно
    int value1 = domino_type.first;
    int value2 = domino_type.second;
    int rows = d_board.get_r();
    int cols = d_board.get_c();
    vector<pair<pair<int, int>, pair<int, int>>> pos_place;

    for (int i = 0; i < rows; ++i) {//перевіряємо горизонтальні доміно
        for (int j = 0; j < cols - 1; ++j) {
            if (!is_cover[i][j] && !is_cover[i][j+1]) {
                int v1 = d_board.get_val(i, j);
                int v2 = d_board.get_val(i, j+1);
                if ((v1 == value1 && v2 == value2) || (v1 == value2 && v2 == value1)) {
                    pos_place.push_back({{i, j}, {i, j+1}});
                }
            }
        }
    }
    for (int i = 0; i < rows - 1; ++i) {// перевіряємо вертикальні доміно
        for (int j = 0; j < cols; ++j) {
            if (!is_cover[i][j] && !is_cover[i+1][j]) {
                int v1 = d_board.get_val(i, j);
                int v2 = d_board.get_val(i+1, j);
                if ((v1 == value1 && v2 == value2) || (v1 == value2 && v2 == value1)) {
                    pos_place.push_back({{i, j}, {i+1, j}});
                }
            }
        }
    }
    if (pos_place.empty() && domino_available[domino_type] > 0) {//немає можливих розміщень і доміно все ще потрібно-розв'язок неможливий
        return false;
    }
    for (const auto& placement : pos_place) { // pозміщуємо доміно
        is_cover[placement.first.first][placement.first.second] = true;
        is_cover[placement.second.first][placement.second.second] = true;
        find_solution.push_back({
            {placement.first.first, placement.first.second},
            {placement.second.first, placement.second.second}
        });
        domino_available[domino_type]--;
        if (domino_available[domino_type] == 0) {
            break;
        }
    }
    return true;
}
bool dominosa_solve::preprocess_dominoes() {
    int rows = d_board.get_r();
    int cols = d_board.get_c();
    bool made_progress = true;
    while (made_progress) {//поки можливо встановити очевидні доміно
        made_progress = false;
        for (int i = 0; i < rows; ++i) {//для кожної непокритої клітинки
            for (int j = 0; j < cols; ++j) {
                if (is_cover[i][j]) continue;
                int valid_move = 0;
                pair<int, int> valid_posR = {-1, -1}; //для розміщення вправо
                pair<int, int> valid_posD = {-1, -1}; //для розміщення вниз
                pair<int, int> valid_posL = {-1, -1}; //для розміщення вліво
                pair<int, int> valid_posU = {-1, -1}; //для розміщення вверх
                int valid_dir = 0; // 1-вправо, 2-вниз, 3-вліво, 4-вверх
                // Перевіряємо чотири напрямки
                if (j < cols - 1 && !is_cover[i][j+1]) {//вправо
                    pair<int, int> domino = minmax(d_board.get_val(i, j), d_board.get_val(i, j+1));
                    if (domino_available.count(domino) && domino_available[domino] > 0) {
                        valid_move++;
                        valid_posR = {i, j+1};
                        valid_dir = 1;
                    }
                }
                if (i < rows - 1 && !is_cover[i+1][j]) { //вниз
                    pair<int, int> domino = minmax(d_board.get_val(i, j), d_board.get_val(i+1, j));
                    if (domino_available.count(domino) && domino_available[domino] > 0) {
                        valid_move++;
                        valid_posD = {i+1, j};
                        if (valid_dir == 0) valid_dir = 2;
                    }
                }
                if (j > 0 && !is_cover[i][j-1]) { //вліво
                    pair<int, int> domino = minmax(d_board.get_val(i, j), d_board.get_val(i, j-1));
                    if (domino_available.count(domino) && domino_available[domino] > 0) {
                        valid_move++;
                        valid_posL = {i, j-1};
                        if (valid_dir == 0) valid_dir = 3;
                    }
                }
                if (i > 0 && !is_cover[i-1][j]) {//вверх
                    pair<int, int> domino = minmax(d_board.get_val(i, j), d_board.get_val(i-1, j));
                    if (domino_available.count(domino) && domino_available[domino] > 0) {
                        valid_move++;
                        valid_posU = {i-1, j};
                        if (valid_dir == 0) valid_dir = 4;
                    }
                }
                if (valid_moves == 1) {
                    made_progress = true;
                    
                    int value_1 = d_board.get_val(i, j);
                    int value_2;
                    pair<int, int> next_pos;
                    
                    if (valid_dir == 1) { //вправо
                        next_pos = valid_posR;
                        value_2 = d_board.get_val(next_pos.first, next_pos.second);
                        is_cover[i][j] = true;
                        is_cover[next_pos.first][next_pos.second] = true;
                        find_solution.push_back({{i, j}, {next_pos.first, next_pos.second}});
                    }
                    else if (valid_dir == 2) { //вниз
                        next_pos = valid_posD;
                        value_2 = d_board.get_val(next_pos.first, next_pos.second);
                        is_cover[i][j] = true;
                        is_cover[next_pos.first][next_pos.second] = true;
                        find_solution.push_back({{i, j}, {next_pos.first, next_pos.second}});
                    }
                    else if (valid_dir == 3) { //вліво
                        next_pos = valid_posL;
                        value_2 = d_board.get_val(next_pos.first, next_pos.second);
                        is_cover[i][j] = true;
                        is_cover[next_pos.first][next_pos.second] = true;
                        find_solution.push_back({{i, j}, {next_pos.first, next_pos.second}});
                    }
                    else if (valid_dir == 4) { //вверх
                        next_pos = valid_posU;
                        value_2 = d_board.get_val(next_pos.first, next_pos.second);
                        is_cover[i][j] = true;
                        is_cover[next_pos.first][next_pos.second] = true;
                        find_solution.push_back({{i, j}, {next_pos.first, next_pos.second}});
                    }
                    pair<int, int> domino = minmax(value_1, value_2);//кіль-сть достуаних доміно ми зменшуємо
                    domino_available[domino]--;
                    if (domino_available[domino] == 0) {//якщо домінуюча пара більше недоступна, спроба розмістити всі інші пари цього типу
                        if (!placeall_dominoes(domino)) {
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true; //обробка пройшла успішно
}
pair<int, int> dominosa_solve::grab_next_cell() {
    int best_row = -1, best_col = -1;
    int minimum_option = numeric_limits<int>::max();

    for (int i = 0; i < d_board.get_r(); ++i) {//пріоритет клітинкам з меншими можливостями
        for (int j = 0; j < d_board.get_c(); ++j) {
            if (!is_cover[i][j]) {
                int option = 0;
                int value_1 = d_board.get_val(i, j);
                if (j + 1 < d_board.get_c() && !is_cover[i][j + 1]) { // я перевіряю всі можливі варіанти розміщення
                    pair<int, int> domino = minmax(value_1, d_board.get_val(i, j + 1));
                    if (domino_available.count(domino) && domino_available[domino] > 0) {
                        option++;
                    }
                }
                
                if (i + 1 < d_board.get_r() && !is_cover[i + 1][j]) {
                    pair<int, int> domino = minmax(value_1, d_board.get_val(i + 1, j));
                    if (domino_available.count(domino) && domino_available[domino] > 0) {
                        option++;
                    }
                }
                if (j > 0 && !is_cover[i][j - 1]) { //перевірка варіантів зліва та зверху
                    pair<int, int> domino = minmax(value_1, d_board.get_val(i, j - 1));
                    if (domino_available.count(domino) && domino_available[domino] > 0) {
                        option++;
                    }
                }
                if (i > 0 && !is_cover[i - 1][j]) {
                    pair<int, int> domino = minmax(value_1, d_board.get_val(i - 1, j));
                    if (domino_available.count(domino) && domino_available[domino] > 0) {
                        option++;
                    }
                }
                if (option == 0) { //у клітинки нема варіантів, але вона ще не покрита
                    bool is1_uncovered = true;//чи є ця клітинка єдиною непокритою?
                    for (int r = 0; r < d_board.get_r(); ++r) {
                        for (int c = 0; c < d_board.get_c(); ++c) {
                            if (!is_cover[r][c] && (r != i || c != j)) {
                                is_only_uncovered = false;
                                break;
                            }
                        }
                        if (!is1_uncovered) break;
                    }
                    if (is1_uncovered) {
                        return {-2, -2};//ми не можемо знайти розв'язок якщо це єдина непокрита клітинка
                    }
                    return {i, j};//в іншому випадку вчеоно обираємо цю коітинку але з особилвим підходом
                }
                if (option < minimum_option) {
                    minimum_option = option;
                    best_row = i;
                    best_col = j;
                }
            }
        }
    }
    if (best_row == -1) { //всі клітинки покриті
        return {-1, -1};
    }
    return {best_row, best_col};
}

bool dominosa_solve::super_solve_recurse() {
    pair<int, int> next_cell = grab_next_cell();
    int i = next_cell.first;
    int j = next_cell.second;
    if (i == -1) {//розвязок знайдено
        cout << "All cells covered. Solution complete." << endl;
        return true;
    }
    if (i == -2) {//розвязорк не знайдено
        cout << "Solution impossible - isolated uncovered cell." << endl;
        return false;
    }
    
    int value_1 = d_board.get_val(i, j);
    struct DominoDir {//перевіряємо всі чотири напрямки і сортуємо за пріоритетом
        int direction; // 1-вправо, 2-вниз, 3-вліво, 4-вверх
        int row2, col2; // коорд. другої клітинки доміно
        pair<int, int> domino_type; // тип доміно ака пара значень
    };
    vector<DominoDirection> possible_dir;
    
    if (j + 1 < d_board.get_c() && !is_cover[i][j + 1]) {//це вправо
        int value_2 = d_board.get_val(i, j + 1);
        pair<int, int> domino = minmax(value_1, value_2);
        if (domino_available[domino] > 0) {
            possible_dir.push_back({1, i, j+1, domino});
        }
    }
    if (i + 1 < d_board.get_r() && !is_cover[i + 1][j]) {//це вниз
        int value_2 = d_board.get_val(i + 1, j);
        pair<int, int> domino = minmax(value_1, value_2);
        if (domino_available[domino] > 0) {
            possible_dir.push_back({2, i+1, j, domino});
        }
    }
    if (j > 0 && !is_cover[i][j - 1]) {//це вліво
        int value_2 = d_board.get_val(i, j - 1);
        pair<int, int> domino = minmax(value_1, value_2);
        if (domino_available[domino] > 0) {
            possible_dir.push_back({3, i, j-1, domino});
        }
    }
    if (i > 0 && !is_cover[i - 1][j]) {//це вверх
        int value_2 = d_board.get_val(i - 1, j);
        pair<int, int> domino = minmax(value_1, value_2);
        if (domino_available[domino] > 0) {
            possible_dir.push_back({4, i-1, j, domino});
        }
    }
    sort(possible_dir.begin(), possible_dir.end(), [&](const DominoDir& a, const DominoDir& b) {//сортування напрямків за кількістю доступних доміно для них, ще це можна назвати евристикою
            return domino_available[a.domino_type] < domino_available[b.domino_type];
        });
    for (const auto& dir : possible_dir) { //перебираємо можливі напрямки у вже сротованому порядку
        int row2 = dir.row2;
        int col2 = dir.col2;
        string dir_name;
        is_cover[i][j] = true;
        is_cover[row2][col2] = true;
        domino_available[dir.domino_type]--;
        find_solution.push_back({{i, j}, {row2, col2}});
        
        if (super_solve_recurse()) {
            return true;
        }
        find_solution.pop_back();
        domino_available[dir.domino_type]++;
        is_cover[i][j] = false;
        is_cover[row2][col2] = false;
    }
    
    return false;
}
bool dominosa_solve::solve() {
    cout << "Starting preprocessing..." << endl;
    if (!preprocess_mandatory_dominoes()) {
        cout << "Preprocessing detected unsolvable board!" << endl;
        return false;
    }
    bool all_covered = true;//перевірка чи всі клітинки покриті
    for (int i = 0; i < d_board.get_r(); ++i) {
        for (int j = 0; j < d_board.get_c(); ++j) {
            if (!is_cover[i][j]) {
                all_covered = false;
                break;
            }
        }
        if (!all_covered) break;
    }
    if (all_covered) {
        cout << "Board completely solved during preprocessing!" << endl;
        return true;
    }
    cout << "Starting recursive solving..." << endl;
    return super_solve_recurse();
}
void dominosa_solve::show_solution() const {
    int rows = d_board.get_r();
    int cols = d_board.get_c();
    int cell_len = 0;
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            cell_len = max(cell_len, (int)to_string(d_board.get_val(i, j)).length());
    vector<vector<bool>> horiz(rows, vector<bool>(cols - 1, false));//готуєм кордони доміно
    vector<vector<bool>> vert(rows - 1, vector<bool>(cols, false));
    for (const auto& dom : find_solution) {
        auto [c1, c2] = dom;
        int r1 = c1.row, c11 = c1.col;
        int r2 = c2.row, c22 = c2.col;
        if (r1 == r2) horiz[r1][min(c11, c22)] = true;
        else vert[min(r1, r2)][c11] = true;
    }
    cout << "+";//верхня рамочка
    for (int j = 0; j < cols; ++j)
        cout << string(cell_w + 2, '-') << "+";
    cout << "\n";
    for (int i = 0; i < rows; ++i) {//кожен рядок з кордонами
        cout << "|";//рядки зі значенями
        for (int j = 0; j < cols; ++j) {
            string val = to_string(d_board.get_val(i, j));
            int pad = cell_w - val.length();
            cout << " " << val << string(pad + 1, ' ');
            if (j < cols - 1) cout << (horiz[i][j] ? " " : "|");//вертикальна межа для пустих доміно
        }
        cout << "|\n";
        cout << "+";//ця штука розділяє рядки
        for (int j = 0; j < cols; ++j) {
            if (i < rows - 1 && vert[i][j])
                cout << string(cell_w + 2, ' ');
            else
                cout << string(cell_w + 2, '-');
            cout << "+";
        }
        cout << "\n";
    }
}