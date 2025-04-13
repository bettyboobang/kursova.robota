//ToDO перевірка правильності обробки аргументів 
//ToDO створення вектора для збереження даних ігр.поля
//ToDO створити строковий потік для розбору рядка
//ToDO створити розумний вказівник
//ToDO робота з файлом
//main частина

#include "dominosa.interface.h"
#include "board.h"
#include "solvedominosa.h"
#include <iostream>
#include <fstream>//для роботи з файлом
#include <sstream>//для строчкового потоку
#include <vector>
#include <memory> //для розумних вказівників
using namespace std;

int main() {
	if(args != 2) {
		cerr << "Usage: " << argv[0] << " <input_file> " << endl;
		return 1;
	}
	string filename = argv[1];
	ifstream inputFile(filename);
	 if (!inputFile.is_open()) {
        cerr << "Could not open file: " << filename <<endl;
        return 1;
    }
    vector<vector<int>> grid;//вектор доя даних ігрового поля
    string line;// потрібна дшя зберігання кодного рядка з файлу
    while(getline(inputFile, line)) {
    	vector<int> row;// ветор для зберігання чиел у рядку
    	stringstream ss(line);//стоковий потік для розбору рядка
    	int value;//інт для зберігання кодного числа з рядка
    	while(ss >> value) {
    		row.push_back(value);
    	}
    	if(!row.empty()) {
    		grid.push_back(row);
    	}
    }
    inputFile.close();

    if(grid.empty()) {//перевірка чи зчитані дані з поля 
    	cerr << "Input grid is empty." << endl;
        return 1; 
    }
    board board(grid);//створюється розумний вкказівник на обєкт классу солвер використовуючи інтерфейс 
    unique_ptr <dominosa_interface> super_solver = make_unique <dominosa_interface>(board);
    if(super_solver -> super_solve()) {//виклик методу розвязання головоломки
    	super_solver -> show_solution();
    } else {
    	cout << "No solution found." << endl;
    }
    return 0;
}