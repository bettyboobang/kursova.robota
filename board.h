//ToDO конструктор який приймає двовимірний вектор з даними поля +
//ToDO костяк методів для кіль-сті рядів, кіль-сті стоіпців, отримання значення клітіни, превірка валідності координат +
//ініціалізує дошку
#ifndef BOARD_H       
#define BOARD_H
#include <vector>     
#include <string>    
#include <stdexcept>  
using namespace std;

class board { //клас що представляє 2D ігрову дошку або сітку цілих чисел
    vector<vector<int>> grid; //для зберігання цілих чисел, що представляють значення на клітинках дошки
    int rows;                             
    int cols;                       
    void print_horizontal_border_helper(int cell_width) const; //ф-ція для друку горизонтальної межі
public: 
    board(const vector<vector<int>>& info); //ініціалізує дошку з вектора
    int get_r() const;// метод кількості рядків 
    int get_c() const;// метод кількості стовпців
    int get_val(int row, int col) const; // метод значення клітикки
    bool is_val(int row, int col) const; // метод перевірки валідності координат
    int calculate_cell_width() const; // для обчислення необхідної ширини комірки для друку
    void print_board() const; // поточ. стан дошки на стандартний вивід з форматуванням
};
#endif