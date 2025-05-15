#ifndef DOMINOSA_SOLVE_H
#define DOMINOSA_SOLVE_H
#include "dominosa.interface.h"
#include "board.h"
#include <vector>
#include <map>
#include <utility>
#include <string>
using namespace std; 

const int NEIGHBOR_DIRECTIONS_COUNT = 4;

class dominosa_solve : public dominosa::dominosa_interface {
  board d_board; // <--- ВИПРАВЛЕНО: видалено зайву крапку
  map<pair<int, int>, int> domino_available; //пара (значення доміно), значення (кількість): 1 (якщо ще не розміщено), 0 (якщо розміщено)
  vector<pair<dominosa::coordinate, dominosa::coordinate>> find_solution; //а це для відстеження покритих кліт.поля (зберігає знайдені пари координат доміно)
  vector<vector<bool>> is_cover;
  bool show_steps;
  struct domino_move {
      int r2;
      int c2;
      pair<int, int> domino_type;
  };
  void init_dominosa(); //для ініціалізаціх набору всіх доміно з можливих (очікувана кіль-сть 1)
  pair<int, int> grab_next_cell(); //непокрита клітинка для спроби розмістити доміно
  bool preprocess_dominoes();//попередня обробка для знаходження очевидних доміно
  void place_domino(int r1, int c1, int r2, int c2, const pair<int, int>& domino_type); //розміщення доміно
  void backtrack_domino(int r1, int c1, int r2, int c2, const pair<int, int>& domino_type); //відкат розміщення доміно
  bool super_solve_recurse();//рекурсивна функція розв'язання
  void print_step(const string& message); //друк кроку розв'язання
public:
  dominosa_solve(const board &b, bool display_steps = false); //розвязувач з дошкою якак передається
  static pair<int, int> make_domino_key(int a, int b); //створення ключа доміно (канонічної пари)
  bool solve() override;//основний метод розв'язання
  void show_solution() const override;  //відображення розв'язку
};

#endif