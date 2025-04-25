//треба додати попередню обробку, яка виявляє домінуючі пари, ці пари встановлюються перед початком рекурсивного розвязання
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
    
    // Перевірка валідності розміру дошки
    if ((rows * cols) % 2 != 0) {
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
        domino_available[pair.first] = pair.second; // Встановлюємо реальну кількість
    }
}
bool dominosa_solve::super_solve_recurse() {
    pair<int, int> next_cell = grab_next_cell();
    int i = next_cell.first;
    int j = next_cell.second;
    
    // Всі клітинки покриті, розв'язок знайдено
    if (i == -1) {
        cout << "All cells covered. Solution complete." << endl;
        return true;
    }
    
    // Розв'язок неможливий
    if (i == -2) {
        cout << "Solution impossible - isolated uncovered cell." << endl;
        return false;
    }
    
    int value_1 = d_board.get_val(i, j);
    bool placed = false;
    
    // Спроба розмістити доміно у всіх чотирьох напрямках
    
    // Вправо
    if (j + 1 < d_board.get_c() && !is_cover[i][j + 1]) {
        int value_2 = d_board.get_val(i, j + 1);
        pair<int, int> domino = minmax(value_1, value_2);
        if (domino_available[domino] > 0) {
            cout << "Trying horizontal right at (" << i << "," << j << ") and (" << i << "," << j+1 << ")" << endl;
            is_cover[i][j] = true;
            is_cover[i][j + 1] = true;
            domino_available[domino]--;
            find_solution.push_back({{i, j}, {i, j + 1}});
            if (super_solve_recurse()) {
                return true;
            }
            find_solution.pop_back();
            domino_available[domino]++;
            is_cover[i][j] = false;
            is_cover[i][j + 1] = false;
        }
    }
    
    // Вниз
    if (i + 1 < d_board.get_r() && !is_cover[i + 1][j]) {
        int value_2 = d_board.get_val(i + 1, j); 
        pair<int, int> domino = minmax(value_1, value_2);
        if (domino_available[domino] > 0) {
            cout << "Trying vertical down at (" << i << "," << j << ") and (" << i+1 << "," << j << ")" << endl;
            is_cover[i][j] = true;
            is_cover[i + 1][j] = true;
            domino_available[domino]--;
            find_solution.push_back({{i, j}, {i + 1, j}});
            if (super_solve_recurse()) {
                return true;
            }
            find_solution.pop_back();
            domino_available[domino]++;
            is_cover[i][j] = false;
            is_cover[i + 1][j] = false;
        }
    }
    
    // Вліво
    if (j > 0 && !is_cover[i][j - 1]) {
        int value_2 = d_board.get_val(i, j - 1);
        pair<int, int> domino = minmax(value_1, value_2);
        if (domino_available[domino] > 0) {
            cout << "Trying horizontal left at (" << i << "," << j << ") and (" << i << "," << j-1 << ")" << endl;
            is_cover[i][j] = true;
            is_cover[i][j - 1] = true;
            domino_available[domino]--;
            find_solution.push_back({{i, j}, {i, j - 1}});
            if (super_solve_recurse()) {
                return true;
            }
            find_solution.pop_back();
            domino_available[domino]++;
            is_cover[i][j] = false;
            is_cover[i][j - 1] = false;
        }
    }
    
    // Вверх
    if (i > 0 && !is_cover[i - 1][j]) {
        int value_2 = d_board.get_val(i - 1, j);
        pair<int, int> domino = minmax(value_1, value_2);
        if (domino_available[domino] > 0) {
            cout << "Trying vertical up at (" << i << "," << j << ") and (" << i-1 << "," << j << ")" << endl;
            is_cover[i][j] = true;
            is_cover[i - 1][j] = true;
            domino_available[domino]--;
            find_solution.push_back({{i, j}, {i - 1, j}});
            if (super_solve_recurse()) {
                return true;
            }
            find_solution.pop_back();
            domino_available[domino]++;
            is_cover[i][j] = false;
            is_cover[i - 1][j] = false;
        }
    }
    
    return false;
}

