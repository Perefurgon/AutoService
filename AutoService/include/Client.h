#ifndef CLIENT_H
#define CLIENT_H

#include "Person.h"

using namespace std;

/**
 * Класс Client - наследник Person
 * Представляет клиента автосервиса
 * Демонстрирует наследование и полиморфизм
 */
class Client : public Person {
private:
    double discount;  // Скидка постоянного клиента (0-100%)

public:
    // Конструктор по умолчанию
    Client();
    
    // Параметризованный конструктор
    Client(int id, const string& name, const string& phone, double discount = 0.0);
    
    // Конструктор копирования
    Client(const Client& other);
    
    // Оператор присваивания
    Client& operator=(const Client& other);
    
    // Деструктор
    ~Client() override;
    
    // Геттер для скидки
    double getDiscount() const;
    
    // Сеттер для скидки с валидацией
    void setDiscount(double discount);
    
    // Переопределение виртуального метода (полиморфизм)
    string getInfo() const override;
    
    // Переопределение метода сериализации
    string serialize() const override;
    
    // Статический метод десериализации
    static Client deserialize(const string& data);
};

#endif // CLIENT_H
