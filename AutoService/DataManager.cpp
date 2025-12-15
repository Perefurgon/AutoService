#include "../include/DataManager.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <direct.h> // для _mkdir
#include <sys/stat.h>

// Приватный конструктор
DataManager::DataManager() {
    dataPath = "data/";
    clientsFile = dataPath + "clients.dat";
    employeesFile = dataPath + "employees.dat";
    usersFile = dataPath + "users.dat";
    carsFile = dataPath + "cars.dat";
    servicesFile = dataPath + "services.dat";
    ordersFile = dataPath + "orders.dat";
}

// Получение единственного экземпляра (Singleton)
DataManager& DataManager::getInstance() {
    static DataManager instance;
    return instance;
}

// Инициализация данных по умолчанию
void DataManager::initDefaultData() {
    // Создаём пользователей по умолчанию
    if (users.empty()) {
        users.push_back(User(User::getNextId(), "Администратор", "+375291111111",
                            "admin", "admin123", UserRole::ROLE_ADMIN));
        users.push_back(User(User::getNextId(), "Пользователь", "+375292222222",
                            "user", "user123", UserRole::ROLE_USER));
    }
    
    // Создаём виды работ по умолчанию
    if (serviceTypes.empty()) {
        serviceTypes.push_back(ServiceType(ServiceType::getNextId(), "Замена моторного масла", 45.00));
        serviceTypes.push_back(ServiceType(ServiceType::getNextId(), "Замена масляного фильтра", 15.00));
        serviceTypes.push_back(ServiceType(ServiceType::getNextId(), "Замена воздушного фильтра", 25.00));
        serviceTypes.push_back(ServiceType(ServiceType::getNextId(), "Замена салонного фильтра", 20.00));
        serviceTypes.push_back(ServiceType(ServiceType::getNextId(), "Диагностика двигателя", 80.00));
        serviceTypes.push_back(ServiceType(ServiceType::getNextId(), "Диагностика ходовой части", 60.00));
        serviceTypes.push_back(ServiceType(ServiceType::getNextId(), "Замена передних тормозных колодок", 120.00));
        serviceTypes.push_back(ServiceType(ServiceType::getNextId(), "Замена задних тормозных колодок", 110.00));
        serviceTypes.push_back(ServiceType(ServiceType::getNextId(), "Шиномонтаж (4 колеса)", 40.00));
        serviceTypes.push_back(ServiceType(ServiceType::getNextId(), "Балансировка колёс", 20.00));
        serviceTypes.push_back(ServiceType(ServiceType::getNextId(), "Развал-схождение", 60.00));
        serviceTypes.push_back(ServiceType(ServiceType::getNextId(), "Компьютерная диагностика", 80.00));
    }
    
    // Создаём сотрудников по умолчанию
    if (employees.empty()) {
        employees.push_back(Employee(Employee::getNextId(), "Смирнов Алексей Петрович", 
                                     "+375293333333", "Мастер"));
        employees.push_back(Employee(Employee::getNextId(), "Кузнецов Дмитрий Иванович", 
                                     "+375294444444", "Мастер"));
    }
}

// Обновление счётчиков ID
void DataManager::updateIdCounters() {
    int maxClientId = 0;
    for (const auto& c : clients) {
        if (c.getId() > maxClientId) maxClientId = c.getId();
    }
    Person::setNextId(maxClientId + 1);
    
    int maxCarId = 0;
    for (const auto& c : cars) {
        if (c.getId() > maxCarId) maxCarId = c.getId();
    }
    Car::setNextId(maxCarId + 1);
    
    int maxServiceId = 0;
    for (const auto& s : serviceTypes) {
        if (s.getId() > maxServiceId) maxServiceId = s.getId();
    }
    ServiceType::setNextId(maxServiceId + 1);
    
    int maxOrderId = 0;
    for (const auto& o : orders) {
        if (o.getId() > maxOrderId) maxOrderId = o.getId();
    }
    ServiceOrder::setNextId(maxOrderId + 1);
}

