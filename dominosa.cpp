//ToDO перевірка правильності обробки аргументів
//ToDO створення вектора для збереження даних ігр.поля
//ToDO створити розумний вказівник
//ToDO перероюити пррграму з вводом користувача
//вирішила доддати функцію та бібліотеку для показу за скільки часу головоломка розвязлась
//додоати перевірку вводу
//main частина
#include "board.h"
#include "solvedominosa.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <chrono>
using namespace std;

bool read_posint(const string &prompt, int &value) {//преревірка бо вводити можна тільки позитивні числа
    string line;
    while (true) {
        cout << prompt;
        if (!getline(cin, line)) return false;
        stringstream ss(line);
        if (ss >> value && ss.eof() && value > 0) {
            return true;
        }
        cout << "Invalid input, please enter a positive integer." << endl;
    }
}
bool read_int(const string &prompt, int &value) {//прервірка чи ввів коримтувач ціле число
    string line;
    while (true) {
        if (!prompt.empty()) cout << prompt;
        if (!getline(cin, line)) return false;
        stringstream ss(line);
        if (ss >> value && ss.eof()) {
            return true;
        }
        cout << "Invalid input, please enter an integer." << endl;
    }
}
board read_board() {
    int rows, cols;
    read_posint("Enter the number of rows: ", rows);
    read_posint("Enter the number of columns: ", cols);
    if ((rows * cols) % 2 != 0) { // парна кіль-сть комірок
        cerr << "Invalid board dimensions: The number of cells must be even." << endl;
        exit(1);
    }
    vector<vector<int>> grid(rows, vector<int>(cols));
    cout << "Enter the board values row by row (each value separated by space):" << endl;
    for (int i = 0; i < rows; ++i) {
        cout << "Row " << (i + 1) << ": ";
        for (int j = 0; j < cols; ++j) {
            read_int("", grid[i][j]);
        }
    }
    return board(grid);
}

int main() {
    try {
        board game_board = read_board();
        cout << "\n";
        game_board.print_board();  // гарний вивід початкового поля

        auto start_time = chrono::high_resolution_clock::now(); // тут створюємо розв'язувач і починаємо розв'язання
        dominosa_solve solver(game_board);
        bool solved = solver.solve();
        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
        cout << "\nTime taken: " << duration << " milliseconds" << endl;

        if (solved) {
            cout << "\nSolution found!" << endl;
            solver.show_solution();
        } else {
            cout << "\nNo solution exists for this board." << endl;
        }

    } catch (const exception &e) {//килаємо ексепшон
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
