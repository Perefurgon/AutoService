#ifndef USER_H
#define USER_H

#include "Person.h"
#include <functional>

/**
 * Класс User - наследник Person
 * Представляет пользователя системы с авторизацией
 */
class User : public Person {
private:
    std::string login;
    std::string passwordHash;  // Хранится хэш пароля
    UserRole role;
    
    // Приватный метод хэширования пароля
    static std::string hashPassword(const std::string& password);

public:
    // Конструктор по умолчанию
    User();
    
    // Параметризованный конструктор
    User(int id, const std::string& name, const std::string& phone,
         const std::string& login, const std::string& password, UserRole role);
    
    // Конструктор для десериализации (с уже хэшированным паролем)
    User(int id, const std::string& name, const std::string& phone,
         const std::string& login, const std::string& passwordHash, 
         UserRole role, bool isHashed);
    
    // Конструктор копирования
    User(const User& other);
    
    // Оператор присваивания
    User& operator=(const User& other);
    
    // Деструктор
    ~User() override;
    
    // Геттеры
    std::string getLogin() const;
    UserRole getRole() const;
    std::string getRoleString() const;
    
    // Сеттеры
    void setLogin(const std::string& login);
    void setPassword(const std::string& password);
    void setRole(UserRole role);
    
    // Проверка пароля
    bool checkPassword(const std::string& password) const;
    
    // Переопределение виртуального метода (полиморфизм)
    std::string getInfo() const override;
    
    // Переопределение метода сериализации
    std::string serialize() const override;
    
    // Статический метод десериализации
    static User deserialize(const std::string& data);
    
    // Статический метод для преобразования строки в роль
    static UserRole stringToRole(const std::string& roleStr);
    
    // Статический метод для преобразования роли в строку
    static std::string roleToString(UserRole role);
};

#endif // USER_H
