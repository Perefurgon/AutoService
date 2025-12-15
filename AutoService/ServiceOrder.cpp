#include "../include/ServiceOrder.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

// Инициализация статического члена
int ServiceOrder::nextId = 1;

// Конструктор по умолчанию
ServiceOrder::ServiceOrder()
    : id(0), clientId(0), carId(0), employeeId(0), date(""),
      status(OrderStatus::NEW), totalCost(0.0) {}

// Параметризованный конструктор
ServiceOrder::ServiceOrder(int id, int clientId, int carId, int employeeId,
                           const std::string& date, OrderStatus status,
                           const std::vector<int>& serviceIds, double totalCost)
    : id(id), clientId(clientId), carId(carId), employeeId(employeeId),
      date(date), status(status), serviceIds(serviceIds), totalCost(totalCost) {}

// Конструктор копирования
ServiceOrder::ServiceOrder(const ServiceOrder& other)
    : id(other.id), clientId(other.clientId), carId(other.carId),
      employeeId(other.employeeId), date(other.date), status(other.status),
      serviceIds(other.serviceIds), totalCost(other.totalCost) {}

// Оператор присваивания
ServiceOrder& ServiceOrder::operator=(const ServiceOrder& other) {
    if (this != &other) {
        id = other.id;
        clientId = other.clientId;
        carId = other.carId;
        employeeId = other.employeeId;
        date = other.date;
        status = other.status;
        serviceIds = other.serviceIds;
        totalCost = other.totalCost;
    }
    return *this;
}

// Деструктор
ServiceOrder::~ServiceOrder() {}

// Геттеры
int ServiceOrder::getId() const { return id; }
int ServiceOrder::getClientId() const { return clientId; }
int ServiceOrder::getCarId() const { return carId; }
int ServiceOrder::getEmployeeId() const { return employeeId; }
std::string ServiceOrder::getDate() const { return date; }
OrderStatus ServiceOrder::getStatus() const { return status; }

std::string ServiceOrder::getStatusString() const {
    return statusToString(status);
}

std::vector<int> ServiceOrder::getServiceIds() const { return serviceIds; }
double ServiceOrder::getTotalCost() const { return totalCost; }

// Сеттеры
void ServiceOrder::setClientId(int clientId) { this->clientId = clientId; }
void ServiceOrder::setCarId(int carId) { this->carId = carId; }
void ServiceOrder::setEmployeeId(int employeeId) { this->employeeId = employeeId; }
void ServiceOrder::setDate(const std::string& date) { this->date = date; }
void ServiceOrder::setStatus(OrderStatus status) { this->status = status; }
void ServiceOrder::setTotalCost(double cost) { 
    if (cost >= 0) {
        this->totalCost = cost; 
    }
}

// Управление услугами
void ServiceOrder::addService(int serviceId) {
    if (!hasService(serviceId)) {
        serviceIds.push_back(serviceId);
    }
}

void ServiceOrder::removeService(int serviceId) {
    auto it = std::find(serviceIds.begin(), serviceIds.end(), serviceId);
    if (it != serviceIds.end()) {
        serviceIds.erase(it);
    }
}

void ServiceOrder::clearServices() {
    serviceIds.clear();
}

bool ServiceOrder::hasService(int serviceId) const {
    return std::find(serviceIds.begin(), serviceIds.end(), serviceId) != serviceIds.end();
}

// Получение информации
std::string ServiceOrder::getInfo() const {
    std::ostringstream oss;
    oss << "Заявка #" << id << " от " << date 
        << ", Статус: " << getStatusString()
        << ", Сумма: " << std::fixed << std::setprecision(2) << totalCost << " BYN";
    return oss.str();
}

// Сериализация
std::string ServiceOrder::serialize() const {
    std::ostringstream oss;
    oss << id << ";" << clientId << ";" << carId << ";" << employeeId << ";"
        << date << ";" << statusToString(status) << ";";
    
    // Сериализация списка услуг
    for (size_t i = 0; i < serviceIds.size(); ++i) {
        oss << serviceIds[i];
        if (i < serviceIds.size() - 1) {
            oss << ",";
        }
    }
    oss << ";" << std::fixed << std::setprecision(2) << totalCost;
    
    return oss.str();
}

// Статический метод десериализации
ServiceOrder ServiceOrder::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string token;
    
    int id, clientId, carId, employeeId;
    std::string date, statusStr, servicesStr;
    double totalCost;
    std::vector<int> serviceIds;
    
    std::getline(iss, token, ';');
    id = std::stoi(token);
    
    std::getline(iss, token, ';');
    clientId = std::stoi(token);
    
    std::getline(iss, token, ';');
    carId = std::stoi(token);
    
    std::getline(iss, token, ';');
    employeeId = std::stoi(token);
    
    std::getline(iss, date, ';');
    std::getline(iss, statusStr, ';');
    std::getline(iss, servicesStr, ';');
    
    std::getline(iss, token, ';');
    totalCost = std::stod(token);
    
    // Парсинг списка услуг
    if (!servicesStr.empty()) {
        std::istringstream servicesStream(servicesStr);
        std::string serviceId;
        while (std::getline(servicesStream, serviceId, ',')) {
            if (!serviceId.empty()) {
                serviceIds.push_back(std::stoi(serviceId));
            }
        }
    }
    
    OrderStatus status = stringToStatus(statusStr);
    
    return ServiceOrder(id, clientId, carId, employeeId, date, status, serviceIds, totalCost);
}

// Преобразование статуса в строку
std::string ServiceOrder::statusToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::NEW:
            return "Новая";
        case OrderStatus::IN_PROGRESS:
            return "В работе";
        case OrderStatus::COMPLETED:
            return "Выполнена";
        case OrderStatus::ISSUED:
            return "Выдана";
        default:
            return "Неизвестно";
    }
}

// Преобразование строки в статус
OrderStatus ServiceOrder::stringToStatus(const std::string& statusStr) {
    if (statusStr == "Новая" || statusStr == "NEW") {
        return OrderStatus::NEW;
    } else if (statusStr == "В работе" || statusStr == "IN_PROGRESS") {
        return OrderStatus::IN_PROGRESS;
    } else if (statusStr == "Выполнена" || statusStr == "COMPLETED") {
        return OrderStatus::COMPLETED;
    } else if (statusStr == "Выдана" || statusStr == "ISSUED") {
        return OrderStatus::ISSUED;
    }
    return OrderStatus::NEW;
}

// Статический метод для получения следующего ID
int ServiceOrder::getNextId() {
    return nextId++;
}

// Статический метод для установки счётчика ID
void ServiceOrder::setNextId(int id) {
    nextId = id;
}
