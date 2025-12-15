#ifndef CLIENT_H
#define CLIENT_H

#include "Person.h"

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
    Client(int id, const std::string& name, const std::string& phone, double discount = 0.0);
    
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
    std::string getInfo() const override;
    
    // Переопределение метода сериализации
    std::string serialize() const override;
    
    // Статический метод десериализации
    static Client deserialize(const std::string& data);
};

#endif // CLIENT_H
