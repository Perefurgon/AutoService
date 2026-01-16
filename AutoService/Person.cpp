#include "../include/Person.h"

// Инициализация статического члена
int Person::nextId = 1;

// Конструктор по умолчанию
Person::Person() : id(0), name(""), phone("") {}

// Параметризованный конструктор
Person::Person(int id, const string& name, const string& phone)
    : id(id), name(name), phone(phone) {}

// Конструктор копирования
Person::Person(const Person& other)
    : id(other.id), name(other.name), phone(other.phone) {}

// Оператор присваивания
Person& Person::operator=(const Person& other) {
    if (this != &other) {
        id = other.id;
        name = other.name;
        phone = other.phone;
    }
    return *this;
}

// Виртуальный деструктор
Person::~Person() {}

// Геттеры
int Person::getId() const {
    return id;
}

string Person::getName() const {
    return name;
}

string Person::getPhone() const {
    return phone;
}

// Сеттеры
void Person::setName(const string& name) {
    this->name = name;
}

void Person::setPhone(const string& phone) {
    this->phone = phone;
}

// Сериализация базового класса
string Person::serialize() const {
    ostringstream oss;
    oss << id << ";" << name << ";" << phone;
    return oss.str();
}

// Статический метод для получения следующего ID
int Person::getNextId() {
    return nextId++;
}

// Статический метод для установки счётчика ID
void Person::setNextId(int id) {
    nextId = id;
}
