#include "../include/Car.h"
#include <sstream>

// Инициализация статического члена
int Car::nextId = 1;

// Конструктор по умолчанию
Car::Car() : id(0), brand(""), model(""), year(0), regNumber(""), clientId(0) {}

// Параметризованный конструктор
Car::Car(int id, const std::string& brand, const std::string& model,
         int year, const std::string& regNumber, int clientId)
    : id(id), brand(brand), model(model), year(year), 
      regNumber(regNumber), clientId(clientId) {}

// Конструктор копирования
Car::Car(const Car& other)
    : id(other.id), brand(other.brand), model(other.model),
      year(other.year), regNumber(other.regNumber), clientId(other.clientId) {}

// Оператор присваивания
Car& Car::operator=(const Car& other) {
    if (this != &other) {
        id = other.id;
        brand = other.brand;
        model = other.model;
        year = other.year;
        regNumber = other.regNumber;
        clientId = other.clientId;
    }
    return *this;
}

// Деструктор
Car::~Car() {}

// Геттеры
int Car::getId() const { return id; }
std::string Car::getBrand() const { return brand; }
std::string Car::getModel() const { return model; }
int Car::getYear() const { return year; }
std::string Car::getRegNumber() const { return regNumber; }
int Car::getClientId() const { return clientId; }

// Сеттеры
void Car::setBrand(const std::string& brand) { this->brand = brand; }
void Car::setModel(const std::string& model) { this->model = model; }
void Car::setYear(int year) { 
    if (year >= 1900 && year <= 2100) {
        this->year = year; 
    }
}
void Car::setRegNumber(const std::string& regNumber) { this->regNumber = regNumber; }
void Car::setClientId(int clientId) { this->clientId = clientId; }

// Получение полной информации
std::string Car::getInfo() const {
    std::ostringstream oss;
    oss << brand << " " << model << " (" << year << "), Госномер: " << regNumber;
    return oss.str();
}

// Получение краткой информации
std::string Car::getShortInfo() const {
    std::ostringstream oss;
    oss << brand << " " << model;
    return oss.str();
}

// Сериализация
std::string Car::serialize() const {
    std::ostringstream oss;
    oss << id << ";" << brand << ";" << model << ";" 
        << year << ";" << regNumber << ";" << clientId;
    return oss.str();
}

// Статический метод десериализации
Car Car::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string token;
    
    int id, year, clientId;
    std::string brand, model, regNumber;
    
    std::getline(iss, token, ';');
    id = std::stoi(token);
    
    std::getline(iss, brand, ';');
    std::getline(iss, model, ';');
    
    std::getline(iss, token, ';');
    year = std::stoi(token);
    
    std::getline(iss, regNumber, ';');
    
    std::getline(iss, token, ';');
    clientId = std::stoi(token);
    
    return Car(id, brand, model, year, regNumber, clientId);
}

// Статический метод для получения следующего ID
int Car::getNextId() {
    return nextId++;
}

// Статический метод для установки счётчика ID
void Car::setNextId(int id) {
    nextId = id;
}
