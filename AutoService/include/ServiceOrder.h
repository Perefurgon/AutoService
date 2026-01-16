#ifndef SERVICE_ORDER_H
#define SERVICE_ORDER_H

#include "Person.h"
#include <string>
#include <vector>

using namespace std;

/**
 * Класс ServiceOrder
 * Представляет заявку на обслуживание автомобиля
 */
class ServiceOrder {
private:
    int id;
    int clientId;           // ID клиента
    int carId;              // ID автомобиля
    int employeeId;         // ID исполнителя (мастера)
    string date;       // Дата приёма (формат: ДД.ММ.ГГГГ)
    OrderStatus status;     // Статус заявки
    vector<int> serviceIds;  // Список ID услуг
    double totalCost;       // Итоговая стоимость
    static int nextId;

public:
    // Конструктор по умолчанию
    ServiceOrder();
    
    // Параметризованный конструктор
    ServiceOrder(int id, int clientId, int carId, int employeeId,
                 const string& date, OrderStatus status,
                 const vector<int>& serviceIds, double totalCost);
    
    // Конструктор копирования
    ServiceOrder(const ServiceOrder& other);
    
    // Оператор присваивания
    ServiceOrder& operator=(const ServiceOrder& other);
    
    // Деструктор
    ~ServiceOrder();
    
    // Геттеры
    int getId() const;
    int getClientId() const;
    int getCarId() const;
    int getEmployeeId() const;
    string getDate() const;
    OrderStatus getStatus() const;
    string getStatusString() const;
    vector<int> getServiceIds() const;
    double getTotalCost() const;
    
    // Сеттеры
    void setClientId(int clientId);
    void setCarId(int carId);
    void setEmployeeId(int employeeId);
    void setDate(const string& date);
    void setStatus(OrderStatus status);
    void setTotalCost(double cost);
    
    // Управление услугами
    void addService(int serviceId);
    void removeService(int serviceId);
    void clearServices();
    bool hasService(int serviceId) const;
    
    // Получение информации
    string getInfo() const;
    
    // Сериализация
    string serialize() const;
    
    // Статический метод десериализации
    static ServiceOrder deserialize(const string& data);
    
    // Преобразование статуса в строку
    static string statusToString(OrderStatus status);
    
    // Преобразование строки в статус
    static OrderStatus stringToStatus(const string& statusStr);
    
    // Статический метод для получения следующего ID
    static int getNextId();
    
    // Статический метод для установки счётчика ID
    static void setNextId(int id);
};

#endif // SERVICE_ORDER_H
