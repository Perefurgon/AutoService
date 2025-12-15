#include "../include/ServiceType.h"
#include <sstream>
#include <iomanip>

// Инициализация статического члена
int ServiceType::nextId = 1;

// Конструктор по умолчанию
ServiceType::ServiceType() : id(0), name(""), price(0.0) {}

// Параметризованный конструктор
ServiceType::ServiceType(int id, const std::string& name, double price)
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
std::string ServiceType::getName() const { return name; }
double ServiceType::getPrice() const { return price; }

// Сеттеры
void ServiceType::setName(const std::string& name) { 
    this->name = name; 
}

void ServiceType::setPrice(double price) { 
    if (price >= 0) {
        this->price = price; 
    }
}

// Получение информации
std::string ServiceType::getInfo() const {
    std::ostringstream oss;
    oss << name << " - " << std::fixed << std::setprecision(2) << price << " BYN";
    return oss.str();
}

// Сериализация
std::string ServiceType::serialize() const {
    std::ostringstream oss;
    oss << id << ";" << name << ";" << std::fixed << std::setprecision(2) << price;
    return oss.str();
}

// Статический метод десериализации
ServiceType ServiceType::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string token;
    
    int id;
    std::string name;
    double price;
    
    std::getline(iss, token, ';');
    id = std::stoi(token);
    
    std::getline(iss, name, ';');
    
    std::getline(iss, token, ';');
    price = std::stod(token);
    
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
