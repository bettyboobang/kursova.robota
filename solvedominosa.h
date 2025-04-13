#ifndef D_SOLVE_H
#define D_SOLVE_H
#include "dominosa.interface.h"
#include "board.h"
#include <iostream>
#include <vector>
#include <map>
#include <utility> //для пар тобто обєктів що можуть тримати в собі два різних типи значень
using namespace std;

namespace dominosa {//тут я представляю координати на полі
   struct coordinate {
   	int row;
   	int col;
   	coordinate(int row, int col) : row(row), col(col) {}   	
   };
}
class dominosa_solve : public dominosa_interface {
     board d_board;
     int num_max;
     map <pair<int, int>, bool> domino_avaliable;//для відтеження доступних доміно
     vector <pair<dominosa::coordinate, dominosa::coordinate>> find_solution;//зберігає знайдений розвязок ака координати доміно
     vector <vector<bool>> is_cover;//а це для відстеження покритих кліт.поля
     
     void init_dominosa(); //для ініціалізаціх набору всіх доміно з можливих
     void super_solve(); // для розвязання головолом.
     pair<int, int> grab_next_cell(); //непокрита клітинка для спроби розмістити доміно
public:
	dominosa_solve(const board &b);
	bool solve() override;//перевизначення компілятор перепірить чи є з такою ж назіою віртуальна фукнція
	void show_solution() const override;
};

#endif