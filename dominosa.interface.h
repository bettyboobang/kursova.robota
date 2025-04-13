#ifndef D_INTERFACE_H
#define D_INTERFACE

class dominosa_interface {
public:
	virtual ~dominosa_interface() = default;//для правильного звільнення ресурсів тому що використовуємо поліморфізм
	virtual bool solve() = 0; // метод розвязання яки йще буде реалізований в дочірніх класах
	virtual void show_solution() const = 0;  //також метод для дочірнього класу проте для показу розвязку
};

#endif