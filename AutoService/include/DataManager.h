#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "Client.h"
#include "Employee.h"
#include "User.h"
#include "Car.h"
#include "ServiceType.h"
#include "ServiceOrder.h"
#include <vector>
#include <string>

/**
 * Класс DataManager
 * Управляет загрузкой и сохранением данных в файлы
 * Реализует паттерн Singleton
 */
class DataManager {
private:
    // Пути к файлам данных
    std::string dataPath;
    std::string clientsFile;
    std::string employeesFile;
    std::string usersFile;
    std::string carsFile;
    std::string servicesFile;
    std::string ordersFile;
    
    // Коллекции данных
    std::vector<Client> clients;
    std::vector<Employee> employees;
    std::vector<User> users;
    std::vector<Car> cars;
    std::vector<ServiceType> serviceTypes;
    std::vector<ServiceOrder> orders;
    
    // Приватный конструктор (Singleton)
    DataManager();
    
    // Инициализация данных по умолчанию
    void initDefaultData();
    
    // Обновление счётчиков ID
    void updateIdCounters();

public:
    // Получение единственного экземпляра
    static DataManager& getInstance();
    
    // Запрет копирования
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;
    
    // Загрузка всех данных
    void loadAll();
    
    // Сохранение всех данных
    void saveAll();
    
    // === МЕТОДЫ ДЛЯ РАБОТЫ С КЛИЕНТАМИ ===
    std::vector<Client>& getClients();
    const Client* findClientById(int id) const;
    void addClient(const Client& client);
    bool updateClient(int id, const Client& client);
    bool deleteClient(int id);
    std::vector<Client> searchClientsByName(const std::string& name) const;
    
    // === МЕТОДЫ ДЛЯ РАБОТЫ С СОТРУДНИКАМИ ===
    std::vector<Employee>& getEmployees();
    const Employee* findEmployeeById(int id) const;
    void addEmployee(const Employee& employee);
    bool updateEmployee(int id, const Employee& employee);
    bool deleteEmployee(int id);
    
    // === МЕТОДЫ ДЛЯ РАБОТЫ С ПОЛЬЗОВАТЕЛЯМИ ===
    std::vector<User>& getUsers();
    const User* findUserById(int id) const;
    const User* findUserByLogin(const std::string& login) const;
    void addUser(const User& user);
    bool updateUser(int id, const User& user);
    bool deleteUser(int id);
    
    // === МЕТОДЫ ДЛЯ РАБОТЫ С АВТОМОБИЛЯМИ ===
    std::vector<Car>& getCars();
    const Car* findCarById(int id) const;
    std::vector<Car> getCarsByClientId(int clientId) const;
    void addCar(const Car& car);
    bool updateCar(int id, const Car& car);
    bool deleteCar(int id);
    
    // === МЕТОДЫ ДЛЯ РАБОТЫ С ВИДАМИ РАБОТ ===
    std::vector<ServiceType>& getServiceTypes();
    const ServiceType* findServiceTypeById(int id) const;
    void addServiceType(const ServiceType& serviceType);
    bool updateServiceType(int id, const ServiceType& serviceType);
    bool deleteServiceType(int id);
    
    // === МЕТОДЫ ДЛЯ РАБОТЫ С ЗАЯВКАМИ ===
    std::vector<ServiceOrder>& getOrders();
    const ServiceOrder* findOrderById(int id) const;
    std::vector<ServiceOrder> getOrdersByClientId(int clientId) const;
    std::vector<ServiceOrder> getOrdersByStatus(OrderStatus status) const;
    std::vector<ServiceOrder> getOrdersByDate(const std::string& date) const;
    std::vector<ServiceOrder> getOrdersByDateRange(const std::string& startDate, 
                                                    const std::string& endDate) const;
    void addOrder(const ServiceOrder& order);
    bool updateOrder(int id, const ServiceOrder& order);
    bool deleteOrder(int id);
    
    // Расчёт стоимости заявки
    double calculateOrderCost(const std::vector<int>& serviceIds, double discount) const;
};

#endif // DATA_MANAGER_H
