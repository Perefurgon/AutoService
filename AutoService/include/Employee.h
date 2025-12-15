#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include "Person.h"

/**
 * Класс Employee - наследник Person
 * Представляет сотрудника автосервиса (мастера)
 */
class Employee : public Person {
private:
    std::string position;  // Должность

public:
    // Конструктор по умолчанию
    Employee();
    
    // Параметризованный конструктор
    Employee(int id, const std::string& name, const std::string& phone, 
             const std::string& position);
    
    // Конструктор копирования
    Employee(const Employee& other);
    
    // Оператор присваивания
    Employee& operator=(const Employee& other);
    
    // Деструктор
    ~Employee() override;
    
    // Геттер для должности
    std::string getPosition() const;
    
    // Сеттер для должности
    void setPosition(const std::string& position);
    
    // Переопределение виртуального метода (полиморфизм)
    std::string getInfo() const override;
    
    // Переопределение метода сериализации
    std::string serialize() const override;
    
    // Статический метод десериализации
    static Employee deserialize(const std::string& data);
};

#endif // EMPLOYEE_H
