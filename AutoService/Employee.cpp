#include "../include/Employee.h"
#include <sstream>

// Конструктор по умолчанию
Employee::Employee() : Person(), position("") {}

// Параметризованный конструктор
Employee::Employee(int id, const std::string& name, const std::string& phone,
                   const std::string& position)
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
std::string Employee::getPosition() const {
    return position;
}

// Сеттер для должности
void Employee::setPosition(const std::string& position) {
    this->position = position;
}

// Переопределение виртуального метода getInfo()
std::string Employee::getInfo() const {
    std::ostringstream oss;
    oss << "Сотрудник: " << name << ", Должность: " << position << ", Тел: " << phone;
    return oss.str();
}

// Сериализация в строку
std::string Employee::serialize() const {
    std::ostringstream oss;
    oss << id << ";" << name << ";" << phone << ";" << position;
    return oss.str();
}

// Статический метод десериализации
Employee Employee::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string token;
    
    int id;
    std::string name, phone, position;
    
    std::getline(iss, token, ';');
    id = std::stoi(token);
    
    std::getline(iss, name, ';');
    std::getline(iss, phone, ';');
    std::getline(iss, position, ';');
    
    return Employee(id, name, phone, position);
}
