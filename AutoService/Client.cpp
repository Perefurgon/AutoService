#include "../include/Client.h"
#include <sstream>
#include <iomanip>

// Конструктор по умолчанию
Client::Client() : Person(), discount(0.0) {}

// Параметризованный конструктор
Client::Client(int id, const string& name, const string& phone, double discount)
    : Person(id, name, phone), discount(0.0) {
    setDiscount(discount);  // Используем сеттер для валидации
}

// Конструктор копирования
Client::Client(const Client& other)
    : Person(other), discount(other.discount) {}

// Оператор присваивания
Client& Client::operator=(const Client& other) {
    if (this != &other) {
        Person::operator=(other);  // Вызов оператора базового класса
        discount = other.discount;
    }
    return *this;
}

// Деструктор
Client::~Client() {}

// Геттер для скидки
double Client::getDiscount() const {
    return discount;
}

// Сеттер для скидки с валидацией
void Client::setDiscount(double discount) {
    if (discount >= 0.0 && discount <= 100.0) {
        this->discount = discount;
    }
}

// Переопределение виртуального метода getInfo()
string Client::getInfo() const {
    ostringstream oss;
    oss << "Клиент: " << name << ", Тел: " << phone;
    if (discount > 0) {
        oss << ", Скидка: " << fixed << setprecision(1) << discount << "%";
    }
    return oss.str();
}

// Сериализация в строку
string Client::serialize() const {
    ostringstream oss;
    oss << id << ";" << name << ";" << phone << ";" 
        << fixed << setprecision(1) << discount;
    return oss.str();
}

// Статический метод десериализации
Client Client::deserialize(const string& data) {
    istringstream iss(data);
    string token;
    
    int id;
    string name, phone;
    double discount;
    
    getline(iss, token, ';');
    id = stoi(token);
    
    getline(iss, name, ';');
    getline(iss, phone, ';');
    
    getline(iss, token, ';');
    discount = stod(token);
    
    return Client(id, name, phone, discount);
}
