#ifndef USER_H
#define USER_H

#include "Person.h"
#include <functional>

using namespace std;

/**
 * Класс User - наследник Person
 * Представляет пользователя системы с авторизацией
 */
class User : public Person {
private:
    string login;
    string passwordHash;  // Хранится хэш пароля
    UserRole role;
    
    // Приватный метод хэширования пароля
    static string hashPassword(const string& password);

public:
    // Конструктор по умолчанию
    User();
    
    // Параметризованный конструктор
    User(int id, const string& name, const string& phone,
         const string& login, const string& password, UserRole role);
    
    // Конструктор для десериализации (с уже хэшированным паролем)
    User(int id, const string& name, const string& phone,
         const string& login, const string& passwordHash, 
         UserRole role, bool isHashed);
    
    // Конструктор копирования
    User(const User& other);
    
    // Оператор присваивания
    User& operator=(const User& other);
    
    // Деструктор
    ~User() override;
    
    // Геттеры
    string getLogin() const;
    UserRole getRole() const;
    string getRoleString() const;
    
    // Сеттеры
    void setLogin(const string& login);
    void setPassword(const string& password);
    void setRole(UserRole role);
    
    // Проверка пароля
    bool checkPassword(const string& password) const;
    
    // Переопределение виртуального метода (полиморфизм)
    string getInfo() const override;
    
    // Переопределение метода сериализации
    string serialize() const override;
    
    // Статический метод десериализации
    static User deserialize(const string& data);
    
    // Статический метод для преобразования строки в роль
    static UserRole stringToRole(const string& roleStr);
    
    // Статический метод для преобразования роли в строку
    static string roleToString(UserRole role);
};

#endif // USER_H
