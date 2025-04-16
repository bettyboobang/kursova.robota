//ToDO перевірка правильності обробки аргументів
//ToDO створення вектора для збереження даних ігр.поля
//ToDO створити розумний вказівник
//ToDO перероюити пррграму з вводом користувача
//main частина
#include "dominosa.interface.h"
#include "board.h"
#include "solvedominosa.h"
#include <iostream>
#include <vector>
#include <memory> //для розумних вказівників
#include <sstream>
using namespace std;

int main() {
    int rows, cols;
    cout << "Enter the number of rows: ";
    cin >> rows;
    cout << "Enter the number of columns: ";
    cin >> cols;
    if (rows <= 0 || cols <= 0) {
        cerr << "Invalid board dimensions." << endl;
        return 1;
    }
    vector<vector<int>> grid(rows, vector<int>(cols));
    cout << "Enter the board values row by row:" << endl;
    for (int i = 0; i < rows; ++i) {
        cout << "Row " << i + 1 << ": ";
        for (int j = 0; j < cols; ++j) {
            cin >> grid[i][j];
        }
    }
    board gameBoard(grid);
    cout << "Board created with " << gameBoard.get_r() << " rows and " << gameBoard.get_c() << " columns." << endl;
    unique_ptr<dominosa_interface> super_solver = make_unique<dominosa_solve>(gameBoard);
    cout << "Attempting to solve..." << endl;
    bool solved = super_solver->solve();
    cout << "Solution found: " << (solved ? "Yes" : "No") << endl;
    if (solved) {
        cout << "Showing solution:" << endl;
        super_solver->show_solution();
    } else {
        cout << "No solution found." << endl;
    }
    return 0;
}