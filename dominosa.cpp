//головна частина
#include "board.h"
#include "solvedominosa.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <iomanip>
#include <stdexcept>
#include <cmath>
#include <utility>
#include <algorithm>
using namespace std;
const int MAIN_MENU_OPTION_ENTER_BOARD = 1;
const int MAIN_MENU_OPTION_EXIT = 2;
bool read_positive_integer(const string &prompt, int &value) { //преревірка бо вводити можна тільки позитивні числа
    string line; //для тимчасового зберігання вводу користувача
    while (true) { //переривається тільки при успішному вводі або критичній помилці
        cout << prompt; //підказка для користувача
        if (!getline(cin, line)) { //зчитування всього рядка, введеного користувачем
            cerr << "\nInput stream error or EOF reached." << endl;
            return false;
        }
        stringstream ss(line); //рядковий поток ss з введеного рядка line для парсингу числа
        if (ss >> value && ss.eof()) { //преревіряю чи вдалося зчитати число чи досягнуто кінця рядка, і чи число позитивне
            if (value > 0) { //чи зчитане число є строго позитивним
                return true;
            } else {
                cerr << "Error: Input must be a strictly positive integer (greater than 0)." << endl;
            }
        } else {
            cerr << "Invalid input. Please enter a single positive integer." << endl;//тут очищати cin не потрібно бо через getline читаємо рядок
        }
    }
}

board read_board_from_user() { //читання дошки від користувача
    int num_rows, num_cols; //для зберігання кількості рядків та стовпців
    if (!read_positive_integer("Enter the number of rows: ", num_rows)) {
        throw runtime_error("Board creation failed: Could not read number of rows.");
    }
    if (!read_positive_integer("Enter the number of columns: ", num_cols)) {
        throw runtime_error("Board creation failed: Could not read number of columns.");
    }

    // Тепер num_rows та num_cols ініціалізовані
    if ((num_rows * num_cols) % 2 != 0) { // парна кіль-сть комірок
        // Додано значення num_rows та num_cols до повідомлення про помилку для ясності
        throw runtime_error("Board creation failed: The total number of cells (rows * cols = " + to_string(num_rows) + " * " + to_string(num_cols) + " = " + to_string(num_rows * num_cols) + ") must be even.");
    }
    vector<vector<int>> grid_values(num_rows, vector<int>(num_cols));//для сберігання числових значень клітинок дошки
    cout << "Enter the board values row by row (non-negative integers, separated by space):" << endl;

    for (int i = 0; i < num_rows; ++i) { //для зчитування значень для кожної клітинки дошки, рядок за рядком
        cout << "Row " << (i + 1) << " (" << num_cols << " values): "; // Підказка для поточного рядка
        string row_input_line; //для зчитування значень одного рядка
        if (!getline(cin, row_input_line)) { //читаю весь рядок, введений користувачем
            throw runtime_error("Board creation failed: Error reading input for row " + to_string(i+1) + " (Input error or EOF).");
        }
        stringstream ss_row_parser(row_input_line); //для парсингу чисел з прочитаного рядка
        for (int j = 0; j < num_cols; ++j) { //ітерація по стовпцях поточного рядка
            int cell_value_input; //для зчитаного значення клітинки тимчавого
            if (!(ss_row_parser >> cell_value_input)) { //читаю по одному числу з рядка
                string error_detail = ss_row_parser.eof() ? "Not enough values provided." : "Invalid non-integer value encountered.";
                throw runtime_error("Board creation failed: Error reading value for cell ("+ to_string(i) + "," + to_string(j) + "). " + error_detail);
            }
            if (cell_value_input < 0) {//чи введене значення клітинки невід'ємне
                throw runtime_error("Board creation failed: Cell value at (" + to_string(i) + "," + to_string(j) +") must be non-negative. You entered: " + to_string(cell_value_input));
            }
            grid_values[i][j] = cell_value_input; //валідне значення у тимчасовий вектор
        }
        char check_for_extra_chars;//перевірка на зайві дані в рядку
        if (ss_row_parser >> check_for_extra_chars) { //якщо вдалося зчитати ще щось
            throw runtime_error("Board creation failed: Too many values entered for row " + to_string(i+1) + ".");
        }
    }
    try { //створюю та повертаю обєкт дошки
        return board(grid_values);
    } catch (const invalid_argument& e) { //виняток від конструктора board
        throw runtime_error(string("Board creation failed in constructor: ") + e.what());
    }
}

