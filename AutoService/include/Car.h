#ifndef CAR_H
#define CAR_H

#include <string>

/**
 * Класс Car
 * Представляет автомобиль клиента
 */
class Car {
private:
    int id;
    std::string brand;      // Марка
    std::string model;      // Модель
    int year;               // Год выпуска
    std::string regNumber;  // Госномер
    int clientId;           // ID владельца
    static int nextId;

public:
    // Конструктор по умолчанию
    Car();
    
    // Параметризованный конструктор
    Car(int id, const std::string& brand, const std::string& model,
        int year, const std::string& regNumber, int clientId);
    
    // Конструктор копирования
    Car(const Car& other);
    
    // Оператор присваивания
    Car& operator=(const Car& other);
    
    // Деструктор
    ~Car();
    
    // Геттеры
    int getId() const;
    std::string getBrand() const;
    std::string getModel() const;
    int getYear() const;
    std::string getRegNumber() const;
    int getClientId() const;
    
    // Сеттеры
    void setBrand(const std::string& brand);
    void setModel(const std::string& model);
    void setYear(int year);
    void setRegNumber(const std::string& regNumber);
    void setClientId(int clientId);
    
    // Получение полной информации
    std::string getInfo() const;
    
    // Получение краткой информации
    std::string getShortInfo() const;
    
    // Сериализация
    std::string serialize() const;
    
    // Статический метод десериализации
    static Car deserialize(const std::string& data);
    
    // Статический метод для получения следующего ID
    static int getNextId();
    
    // Статический метод для установки счётчика ID
    static void setNextId(int id);
};

#endif // CAR_H
