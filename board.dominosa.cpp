//ToDO конструктор правильно ініціалізуючий конструктори поля +
//ToDO реалізація методів класу борд +
#include "board.h"
#include <iostream>
using namespace std;

board::board(const std::vector<std::vector<int>> &info) : grid(info) {
    rows = grid.size(); //встановила кількість рядків на основі розміру вектора
    if(rows > 0) {
        cols = grid[0].size();
    } else {
        cols = 0; //кількість стовпців якщо є хоча б 1 рядок, шнший випадк 0
    }
}
int board::get_r() const {//метод кількості рядків 
    return rows;
}
int board::get_c() const {//метод кількості стовпців
    return cols;
}
int board::get_val(int row, int col) const {//метод значення клітикки
    return grid[row][col]; //зазаданими індексами пвертає заяення з вектора
}
bool board::is_val(int row, int col) const {//метод перевірки валідності координат
    return row >= 0 && row < rows && col >= 0 && col < cols;
    //воно поверне тру якщо координати знаходяться в межах поля, іначе фолз
}