void clear_console_screen() {
  cout << "\n============================================================\n" << endl;//це роздільник для гарного виводу прогрмми
}

void pause_execution_for_user(const string& message = "Press 0 and Enter to continue...") {//функція для призупинки коли програма очікує вводу
    cout << message << endl;
    string line_input_pause; //рядок для зчитування вводу
    while (true) {
        if (!getline(cin, line_input_pause)) {
            cerr << "\nInput stream error or EOF. Assuming '0' to continue/exit." << endl;
            break; // Виходимо з циклу при помилці вводу
        }
        // Обрізка пробільних символів з початку та кінця рядка
        // Цей метод обрізки загалом коректний
        size_t first = line_input_pause.find_first_not_of(" \t\n\r\f\v");
        if (string::npos == first) {
            line_input_pause.clear();
        } else {
            size_t last = line_input_pause.find_last_not_of(" \t\n\r\f\v");
            line_input_pause = line_input_pause.substr(first, (last - first + 1));
        }
        // Альтернативний, можливо, простіший спосіб обрізки (якщо перший варіант викликає сумніви):
        // line_input_pause.erase(0, line_input_pause.find_first_not_of(" \t\n\r\f\v"));
        // line_input_pause.erase(line_input_pause.find_last_not_of(" \t\n\r\f\v") + 1);


        if (line_input_pause == "0") { //введено 0 виходим з циклу
            break;
        } else {
          cout << "Invalid input. Please press 0 and Enter." << endl;
        }
    }
}

void print_manual_board_horizontal_line(int num_cols_manual, int cell_width_manual, char corner_char_manual, char line_char_manual) { //ф-ція для друку горизонт.змінної
    cout << corner_char_manual; //друк лівого кутового символу
    for(int j = 0; j < num_cols_manual; ++j) { //для кожного стовпця
        cout << string(cell_width_manual + 2, line_char_manual) << corner_char_manual; //друк сегмента лінії та правого кутового символу
    }
    cout << "\n";
}