bool dominosa_solve::place_all_dominoes_of_type(const pair<int, int>& domino_type) {//нововведення для розміщення всіх доміно певного типу
    int rows = d_board.get_r();
    int cols = d_board.get_c();
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (is_cover[i][j]) continue;
            if (j < cols - 1 && !is_cover[i][j+1]) {//
                pair<int, int> current_domino = minmax(d_board.get_val(i, j), d_board.get_val(i, j+1));
                if (current_domino == domino_type) {
                    is_cover[i][j] = true;
                    is_cover[i][j+1] = true;
                    find_solution.push_back({{i, j}, {i, j+1}});
                    cout << "Forced: Placed horizontal domino at (" << i << "," << j << ") and (" << i << "," << j+1 << ")" << endl;
                }
            }
            
            // Перевіряємо вертикальне доміно
            if (i < rows - 1 && !is_cover[i+1][j]) {
                pair<int, int> current_domino = minmax(d_board.get_val(i, j), d_board.get_val(i+1, j));
                if (current_domino == domino_type) {
                    is_cover[i][j] = true;
                    is_cover[i+1][j] = true;
                    find_solution.push_back({{i, j}, {i+1, j}});
                    cout << "Forced: Placed vertical domino at (" << i << "," << j << ") and (" << i+1 << "," << j << ")" << endl;
                }
            }
        }
    }
    
    return true;
}
pair<int, int> dominosa_solve::grab_next_cell() {
    int best_row = -1, best_col = -1;
    int minimum_option = numeric_limits<int>::max();
    
    // Оптимізована версія - пріоритет клітинкам з меншими можливостями
    for (int i = 0; i < d_board.get_r(); ++i) {
        for (int j = 0; j < d_board.get_c(); ++j) {
            if (!is_cover[i][j]) {
                int option = 0;
                int value_1 = d_board.get_val(i, j);
                
                // Перевіряємо можливі варіанти розміщення
                if (j + 1 < d_board.get_c() && !is_cover[i][j + 1]) {
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
                
                // Також перевіряємо можливі варіанти зліва та зверху
                if (j > 0 && !is_cover[i][j - 1]) {
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
                // Якщо у клітинки нема варіантів, але вона ще не покрита
                if (option == 0) {
                    // Перевіряємо, чи є ця клітинка єдиною непокритою
                    bool is_only_uncovered = true;
                    for (int r = 0; r < d_board.get_r(); ++r) {
                        for (int c = 0; c < d_board.get_c(); ++c) {
                            if (!is_cover[r][c] && (r != i || c != j)) {
                                is_only_uncovered = false;
                                break;
                            }
                        }
                        if (!is_only_uncovered) break;
                    }
                    if (is_only_uncovered) {
                        // Якщо це єдина непокрита клітинка, то ми не можемо знайти розв'язок
                        return {-2, -2};
                    }
                    // Іншому випадку, все одно обираємо цю клітинку, оскільки для неї потрібен спеціальний підхід
                    return {i, j};
                }
                if (option < minimum_option) {
                    minimum_option = option;
                    best_row = i;
                    best_col = j;
                }
            }
        }
    }
    // Якщо всі клітинки покриті
    if (best_row == -1) {
        return {-1, -1};
    }
    return {best_row, best_col};
}

bool dominosa_solve::super_solve_recurse() {//головоломку розвязуєм через рекурсію
    pair<int, int> next_cell = grab_next_cell();
    int i = next_cell.first;
    int j = next_cell.second;
    if (i == -1) {//знайдене рігення
        cout << "All cells covered. Solution complete." << endl;
        return true; // Всі клітинки покриті, розв'язок знайдено
    }
    if (i == -2) { //якщо є клітинка без можл ходів, рішення стаеє неможливим
        return false; // Неможливо розв'язати
    }
    
    int value_1 = d_board.get_val(i, j);
    if (d_board.is_val(i, j + 1) && !is_cover[i][j + 1]) {
        int value_2 = d_board.get_val(i, j + 1);
        pair<int, int> domino = minmax(value_1, value_2);
        if (domino_available[domino] > 0) { // для горизонтального домвно
            cout << "Trying horizontal at (" << i << "," << j << ") and (" << i << "," << j+1 << ")" << endl;
            is_cover[i][j] = true;
            is_cover[i][j + 1] = true;
            domino_available[domino]--; // Зменшуємо лічильник
            find_solution.push_back({{i, j}, {i, j + 1}});
            if (super_solve_recurse()) {//продовження розв'язання
                return true;
            }
            cout << "Backtracking from horizontal at (" << i << "," << j << ") and (" << i << "," << j+1 << ")" << endl;
            find_solution.pop_back();//якщо рекурсія не вдалась поточне розміщення скасовується
            domino_available[domino]++; // Відновлюємо лічильник
            is_cover[i][j] = false;
            is_cover[i][j + 1] = false;
        }
    }
    if (d_board.is_val(i + 1, j) && !is_cover[i + 1][j]) {
        int value_2 = d_board.get_val(i + 1, j); 
        pair<int, int> domino = minmax(value_1, value_2);
        if (domino_available[domino] > 0) { //для вертикального домвно
            cout << "Trying vertical at (" << i << "," << j << ") and (" << i+1 << "," << j << ")" << endl;
            is_cover[i][j] = true;
            is_cover[i+ 1][j] = true;
            domino_available[domino]--; // Зменшуємо лічильник
            find_solution.push_back({{i, j}, {i + 1, j}});
            if (super_solve_recurse()) {//продовження розв'язання
                return true;
            }
            cout << "Backtracking from vertical at (" << i << "," << j << ") and (" << i+1 << "," << j << ")" << endl;
            find_solution.pop_back();//якщо рекурсія не вдалась поточне розміщення скасовується
            domino_available[domino]++; // Відновлюємо лічильник
            is_cover[i][j] = false;
            is_cover[i + 1][j] = false;
        }
    }
    return false;
}

bool dominosa_solve::solve() {//попередньо обробляємо дошку для розставлення зрозумілих доміно
    cout << "Starting preprocessing..." << endl;
    if (!preprocess_mandatory_dominoes()) {
        cout << "Preprocessing detected unsolvable board!" << endl;
        return false;
    }
    
    // Перевіряємо, чи всі клітинки вже покриті
    bool all_covered = true;
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

void dominosa_solve::show_solution() const {//це метод для виведення в консоль
    int row = d_board.get_r();
    int col = d_board.get_c();
    vector<vector<bool>> horizontal_dominosa(row, vector<bool>(col - 1, false));//потрібно для розміщення горизонтальних та вертикальних доміно
    vector<vector<bool>> vertical_dominosa(row - 1, vector<bool>(col, false));
    
    for (const auto& dom : find_solution) {// заповнення проміжних сіток на основі знайденого розв'язку
        int row1 = dom.first.row;
        int col1 = dom.first.col;
        int row2 = dom.second.row;
        int col2 = dom.second.col;
        if (row1 == row2) {// горизонтальне доміно
            horizontal_dominosa[row1][min(col1, col2)] = true;
        } else {
            vertical_dominosa[min(row1, row2)][col1] = true;
        }
    }
    
    for (int a = 0; a < row; ++a) {// Використовуємо Unicode символи для красивішого відображення 
        for (int b = 0; b < col; ++b) {// виводимо числа та горизонтальні зєднання
            cout << " " << d_board.get_val(a, b) << " ";//для виводу числа в поточній клітинці з пробілами для вирівнювання
            if (b < col - 1) {
                if (horizontal_dominosa[a][b]) {
                    cout << "═"; // Використовуємо Unicode символ для горизонт. лінії
                } else {
                    cout << " ";
                }
            }
        }
        cout << endl;
        if (a < row - 1) {// виводимо вертикальні зднання
            for (int b = 0; b < col; ++b) {
                if (vertical_dominosa[a][b]) {
                    cout << " ║ "; // Використовуємо Unicode символ для вертик. лінії
                } else {
                    cout << "   ";
                }
                if (b < col - 1) {
                    cout << " ";
                }
            }
            cout << endl;
        }
    }
}