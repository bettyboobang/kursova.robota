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
//метод кількості рядків 
int board::get_r() const {
	return rows;
}
//метод кількості стовпців
int board::get_c() const {
	return cols;
}
//метод значення клітикки
int board::get_val(int row, int col) const {
	return grid[row][col]; //зазаданими індексами пвертає заяення з вектора
}
//метод перевірки валідності координат
bool board::is_val(int row, int col) const {
	return row >= 0 && row < rows && col >= 0 && col < cols;
	//воно поверне тру якщо координати знаходяться в межах поля, іначе фолз
}
