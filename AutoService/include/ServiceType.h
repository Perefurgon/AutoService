#ifndef SERVICE_TYPE_H
#define SERVICE_TYPE_H

#include <string>

/**
 * Класс ServiceType
 * Представляет вид работы (услугу) автосервиса
 */
class ServiceType {
private:
    int id;
    std::string name;   // Наименование услуги
    double price;       // Базовая стоимость
    static int nextId;

public:
    // Конструктор по умолчанию
    ServiceType();
    
    // Параметризованный конструктор
    ServiceType(int id, const std::string& name, double price);
    
    // Конструктор копирования
    ServiceType(const ServiceType& other);
    
    // Оператор присваивания
    ServiceType& operator=(const ServiceType& other);
    
    // Деструктор
    ~ServiceType();
    
    // Геттеры
    int getId() const;
    std::string getName() const;
    double getPrice() const;
    
    // Сеттеры
    void setName(const std::string& name);
    void setPrice(double price);
    
    // Получение информации
    std::string getInfo() const;
    
    // Сериализация
    std::string serialize() const;
    
    // Статический метод десериализации
    static ServiceType deserialize(const std::string& data);
    
    // Статический метод для получения следующего ID
    static int getNextId();
    
    // Статический метод для установки счётчика ID
    static void setNextId(int id);
};

#endif // SERVICE_TYPE_H
