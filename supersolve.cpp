//треба додати попередню обробку, яка виявляє домінуючі пари, ці пари встановлюються перед початком рекурсивного розвязання
//розвязувач
#include "solvedominosa.h"
#include <iostream> 
#include <vector>         
#include <map>            
#include <utility>       
#include <string>         
#include <algorithm>//std::min, std::max, std::fill
#include <limits>         
#include <iomanip>                
#include <stdexcept> 
#include <cmath> 
using namespace std;   
const int DELTA_ROW[NEIGHBOR_DIRECTIONS_COUNT] = {0, 1, 0, -1}; //зміщення по рядках для кожного з 4 напрямків(праворуч,вниз,ліворуч,вгору)
const int DELTA_COL[NEIGHBOR_DIRECTIONS_COUNT] = {1, 0, -1, 0}; //зміщення по стовпцях для кожного з 4 напрямків

void simple_pause_internal_helper(const string& message = "Press 0 and Enter to continue...") {//функція для призупинки виконання програми та очікування вводу
    cout << message << endl; //для повідомлення-підказки
    string line; //для зчитування повного вводу користувача
    while (true) { //очікування коректного вводу
        if (!getline(cin, line)) { //читання всього рядка, введеного користувачем
             cerr << "\nInput stream error or EOF during pause." << endl; //помилка зчитування або кінця файлу
             break; // вихід з циклу
        }
        line.erase(0, line.find_first_not_of(" \t\n\r\f\v")); //видалення пробільних символів з початку та кінця введеного рядка
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
        if (line.length() == 1 && line[0] == '0') { //очікується 0 від коритувача
             break; //якщо ввелено 0 користувачем з циклу виходимо
        } else { //щось інше
             cout << "Invalid input. Please press 0 and Enter." << endl; //повторний запит до користувача
        }
    } 
}
void print_separator() {//лінія роздільник використовується для візуального структурування виводу в print_step. 
  cout << "\n------------------------------------------------------------\n" << endl;
}
pair<int, int> dominosa_solve::make_domino_key(int a, int b) { //створення ключа доміно
  return make_pair(min(a, b), max(a, b));//повертає пару, де перше значення завжди менше або дорівнює другому.
}
dominosa_solve::dominosa_solve(const board& b, bool display_steps): d_board(b), show_steps(display_steps) { //розвязувач з дошкою яка передається
    int current_rows = d_board.get_r(); //рядки з об'єкта дошки d_board
    int current_cols = d_board.get_c(); //стовпці з об'єкта дошки d_board
    if (current_rows > 0 && current_cols > 0) { //якщо дошка не порожня
        is_cover.assign(current_rows, vector<bool>(current_cols, false)); //ініціалізуємо список знайдених доміно порожнім (всі клітинки спочатку не покриті)
    } else { //дошка порожня
        is_cover.clear(); // Очищуємо вектор і робимо його порожнім
    }
    find_solution.clear(); //очищуємо вектор де зберігатиметься розв'язок
    init_dominosa(); //для ініціалізації набору доступних доміно
}
void dominosa_solve::init_dominosa() { //ініціалізує мапу стандартним набором доміно для діапазону чисел на дошці
    domino_available.clear(); //очищаю мапу доступних доміно перед новою ініціалізацією
    int current_rows = d_board.get_r(); //кільсть. рядків
    int current_cols = d_board.get_c(); //кільсть. стовпців
    if (current_rows == 0 || current_cols == 0) return; //порожня дошка, нічого ініціалізувати
    int min_board_val = 0;//знаходжу мінімальне та максимальне значення на дошці
    int max_board_val = 0;
    bool first_val_processed = false; //для коректної першої ініціалізації

    for (int i = 0; i < current_rows; ++i) { //для пошуку мінімального та максимального значень на ігровій дошці
        for (int j = 0; j < current_cols; ++j) { 
            int val = d_board.get_val(i, j); //значення поточної клітинки
            if (!first_val_processed) { //це перше оброблюване значення на дошці
                 min_board_val = val;     
                 max_board_val = val;  
                 first_val_processed = true;  //це значить значення оброблено
            } else { //для всіх наступних значень
                min_board_val = min(min_board_val, val); //якщо поточне значення менше
                max_board_val = max(max_board_val, val); //якщо поточне значення більше
            }
        }
    }
    for (int a = min_board_val; a <= max_board_val; ++a) { //тут ми знаходимо максимальне число на нашій дошці для визначення набору доміно (ну або ж діапазон)
        for (int b = a; b <= max_board_val; ++b) { //кожне стандартне доміно повинно бути використане 1 раз
            //p.s. //цикл починається з 'a', щоб уникнути дублікатів типу {X,Y} та {Y,X} і самодублікатів типу {X,X}
            domino_available[dominosa_solve::make_domino_key(a, b)] = 1; //очікувана кількість 1
        }
    }
}
void dominosa_solve::place_domino(int r1, int c1, int r2, int c2, const pair<int, int>& domino_type) {//тут розміщує доміно на дошці та оновлює стан розв'язувача
    if (!d_board.is_val(r1, c1) || !d_board.is_val(r2, c2)) {// //чи вихідні координати знаходяться в межах дошки
        cerr << "Error: Placing domino with coordinates out of bounds: [" << r1 << "," << c1 << "] - [" << r2 << "," << c2 << "]!" << endl;
        return;//якщо координати невалідні
    }
    size_t ur1 = static_cast<size_t>(r1); size_t uc1 = static_cast<size_t>(c1);//конвертація координат в для безпечного індексування векторів
    size_t ur2 = static_cast<size_t>(r2); size_t uc2 = static_cast<size_t>(c2);
    int board_cols_count = d_board.get_c(); //для перевірки меж 
    if (is_cover.empty() || ur1 >= is_cover.size() || (board_cols_count > 0 && uc1 >= is_cover[ur1].size()) || ur2 >= is_cover.size() || (board_cols_count > 0 && uc2 >= is_cover[ur2].size()) ) {//чи індекси не виходять за її розміри
         cerr << "Error: is_cover grid access out of bounds in place_domino!" << endl;
         return;
    }
    if (is_cover[r1][c1] || is_cover[r2][c2]) {//чи клітинки ще не покриті
        cerr << "Error: Trying to place domino on already covered cells [" << r1 << "," << c1 << "] - [" << r2 << "," << c2 << "]!" << endl;
        return; //якщо клітинки вже зайняті
    }
    is_cover[r1][c1] = true;//позначимо обидві клітинки як покриті в матриці is_cover
    is_cover[r2][c2] = true;
    auto it = domino_available.find(domino_type); //оновимо лічильник доступних доміно цього типу
    if (it != domino_available.end()) { //перевіряю - доміно ще не було розміщено (більше 0)
        if (it->second > 0) { //його лічильник показує, що воно ще доступне
            it->second--; //зменшуємо лічильник ( з 1 до 0 ака розміщено)
        } else {//спроба розмістити доміно, яке вже розміщено тепер викликає помилку
            cerr << "Error: Trying to place already placed standard domino type (" << domino_type.first << "," << domino_type.second <<")! Count was " << it->second << "." << endl;
        }
    } else {//спроба розмістити пару, якої немає в стандартному наборі викликає помилку
        cerr << "Error: Trying to place non-standard domino type (" << domino_type.first << "," << domino_type.second <<")!" << endl;
    }
    find_solution.push_back({ {r1, c1}, {r2, c2} });//додоано розміщене доміно до списку поточного розв'язку
}
void dominosa_solve::backtrack_domino(int r1, int c1, int r2, int c2, const pair<int, int>& domino_type) {//тут скасовує розміщення доміно, відновлюючи попередній стан розв'язувача
    if (!d_board.is_val(r1, c1) || !d_board.is_val(r2, c2)) {//перевірка меж та чи клітинки покриті перед відкатом
         cerr << "Error: Trying to backtrack domino with coordinates out of bounds: ["<< r1 << "," << c1 << "] - [" << r2 << "," << c2 << "]!" << endl;
        return; //при невалідних координатах
    }
    size_t ur1 = static_cast<size_t>(r1); size_t uc1 = static_cast<size_t>(c1); //конвертація та отримання розмірів для безпечного індексування
    size_t ur2 = static_cast<size_t>(r2); size_t uc2 = static_cast<size_t>(c2);
    int board_cols_count = d_board.get_c();
    if (is_cover.empty() || ur1 >= is_cover.size() || (board_cols_count > 0 && uc1 >= is_cover[ur1].size()) || ur2 >= is_cover.size() || (board_cols_count > 0 && uc2 >= is_cover[ur2].size()) ) { //перевірка меж для доступу до is_cover
         cerr << "Error: is_cover grid access out of bounds in backtrack_domino!" << endl;
         return;
    }
    if (!is_cover[r1][c1] || !is_cover[r2][c2]) {//перевірка чи клітинки покриті перед відкатом
        cerr << "Error: Trying to backtrack domino from non-covered cells [" << r1 << "," << c1 << "] - [" << r2 << "," << c2 << "]!" << endl;
        return;
    }

    if (!find_solution.empty()) { //видаляю останнє доміно зі списку розв'язку find_solution
         pair<dominosa::coordinate, dominosa::coordinate> last_domino_in_solution = find_solution.back(); //чи дійсно останнє доміно у find_solution відповідає тому, що відкочується?
         bool coordinates_match = ((last_domino_in_solution.first.row == r1 && last_domino_in_solution.first.col == c1 && last_domino_in_solution.second.row == r2 && last_domino_in_solution.second.col == c2) || (last_domino_in_solution.first.row == r2 && last_domino_in_solution.first.col == c2 && last_domino_in_solution.second.row == r1 && last_domino_in_solution.second.col == c1));
         if(coordinates_match) { //координати співпадають
            find_solution.pop_back(); //видаляю останній елемент
         } else { //координати не співпадають 
             cerr << "Warning: Backtrack requested for [" << r1 << "," << c1 << "]-[" << r2 << "," << c2 << "] but last domino in solution is [" << last_domino_in_solution.first.row << "," << last_domino_in_solution.first.col  << "]-[" << last_domino_in_solution.second.row << "," << last_domino_in_solution.second.col << "]. State might be inconsistent." << endl;
         }
    } else { //find_solution порожній, але клітинки були покриті
         cerr << "Warning: Trying to backtrack domino when find_solution is empty, but cells were covered." << endl;
         return;
    }
    auto it = domino_available.find(domino_type); //пошук типу доміно в мапі для лічильника доступних доміно
    if (it != domino_available.end()) { //перевіряю,ьдоміно було розміщено (його лічильник мав стати == 0) перед тим як збільшувати
            if (it->second == 0) { //доміно було розміщене
                it->second++; //тепер він 1 бо ми його збілшьшили , доміно знову доступне для використання
            } else { //спроба відкатити доміно, яке не було розміщено
                cerr << "Error: Trying to backtrack a standard domino (" << domino_type.first << "," << domino_type.second <<") that seems not placed correctly (count=" << it->second << ", expected 0)!" << endl;
            }
        } else { //намагання відкотити доміно, типу якого немає в стандартному наборі (помилка)
            cerr << "Error: Trying to backtrack non-standard domino type (" << domino_type.first << "," << domino_type.second <<")!" << endl;
    }
    if (d_board.is_val(r1, c1)) is_cover[r1][c1] = false; //клітинки знову непокриті в матриці is_cover
    if (d_board.is_val(r2, c2)) is_cover[r2][c2] = false;
} 
void dominosa_solve::print_step(const string& message) {//друк кроку
    if (!show_steps) return; //друк відбуватиметься лише якщо show_steps увімкнено
    print_separator();

    cout << "--- Step ---" << endl;
    cout << message << endl; //
    cout << "------------" << endl;
    show_solution(); //виклик методу show_solution() для відображення поточного стану дошки з розміщеними доміно
    cout << "------------" << endl;
    cout << "Remaining standard dominoes: "; //про доміно, які ще залишилися доступними
    bool first_domino_was_printed = true;
    for(const auto& map_entry : domino_available) {
        if (map_entry.second > 0) { //тільки ті, що ще потрібно розмістити (лічильник > 0)
            if (!first_domino_was_printed) {
                cout << ", "; //кома-роздільник
            }
            cout << "(" << map_entry.first.first << "," << map_entry.first.second << ")"; //виводжу тільки тип доміно
            first_domino_was_printed = false;
        }
    }
    if (first_domino_was_printed) { //жодного доступного доміно не було надруковано
        cout << "None remaining";
    }
    cout << endl;
    simple_pause_internal_helper();
} 
bool dominosa_solve::preprocess_dominoes() { //попередня обробка дошки для знаходження очевидних доміно
    int current_rows = d_board.get_r(); 
    int current_cols = d_board.get_c(); 
    if (current_rows <= 0 || current_cols <= 0) { return true; } 

    bool progress_was_made_in_pass; //чи було зроблено хід на поточній ітерації
    do {
        progress_was_made_in_pass = false; //припустимо, що прогресу не буде
        for (size_t i_idx = 0; i_idx < static_cast<size_t>(current_rows); ++i_idx) {//по всіх клітинках дошки ітерація
            for (size_t j_idx = 0; j_idx < static_cast<size_t>(current_cols); ++j_idx) {
                int r = static_cast<int>(i_idx); //поточний рядок
                int c = static_cast<int>(j_idx); //поточний стовпець
                if (i_idx >= is_cover.size() || j_idx >= is_cover[i_idx].size()) { //доступ до is_cover
                    cerr << "Error: is_cover access out of bounds [" << i_idx << "," << j_idx << "] in preprocess_dominoes!" << endl;
                    return false;
                }
                if (is_cover[r][c]) continue; //якщо клітинка вже покрита пропускаємо
                int val1 = d_board.get_val(r, c); //знач. в поточній клітинці
                int num_valid_neighbor_options = 0;//лічильник можливих ходів
                int r2_for_forced_move = -1;//координати для вимушеного ходу
                int c2_for_forced_move = -1;
                pair<int, int> type_of_forced_domino = {-1, -1};
                for (int k = 0; k < NEIGHBOR_DIRECTIONS_COUNT; ++k) { //перебір сусідів
                    int nr = r + DELTA_ROW[k]; //координати сусіда
                    int nc = c + DELTA_COL[k];

                    if (d_board.is_val(nr, nc)) {//сусід в межах дошки
                         size_t unr = static_cast<size_t>(nr);
                         size_t unc = static_cast<size_t>(nc);
                            if (unr >= is_cover.size() || unc >= is_cover[unr].size()) { //доступ до is_cover сусіда
                              cerr << "Error: is_cover neighbor access out of bounds [" << nr << "," << nc << "] in preprocess_dominoes!" << endl;
                              return false;
                            }
                        if (!is_cover[nr][nc]) {//перевіряю наявность в наборі і чи ще не розміщено (> 0)
                            pair<int, int> potential_key = make_domino_key(val1, d_board.get_val(nr, nc)); //ключ доміно
                            auto it_available = domino_available.find(potential_key); //пошук в доступних
                            if (it_available != domino_available.end() && it_available->second > 0) { //знайдено і доступно
                                num_valid_neighbor_options++; //збільшимолічильник варіантів
                                if (num_valid_neighbor_options == 1) { //це перший варіант
                                    r2_for_forced_move = nr;//зберігаємо його
                                    c2_for_forced_move = nc;
                                    type_of_forced_domino = it_available->first;
                                } else { //варіантів більше одного
                                    break; //хід не вимушений, припинити перевірку сусідів для
                                }
                            }
                        }
                    }
                }
                if (num_valid_neighbor_options == 1) { //є тільки один можливий хід з цієї клітинки
                    size_t ufr_check = static_cast<size_t>(r2_for_forced_move);
                    size_t ufc_check = static_cast<size_t>(c2_for_forced_move);
                    if (r2_for_forced_move != -1 && ufr_check < is_cover.size() && ufc_check < is_cover[ufr_check].size() && !is_cover[r2_for_forced_move][c2_for_forced_move]) { //чи сусідня клітинка ще не покрита
                        string msg = "Preprocessing: Placed forced domino (" + to_string(val1) + "," + to_string(d_board.get_val(r2_for_forced_move, c2_for_forced_move)) + ") at [" + to_string(r) + "," + to_string(c) + "] - [" + to_string(r2_for_forced_move) + "," + to_string(c2_for_forced_move) + "]";
                        place_domino(r, c, r2_for_forced_move, c2_for_forced_move, type_of_forced_domino); //розмістити доміно
                        progress_was_made_in_pass = true; //прогрес зроблений
                        print_step(msg); //друкуємо крок
                    }
                } else if (num_valid_neighbor_options == 0 && !is_cover[r][c]) { //для непокритої клітинки немає ходів
                    print_step("Preprocessing Error: Cell [" + to_string(r) + "," + to_string(c) + "] ("+ std::to_string(d_board.get_val(r, c)) + ") is uncovered with no valid standard domino moves.");//немає жодного можливого ходу для непокритої клітинки
                    return false; //дошка нерозв'язна
                }
            } 
        }
    } while (progress_was_made_in_pass); 
    for(const auto& map_entry_check : domino_available) {//перевіряю на від'ємну кількість доміно після попередньої обробки
        if (map_entry_check.second < 0) {
             cerr << "Error: Preprocessing resulted in negative count for domino (" << map_entry_check.first.first << "," << map_entry_check.first.second << ")! Count: " << map_entry_check.second << endl;
             return false; 
        }
    }
    return true;
}
pair<int, int> dominosa_solve::grab_next_cell() {//обирає наступну непокриту клітинку для обробки, використовуючи евристику
    
    const int UNINITIALIZED_OPTIONS_CONST = numeric_limits<int>::max(); //для початкового значення min_options 
    
    int best_r = dominosa::CELL_NOT_FOUND; //рядок найкращої знайденої клітинки
    int best_c = dominosa::CELL_NOT_FOUND; //стовпець найкращої знайденої клітинки 
    int min_options_for_cell = UNINITIALIZED_OPTIONS_CONST; //мін. кількість варіантів ходу
    bool uncovered_cell_found = false; //чи знайдено хоча б одну непокриту клітинку

    int current_rows = d_board.get_r();
    int current_cols = d_board.get_c();
    if (current_rows <= 0 || current_cols <= 0 || is_cover.empty()) {
        return {dominosa::CELL_NOT_FOUND, dominosa::CELL_NOT_FOUND}; //клітинок немає
    }
    for (size_t i_idx = 0; i_idx < static_cast<size_t>(current_rows); ++i_idx) {//ітерація по всіх клітинках дошки
        for (size_t j_idx = 0; j_idx < static_cast<size_t>(current_cols); ++j_idx) {
             int r = static_cast<int>(i_idx); //поточний рядок
             int c = static_cast<int>(j_idx); //поточний стовпець
            if (i_idx >= is_cover.size() || j_idx >= is_cover[i_idx].size()) {
                  cerr << "Error: is_cover access out of bounds [" << i_idx << "," << j_idx << "] in grab_next_cell!" << endl;
                  return {dominosa::INVALID_COORDINATE, dominosa::INVALID_COORDINATE};
            }
            if (!is_cover[r][c]) {//клітинка не покрита
                uncovered_cell_found = true; //позначаю, що знайдено непокриту клітинку
                int options_count_for_this_cell = 0; //лічильник можливих ходів для поточної клітинки
                int val1 = d_board.get_val(r, c); //знач. в поточній клітинці
                for (int k = 0; k < NEIGHBOR_DIRECTIONS_COUNT; ++k) { //прербираю сусідів для підрахунку можливих ходів
                    int nr = r + DELTA_ROW[k]; //коорд.сусіда
                    int nc = c + DELTA_COL[k];
                    if (d_board.is_val(nr, nc)) { //сусід в межах дошки
                         size_t unr = static_cast<size_t>(nr);
                         size_t unc = static_cast<size_t>(nc);
                        if (unr >= is_cover.size() || unc >= is_cover[unr].size()) {//доступ до is_cover сусіда
                              cerr << "Error: is_cover neighbor access out of bounds [" << nr << "," << nc << "] in grab_next_cell!" << endl;
                              return {dominosa::INVALID_COORDINATE, dominosa::INVALID_COORDINATE};
                        }
                        if (!is_cover[nr][nc]) { //сусід також не покритий
                            pair<int, int> key = make_domino_key(val1, d_board.get_val(nr, nc));//перевіряю наявность в наборі
                            auto it_available = domino_available.find(key); //знайдемо  доміно в доступних
                            if (it_available != domino_available.end() && it_available->second > 0) { //знайдено і доступно
                                options_count_for_this_cell++; //збільш.лічильник варіантів для
                            }
                        }
                    }
                }
                if (options_count_for_this_cell == 0) {//якщо ходів немає з цієї клітинки
                    if(show_steps) print_step("Dead end: Cell [" + to_string(r) + "," + to_string(c) + "] is uncovered with no valid moves.");
                    return {dominosa::NO_VALID_MOVES, dominosa::NO_VALID_MOVES}; //повертаємо сигнал про відсутність ходів
                }
                if (options_count_for_this_cell < min_options_for_cell) { //знахожду клітинку з мінімальною кількістю можливих ходів тут евристика
                    min_options_for_cell = options_count_for_this_cell; //оновили  мінімальну кількість варіантів
                    best_r = r;//збережено координати цієї найкращої клітинки
                    best_c = c;
                     if (min_options_for_cell == 1) {//знайшли клітинку з єдиним можливим ходом, повертаємо її негайно
                        return {best_r, best_c};
                    }
                }
            }
        }
    }
     //p.s. обробка результатів після повного сканування дошки
    if (!uncovered_cell_found) { //не знайдено жодної непокритої клітинки
        return {dominosa::CELL_NOT_FOUND, dominosa::CELL_NOT_FOUND}; //всі клітинки покриті і дошка розв'язана
    }
    return {best_r, best_c}; //повертаю знайдену найкращу клітинку та що з найменшою кількістю варіантів
}
bool dominosa_solve::super_solve_recurse() {//найголовніша частина бектрекінгу 
    // 1. Отримання координат наступної непокритої клітинки для спроби розміщення доміно.
    pair<int, int> next_cell_to_process = grab_next_cell();
    int r1 = next_cell_to_process.first; //рядок першої клітинки
    int c1 = next_cell_to_process.second; //стовпець першої клітинки
    
    if (r1 == dominosa::CELL_NOT_FOUND) {//всі клітинки на дошці покрито
        return true; //розв'язок знайдено для цієї гілки рекурсії
    }
    if (r1 == dominosa::NO_VALID_MOVES || r1 == dominosa::INVALID_COORDINATE) {//для поточної непокритої клітинки немає можливих ходів
        return false; //не веде до розв'язку, потрібно відкотитися
    }
    int value_at_r1c1 = d_board.get_val(r1, c1); //генерація всіх можливих ходів з поточної клітинки
    vector<domino_move> list_of_possible_moves;//вектор для зберігання можливих ходів 
    for (int k = 0; k < NEIGHBOR_DIRECTIONS_COUNT; ++k) {//всі можливі ходи з поточної клітинки, 4 сусіди
        int r2_candidate = r1 + DELTA_ROW[k]; //коорд. потенційної другої клітинки доміно 
        int c2_candidate = c1 + DELTA_COL[k]; 

        if (d_board.is_val(r2_candidate, c2_candidate)) { //друга клітинка знаходиться в межах дошки
            size_t ur2_check = static_cast<size_t>(r2_candidate); 
            size_t uc2_check = static_cast<size_t>(c2_candidate); 
            if (ur2_check >= is_cover.size() || uc2_check >= is_cover[ur2_check].size()){
                cerr << "Error: is_cover neighbor access out of bounds [" << r2_candidate << "," << c2_candidate << "] in super_solve_recurse!" << endl;//перевірка меж для is_cover другої клітинки
                continue; //пропустити цього сусіда
            }
            if (!is_cover[r2_candidate][c2_candidate] ) { //чи сусідня клітинка в межах дошки і не покрита
                pair<int, int> current_domino_key_type = make_domino_key(value_at_r1c1, d_board.get_val(r2_candidate, c2_candidate));//канонічний ключ для потенційного доміно
                auto it_available_check = domino_available.find(current_domino_key_type); //треба знайти цей тип в мапі доступних доміно
                if (it_available_check != domino_available.end() && it_available_check->second > 0) { //наявність доміно в наборі та його доступність (> 0)
                    //p.s. додаємо цей валідний хід до списку можливих ходів
                    list_of_possible_moves.push_back({r2_candidate, c2_candidate, it_available_check->first}); //використаю ключ з мапи для гарантії канонічності
                }
            }
        }
    }
    for (const auto& current_move_to_try : list_of_possible_moves) { //перебираємо можливі ходи для кожного можливого
        int r2_to_place = current_move_to_try.r2; //коорд. другої клітинки з поточного ходу
        int c2_to_place = current_move_to_try.c2; 
        string step_message = "Recursive step: Trying domino (" + to_string(value_at_r1c1) + "," + to_string(d_board.get_val(r2_to_place,c2_to_place)) + ") at [" + to_string(r1) + "," + to_string(c1) + "] - [" + to_string(r2_to_place) + "," + to_string(c2_to_place) + "]";
        place_domino(r1, c1, r2_to_place, c2_to_place, current_move_to_try.domino_type);//розміщуємо доміно на дошці
        if (show_steps) { //увімкнено покроковий режим
            print_step(step_message); //виводимо інформацію про зроблений крок
        }
        if (super_solve_recurse()) {//намагаємося розв'язати решту дошки
            return true; // було знайдено повне покриття дошки, поточний хід є частиною цього успішного шляху
        }
        if (show_steps) { //якщо рекурсивний виклик повернув хибу
             string backtrack_message = "Backtracking: Removing domino (" + to_string(value_at_r1c1) + "," + to_string(d_board.get_val(r2_to_place,c2_to_place)) + ") from [" + to_string(r1) + "," + to_string(c1) + "] - [" + to_string(r2_to_place) + "," + to_string(c2_to_place) + "]";
        }
        backtrack_domino(r1, c1, r2_to_place, c2_to_place, current_move_to_try.domino_type);//рекурсивний виклик не знайшов рішення,відкатуємо хід(прошрама повертається на 355 рядок)
    }
    return false; //жоден з можливих ходів не призвів до рішення, повертаємо хибу
}
bool dominosa_solve::solve() {//повертає тру, якщо розв'язок знайдено, інакше фолз
    find_solution.clear();
    int current_rows = d_board.get_r();//отримання поточних розмірів дошки
    int current_cols = d_board.get_c();
    if (current_rows > 0 && current_cols > 0) { //чи дошка має дійсні розміри
        if (is_cover.size() != static_cast<size_t>(current_rows) || is_cover.empty() || (current_cols > 0 && is_cover[0].size() != static_cast<size_t>(current_cols))) { //чи потрібно переініціалізувати матрицю is_cover
            is_cover.assign(current_rows, vector<bool>(current_cols, false)); //якщо потрібно, переініціалізувати is_cover як матрицю булевих значень
        } else {//is_cover вже має правильні розміри, заповнити її значеннями хиба
            for(size_t i=0; i < static_cast<size_t>(current_rows); ++i) {
                fill(is_cover[i].begin(), is_cover[i].end(), false);
            }
        }
    } else {//дошка порожня
        is_cover.clear();
        return (current_rows == 0 && current_cols == 0); //порожня дошка 0x0 вважається розв'язаною
    }
    init_dominosa();//ініціалізація стану доміно
    if (show_steps) {
        print_step("Starting preprocessing stage..."); //попередня обробка
    }
    if (!preprocess_dominoes()) {//якщо попередня обробка повертає хиба,наприклад, виявлено суперечність або неможливість розмістити доміно
        find_solution.clear();
        return false;         
    }
    bool all_cells_covered_after_preprocessing = true; //чи попередня обробка вже розв'язала дошку
    for (size_t i = 0; i < static_cast<size_t>(current_rows); ++i) {//чи всі клітинки дошки покриті
        if (i >= is_cover.size()) { 
            all_cells_covered_after_preprocessing = false; 
            break;
        }
        for (size_t j = 0; j < static_cast<size_t>(current_cols); ++j) {
            if (j >= is_cover[i].size() || !is_cover[i][j]) {
                all_cells_covered_after_preprocessing = false; 
                break; //знайдено не покриту клітинку, виходимо
            }
        }
        if (!all_cells_covered_after_preprocessing) break; //якщо прапорець вже хиба, вийти з зовнішнього циклу
    }
    if (all_cells_covered_after_preprocessing) {  //якщо всі клітинки були покриті на етапі попередньої обробки
        bool counts_are_consistent_after_pp = true; //для перевірки узгодженості кількості доміно
        size_t expected_dominoes_on_board = static_cast<size_t>((d_board.get_r() * d_board.get_c()) / 2); //розрахунок очікуваної кількості доміно на дошці (площа дошки / 2)
        if (find_solution.size() != expected_dominoes_on_board) { //кількість знайдених доміно не відповідає очікуваній
            counts_are_consistent_after_pp = false; //прапорець неузгодженості
        }
        if (counts_are_consistent_after_pp) {//кількість доміно узгоджена (і всі клітинки покриті)
            if (show_steps) {
                print_step("Solved by preprocessing! All cells covered.");
            }
            return true;
        } else {
            if(show_steps) { //кількість доміно не узгоджена, хоча всі клітинки покриті
                print_step("Inconsistency after preprocessing: board covered but domino counts/usage are wrong."); //неузгодженність
            }
            find_solution.clear(); 
            return false;         
        }
    }
    if (show_steps) { //запуск основного рекурсивного пошуку
        print_step("Preprocessing complete. Starting recursive search...");
    }
    bool recursion_was_successful = super_solve_recurse();
    if (recursion_was_successful) { //перевірка результату рекурсії
        size_t expected_dominoes_on_board = static_cast<size_t>((d_board.get_r() * d_board.get_c()) / 2);//розрахунок очікуваної кількості доміно
        if (find_solution.size() != expected_dominoes_on_board) {//чи кількість знайдених доміно відповідає очікуваній          
            find_solution.clear(); 
            return false;      
        }
        for(const auto& map_entry_final_check : domino_available) { //перевірка стану доступних доміно наприклад, чи не використано більше доміно, ніж було
            if (map_entry_final_check.second < 0) {//кількість доступних доміно певного типу стала від'ємною
                find_solution.clear(); //очистити некоректне рішення
                return false;       
            }
        }
        if (show_steps) {//всі перевірки пройдені успішно.
            print_step("Recursive search completed successfully! Valid solution found."); //успішне знаходження валідного розв'язку
        }
        return true; //розв'язок знайдено 
    } else { //рекурсія не знайшла розв'язку
        if (show_steps) { print_step("Recursive search failed to find a solution."); } 
        find_solution.clear();
    }
    return recursion_was_successful;
}
void dominosa_solve::show_solution() const { //відображення розв'язку
    int current_rows = d_board.get_r();//отримання поточних розмірів дошки.
    int current_cols = d_board.get_c();
    if (current_rows == 0 || current_cols == 0) {
        cout << "Board is empty." << endl; //для порожньої дошки
        return;
    }
    int cell_w = max(1, d_board.calculate_cell_width()); //розрахунок ширини однієї клітинки
    vector<vector<bool>> horizontal_borders(current_rows, vector<bool>(current_cols > 0 ? current_cols - 1 : 0, true)); //ініціалізація меж як видимі
    vector<vector<bool>> vertical_borders(current_rows > 0 ? current_rows - 1 : 0, vector<bool>(current_cols, true));
    for (const auto& current_domino_in_solution : find_solution) { //позначення межі, де розміщено доміно, як невидимі
        int r1 = current_domino_in_solution.first.row;//коорд. першої клітинки доміно.
        int c1 = current_domino_in_solution.first.col;
        int r2 = current_domino_in_solution.second.row;//коорд. другої клітинки доміно.
        int c2 = current_domino_in_solution.second.col;
        if (d_board.is_val(r1, c1) && d_board.is_val(r2, c2)) { //перевірка валідності координат перед доступом до векторів меж
            size_t ur1 = static_cast<size_t>(r1); //перетворення координат до типу size_t для індексації векторів
            size_t uc1 = static_cast<size_t>(c1);
            if (r1 == r2 && abs(c1 - c2) == 1) { //горизонтальне доміно
                int col_of_border = min(c1, c2);  //визначення стовпця, де проходить межа ака менший з двох стовпців клітинок доміно
                size_t ubc_border = static_cast<size_t>(col_of_border);
                if (ur1 < horizontal_borders.size() && ubc_border < horizontal_borders[ur1].size()) { //перевірка індексу межі
                    horizontal_borders[ur1][ubc_border] = false; //стерти горизонтальну межу.
                } else {//відбувається спроба доступу за межі масиву.
                    cerr << "Warning: Out-of-bounds access for horizontal_borders in show_solution." << endl;
                }
            }
            else if (c1 == c2 && abs(r1 - r2) == 1) { //вертикальне доміно
                int row_of_border = min(r1, r2);//визначення рядка, де проходить межа ака менший з двох рядків клітинок доміно
                size_t ubr_border = static_cast<size_t>(row_of_border);
                 if (ubr_border < vertical_borders.size() && uc1 < vertical_borders[ubr_border].size()) { //перевірка індексу за межі вектора
                    vertical_borders[ubr_border][uc1] = false; //стерти вертикальну межу.
                } else { //відбувається спроба доступу за межі масиву
                    cerr << "Warning: Out-of-bounds access for vertical_borders in show_solution." << endl;
                }
            }
             else if (!(r1 == r2 && c1 == c2)) { //доміно не валідне не сусіднє і не однакові клітинки
                cerr << "Warning: Solution in find_solution contains non-adjacent domino: [" << r1 << "," << c1 << "]-[" << r2 << "," << c2 << "]" << endl;
             }
        } else { //координати доміно зі списку розв'язку невалідні
            cerr << "Warning: Solution in find_solution contains invalid coordinates: [" << r1 << "," << c1 << "]-[" << r2 << "," << c2 << "]" << endl;
        }
    }
    cout << " "; //відступ для друку дошки
    for (int j = 0; j < current_cols; ++j) cout << "+" << string(cell_w, '-'); //верхня рамка
    cout << "+" << endl;

    for (int i = 0; i < current_rows; ++i) { //друк дошки ітерація по рядках
        cout << " "; //відступ для рядка
        for (int j = 0; j < current_cols; ++j) { //друк рядка значень ітерація по стовпцях для друку клітинок та вертикальних меж
            size_t ui = static_cast<size_t>(i);
            size_t uj = static_cast<size_t>(j); 
            bool should_draw_left_border = (j == 0) || (j > 0 && ui < horizontal_borders.size() && (uj-1) < horizontal_borders[ui].size() && horizontal_borders[ui][uj-1]); //завжди малюємо для першого стовпця
            cout << (should_draw_left_border ? "|" : " "); //друк межі або пробілу
            string cell_content_str = to_string(d_board.get_val(i, j)); //вирівнювання значення всередині комірки
            int total_padding = cell_w - static_cast<int>(cell_content_str.length()); //розрахунок лівого відступу.
            int left_padding = total_padding / 2; //розрахунок лівого відступу.
            int right_padding = total_padding - left_padding; //розрахунок правого відступу.
            cout << string(left_padding, ' ') << cell_content_str << string(right_padding, ' ');//друк числа з відступами для центрування всередині клітинки
        }
        cout << "|" << endl; //права вертикальна межа для всього рядка
        cout << " "; //друк вертикальних меж між рядками (якщо це не останній рядок)
        for (int j = 0; j < current_cols; ++j) {  //ітерація по стовпцях для друку горизонтальних сегментів меж
            size_t ui = static_cast<size_t>(i); //i як size_t
            size_t uj = static_cast<size_t>(j); //j як size_t
            cout << "+"; //друк перехрестя(для цього використала +)
            bool should_draw_bottom_segment = (i == current_rows - 1) || (ui < vertical_borders.size() && uj < vertical_borders[ui].size() && vertical_borders[ui][uj]); //чи малювати горизонтальний сегмент межі

            cout << string(cell_w, should_draw_bottom_segment ? '-' : ' '); //друк сегмента або пробілів
        }
        cout << "+" << endl; //кутовий символ
    }
}    