// Загрузка всех данных
void DataManager::loadAll() {
    // Создаём папку data если не существует
    #ifdef _WIN32
        _mkdir(dataPath.c_str());
    #else
        mkdir(dataPath.c_str(), 0777);
    #endif

    std::string line;
    
    // Загрузка пользователей
    std::ifstream usersIn(usersFile);
    if (usersIn.is_open()) {
        while (std::getline(usersIn, line)) {
            if (!line.empty()) {
                users.push_back(User::deserialize(line));
            }
        }
        usersIn.close();
    }
    
    // Загрузка клиентов
    std::ifstream clientsIn(clientsFile);
    if (clientsIn.is_open()) {
        while (std::getline(clientsIn, line)) {
            if (!line.empty()) {
                clients.push_back(Client::deserialize(line));
            }
        }
        clientsIn.close();
    }
    
    // Загрузка сотрудников
    std::ifstream employeesIn(employeesFile);
    if (employeesIn.is_open()) {
        while (std::getline(employeesIn, line)) {
            if (!line.empty()) {
                employees.push_back(Employee::deserialize(line));
            }
        }
        employeesIn.close();
    }
    
    // Загрузка автомобилей
    std::ifstream carsIn(carsFile);
    if (carsIn.is_open()) {
        while (std::getline(carsIn, line)) {
            if (!line.empty()) {
                cars.push_back(Car::deserialize(line));
            }
        }
        carsIn.close();
    }
    
    // Загрузка видов работ
    std::ifstream servicesIn(servicesFile);
    if (servicesIn.is_open()) {
        while (std::getline(servicesIn, line)) {
            if (!line.empty()) {
                serviceTypes.push_back(ServiceType::deserialize(line));
            }
        }
        servicesIn.close();
    }
    
    // Загрузка заявок
    std::ifstream ordersIn(ordersFile);
    if (ordersIn.is_open()) {
        while (std::getline(ordersIn, line)) {
            if (!line.empty()) {
                orders.push_back(ServiceOrder::deserialize(line));
            }
        }
        ordersIn.close();
    }
    
    // Инициализация данных по умолчанию если файлы пустые
    initDefaultData();
    
    // Обновление счётчиков
    updateIdCounters();
}

// Сохранение всех данных
void DataManager::saveAll() {
    // Сохранение пользователей
    std::ofstream usersOut(usersFile);
    if (usersOut.is_open()) {
        for (const auto& u : users) {
            usersOut << u.serialize() << "\n";
        }
        usersOut.close();
    }
    
    // Сохранение клиентов
    std::ofstream clientsOut(clientsFile);
    if (clientsOut.is_open()) {
        for (const auto& c : clients) {
            clientsOut << c.serialize() << "\n";
        }
        clientsOut.close();
    }
    
    // Сохранение сотрудников
    std::ofstream employeesOut(employeesFile);
    if (employeesOut.is_open()) {
        for (const auto& e : employees) {
            employeesOut << e.serialize() << "\n";
        }
        employeesOut.close();
    }
    
    // Сохранение автомобилей
    std::ofstream carsOut(carsFile);
    if (carsOut.is_open()) {
        for (const auto& c : cars) {
            carsOut << c.serialize() << "\n";
        }
        carsOut.close();
    }
    
    // Сохранение видов работ
    std::ofstream servicesOut(servicesFile);
    if (servicesOut.is_open()) {
        for (const auto& s : serviceTypes) {
            servicesOut << s.serialize() << "\n";
        }
        servicesOut.close();
    }
    
    // Сохранение заявок
    std::ofstream ordersOut(ordersFile);
    if (ordersOut.is_open()) {
        for (const auto& o : orders) {
            ordersOut << o.serialize() << "\n";
        }
        ordersOut.close();
    }
}

// === МЕТОДЫ ДЛЯ РАБОТЫ С КЛИЕНТАМИ ===

std::vector<Client>& DataManager::getClients() {
    return clients;
}

