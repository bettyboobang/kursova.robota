//ToDO конструктор який приймає двовимірний вектор з даними поля +
//ToDO костяк методів для кіль-сті рядів, кіль-сті стоіпців, отримання значення клітіни, превірка валідності координат +
//ініціалізує дошку
#ifndef BOARD_H
#define BOARD_H
#include <vector>
using namespace std;

class board {
    vector<vector<int>> grid;
    int rows;
    int cols;
public:
    board(const std::vector<vector<int>> &info); // конструктор з Todo списку
    int get_r() const;
    int get_c() const;
    int get_val(int row, int col) const;
    bool is_val(int row, int col) const; 
};
#endif