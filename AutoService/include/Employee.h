#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include "Person.h"

using namespace std;

/**
 * Класс Employee - наследник Person
 * Представляет сотрудника автосервиса (мастера)
 */
class Employee : public Person {
private:
    string position;  // Должность

public:
    // Конструктор по умолчанию
    Employee();
    
    // Параметризованный конструктор
    Employee(int id, const string& name, const string& phone, 
             const string& position);
    
    // Конструктор копирования
    Employee(const Employee& other);
    
    // Оператор присваивания
    Employee& operator=(const Employee& other);
    
    // Деструктор
    ~Employee() override;
    
    // Геттер для должности
    string getPosition() const;
    
    // Сеттер для должности
    void setPosition(const string& position);
    
    // Переопределение виртуального метода (полиморфизм)
    string getInfo() const override;
    
    // Переопределение метода сериализации
    string serialize() const override;
    
    // Статический метод десериализации
    static Employee deserialize(const string& data);
};

#endif // EMPLOYEE_H
