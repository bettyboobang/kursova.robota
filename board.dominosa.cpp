//ToDO конструктор правильно ініціалізуючий конструктори поля +
//ToDO реалізація методів класу борд +
#include "board.h"    
#include <vector>    
#include <string>      
#include <iostream>    
#include <iomanip>     
#include <algorithm>  
#include <stdexcept>   
using namespace std;   

board::board(const vector<vector<int>> &info) : grid(info) { //приймає  посилання на вектор для ініціалізації дошки
    rows = grid.size(); //встановила кількість рядків на основі розміру зовнішнього вектора grid
    if (rows > 0) { //є хоча б один рядок
        cols = grid[0].size(); //кількість стовпців якщо є хоча б 1 рядок
        for (size_t i = 1; i < grid.size(); ++i) {//чи всі рядки мають однакову довжину
            if (grid[i].size() != static_cast<size_t>(cols)) { //порівнюємо довжину поточного рядка з довжиною стовпців
                throw invalid_argument("Board initialization failed: Input grid contains rows with different lengths (must be rectangular).");
            }
        }    
    } else { //рядків немає
        cols = 0; 
    }
}
int board::get_r() const { //гетер для кількості рядків
  return rows;
}
int board::get_c() const {//гетер для кількості стовпців
  return cols; 
}
int board::get_val(int row, int col) const { //за заданими індексами повертає заяення з вектора для отримання значення з кліт.
    if (!is_val(row, col)) { //перевірка меж для безпеки, викликаємо публічний метод is_val() цього ж класу
        throw out_of_range("Accessing board cell out of bounds in get_val. Row: " + to_string(row) + ", Col: " + to_string(col) + ", Bounds: " + to_string(rows) + "x" + to_string(cols));
    }
    return grid[row][col]; //координати валідні, повертаємо значення з відповідної клітинки 
} 
bool board::is_val(int row, int col) const {//воно поверне тру якщо координати знаходяться в межах поля, іначе фолз
  return row >= 0 && row < rows && col >= 0 && col < cols;
} 
int board::calculate_cell_width() const { //для обчислення необхідної ширини комірки для друку(максимальне значення)
    if (rows == 0 || cols == 0) {
        return 1; //мін. ширина для порожньої дошки
    }
    int max_val_on_board = 0;//для зберігання максимального значення, знайденого на дошці
    bool first_value_has_been_processed = false; //для коректної ініціалізації max_val_on_board 
    for (int i = 0; i < rows; ++i) {  //ітерація по всіх клітинках дошки для знаходження максимального значення
        for (int j = 0; j < cols; ++j) {
            int current_cell_value = grid[i][j]; //отримання значення поточної клітинки
            if (!first_value_has_been_processed) { //це перше значення, яке ми обробляємо
                max_val_on_board = current_cell_value;   
                first_value_has_been_processed = true;  //перше значення оброблено
            } else { 
                max_val_on_board = max(max_val_on_board, current_cell_value);//якщо поточне значення більше за поточний максимум
            }
        }
    }
    int max_value_length = to_string(max_val_on_board).length();//максимальне значення на рядок, щоб отримати його довжину в символах
    return max(1, max_value_length); //повертаємо ширину,не менше 1
}
void board::print_horizontal_border_helper(int cell_width) const {//для друку горизрнт.лінії
    cout << " +"; //це відступ для номерів рядків (або просто для вирівнювання ліній з клітинками)
    for (int j = 0; j < cols; ++j) {
        cout << string(cell_width + 2, '-') << "+"; //ширина комірки + 2 пробіли для вирівнювання з вмістом клітинок
    }
    cout << endl;
}
void board::print_board() const {//для друку дошки
    if (rows == 0 || cols == 0) {
        cout << "Board is empty." << endl;
        return; // Вихід з методу
    }
    int cell_display_width = calculate_cell_width();//обчислення оптимальної ширини клітинки для поточного стану дошки
    cout << "Current Board State (" << rows << "x" << cols << "):" << endl; //поточ. стан дошки на стандартний вивід з форматуванням ( типу рамки, вирівнювання)
    cout << "   "; //друк номерів стовпців над верхньою межею дошки.
    for (int j = 0; j < cols; ++j) { //ітерація по стовпцях
         cout << " " << setw(cell_display_width) << j << "  ";
    }
     cout << endl; 
    print_horizontal_border_helper(cell_display_width); //це верхня рамка
    for (int i = 0; i < rows; ++i) {  //ітерація по кожному рядку дошки для друку вмісту клітинок та горизонтальних меж
        cout << setw(2) << i << "|";//номер рядка (вирівняний праворуч, ширина 2)
        for (int j = 0; j < cols; ++j) {//це значення комірки було вирівняне праворуч всередині комірки
            cout << " " << setw(cell_display_width) << grid[i][j] << " |";//вткористовую setw для вирівнювання
        }
        cout << endl; 
        print_horizontal_border_helper(cell_display_width); //лінія що розділяє між рядками після останнього рядка
    }
}