void display_board_for_manual_solving(const board& game_board_manual, const vector<vector<bool>>& coverage_grid_manual, const vector<pair<dominosa::coordinate, dominosa::coordinate>>& placed_dominoes_list_manual) {//для відображення дошки в режимі ручного розв'язання
    clear_console_screen();
    cout << "--- Manual Solving Mode ---" << endl;
    int current_rows_manual = game_board_manual.get_r();
    int current_cols_manual = game_board_manual.get_c();
    if (current_rows_manual == 0 || current_cols_manual == 0) {
        cout << "Board is empty." << endl;
        return;
    }
    int cell_w_manual = max(1, game_board_manual.calculate_cell_width()); //розрахунок ширини клітинки
    vector<vector<bool>> horizontal_borders_manual(current_rows_manual, vector<bool>(current_cols_manual > 0 ? current_cols_manual - 1 : 0, true));//ініціалізація матриць для зберігання інформації про межі
    vector<vector<bool>> vertical_borders_manual(current_rows_manual > 0 ? current_rows_manual - 1 : 0, vector<bool>(current_cols_manual, true)); //спочатку всі межі вважаються видимими

    for (const auto& dom_manual : placed_dominoes_list_manual) {//стираю внутрішні межі, які перекриваються вже розміщеними доміно
        int r1 = dom_manual.first.row, c1 = dom_manual.first.col;
        int r2 = dom_manual.second.row, c2 = dom_manual.second.col;
        if (game_board_manual.is_val(r1, c1) && game_board_manual.is_val(r2, c2)) { //перевіряю валідность перед доступом до меж
            size_t ur1 = static_cast<size_t>(r1); size_t uc1 = static_cast<size_t>(c1); // size_t uc2 = static_cast<size_t>(c2);
            if (r1 == r2 && abs(c1 - c2) == 1) { //доміно горизонтальне
                int border_col_idx_manual = min(c1, c2); //визначення стовпця межі
                size_t ubc_manual = static_cast<size_t>(border_col_idx_manual);
                if (ur1 < horizontal_borders_manual.size() && ubc_manual < horizontal_borders_manual[ur1].size()) {//доступ та позначення межі як відсутньої
                    horizontal_borders_manual[ur1][ubc_manual] = false;
                }
            } else if (c1 == c2 && abs(r1 - r2) == 1) { //доміно вертикальне
                int border_row_idx_manual = min(r1, r2); //визначення рядка межі
                 size_t ubr_manual = static_cast<size_t>(border_row_idx_manual);
                if (ubr_manual < vertical_borders_manual.size() && uc1 < vertical_borders_manual[ubr_manual].size()) {
                    vertical_borders_manual[ubr_manual][uc1] = false;
                }
            }
        }
    }
    cout << "   "; //для відступу номера рядка
     for (int j = 0; j < current_cols_manual; ++j) { //вирівнювання номера стовпця
        cout << " " << setw(cell_w_manual) << j << "  "; //друк номера стовпця
    }
    cout << endl;
    cout << "  ";
    print_manual_board_horizontal_line(current_cols_manual, cell_w_manual, '+', '-');
    for (size_t i = 0; i < static_cast<size_t>(current_rows_manual); ++i) { //ітерація по рядках для друку клітинок та горизонтальних меж між ними
        cout << setw(2) << i << " "; //номер рядка
        for (size_t j = 0; j < static_cast<size_t>(current_cols_manual); ++j) { //ітерація по стовпцях поточного рядка
            char current_left_cell_border = '|'; //друк лівої межі для поточної клітинки
            if (j > 0 && i < horizontal_borders_manual.size() && (j-1) < horizontal_borders_manual[i].size() && !horizontal_borders_manual[i][j-1]) {
                 current_left_cell_border = ' ';
            }
            cout << current_left_cell_border;
            string current_cell_value_str;
            if (i < coverage_grid_manual.size() && j < coverage_grid_manual[i].size() && coverage_grid_manual[i][j]) {//якщо клітинка покривається користувачем показую Х
                current_cell_value_str = string(cell_w_manual, 'X'); //X, якщо покрита
                 cout << " " << setfill(' ') << setw(cell_w_manual) << current_cell_value_str << " ";
            } else { //клітинка вільна
                current_cell_value_str = to_string(game_board_manual.get_val(static_cast<int>(i), static_cast<int>(j))); //її значення
                int current_padding_total = cell_w_manual - static_cast<int>(current_cell_value_str.length());//вирівнює хначення весереиі коомірки
                int current_padding_left = current_padding_total / 2;
                int current_padding_right = current_padding_total - current_padding_left;
                cout << string(current_padding_left + 1, ' ') << current_cell_value_str << string(current_padding_right + 1, ' ');
            }
        }
        cout << "|" << endl; //права межа для всього рядка
        cout << "  "; //відступ під рядко
        if (i < vertical_borders_manual.size()) { //чи це не останній ряд
            cout << "+";
            for (size_t j_col_idx = 0; j_col_idx < static_cast<size_t>(current_cols_manual); ++j_col_idx) { //для кожного стовпця // Renamed j to j_col_idx to avoid conflict
                cout << ( (j_col_idx < vertical_borders_manual[i].size() && vertical_borders_manual[i][j_col_idx]) ? string(cell_w_manual + 2, '-') : string(cell_w_manual + 2, ' ') ) << "+";
            }
            cout << "\n";
        } else if (i == static_cast<size_t>(current_rows_manual) - 1 ) { //це останній ряд клітинок, друкуємо нижню межу дошки
            print_manual_board_horizontal_line(current_cols_manual, cell_w_manual, '+', '-');
        }
    }
    cout << endl;
}

