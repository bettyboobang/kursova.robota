//ToDO ініціалізувати розвязувач з преданою дошкою +
//ToDO ініціалізувати вектор для відстеження які клітинки вже покриті доміно +
//ToDO метод для вибору наступної непокритої клітини для доміно +
//ToDO Розвязати головоломку через рекурсію +
//ToDO Вивід головоломки на консоль +
#include "solverdominosa.h"
#include <iostream>
#include <algorithm>
#include <limits>
using namespace std;

super_solve::super_solve(const board& b) : board(b) {//розвязувач з дошкою якак передається
	num_max = 0; //тут ми знаходимо максимальне число на нашій дошці для визначення набору доміно
	for(int i = 0; i < board.get_row(); ++i) {
		for(int j = 0; c < board.get_col(); ++j) {
			if(board.get_value(i, j) > num_max) {
				num_max = board.get_value(i, j);
			}
		}
	}
    covered.assign(board.get_row(), vector<bool>(board.get_col(), false));//ініціалізація для відстеження клітінок що вже покриті доміно
    init_dominosa();
} 
void super_solve::init_dominosa() {//ініціалізація набору всіх можливих доміно
	for(int p = 0; p <= num_max; ++p) {
		for(int q = 0; q <= num_max; ++q) {
			domino_available[{p, q}] = true;
		}
	}
}
pair<int, int> super_solve:: grab_next_cell() {//метод для вибору невикористаної клітинки для розміщення домвно
	int best_row = -1, best_col = -1;
	int minimum_option = numeric_limits<int>::max();
	for(int i = 0; i < board.get_row(); ++i) {//перебираємо всі клітинки
		for(int j = 0; j < board.get_col(); ++j) {
		   if(!covered[i][j]) {//тут якщо в клітинці ще нічого немає
		   	int option = 0;
		   	int value_1 = board.get_value(i, j);
		   	if(board.is_valid(i + 1, j) && !covered[i +1][j]) {// чи можливк розміщення горизонтального доміно
		   		int value_2 = board.get_value(i + 1, j);
		   		if(domino_available.count(minmax(value_1, value_2))) {
		   			option++;
		   		}
		   	}
		   	if (board.is_valid(i + 1, j) && !covered[i + 1][j]) {// а тут чи можливе розміщення вертикального доміно
                    int value_2 = board.get_value(i + 1, j);
                    if ( domino_available.count(minmax(value_1, value_2))) {
                        option++;
                    }
                }
		    if(option < minimum_option) {//обриння клітинки з наймпенщою кількістю можливих варіантів розміщення
		    	minimum_options = option;
		    	best_row = i;
		    	best_col = j;
		    } else if(option == minimum_option && best_row == -1) {
		    	best_row = i;
		    	best_col = j;
		    }
		}
	}
	return {best_row, best_col};// повертання координат самої крутої наступної клітини
}
bool super_solve::super_solve_recurse() {//головоломку розвязуєм серез рекурсію
	pair<int, int> next_cell = grab_next_cell();// тут ми отримуємо координати наступної вільної клітини
	int i = next_cell.first;
	int j = next_cell.second;
	if(i == -1) {//розвязок знайдено якщо всі клітинки покриті
		return true;
	}
	int value_1 = board.get_value(i, j);//спроба розміщення горизонталного доміно
	if(board.is_valid(i, c + 1) && covered[i][j + 1]) {
		int value_2 = board.get_value(i, j + 1);
		pair<int, int> dominosa = minmax(value_1, value_2);
		if(domino_available[dominosa]) {//перевіряємо доміно на доступність
			covered[i][j] = true;
			covered[i][j + 1] = true;
			domino_available[dominosa] = false;
			solution.push_back({{i, j}, {i, j + 1}});
			if (super_solve_recurse()) {
				return true;
			}
			solution.pop_back();//скасування поточного розміщення
			domino_available[dominosa] = true;
			covered[i][j] = false;
			covered[i][j + 1] = false;
		}
	}
	if(board.is_valid(i + 1, j) && !covered[i + 1][j]) {//спроба розміщення вертикального доміно
      int value_2 = board.get_value(i + 1, j);
      pair<int, int> dominosa = minmax(value_1, value_2);
        if(domino_available[dominosa]) {//перевірка доступності доміно
      	   covered[i][j] = true;
      	   covered[i + 1][j] = true;
      	   domino_available[dominosa] = false;
      	   solution.push_back({{i, j}, {i + 1, c}});
      	    if(super_solve_recurse()) {//продовження розвзання
      		    return true;
      	    }
      	    solution.pop_back();//якщо рекурсія не вдалась поточне розміщення скасовується
      	    domino_available[dominosa] = true;
      	    covered[i][j] = false;
      	    covered[i + 1][j] = false;
        }
	}
    return false;// якщо розмістити доміно не вдалося
}
bool super_solve::solve() {
	return super_solve_recurse();
}
void super_solve::print_solve() const {//це метод для виведення в консоль
	int row = board.get_row();
	int col = board.get_col();
	vector<vector<bool>> horizontal_dominosa(row, vector<bool>(col - 1, false));//потрібно длярозміщення горизонтальних та вертикальних доміно
	vector<vector<bool>> vertical_dominosa(row - 1, vector<bool>(col, false));
	for(const auto& dominosa : solution) {// заповнення промвжних сіток(??) на основі знайденоого розвязку
		int row1 = dominosa.first.row;
		int col1 = dominosa.second.col;
		int row2 = dominosa.first.row;
		int col2 = dominosa.second.col;
		if(row1 == row2) {// горизонтальне доміно тоді коли координати рядків однакові, інакше доміно горизонтальне
			horizontal_dominosa[row1][min(col1, col2)] = true;
		} else {
			vertical_dominosa[min(row1, row2)][col1] = true;
		}
	}
	for(int a = 0; a < row; ++a) {
		for(int b = 0; b < col;++b) {
			cout << board.get_value(a, b);//для виводу числа в поточній клітинці
		    if(b < col - 1) {
		    	if(horizontal_dominosa[a][b]) {
		    		cout << "-";
		    	} else {
		    		cout << " ";
		    	}
		    }
		}
		cout << endl;
		if(a < row - 1) {
			for(int b = 0; b < col; ++b) {
				if(vertical_dominosa[a][b]) {
				  cout << "|";
				} else {
					cout << " ";
				}
				if(b < col - 1) {
					cout << " ";//пробіл між вкртикальними доміно
				}
			}
			cout << endl;
		}
	}
}