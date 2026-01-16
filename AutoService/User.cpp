#include "../include/User.h"
#include <sstream>

// Функция хэширования
string User::hashPassword(const string& password) {
    hash<string> hasher;
    size_t hash = hasher(password);
    ostringstream oss;
    oss << hex << hash;
    return oss.str();
}

// Конструктор по умолчанию
User::User() : Person(), login(""), passwordHash(""), role(UserRole::ROLE_USER) {}

// Параметризованный конструктор
User::User(int id, const string& name, const string& phone,
           const string& login, const string& password, UserRole role)
    : Person(id, name, phone), login(login), role(role) {
    passwordHash = hashPassword(password);
}

// Конструктор для десериализации
User::User(int id, const string& name, const string& phone,
           const string& login, const string& passwordHash,
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
string User::getLogin() const {
    return login;
}

UserRole User::getRole() const {
    return role;
}

string User::getRoleString() const {
    return roleToString(role);
}

// Сеттеры
void User::setLogin(const string& login) {
    this->login = login;
}

void User::setPassword(const string& password) {
    this->passwordHash = hashPassword(password);
}

void User::setRole(UserRole role) {
    this->role = role;
}

// Проверка пароля
bool User::checkPassword(const string& password) const {
    return passwordHash == hashPassword(password);
}

// Переопределение виртуального метода getInfo()
string User::getInfo() const {
    ostringstream oss;
    oss << "Пользователь: " << name << " (" << login << "), Роль: " << getRoleString();
    return oss.str();
}

// Сериализация в строку
string User::serialize() const {
    ostringstream oss;
    oss << id << ";" << name << ";" << phone << ";" 
        << login << ";" << passwordHash << ";" << roleToString(role);
    return oss.str();
}

// Статический метод десериализации
User User::deserialize(const string& data) {
    istringstream iss(data);
    string token;
    
    int id;
    string name, phone, login, passwordHash, roleStr;
    
    getline(iss, token, ';');
    id = stoi(token);
    
    getline(iss, name, ';');
    getline(iss, phone, ';');
    getline(iss, login, ';');
    getline(iss, passwordHash, ';');
    getline(iss, roleStr, ';');
    
    UserRole role = stringToRole(roleStr);
    
    return User(id, name, phone, login, passwordHash, role, true);
}

// Преобразование строки в роль
UserRole User::stringToRole(const string& roleStr) {
    if (roleStr == "ROLE_ADMIN" || roleStr == "Администратор") {
        return UserRole::ROLE_ADMIN;
    }
    return UserRole::ROLE_USER;
}

// Преобразование роли в строку
string User::roleToString(UserRole role) {
    switch (role) {
        case UserRole::ROLE_ADMIN:
            return "Администратор";
        case UserRole::ROLE_USER:
            return "Пользователь";
        default:
            return "Неизвестно";
    }
}
