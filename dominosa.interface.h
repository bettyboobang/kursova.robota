//інтерфейс
#ifndef DOMINOSA_INTERFACE_H
#define DOMINOSA_INTERFACE_H
#include <vector>
#include <utility>

namespace dominosa {//для уникнення конфліктів імен та логічного групування
    const int CELL_NOT_FOUND = -1; //всі клітинки на дошці вже покриті
    const int NO_VALID_MOVES = -2; //для поточної непокритої клітинки немає можливих валідних ходів
    const int INVALID_COORDINATE = -3; //про помилку, пов'язану з невалідними координатами

    struct coordinate { //для представлення координат
        int row;
        int col;
    };
    class dominosa_interface {
    public:
        virtual ~dominosa_interface() = default;
        virtual bool solve() = 0;//повертає тру, якщо розв'язок головоломки знайдено, інакше фолз
        virtual void show_solution() const = 0;//для відображення розвязку
    };

}

#endif