const Client* DataManager::findClientById(int id) const {
    for (const auto& c : clients) {
        if (c.getId() == id) {
            return &c;
        }
    }
    return nullptr;
}

void DataManager::addClient(const Client& client) {
    clients.push_back(client);
}

bool DataManager::updateClient(int id, const Client& client) {
    for (auto& c : clients) {
        if (c.getId() == id) {
            c = client;
            return true;
        }
    }
    return false;
}

bool DataManager::deleteClient(int id) {
    auto it = std::remove_if(clients.begin(), clients.end(),
        [id](const Client& c) { return c.getId() == id; });
    if (it != clients.end()) {
        clients.erase(it, clients.end());
        return true;
    }
    return false;
}

std::vector<Client> DataManager::searchClientsByName(const std::string& name) const {
    std::vector<Client> result;
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    for (const auto& c : clients) {
        std::string clientName = c.getName();
        std::transform(clientName.begin(), clientName.end(), clientName.begin(), ::tolower);
        if (clientName.find(lowerName) != std::string::npos) {
            result.push_back(c);
        }
    }
    return result;
}

// === МЕТОДЫ ДЛЯ РАБОТЫ С СОТРУДНИКАМИ ===

std::vector<Employee>& DataManager::getEmployees() {
    return employees;
}

const Employee* DataManager::findEmployeeById(int id) const {
    for (const auto& e : employees) {
        if (e.getId() == id) {
            return &e;
        }
    }
    return nullptr;
}

void DataManager::addEmployee(const Employee& employee) {
    employees.push_back(employee);
}

bool DataManager::updateEmployee(int id, const Employee& employee) {
    for (auto& e : employees) {
        if (e.getId() == id) {
            e = employee;
            return true;
        }
    }
    return false;
}

bool DataManager::deleteEmployee(int id) {
    auto it = std::remove_if(employees.begin(), employees.end(),
        [id](const Employee& e) { return e.getId() == id; });
    if (it != employees.end()) {
        employees.erase(it, employees.end());
        return true;
    }
    return false;
}

// === МЕТОДЫ ДЛЯ РАБОТЫ С ПОЛЬЗОВАТЕЛЯМИ ===

std::vector<User>& DataManager::getUsers() {
    return users;
}

const User* DataManager::findUserById(int id) const {
    for (const auto& u : users) {
        if (u.getId() == id) {
            return &u;
        }
    }
    return nullptr;
}

const User* DataManager::findUserByLogin(const std::string& login) const {
    for (const auto& u : users) {
        if (u.getLogin() == login) {
            return &u;
        }
    }
    return nullptr;
}

void DataManager::addUser(const User& user) {
    users.push_back(user);
}

bool DataManager::updateUser(int id, const User& user) {
    for (auto& u : users) {
        if (u.getId() == id) {
            u = user;
            return true;
        }
    }
    return false;
}

bool DataManager::deleteUser(int id) {
    auto it = std::remove_if(users.begin(), users.end(),
        [id](const User& u) { return u.getId() == id; });
    if (it != users.end()) {
        users.erase(it, users.end());
        return true;
    }
    return false;
}

// === МЕТОДЫ ДЛЯ РАБОТЫ С АВТОМОБИЛЯМИ ===

std::vector<Car>& DataManager::getCars() {
    return cars;
}

const Car* DataManager::findCarById(int id) const {
    for (const auto& c : cars) {
        if (c.getId() == id) {
            return &c;
        }
    }
    return nullptr;
}

std::vector<Car> DataManager::getCarsByClientId(int clientId) const {
    std::vector<Car> result;
    for (const auto& c : cars) {
        if (c.getClientId() == clientId) {
            result.push_back(c);
        }
    }
    return result;
}

void DataManager::addCar(const Car& car) {
    cars.push_back(car);
}

bool DataManager::updateCar(int id, const Car& car) {
    for (auto& c : cars) {
        if (c.getId() == id) {
            c = car;
            return true;
        }
    }
    return false;
}

