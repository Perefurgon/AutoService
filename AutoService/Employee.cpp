#include "../include/Employee.h"
#include <sstream>

// Конструктор по умолчанию
Employee::Employee() : Person(), position("") {}

// Параметризованный конструктор
Employee::Employee(int id, const string& name, const string& phone,
                   const string& position)
    : Person(id, name, phone), position(position) {}

// Конструктор копирования
Employee::Employee(const Employee& other)
    : Person(other), position(other.position) {}

// Оператор присваивания
Employee& Employee::operator=(const Employee& other) {
    if (this != &other) {
        Person::operator=(other);
        position = other.position;
    }
    return *this;
}

// Деструктор
Employee::~Employee() {}

// Геттер для должности
string Employee::getPosition() const {
    return position;
}

// Сеттер для должности
void Employee::setPosition(const string& position) {
    this->position = position;
}

// Переопределение виртуального метода getInfo()
string Employee::getInfo() const {
    ostringstream oss;
    oss << "Сотрудник: " << name << ", Должность: " << position << ", Тел: " << phone;
    return oss.str();
}

// Сериализация в строку
string Employee::serialize() const {
    ostringstream oss;
    oss << id << ";" << name << ";" << phone << ";" << position;
    return oss.str();
}

// Статический метод десериализации
Employee Employee::deserialize(const string& data) {
    istringstream iss(data);
    string token;
    
    int id;
    string name, phone, position;
    
    getline(iss, token, ';');
    id = stoi(token);
    
    getline(iss, name, ';');
    getline(iss, phone, ';');
    getline(iss, position, ';');
    
    return Employee(id, name, phone, position);
}
