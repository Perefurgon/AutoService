#include "../include/ServiceType.h"
#include <sstream>
#include <iomanip>

// Инициализация статического члена
int ServiceType::nextId = 1;

// Конструктор по умолчанию
ServiceType::ServiceType() : id(0), name(""), price(0.0) {}

// Параметризованный конструктор
ServiceType::ServiceType(int id, const string& name, double price)
    : id(id), name(name), price(price > 0 ? price : 0.0) {}

// Конструктор копирования
ServiceType::ServiceType(const ServiceType& other)
    : id(other.id), name(other.name), price(other.price) {}

// Оператор присваивания
ServiceType& ServiceType::operator=(const ServiceType& other) {
    if (this != &other) {
        id = other.id;
        name = other.name;
        price = other.price;
    }
    return *this;
}

// Деструктор
ServiceType::~ServiceType() {}

// Геттеры
int ServiceType::getId() const { return id; }
string ServiceType::getName() const { return name; }
double ServiceType::getPrice() const { return price; }

// Сеттеры
void ServiceType::setName(const string& name) { 
    this->name = name; 
}

void ServiceType::setPrice(double price) { 
    if (price >= 0) {
        this->price = price; 
    }
}

// Получение информации
string ServiceType::getInfo() const {
    ostringstream oss;
    oss << name << " - " << fixed << setprecision(2) << price << " BYN";
    return oss.str();
}

// Сериализация
string ServiceType::serialize() const {
    ostringstream oss;
    oss << id << ";" << name << ";" << fixed << setprecision(2) << price;
    return oss.str();
}

// Статический метод десериализации
ServiceType ServiceType::deserialize(const string& data) {
    istringstream iss(data);
    string token;
    
    int id;
    string name;
    double price;
    
    getline(iss, token, ';');
    id = stoi(token);
    
    getline(iss, name, ';');
    
    getline(iss, token, ';');
    price = stod(token);
    
    return ServiceType(id, name, price);
}

// Статический метод для получения следующего ID
int ServiceType::getNextId() {
    return nextId++;
}

// Статический метод для установки счётчика ID
void ServiceType::setNextId(int id) {
    nextId = id;
}