bool DataManager::deleteCar(int id) {
    auto it = std::remove_if(cars.begin(), cars.end(),
        [id](const Car& c) { return c.getId() == id; });
    if (it != cars.end()) {
        cars.erase(it, cars.end());
        return true;
    }
    return false;
}

// === МЕТОДЫ ДЛЯ РАБОТЫ С ВИДАМИ РАБОТ ===

std::vector<ServiceType>& DataManager::getServiceTypes() {
    return serviceTypes;
}

const ServiceType* DataManager::findServiceTypeById(int id) const {
    for (const auto& s : serviceTypes) {
        if (s.getId() == id) {
            return &s;
        }
    }
    return nullptr;
}

void DataManager::addServiceType(const ServiceType& serviceType) {
    serviceTypes.push_back(serviceType);
}

bool DataManager::updateServiceType(int id, const ServiceType& serviceType) {
    for (auto& s : serviceTypes) {
        if (s.getId() == id) {
            s = serviceType;
            return true;
        }
    }
    return false;
}

bool DataManager::deleteServiceType(int id) {
    auto it = std::remove_if(serviceTypes.begin(), serviceTypes.end(),
        [id](const ServiceType& s) { return s.getId() == id; });
    if (it != serviceTypes.end()) {
        serviceTypes.erase(it, serviceTypes.end());
        return true;
    }
    return false;
}

// === МЕТОДЫ ДЛЯ РАБОТЫ С ЗАЯВКАМИ ===

std::vector<ServiceOrder>& DataManager::getOrders() {
    return orders;
}

const ServiceOrder* DataManager::findOrderById(int id) const {
    for (const auto& o : orders) {
        if (o.getId() == id) {
            return &o;
        }
    }
    return nullptr;
}

std::vector<ServiceOrder> DataManager::getOrdersByClientId(int clientId) const {
    std::vector<ServiceOrder> result;
    for (const auto& o : orders) {
        if (o.getClientId() == clientId) {
            result.push_back(o);
        }
    }
    return result;
}

std::vector<ServiceOrder> DataManager::getOrdersByStatus(OrderStatus status) const {
    std::vector<ServiceOrder> result;
    for (const auto& o : orders) {
        if (o.getStatus() == status) {
            result.push_back(o);
        }
    }
    return result;
}

std::vector<ServiceOrder> DataManager::getOrdersByDate(const std::string& date) const {
    std::vector<ServiceOrder> result;
    for (const auto& o : orders) {
        if (o.getDate() == date) {
            result.push_back(o);
        }
    }
    return result;
}

std::vector<ServiceOrder> DataManager::getOrdersByDateRange(const std::string& startDate,
                                                             const std::string& endDate) const {
    std::vector<ServiceOrder> result;
    for (const auto& o : orders) {
        if (o.getDate() >= startDate && o.getDate() <= endDate) {
            result.push_back(o);
        }
    }
    return result;
}

void DataManager::addOrder(const ServiceOrder& order) {
    orders.push_back(order);
}

bool DataManager::updateOrder(int id, const ServiceOrder& order) {
    for (auto& o : orders) {
        if (o.getId() == id) {
            o = order;
            return true;
        }
    }
    return false;
}

bool DataManager::deleteOrder(int id) {
    auto it = std::remove_if(orders.begin(), orders.end(),
        [id](const ServiceOrder& o) { return o.getId() == id; });
    if (it != orders.end()) {
        orders.erase(it, orders.end());
        return true;
    }
    return false;
}

// Расчёт стоимости заявки
double DataManager::calculateOrderCost(const std::vector<int>& serviceIds, double discount) const {
    double total = 0.0;
    for (int id : serviceIds) {
        const ServiceType* service = findServiceTypeById(id);
        if (service) {
            total += service->getPrice();
        }
    }
    
    // Применяем скидку
    if (discount > 0 && discount <= 100) {
        total = total * (1.0 - discount / 100.0);
    }
    
    return total;
}
