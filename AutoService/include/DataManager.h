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

using namespace std;

/**
 * Класс DataManager
 * Управляет загрузкой и сохранением данных в файлы
 * Реализует паттерн Singleton
 */
class DataManager {
private:
    // Пути к файлам данных
    string dataPath;
    string clientsFile;
    string employeesFile;
    string usersFile;
    string carsFile;
    string servicesFile;
    string ordersFile;
    
    // Коллекции данных
    vector<Client> clients;
    vector<Employee> employees;
    vector<User> users;
    vector<Car> cars;
    vector<ServiceType> serviceTypes;
    vector<ServiceOrder> orders;
    
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
    vector<Client>& getClients();
    const Client* findClientById(int id) const;
    void addClient(const Client& client);
    bool updateClient(int id, const Client& client);
    bool deleteClient(int id);
    vector<Client> searchClientsByName(const string& name) const;
    
    // === МЕТОДЫ ДЛЯ РАБОТЫ С СОТРУДНИКАМИ ===
    vector<Employee>& getEmployees();
    const Employee* findEmployeeById(int id) const;
    void addEmployee(const Employee& employee);
    bool updateEmployee(int id, const Employee& employee);
    bool deleteEmployee(int id);
    
    // === МЕТОДЫ ДЛЯ РАБОТЫ С ПОЛЬЗОВАТЕЛЯМИ ===
    vector<User>& getUsers();
    const User* findUserById(int id) const;
    const User* findUserByLogin(const string& login) const;
    void addUser(const User& user);
    bool updateUser(int id, const User& user);
    bool deleteUser(int id);
    
    // === МЕТОДЫ ДЛЯ РАБОТЫ С АВТОМОБИЛЯМИ ===
    vector<Car>& getCars();
    const Car* findCarById(int id) const;
    vector<Car> getCarsByClientId(int clientId) const;
    void addCar(const Car& car);
    bool updateCar(int id, const Car& car);
    bool deleteCar(int id);
    
    // === МЕТОДЫ ДЛЯ РАБОТЫ С ВИДАМИ РАБОТ ===
    vector<ServiceType>& getServiceTypes();
    const ServiceType* findServiceTypeById(int id) const;
    void addServiceType(const ServiceType& serviceType);
    bool updateServiceType(int id, const ServiceType& serviceType);
    bool deleteServiceType(int id);
    
    // === МЕТОДЫ ДЛЯ РАБОТЫ С ЗАЯВКАМИ ===
    vector<ServiceOrder>& getOrders();
    const ServiceOrder* findOrderById(int id) const;
    vector<ServiceOrder> getOrdersByClientId(int clientId) const;
    vector<ServiceOrder> getOrdersByStatus(OrderStatus status) const;
    vector<ServiceOrder> getOrdersByDate(const string& date) const;
    vector<ServiceOrder> getOrdersByDateRange(const string& startDate, 
                                                    const string& endDate) const;
    void addOrder(const ServiceOrder& order);
    bool updateOrder(int id, const ServiceOrder& order);
    bool deleteOrder(int id);
    
    // Расчёт стоимости заявки
    double calculateOrderCost(const vector<int>& serviceIds, double discount) const;
};

#endif // DATA_MANAGER_H
