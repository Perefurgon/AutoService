#ifndef CAR_H
#define CAR_H

#include <string>

using namespace std;

/**
 * Класс Car
 * Представляет автомобиль клиента
 */
class Car {
private:
    int id;
    string brand;      // Марка
    string model;      // Модель
    int year;          // Год выпуска
    string regNumber;  // Госномер
    int clientId;      // ID владельца
    static int nextId;

public:
    // Конструктор по умолчанию
    Car();
    
    // Параметризованный конструктор
    Car(int id, const string& brand, const string& model,
        int year, const string& regNumber, int clientId);
    
    // Конструктор копирования
    Car(const Car& other);
    
    // Оператор присваивания
    Car& operator=(const Car& other);
    
    // Деструктор
    ~Car();
    
    // Геттеры
    int getId() const;
    string getBrand() const;
    string getModel() const;
    int getYear() const;
    string getRegNumber() const;
    int getClientId() const;
    
    // Сеттеры
    void setBrand(const string& brand);
    void setModel(const string& model);
    void setYear(int year);
    void setRegNumber(const string& regNumber);
    void setClientId(int clientId);
    
    // Получение полной информации
    string getInfo() const;
    
    // Получение краткой информации
    string getShortInfo() const;
    
    // Сериализация
    string serialize() const;
    
    // Статический метод десериализации
    static Car deserialize(const string& data);
    
    // Статический метод для получения следующего ID
    static int getNextId();
    
    // Статический метод для установки счётчика ID
    static void setNextId(int id);
};

#endif // CAR_H
