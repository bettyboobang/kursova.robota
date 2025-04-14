//ToDO ініціалізувати розвязувач з преданою дошкою +
//ToDO ініціалізувати вектор для відстеження які клітинки вже покриті доміно +
//ToDO метод для вибору наступної непокритої клітини для доміно +
//ToDO Розвязати головоломку через рекурсію +
//ToDO Вивід головоломки на консоль +
#include "solvedominosa.h"
#include <iostream>
#include <algorithm>
#include <limits>
using namespace std;

dominosa_solve::dominosa_solve(const board& b) : d_board(b) {//розвязувач з дошкою якак передається
    num_max = 0; //тут ми знаходимо максимальне число на нашій дошці для визначення набору доміно
    for (int i = 0; i < d_board.get_r(); ++i) {
        for (int j = 0; j < d_board.get_c(); ++j) {
            if (d_board.get_val(i, j) > num_max) {
                num_max = d_board.get_val(i, j); 
            }
        }
    }
    is_cover.assign(d_board.get_r(), vector<bool>(d_board.get_c(), false)); // 🛠️ Виправлено board -> d_board
    init_dominosa(); 
}
void dominosa_solve::init_dominosa() {//ініціалізація набору всіх можливих доміно
    for (int p = 0; p <= num_max; ++p) {
        for (int q = 0; q <= num_max; ++q) {
            domino_available[{p, q}] = true;
        }
    }
}

pair<int, int> dominosa_solve::grab_next_cell() {//метод для вибору невикористаної клітинки для розміщення домвно
    int best_row = -1, best_col = -1;
    int minimum_option = numeric_limits<int>::max();
    for (int i = 0; i < d_board.get_r(); ++i) {//перебираємо всі клітинки
        for (int j = 0; j < d_board.get_c(); ++j) {
            if (!is_cover[i][j]) {//тут якщо в клітинці ще нічого немає
                int option = 0;
                int value_1 = d_board.get_val(i, j);
                if (d_board.is_val(i + 1, j) && !is_cover[i + 1][j]) {//чи можливе розміщення вертикального доміно
                    int value_2 = d_board.get_val(i + 1, j);
                   if (domino_available.count(minmax(value_1, value_2))) {
                        option++;
                    }
                }
                if (d_board.is_val(i, j + 1) && !is_cover[i][j + 1]) {//чи можливе розміщення горизонтального доміно
                    int value_2 = d_board.get_val(i, j + 1);
                    if (domino_available.count(minmax(value_1, value_2))) {
                        option++;
                    }
                }
                if (option < minimum_option) {//обрання клітинки з найменшою кількістю можливих варіантів розміщення
                    minimum_option = option;
                    best_row = i;
                    best_col = j;
                } else if (option == minimum_option && best_row == -1) {
                    best_row = i;
                    best_col = j;
                }
            }
        }
    }
    return {best_row, best_col};// повертання координат самої крутої наступної клітини
}

bool dominosa_solve::super_solve_recurse() {//головоломку розвязуєм через рекурсію
    pair<int, int> next_cell = grab_next_cell();// тут ми отримуємо координати наступної вільної клітини
    int i = next_cell.first;
    int j = next_cell.second;
    if (i == -1) {//розвязок знайдено якщо всі клітинки покриті
        return true;
    }

    int value_1 = d_board.get_val(i, j);//спроба розміщення горизонтального доміно
    if (d_board.is_val(i, j + 1) && !is_cover[i][j + 1]) {
        int value_2 = d_board.get_val(i, j + 1);
        pair<int, int> dominosa = minmax(value_1, value_2);
        if(domino_available[dominosa]) {//перевіряємо доміно на доступність
            is_cover[i][j] = true;
            is_cover[i][j + 1] = true;
            domino_available[dominosa] = false;
            find_solution.push_back({{i, j}, {i, j + 1}});
            if (super_solve_recurse()) {
                return true;
            }
            find_solution.pop_back();//скасування поточного розміщення
            domino_available[dominosa] = true;
            is_cover[i][j] = false;
            is_cover[i][j + 1] = false;
        }
    }

    if (d_board.is_val(i + 1, j) && !is_cover[i + 1][j]) {//спроба розміщення вертикального доміно
        int value_2 = d_board.get_val(i + 1, j);
        pair<int, int> dominosa = minmax(value_1, value_2);
        if (domino_available[dominosa]) {//перевірка доступності доміно
            is_cover[i][j] = true;
            is_cover[i + 1][j] = true;
            domino_available[dominosa] = false;
            find_solution.push_back({{i, j}, {i + 1, j}});
            if (super_solve_recurse()) {//продовження розв’язання
                return true;
            }
            find_solution.pop_back();//якщо рекурсія не вдалась поточне розміщення скасовується
            domino_available[dominosa] = true;
            is_cover[i][j] = false;
            is_cover[i + 1][j] = false;
        }
    }
    return false;// якщо розмістити доміно не вдалося
}

bool dominosa_solve::solve() {
    return super_solve_recurse();
}

void dominosa_solve::show_solution() const {//це метод для виведення в консоль
    int row = d_board.get_r();
    int col = d_board.get_c();
    vector<vector<bool>> horizontal_dominosa(row, vector<bool>(col - 1, false));//потрібно для розміщення горизонтальних та вертикальних доміно
    vector<vector<bool>> vertical_dominosa(row - 1, vector<bool>(col, false));

    for (const auto& dom : find_solution) {// заповнення проміжних сіток на основі знайденого розв’язку
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

    for (int a = 0; a < row; ++a) {
        for (int b = 0; b < col; ++b) {
            cout << d_board.get_val(a, b);//для виводу числа в поточній клітинці
            if (b < col - 1) {
                if (horizontal_dominosa[a][b]) {
                    cout << "-";
                } else {
                    cout << " ";
                }
            }
        }
        cout << endl;
        if (a < row - 1) {
            for (int b = 0; b < col; ++b) {
                if (vertical_dominosa[a][b]) {
                    cout << "|";
                } else {
                    cout << " ";
                }
                if (b < col - 1) {
                    cout << " ";//пробіл між вертикальними доміно
                }
            }
            cout << endl;
        }
    }
}
