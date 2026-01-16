#include "../include/Car.h"
#include <sstream>

// Инициализация статического члена
int Car::nextId = 1;

// Конструктор по умолчанию
Car::Car() : id(0), brand(""), model(""), year(0), regNumber(""), clientId(0) {}

// Параметризованный конструктор
Car::Car(int id, const string& brand, const string& model,
         int year, const string& regNumber, int clientId)
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
string Car::getBrand() const { return brand; }
string Car::getModel() const { return model; }
int Car::getYear() const { return year; }
string Car::getRegNumber() const { return regNumber; }
int Car::getClientId() const { return clientId; }

// Сеттеры
void Car::setBrand(const string& brand) { this->brand = brand; }
void Car::setModel(const string& model) { this->model = model; }
void Car::setYear(int year) { 
    if (year >= 1900 && year <= 2100) {
        this->year = year; 
    }
}
void Car::setRegNumber(const string& regNumber) { this->regNumber = regNumber; }
void Car::setClientId(int clientId) { this->clientId = clientId; }

// Получение полной информации
string Car::getInfo() const {
    ostringstream oss;
    oss << brand << " " << model << " (" << year << "), Госномер: " << regNumber;
    return oss.str();
}

// Получение краткой информации
string Car::getShortInfo() const {
    ostringstream oss;
    oss << brand << " " << model;
    return oss.str();
}

// Сериализация
string Car::serialize() const {
    ostringstream oss;
    oss << id << ";" << brand << ";" << model << ";" 
        << year << ";" << regNumber << ";" << clientId;
    return oss.str();
}

// Статический метод десериализации
Car Car::deserialize(const string& data) {
    istringstream iss(data);
    string token;
    
    int id, year, clientId;
    string brand, model, regNumber;
    
    getline(iss, token, ';');
    id = stoi(token);
    
    getline(iss, brand, ';');
    getline(iss, model, ';');
    
    getline(iss, token, ';');
    year = stoi(token);
    
    getline(iss, regNumber, ';');
    
    getline(iss, token, ';');
    clientId = stoi(token);
    
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
