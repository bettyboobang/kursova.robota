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
     map<pair<int, int>, int> domino_available;//для відтеження доступних доміно
     vector <pair<dominosa::coordinate, dominosa::coordinate>> find_solution;//зберігає знайдений розвязок ака координати доміно
     vector <vector<bool>> is_cover;//а це для відстеження покритих кліт.поля
     void init_dominosa(); //для ініціалізаціх набору всіх доміно з можливих
     pair<int, int> grab_next_cell(); //непокрита клітинка для спроби розмістити доміно
     bool preprocess_mandatory_dominoes(); // попередня обробка для знаходження очевидних доміно
     bool place_all_dominoes_of_type(const pair<int, int>& domino_type); // розміщення всіх доміно якогось певного типу     
public:
   dominosa_solve(const board &b);
   bool solve() override;//перевизначення компілятор перепірить чи є з такою ж назіою віртуальна фукнція
   void show_solution() const override;
   bool super_solve_recurse(); 
};
#endif