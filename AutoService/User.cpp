#include "../include/User.h"
#include <sstream>

// Простая функция хэширования (для учебных целей)
std::string User::hashPassword(const std::string& password) {
    std::hash<std::string> hasher;
    size_t hash = hasher(password);
    std::ostringstream oss;
    oss << std::hex << hash;
    return oss.str();
}

// Конструктор по умолчанию
User::User() : Person(), login(""), passwordHash(""), role(UserRole::ROLE_USER) {}

// Параметризованный конструктор
User::User(int id, const std::string& name, const std::string& phone,
           const std::string& login, const std::string& password, UserRole role)
    : Person(id, name, phone), login(login), role(role) {
    passwordHash = hashPassword(password);
}

// Конструктор для десериализации
User::User(int id, const std::string& name, const std::string& phone,
           const std::string& login, const std::string& passwordHash,
           UserRole role, bool isHashed)
    : Person(id, name, phone), login(login), passwordHash(passwordHash), role(role) {}

// Конструктор копирования
User::User(const User& other)
    : Person(other), login(other.login), 
      passwordHash(other.passwordHash), role(other.role) {}

// Оператор присваивания
User& User::operator=(const User& other) {
    if (this != &other) {
        Person::operator=(other);
        login = other.login;
        passwordHash = other.passwordHash;
        role = other.role;
    }
    return *this;
}

// Деструктор
User::~User() {}

// Геттеры
std::string User::getLogin() const {
    return login;
}

UserRole User::getRole() const {
    return role;
}

std::string User::getRoleString() const {
    return roleToString(role);
}

// Сеттеры
void User::setLogin(const std::string& login) {
    this->login = login;
}

void User::setPassword(const std::string& password) {
    this->passwordHash = hashPassword(password);
}

void User::setRole(UserRole role) {
    this->role = role;
}

// Проверка пароля
bool User::checkPassword(const std::string& password) const {
    return passwordHash == hashPassword(password);
}

// Переопределение виртуального метода getInfo()
std::string User::getInfo() const {
    std::ostringstream oss;
    oss << "Пользователь: " << name << " (" << login << "), Роль: " << getRoleString();
    return oss.str();
}

// Сериализация в строку
std::string User::serialize() const {
    std::ostringstream oss;
    oss << id << ";" << name << ";" << phone << ";" 
        << login << ";" << passwordHash << ";" << roleToString(role);
    return oss.str();
}

// Статический метод десериализации
User User::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string token;
    
    int id;
    std::string name, phone, login, passwordHash, roleStr;
    
    std::getline(iss, token, ';');
    id = std::stoi(token);
    
    std::getline(iss, name, ';');
    std::getline(iss, phone, ';');
    std::getline(iss, login, ';');
    std::getline(iss, passwordHash, ';');
    std::getline(iss, roleStr, ';');
    
    UserRole role = stringToRole(roleStr);
    
    return User(id, name, phone, login, passwordHash, role, true);
}

// Преобразование строки в роль
UserRole User::stringToRole(const std::string& roleStr) {
    if (roleStr == "ROLE_ADMIN" || roleStr == "Администратор") {
        return UserRole::ROLE_ADMIN;
    }
    return UserRole::ROLE_USER;
}

// Преобразование роли в строку
std::string User::roleToString(UserRole role) {
    switch (role) {
        case UserRole::ROLE_ADMIN:
            return "Администратор";
        case UserRole::ROLE_USER:
            return "Пользователь";
        default:
            return "Неизвестно";
    }
}
