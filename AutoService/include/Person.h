#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <sstream>

// Перечисление ролей пользователей
enum class UserRole {
    ROLE_ADMIN,
    ROLE_USER
};

// Перечисление статусов заявки
enum class OrderStatus {
    NEW,        // Новая
    IN_PROGRESS,// В работе
    COMPLETED,  // Выполнена
    ISSUED      // Выдана
};

/**
 * Абстрактный базовый класс Person
 * Демонстрирует принципы абстрагирования и инкапсуляции
 */
class Person {
protected:
    int id;
    std::string name;
    std::string phone;
    static int nextId;  // Статическое поле для генерации ID

public:
    // Конструктор по умолчанию
    Person();
    
    // Параметризованный конструктор
    Person(int id, const std::string& name, const std::string& phone);
    
    // Конструктор копирования
    Person(const Person& other);
    
    // Оператор присваивания
    Person& operator=(const Person& other);
    
    // Виртуальный деструктор
    virtual ~Person();
    
    // Геттеры (константные методы)
    int getId() const;
    std::string getName() const;
    std::string getPhone() const;
    
    // Сеттеры
    void setName(const std::string& name);
    void setPhone(const std::string& phone);
    
    // Чисто виртуальный метод - делает класс абстрактным
    virtual std::string getInfo() const = 0;
    
    // Виртуальный метод сериализации
    virtual std::string serialize() const;
    
    // Статический метод для получения следующего ID
    static int getNextId();
    
    // Статический метод для установки счётчика ID
    static void setNextId(int id);
};

#endif // PERSON_H