void solve_board_manually(const board& game_board_to_solve) {//ф-ція для ручного розвязання
  int current_rows_manual_solve = game_board_to_solve.get_r();//отримання розмірів дошки
  int current_cols_manual_solve = game_board_to_solve.get_c();
   if (current_rows_manual_solve <= 0 || current_cols_manual_solve <= 0) { //перевірка на порожню дошку
        cout << "Cannot solve an empty board manually." << endl;
        pause_execution_for_user();
        return;
    }
    map<pair<int, int>, int> manual_domino_availability;//мапа для відстеження доступності типів доміно
    int min_val_on_board = 0;
    int max_val_on_board = 0;
    bool first_value_processed_for_range = false;
    if (current_rows_manual_solve > 0 && current_cols_manual_solve > 0) { //діапазон значень на дошці для генерації стандартного набору доміно
        min_val_on_board = game_board_to_solve.get_val(0, 0); //перше значення
        max_val_on_board = game_board_to_solve.get_val(0, 0);
        first_value_processed_for_range = true;
        for (int r_idx_range = 0; r_idx_range < current_rows_manual_solve; ++r_idx_range) {
            for (int c_idx_range = 0; c_idx_range < current_cols_manual_solve; ++c_idx_range) {
                int val_current = game_board_to_solve.get_val(r_idx_range, c_idx_range);
                min_val_on_board = min(min_val_on_board, val_current);
                max_val_on_board = max(max_val_on_board, val_current);
            }
        }
    }
    if (!first_value_processed_for_range) { //не вдалося визначити діапазон
        cerr << "Error: Could not determine value range for manual solving (board might be effectively empty)." << endl;
        pause_execution_for_user();
        return;
    }
    for (int a_val = min_val_on_board; a_val <= max_val_on_board; ++a_val) {//генерація очікуваного набору доміно
        for (int b_val = a_val; b_val <= max_val_on_board; ++b_val) {
            manual_domino_availability[dominosa_solve::make_domino_key(a_val, b_val)] = 1;
        }
    }
    int expected_total_dominoes = (current_rows_manual_solve * current_cols_manual_solve) / 2;//очікувана кільсть. доміно для повного покриття дошки
    vector<vector<bool>> user_coverage_grid(current_rows_manual_solve, vector<bool>(current_cols_manual_solve, false)); //ініціалізація стану для ручного розв'язання
    vector<pair<dominosa::coordinate, dominosa::coordinate>> user_placed_solution_list; //список розміщених доміно
    int num_of_covered_cells = 0;
    int num_of_dominoes_placed = 0;
    while (num_of_covered_cells < (current_rows_manual_solve * current_cols_manual_solve)) {//продовжується, доки не всі клітинки покриті
        display_board_for_manual_solving(game_board_to_solve, user_coverage_grid, user_placed_solution_list); //відображення поточного стану
        cout << "Cells covered: " << num_of_covered_cells << " / " << (current_rows_manual_solve * current_cols_manual_solve) << endl;//прогрес
        cout << "Dominoes placed: " << num_of_dominoes_placed << " / " << expected_total_dominoes << endl;
        cout << "Enter coordinates for the two cells of the domino (row1 col1 row2 col2)," << endl;
        cout << "or enter -1 to exit manual mode: ";
        int r1_coord_input = -1, c1_coord_input = -1, r2_coord_input = -1, c2_coord_input = -1; //для координат
        string user_typed_line; //для вводу
        if (!getline(cin, user_typed_line)) { //
            cerr << "\nInput error or EOF during manual solve. Exiting manual mode." << endl;
            break;
        }
        // Обрізка пробільних символів з початку та кінця рядка
        size_t first_trim = user_typed_line.find_first_not_of(" \t\n\r\f\v");
        if (string::npos == first_trim) {
            user_typed_line.clear();
        } else {
            size_t last_trim = user_typed_line.find_last_not_of(" \t\n\r\f\v");
            user_typed_line = user_typed_line.substr(first_trim, (last_trim - first_trim + 1));
        }
        
        if (user_typed_line == "-1") {
            cout << "Exiting manual solve mode." << endl;
            break;
        }
        stringstream ss_parse_coordinates(user_typed_line);//для парсингу рядка
        if (!(ss_parse_coordinates >> r1_coord_input >> c1_coord_input >> r2_coord_input >> c2_coord_input && ss_parse_coordinates.eof())) { //спроба зчитати 4 числа
            cout << "Invalid input format. Please enter exactly four integers or -1." << endl;
            pause_execution_for_user("Press 0 and Enter to try again.");
            continue;
        }
        string validation_error_message_text = "";//валідуємо ходи
        if (!game_board_to_solve.is_val(r1_coord_input, c1_coord_input) || !game_board_to_solve.is_val(r2_coord_input, c2_coord_input)) {//чи координати знаходяться в межах дошки
            validation_error_message_text = "Invalid coordinates: Out of board bounds.";
        } else if (user_coverage_grid[static_cast<size_t>(r1_coord_input)][static_cast<size_t>(c1_coord_input)] || user_coverage_grid[static_cast<size_t>(r2_coord_input)][static_cast<size_t>(c2_coord_input)]) { //чи клітинки вже не покриті. Static_cast для індексів.
            validation_error_message_text = "Invalid move: One or both cells are already covered.";
        } else if (r1_coord_input == r2_coord_input && c1_coord_input == c2_coord_input) { //чи не одна й та ж клітинка
            validation_error_message_text = "Invalid move: Cannot place a domino on a single cell.";
        } else if (!((abs(r1_coord_input - r2_coord_input) == 1 && c1_coord_input == c2_coord_input) || (abs(c1_coord_input - c2_coord_input) == 1 && r1_coord_input == r2_coord_input))) { //перевірка на сусідство клітинок
            validation_error_message_text = "Invalid move: Cells are not adjacent.";
        } else { //попередні перевірки пройдені, перевіряємо доступність типу доміно
            int val_cell1 = game_board_to_solve.get_val(r1_coord_input, c1_coord_input);
            int val_cell2 = game_board_to_solve.get_val(r2_coord_input, c2_coord_input);
            pair<int, int> current_domino_key_to_check = dominosa_solve::make_domino_key(val_cell1, val_cell2); //створення ключа для типу доміно
            auto it_manual_check = manual_domino_availability.find(current_domino_key_to_check);
            if (it_manual_check == manual_domino_availability.end()) { //якщо такого типу доміно немає в згенерованому наборі
                validation_error_message_text = "Error: Domino type {" + to_string(val_cell1) + "," + to_string(val_cell2) + "} is not part of the standard set for this board's number range.";
            } else if (it_manual_check->second <= 0) { // лічильник цього типу доміно 0 або меньше ака воно використнане
                validation_error_message_text = "Invalid move: Domino type {" + to_string(val_cell1) + "," + to_string(val_cell2) + "} has already been used.";
            }
        }
        if (!validation_error_message_text.empty()) { //повідомлення про помилку валідації
            cout << validation_error_message_text << endl;
            pause_execution_for_user("Press 0 and Enter to try again.");
        } else { //хід валідний
            user_coverage_grid[static_cast<size_t>(r1_coord_input)][static_cast<size_t>(c1_coord_input)] = true; //позначити клітинки як покриті. Static_cast для індексів.
            user_coverage_grid[static_cast<size_t>(r2_coord_input)][static_cast<size_t>(c2_coord_input)] = true;
            user_placed_solution_list.push_back({{r1_coord_input, c1_coord_input}, {r2_coord_input, c2_coord_input}}); //додати доміно до списку розв'язку
            num_of_covered_cells += 2; //лічильник покритих клітинок збільшуємо
            int val_placed1 = game_board_to_solve.get_val(r1_coord_input, c1_coord_input);//оновлення лічильника доступності використаного типу доміно
            int val_placed2 = game_board_to_solve.get_val(r2_coord_input, c2_coord_input);
            pair<int, int> key_of_placed_domino = dominosa_solve::make_domino_key(val_placed1, val_placed2);
            manual_domino_availability[key_of_placed_domino]--; //зменшити лічильник доступності цього типу
            num_of_dominoes_placed++; //збільшити лічильник розміщених доміно
            cout << "Placed domino {" << val_placed1 << "," << val_placed2 << "} covering cells (" << r1_coord_input << "," << c1_coord_input << ") and (" << r2_coord_input << "," << c2_coord_input << ")." << endl;
             // Невеличка пауза, щоб користувач побачив повідомлення перед оновленням дошки
            if (num_of_covered_cells < (current_rows_manual_solve * current_cols_manual_solve)) { // Якщо гра не закінчена
                 pause_execution_for_user("Press 0 and Enter for next move.");
            }
        }
    }
    clear_console_screen();
    cout << "--- Manual Solving Finished ---" << endl;
    display_board_for_manual_solving(game_board_to_solve, user_coverage_grid, user_placed_solution_list);
    if (num_of_covered_cells == (current_rows_manual_solve * current_cols_manual_solve)) { //всі клітинки покрито
        cout << "Congratulations! You have covered all cells." << endl; // Removed "You're a genius :)" to avoid repetition if all checks pass
        if (num_of_dominoes_placed != expected_total_dominoes) {
            cout << "Warning: The number of placed dominoes (" << num_of_dominoes_placed << ") does not match the expected number for a full solution (" << expected_total_dominoes<< ")." << endl;
        }
        bool no_overused_dominoes_found = true;
        for(const auto& availability_entry : manual_domino_availability) {
            if (availability_entry.second < 0) { //використано більше 1 разу
                no_overused_dominoes_found = false;
                cerr << "Error: Domino type {" << availability_entry.first.first << "," << availability_entry.first.second << "} was used more than allowed!" << endl;
            }
        }
        if (no_overused_dominoes_found && num_of_dominoes_placed == expected_total_dominoes) { //немає перебору і кількість правильна
            cout << "You're a genius :)" << endl;
        } else { // є проблеми з унікальністю або кількістю
            cout << "However, there might be an issue with domino uniqueness or count." << endl;
        }

    } else { //якщо користувач вийшов з ручного режиму до повного покриття дошки
        cout << "Manual solving exited before covering all cells." << endl;
        cout << "Cells covered: " << num_of_covered_cells << " / " << (current_rows_manual_solve * current_cols_manual_solve) << endl;
    }
    pause_execution_for_user("Press 0 and Enter to return to the main menu.");
}
int main() { //меню программи
    while (true) {
        clear_console_screen();
        cout << "===== Welcome to Dominosa puzzle game! =====" << endl;
        cout << 1 << ". Enter board and Solve" << endl;
        cout << 2 << ". Exit" << endl;
        cout << "Choose your option (1 or 2): ";
        int main_menu_user_choice = 0;
        string main_menu_input_line;
        if (!getline(cin, main_menu_input_line)) { //преевірка введення числа (зчитування рядка)
             cerr << "\nInput error or EOF. Exiting." << endl;
             break;
        }
        stringstream main_menu_ss_parser(main_menu_input_line);//для парсингу
        if (!(main_menu_ss_parser >> main_menu_user_choice && main_menu_ss_parser.eof())) {
            cout << "Invalid input. Please enter only the number 1 or 2." << endl;
            pause_execution_for_user();
            continue; //повертаю коритувача на посаток цикл
        }

        if (main_menu_user_choice == MAIN_MENU_OPTION_EXIT) {//користувач виходить з головного циклу(меню програми)
            cout << "Exiting program. Goodbye!" << endl;
            break;
        }
        else if (main_menu_user_choice == MAIN_MENU_OPTION_ENTER_BOARD) {//спроба завантадити дошку
            try {
                clear_console_screen();
                cout << "--- Enter Board Data ---" << endl;
                board game_board_instance = read_board_from_user(); //зчитування та створення дошки
                bool should_return_to_main_menu = false; //якщо дошка завантажилась успішно, перекидужмось в меню розвязання
                while (!should_return_to_main_menu) { //меню розвязання
                    clear_console_screen();
                    cout << "--- Board Loaded Successfully ---" << endl;
                    game_board_instance.print_board(); //виводимо дошку на екран (поточний стан дошки)
                    cout << "\n--- Choose Action ---" << endl;
                    cout << 1 << ". Solve automatically (step-by-step output)" << endl;
                    cout << 2 << ". Solve manually" << endl;
                    cout << 3 << ". Enter a new board (return to main menu)" << endl;
                    cout << "Choose an option (1, 2, or 3): ";
                    int action_choice_in_solve_menu = 0;
                    string solve_menu_input_line;
                     if (!getline(cin, solve_menu_input_line)) { //перевірка введення
                        cerr << "\nInput error or EOF. Returning to main menu." << endl;
                        should_return_to_main_menu = true;
                        continue;
                     }
                    stringstream solve_menu_ss_parser(solve_menu_input_line); //для парсингу
                    if (!(solve_menu_ss_parser >> action_choice_in_solve_menu && solve_menu_ss_parser.eof())) {
                        cout << "Invalid input. Please enter only the number 1, 2, or 3." << endl;
                        pause_execution_for_user();
                        continue;
                    }
                    switch (action_choice_in_solve_menu) {
                        case 1: {//тут створюємо розв'язувач і починаємо розв'язання
                                clear_console_screen();
                                cout << "Starting automatic solver (step-by-step)..." << endl;
                                game_board_instance.print_board();
                                pause_execution_for_user("\nPress 0 and Enter to begin solving.");
                                dominosa_solve solver_object(game_board_instance, true);//створення об'єкта розв'язувача
                                bool was_solved_successfully = solver_object.solve();
                                clear_console_screen(); //очищаємо екран перед фін.результатом
                                cout << "--- Automatic Solver Finished ---" << endl;
                                if (was_solved_successfully) {
                                    cout << "Yay, a solution was found!" << endl;
                                    solver_object.show_solution(); //показую фін.розвязок
                                } else {
                                    cout << "No solution exists for this board, or an inconsistency was detected :(" << endl;
                                    cout << "\nLast state reached (if any steps were taken):" << endl;//якщо було якесь часткове рішення, програма його покаже
                                    solver_object.show_solution(); //показ поточного стану (може бути порожнім або частковим)
                                }
                                pause_execution_for_user("\nPress 0 and Enter to return to the solving menu.");
                                break;
                            }
                        case 2:
                            solve_board_manually(game_board_instance);
                            break;
                        case 3:
                            cout << "Returning to main menu to enter a new board..." << endl;
                            should_return_to_main_menu = true;
                            pause_execution_for_user();
                            break;
                        default: {
                            cout << "Invalid choice. Please enter 1, 2, or 3." << endl;
                            pause_execution_for_user();
                            break;
                        }
                    }
                }
            } catch (const exception &e) {//килаємо ексепшон
                clear_console_screen();
                cerr << "\nError encountered: " << e.what() << endl;
                cerr << "Please check your input or board data." << endl;
                pause_execution_for_user("Press 0 and Enter to return to main menu.");
            }
        } else {//не 1 і не 2
            cout << "Invalid choice. Please enter 1 or 2." << endl;
            pause_execution_for_user();
        }
    }
    return 0;
}