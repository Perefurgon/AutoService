#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <sstream>

using namespace std;

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
    string name;
    string phone;
    static int nextId;  // Статическое поле для генерации ID

public:
    // Конструктор по умолчанию
    Person();
    
    // Параметризованный конструктор
    Person(int id, const string& name, const string& phone);
    
    // Конструктор копирования
    Person(const Person& other);
    
    // Оператор присваивания
    Person& operator=(const Person& other);
    
    // Виртуальный деструктор
    virtual ~Person();
    
    // Геттеры (константные методы)
    int getId() const;
    string getName() const;
    string getPhone() const;
    
    // Сеттеры
    void setName(const string& name);
    void setPhone(const string& phone);
    
    // Чисто виртуальный метод - делает класс абстрактным
    virtual string getInfo() const = 0;
    
    // Виртуальный метод сериализации
    virtual string serialize() const;
    
    // Статический метод для получения следующего ID
    static int getNextId();
    
    // Статический метод для установки счётчика ID
    static void setNextId(int id);
};

#endif // PERSON_H
