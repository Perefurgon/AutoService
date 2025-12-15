#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX    // Отключает макросы min/max из windows.h
#endif

#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include <climits>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <sstream>

#include <conio.h>
#include <windows.h>

#include "../include/DataManager.h"
#include "../include/ReportGenerator.h"

// Текущий авторизованный пользователь
const User* currentUser = nullptr;

// ====================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ======================

// Очистка экрана
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Настройка консоли для Windows (поддержка UTF-8)
void setupConsole() {
    #ifdef _WIN32
        SetConsoleCP(1251);
        SetConsoleOutputCP(1251);
        // Или для UTF-8:
        //SetConsoleCP(65001);
        //SetConsoleOutputCP(65001);
    #endif
}

// Пауза с ожиданием нажатия любой клавиши
void pause() {
    printf("\nДля подолжения нажмите любую клавишу... ");
    int g = _getch();
}

// Чтение строки с приглашением
std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

// Чтение целого числа с валидацией
int readInt(const std::string& prompt, int minVal = INT_MIN, int maxVal = INT_MAX) {
    while (true) {
        std::cout << prompt;
        int value;
        if (std::cin >> value && value >= minVal && value <= maxVal) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ошибка ввода! Повторите.\n";
    }
}

// Чтение числа с плавающей точкой
double readDouble(const std::string& prompt, double minVal = 0.0) {
    while (true) {
        std::cout << prompt;
        double value;
        if (std::cin >> value && value >= minVal) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ошибка ввода! Повторите.\n";
    }
}

// Подтверждение действия
bool confirm(const std::string& message) {
    std::cout << message << " (Да/Нет): ";
    std::string answer;
    std::getline(std::cin, answer);
    return (answer == "д" || answer == "Д" || answer == "да" || answer == "Да" || answer == "ДА" || answer == "y" || answer == "Y");
}

// Получение текущей даты
std::string getCurrentDate() {
    time_t now = time(0);
    char buffer[12];
    #ifdef _WIN32
        tm ltm;
        localtime_s(&ltm, &now);
        sprintf_s(buffer, sizeof(buffer), "%02d.%02d.%d", ltm.tm_mday, 1 + ltm.tm_mon, 1900 + ltm.tm_year);
    #else
        tm* ltm = localtime(&now);
        sprintf(buffer, "%02d.%02d.%d", ltm->tm_mday, 1 + ltm->tm_mon, 1900 + ltm->tm_year);
    #endif
    return std::string(buffer);
}

// Отображение заголовка
void showHeader(const std::string& title) {
    clearScreen();
    std::cout << std::string(60, '=') << "\n";
    std::cout << "     ИНФОРМАЦИОННО-ПОИСКОВАЯ СИСТЕМА \"АВТОСЕРВИС\"\n";
    if (currentUser) {
        std::cout << "     Пользователь: " << currentUser->getName()
                  << " (" << currentUser->getRoleString() << ")\n";
    }
    std::cout << std::string(60, '=') << "\n\n";
    std::cout << "     " << title << "\n";
    //std::cout << std::string(60, '-') << "\n\n";
    std::cout << std::string(60, ' ') << "\n";
}

// ====================== АВТОРИЗАЦИЯ ======================

// Чтение пароля с маскировкой звездочками
std::string readPassword(const std::string& prompt) {
    std::cout << prompt;

    std::string password;
    char ch;
    bool showStars = true; // флаг для показа звездочек

    while (true) {
        ch = _getch();

        if (ch == 13 || ch == 10) { // Enter
            std::cout << std::endl;
            break;
        }
        else if (ch == 8 || ch == 127) { // Backspace
            if (!password.empty()) {
                password.pop_back();
                if (showStars) {
                    std::cout << "\b \b";
                }
            }
        }
        else if (ch == 3 || ch == 27) { // Ctrl+C или Escape
            password.clear();
            std::cout << std::endl;
            return password;
        }
        else if (ch == 9) { // Tab - переключение видимости пароля
            showStars = !showStars;
            // Перерисовываем строку
            std::cout << "\r" << prompt;
            for (size_t i = 0; i < password.length(); i++) {
                std::cout << (showStars ? '*' : password[i]);
            }
        }
        else if (ch >= 32 && ch <= 126) { // печатные символы
            password.push_back(ch);
            std::cout << (showStars ? '*' : ch);
        }
    }

    return password;
}
bool login() {
    DataManager& dm = DataManager::getInstance();
    int attempts = 0;
    const int MAX_ATTEMPTS = 3;
    
    while (attempts < MAX_ATTEMPTS) {
        clearScreen();
        std::cout << std::string(60, '=') << "\n";
        std::cout << "       ИНФОРМАЦИОННО-ПОИСКОВАЯ СИСТЕМА \"АВТОСЕРВИС\"\n";
        std::cout << "                   Авторизация\n";
        std::cout << std::string(60, '=') << "\n\n";
        
        if (attempts > 0) {
            std::cout << "  Осталось попыток: " << (MAX_ATTEMPTS - attempts) << "\n\n";
        }
        
        std::string login = readLine("  Логин: ");
        std::string password = readPassword("  Пароль: ");
        
        const User* user = dm.findUserByLogin(login);
        
        if (user == nullptr) {
            std::cout << "\n  ОШИБКА: Пользователь не найден!\n";
            attempts++;
            pause();
            continue;
        }
        
        if (!user->checkPassword(password)) {
            std::cout << "\n  ОШИБКА: Неверный пароль!\n";
            attempts++;
            pause();
            continue;
        }
        
        currentUser = user;
        std::cout << "\n  Добро пожаловать, " << user->getName() << "!\n";
        pause();
        return true;
    }
    
    std::cout << "\n  Доступ заблокирован. Обратитесь к администратору.\n";
    pause();
    return false;
}

// ====================== РАБОТА С КЛИЕНТАМИ ======================

void showClients() {
    showHeader("СПИСОК КЛИЕНТОВ");
    DataManager& dm = DataManager::getInstance();
    auto& clients = dm.getClients();
    
    if (clients.empty()) {
        std::cout << "  Список клиентов пуст.\n";
    } else {
        std::cout << std::left << std::setw(6) << "  ID"
                  << std::setw(38) << "ФИО"
                  << std::setw(18) << "Телефон"
                  << "Скидка\n";
        std::cout << "  " << std::string(65, '-') << "\n";
        
        for (const auto& c : clients) {
            std::cout << "  " << std::left << std::setw(5) << c.getId()
                      << std::setw(38) << c.getName()
                      << std::setw(18) << c.getPhone()
                      << std::fixed << std::setprecision(1) << c.getDiscount() << "%\n";
        }
        std::cout << "\n  Всего клиентов: " << clients.size() << "\n";
    }
    pause();
}

void addClient() {
    showHeader("ДОБАВЛЕНИЕ КЛИЕНТА");
    DataManager& dm = DataManager::getInstance();
    
    std::string name = readLine("  ФИО: ");
    if (name.length() < 3) {
        std::cout << "  ОШИБКА: ФИО должно содержать минимум 3 символа!\n";
        pause();
        return;
    }
    
    std::string phone = readLine("  Телефон: ");
    double discount = readDouble("  Скидка (%): ", 0.0);
    
    if (discount > 100) {
        std::cout << "  ОШИБКА: Скидка не может превышать 100%!\n";
        pause();
        return;
    }
    
    int newId = Person::getNextId();
    Client client(newId, name, phone, discount);
    dm.addClient(client);
    dm.saveAll();
    
    std::cout << "\n  Клиент успешно добавлен! ID: " << newId << "\n";
    pause();
}

void editClient() {
    showHeader("РЕДАКТИРОВАНИЕ КЛИЕНТА");
    DataManager& dm = DataManager::getInstance();
    
    int id = readInt("  Введите ID клиента: ");
    const Client* client = dm.findClientById(id);
    
    if (!client) {
        std::cout << "  ОШИБКА: Клиент не найден!\n";
        pause();
        return;
    }
    
    std::cout << "\n  Текущие данные:\n";
    std::cout << "  ФИО: " << client->getName() << "\n";
    std::cout << "  Телефон: " << client->getPhone() << "\n";
    std::cout << "  Скидка: " << client->getDiscount() << "%\n\n";
    
    std::cout << "  Введите новые данные (Enter - оставить без изменений):\n";
    
    std::string name = readLine("  ФИО: ");
    std::string phone = readLine("  Телефон: ");
    std::string discountStr = readLine("  Скидка (%): ");
    
    Client updated = *client;
    if (!name.empty()) updated.setName(name);
    if (!phone.empty()) updated.setPhone(phone);
    if (!discountStr.empty()) updated.setDiscount(std::stod(discountStr));
    
    dm.updateClient(id, updated);
    dm.saveAll();
    
    std::cout << "\n  Данные клиента обновлены!\n";
    pause();
}

void deleteClient() {
    showHeader("УДАЛЕНИЕ КЛИЕНТА");
    DataManager& dm = DataManager::getInstance();
    
    int id = readInt("  Введите ID клиента: ");
    const Client* client = dm.findClientById(id);
    
    if (!client) {
        std::cout << "  ОШИБКА: Клиент не найден!\n";
        pause();
        return;
    }
    
    std::cout << "\n  Клиент: " << client->getName() << "\n";
    std::cout << "  ВНИМАНИЕ: Будут удалены все связанные данные!\n\n";
    
    if (confirm("  Подтвердите удаление")) {
        dm.deleteClient(id);
        dm.saveAll();
        std::cout << "\n  Клиент удалён.\n";
    } else {
        std::cout << "\n  Операция отменена.\n";
    }
    pause();
}

void clientsMenu() {
    while (true) {
        showHeader("КЛИЕНТЫ");
        std::cout << "  1. Просмотр всех клиентов\n";
        std::cout << "  2. Добавить клиента\n";
        std::cout << "  3. Редактировать клиента\n";
        std::cout << "  4. Удалить клиента\n";
        std::cout << "  " << std::string(30, '-') << "\n";
        std::cout << "  0. Назад\n\n";
        
        int choice = readInt("  Выберите пункт меню: ", 0, 4);
        
        switch (choice) {
            case 1: showClients(); break;
            case 2: addClient(); break;
            case 3: editClient(); break;
            case 4: deleteClient(); break;
            case 0: return;
        }
    }
}

// ====================== РАБОТА С АВТОМОБИЛЯМИ ======================

void showCars() {
    showHeader("СПИСОК АВТОМОБИЛЕЙ");
    DataManager& dm = DataManager::getInstance();
    auto& cars = dm.getCars();
    
    if (cars.empty()) {
        std::cout << "  Список автомобилей пуст.\n";
    } else {
        std::cout << std::left << std::setw(6) << "  ID"
                  << std::setw(15) << "Марка"
                  << std::setw(15) << "Модель"
                  << std::setw(8) << "Год"
                  << std::setw(15) << "Госномер"
                  << "Владелец\n";
        std::cout << "  " << std::string(70, '-') << "\n";
        
        for (const auto& car : cars) {
            const Client* owner = dm.findClientById(car.getClientId());
            std::string ownerName = owner ? owner->getName() : "Неизвестен";
            // Сокращаем имя до фамилии
            size_t spacePos = ownerName.find(' ');
            if (spacePos != std::string::npos) {
                ownerName = ownerName.substr(0, spacePos);
            }
            
            std::cout << "  " << std::left << std::setw(5) << car.getId()
                      << std::setw(15) << car.getBrand()
                      << std::setw(15) << car.getModel()
                      << std::setw(8) << car.getYear()
                      << std::setw(15) << car.getRegNumber()
                      << ownerName << "\n";
        }
        std::cout << "\n  Всего автомобилей: " << cars.size() << "\n";
    }
    pause();
}

void addCar() {
    showHeader("ДОБАВЛЕНИЕ АВТОМОБИЛЯ");
    DataManager& dm = DataManager::getInstance();
    
    // Показываем список клиентов
    auto& clients = dm.getClients();
    if (clients.empty()) {
        std::cout << "  Сначала добавьте клиента!\n";
        pause();
        return;
    }
    
    std::cout << "  Список клиентов:\n";
    for (const auto& c : clients) {
        std::cout << "  " << c.getId() << ". " << c.getName() << "\n";
    }
    std::cout << "\n";
    
    int clientId = readInt("  ID клиента: ");
    if (!dm.findClientById(clientId)) {
        std::cout << "  ОШИБКА: Клиент не найден!\n";
        pause();
        return;
    }
    
    std::string brand = readLine("  Марка: ");
    std::string model = readLine("  Модель: ");
    int year = readInt("  Год выпуска: ", 1900, 2100);
    std::string regNumber = readLine("  Госномер: ");
    
    int newId = Car::getNextId();
    Car car(newId, brand, model, year, regNumber, clientId);
    dm.addCar(car);
    dm.saveAll();
    
    std::cout << "\n  Автомобиль добавлен! ID: " << newId << "\n";
    pause();
}

void carsMenu() {
    while (true) {
        showHeader("АВТОМОБИЛИ");
        std::cout << "  1. Просмотр всех автомобилей\n";
        std::cout << "  2. Добавить автомобиль\n";
        std::cout << "  3. Редактировать автомобиль\n";
        std::cout << "  4. Удалить автомобиль\n";
        std::cout << "  " << std::string(30, '-') << "\n";
        std::cout << "  0. Назад\n\n";
        
        int choice = readInt("  Выберите пункт меню: ", 0, 4);
        
        switch (choice) {
            case 1: showCars(); break;
            case 2: addCar(); break;
            case 3: 
                std::cout << "  Функция редактирования автомобиля\n";
                pause();
                break;
            case 4:
                std::cout << "  Функция удаления автомобиля\n";
                pause();
                break;
            case 0: return;
        }
    }
}

// ====================== РАБОТА С ЗАЯВКАМИ ======================

void showOrders() {
    showHeader("СПИСОК ЗАЯВОК");
    DataManager& dm = DataManager::getInstance();
    auto& orders = dm.getOrders();
    
    if (orders.empty()) {
        std::cout << "  Список заявок пуст.\n";
    } else {
        std::cout << std::left << std::setw(5) << "  ID"
                  << std::setw(25) << "Клиент"
                  << std::setw(20) << "Автомобиль"
                  << std::setw(12) << "Дата"
                  << std::setw(12) << "Статус"
                  << "Сумма\n";
        std::cout << "  " << std::string(81, '-') << "\n";
        
        for (const auto& order : orders) {
            const Client* client = dm.findClientById(order.getClientId());
            const Car* car = dm.findCarById(order.getCarId());
            
            std::string clientName = client ? client->getName() : "?";
            size_t spacePos = clientName.find(' ');
            if (spacePos != std::string::npos) {
                //clientName = clientName.substr(0, spacePos);
            }
            
            std::string carInfo = car ? car->getShortInfo() : "?";
            
            std::cout << "  " << std::left << std::setw(3) << order.getId()
                      << std::setw(25) << clientName.substr(0, 24)
                      << std::setw(20) << carInfo.substr(0, 19)
                      << std::setw(12) << order.getDate()
                      << std::setw(12) << order.getStatusString()
                      << std::fixed << std::setprecision(2) << order.getTotalCost() << " BYN\n";
        }
        std::cout << "\n  Всего заявок: " << orders.size() << "\n";
    }
    pause();
}

void createOrder() {
    showHeader("СОЗДАНИЕ ЗАЯВКИ");
    DataManager& dm = DataManager::getInstance();
    
    // Шаг 1: Выбор клиента
    auto& clients = dm.getClients();
    if (clients.empty()) {
        std::cout << "  Сначала добавьте клиента!\n";
        pause();
        return;
    }
    
    std::cout << "  ШАГ 1: Выбор клиента\n";
    std::cout << "  " << std::string(40, '-') << "\n";
    for (const auto& c : clients) {
        std::cout << "  " << c.getId() << ". " << c.getName() << "\n";
    }
    
    int clientId = readInt("\n  ID клиента: ");
    const Client* client = dm.findClientById(clientId);
    if (!client) {
        std::cout << "  ОШИБКА: Клиент не найден!\n";
        pause();
        return;
    }
    
    // Шаг 2: Выбор автомобиля
    std::cout << "\n  ШАГ 2: Выбор автомобиля\n";
    std::cout << "  " << std::string(40, '-') << "\n";
    auto clientCars = dm.getCarsByClientId(clientId);
    if (clientCars.empty()) {
        std::cout << "  У клиента нет автомобилей. Добавьте автомобиль.\n";
        pause();
        return;
    }
    
    for (const auto& car : clientCars) {
        std::cout << "  " << car.getId() << ". " << car.getInfo() << "\n";
    }
    
    int carId = readInt("\n  ID автомобиля: ");
    const Car* car = dm.findCarById(carId);
    if (!car || car->getClientId() != clientId) {
        std::cout << "  ОШИБКА: Автомобиль не найден!\n";
        pause();
        return;
    }
    
    // Шаг 3: Выбор услуг
    std::cout << "\n  ШАГ 3: Выбор услуг\n";
    std::cout << "  " << std::string(40, '-') << "\n";
    auto& services = dm.getServiceTypes();
    for (const auto& s : services) {
        std::cout << "  " << s.getId() << ". " << s.getName() 
                  << " - " << std::fixed << std::setprecision(2) << s.getPrice() << " BYN\n";
    }
    
    std::cout << "\n  Введите ID услуг через запятую: ";
    std::string servicesInput;
    std::getline(std::cin, servicesInput);
    
    std::vector<int> serviceIds;
    std::istringstream iss(servicesInput);
    std::string token;
    while (std::getline(iss, token, ',')) {
        try {
            int serviceId = std::stoi(token);
            if (dm.findServiceTypeById(serviceId)) {
                serviceIds.push_back(serviceId);
            }
        } catch (...) {}
    }
    
    if (serviceIds.empty()) {
        std::cout << "  ОШИБКА: Выберите хотя бы одну услугу!\n";
        pause();
        return;
    }
    
    // Шаг 4: Выбор исполнителя и даты
    std::cout << "\n  ШАГ 4: Исполнитель и дата\n";
    std::cout << "  " << std::string(40, '-') << "\n";
    auto& employees = dm.getEmployees();
    for (const auto& e : employees) {
        std::cout << "  " << e.getId() << ". " << e.getName() << " (" << e.getPosition() << ")\n";
    }
    
    int employeeId = readInt("\n  ID исполнителя: ");
    if (!dm.findEmployeeById(employeeId)) {
        std::cout << "  ОШИБКА: Сотрудник не найден!\n";
        pause();
        return;
    }
    
    std::string date = readLine("  Дата (ДД.ММ.ГГГГ) [Enter - сегодня]: ");
    if (date.empty()) {
        date = getCurrentDate();
    }
    
    // Расчёт стоимости
    double totalCost = dm.calculateOrderCost(serviceIds, client->getDiscount());
    
    // Подтверждение
    std::cout << "\n  " << std::string(40, '=') << "\n";
    std::cout << "  ИТОГО:\n";
    std::cout << "  Клиент: " << client->getName() << "\n";
    std::cout << "  Автомобиль: " << car->getInfo() << "\n";
    std::cout << "  Услуг: " << serviceIds.size() << "\n";
    std::cout << "  Скидка: " << client->getDiscount() << "%\n";
    std::cout << "  Стоимость: " << std::fixed << std::setprecision(2) << totalCost << " BYN\n";
    std::cout << "  " << std::string(40, '=') << "\n";
    
    if (confirm("\n  Создать заявку?")) {
        int newId = ServiceOrder::getNextId();
        ServiceOrder order(newId, clientId, carId, employeeId, date, 
                           OrderStatus::NEW, serviceIds, totalCost);
        dm.addOrder(order);
        dm.saveAll();
        
        std::cout << "\n  Заявка создана! Номер: " << newId << "\n";
    } else {
        std::cout << "\n  Операция отменена.\n";
    }
    pause();
}

void changeOrderStatus() {
    showHeader("ИЗМЕНЕНИЕ СТАТУСА ЗАЯВКИ");
    DataManager& dm = DataManager::getInstance();
    
    int id = readInt("  Введите ID заявки: ");
    const ServiceOrder* order = dm.findOrderById(id);
    
    if (!order) {
        std::cout << "  ОШИБКА: Заявка не найдена!\n";
        pause();
        return;
    }
    
    std::cout << "\n  Заявка #" << id << "\n";
    std::cout << "  Текущий статус: " << order->getStatusString() << "\n\n";
    
    std::cout << "  Выберите новый статус:\n";
    std::cout << "  1. Новая\n";
    std::cout << "  2. В работе\n";
    std::cout << "  3. Выполнена\n";
    std::cout << "  4. Выдана\n\n";
    
    int choice = readInt("  Ваш выбор: ", 1, 4);
    
    OrderStatus newStatus;
    switch (choice) {
        case 1: newStatus = OrderStatus::NEW; break;
        case 2: newStatus = OrderStatus::IN_PROGRESS; break;
        case 3: newStatus = OrderStatus::COMPLETED; break;
        case 4: newStatus = OrderStatus::ISSUED; break;
        default: return;
    }
    
    ServiceOrder updated = *order;
    updated.setStatus(newStatus);
    dm.updateOrder(id, updated);
    dm.saveAll();
    
    std::cout << "\n  Статус изменён на: " << ServiceOrder::statusToString(newStatus) << "\n";
    pause();
}

void editOrder() {
    showHeader("РЕДАКТИРОВАНИЕ ЗАЯВКИ");
    DataManager& dm = DataManager::getInstance();

    auto& orders = dm.getOrders();
    if (orders.empty()) {
        std::cout << "  Нет заявок для редактирования.\n";
        pause();
        return;
    }

    std::cout << "  Список заявок:\n";
    std::cout << "  " << std::string(70, '-') << "\n";
    std::cout << "  " << std::left << std::setw(5) << "ID"
        << std::setw(20) << "Клиент"
        << std::setw(15) << "Авто"
        << std::setw(12) << "Дата"
        << std::setw(12) << "Статус" << "\n";
    std::cout << "  " << std::string(70, '-') << "\n";

    for (const auto& o : orders) {
        const Client* client = dm.findClientById(o.getClientId());
        const Car* car = dm.findCarById(o.getCarId());
        std::string clientName = client ? client->getName() : "???";
        std::string carInfo = car ? (car->getBrand() + " " + car->getModel()) : "???";
        if (clientName.length() > 18) clientName = clientName.substr(0, 18) + "..";
        if (carInfo.length() > 13) carInfo = carInfo.substr(0, 13) + "..";

        std::cout << "  " << std::left << std::setw(5) << o.getId()
            << std::setw(20) << clientName
            << std::setw(15) << carInfo
            << std::setw(12) << o.getDate()
            << std::setw(12) << o.getStatusString() << "\n";
    }
    std::cout << "\n";

    int id = readInt("  Введите ID заявки: ");
    const ServiceOrder* order = dm.findOrderById(id);

    if (!order) {
        std::cout << "  ОШИБКА: Заявка не найдена!\n";
        pause();
        return;
    }

    const Client* client = dm.findClientById(order->getClientId());
    const Car* car = dm.findCarById(order->getCarId());
    const Employee* employee = dm.findEmployeeById(order->getEmployeeId());

    std::cout << "\n  " << std::string(50, '=') << "\n";
    std::cout << "  ТЕКУЩИЕ ДАННЫЕ ЗАЯВКИ #" << id << "\n";
    std::cout << "  " << std::string(50, '=') << "\n";
    std::cout << "  Клиент:     " << (client ? client->getName() : "???") << "\n";
    std::cout << "  Автомобиль: " << (car ? car->getInfo() : "???") << "\n";
    std::cout << "  Исполнитель: " << (employee ? employee->getName() : "???") << "\n";
    std::cout << "  Дата:       " << order->getDate() << "\n";
    std::cout << "  Статус:     " << order->getStatusString() << "\n";
    std::cout << "  Услуги:     ";

    auto serviceIds = order->getServiceIds();
    for (size_t i = 0; i < serviceIds.size(); i++) {
        const ServiceType* service = dm.findServiceTypeById(serviceIds[i]);
        if (service) {
            std::cout << service->getName();
            if (i < serviceIds.size() - 1) std::cout << ", ";
        }
    }
    std::cout << "\n";
    std::cout << "  Стоимость:  " << std::fixed << std::setprecision(2) << order->getTotalCost() << " BYN\n";
    std::cout << "  " << std::string(50, '-') << "\n\n";

    std::cout << "  Что редактировать?\n";
    std::cout << "  1. Исполнителя\n";
    std::cout << "  2. Дату\n";
    std::cout << "  3. Услуги\n";
    std::cout << "  0. Отмена\n\n";

    int editChoice = readInt("  Ваш выбор: ", 0, 3);

    if (editChoice == 0) {
        std::cout << "  Отменено.\n";
        pause();
        return;
    }

    ServiceOrder updated = *order;

    switch (editChoice) {
    case 1: {
        std::cout << "\n  Доступные сотрудники:\n";
        auto& employees = dm.getEmployees();
        for (const auto& e : employees) {
            std::cout << "  " << e.getId() << ". " << e.getName() << " (" << e.getPosition() << ")\n";
        }

        int newEmployeeId = readInt("\n  ID нового исполнителя: ");
        if (!dm.findEmployeeById(newEmployeeId)) {
            std::cout << "  ОШИБКА: Сотрудник не найден!\n";
            pause();
            return;
        }
        updated.setEmployeeId(newEmployeeId);
        break;
    }
    case 2: {
        std::string newDate = readLine("  Новая дата (ДД.ММ.ГГГГ): ");
        if (newDate.empty()) {
            std::cout << "  ОШИБКА: Дата не может быть пустой!\n";
            pause();
            return;
        }
        updated.setDate(newDate);
        break;
    }
    case 3: {
        std::cout << "\n  Доступные услуги:\n";
        auto& services = dm.getServiceTypes();
        for (const auto& s : services) {
            std::cout << "  " << std::left << std::setw(3) << s.getId() << ". "
                << std::setw(35) << s.getName()
                << std::fixed << std::setprecision(2) << s.getPrice() << " BYN\n";
        }

        std::string servicesInput = readLine("\n  ID услуг через запятую: ");
        std::vector<int> newServiceIds;
        std::stringstream ss(servicesInput);
        std::string token;

        while (std::getline(ss, token, ',')) {
            try {
                int serviceId = std::stoi(token);
                if (dm.findServiceTypeById(serviceId)) {
                    newServiceIds.push_back(serviceId);
                }
            }
            catch (...) {}
        }

        if (newServiceIds.empty()) {
            std::cout << "  ОШИБКА: Не выбраны корректные услуги!\n";
            pause();
            return;
        }

        // Clear old services and add new
        std::vector<int> oldIds = updated.getServiceIds();
        for (int sid : oldIds) {
            updated.removeService(sid);
        }
        for (int sid : newServiceIds) {
            updated.addService(sid);
        }

        double discount = client ? client->getDiscount() : 0.0;
        double newTotal = dm.calculateOrderCost(newServiceIds, discount);
        updated.setTotalCost(newTotal);
        break;
    }
    }

    dm.updateOrder(id, updated);
    dm.saveAll();

    std::cout << "\n  Заявка обновлена!\n";
    pause();
}

void deleteOrder() {
    showHeader("УДАЛЕНИЕ ЗАЯВКИ");
    DataManager& dm = DataManager::getInstance();

    int id = readInt("  Введите ID заявки: ");
    const ServiceOrder* order = dm.findOrderById(id);

    if (!order) {
        std::cout << "  ОШИБКА: Заявка не найдена!\n";
        pause();
        return;
    }

    const Client* client = dm.findClientById(order->getClientId());

    std::cout << "\n  Заявка #" << id << "\n";
    std::cout << "  Клиент: " << (client ? client->getName() : "???") << "\n";
    std::cout << "  Дата: " << order->getDate() << "\n";
    std::cout << "  Статус: " << order->getStatusString() << "\n";
    std::cout << "  Сумма: " << std::fixed << std::setprecision(2) << order->getTotalCost() << " BYN\n";

    if (confirm("\n  Подтвердите удаление")) {
        dm.deleteOrder(id);
        dm.saveAll();
        std::cout << "\n  Заявка удалена.\n";
    }
    else {
        std::cout << "\n  Операция отменена.\n";
    }
    pause();
}

void ordersMenu() {
    while (true) {
        showHeader("ЗАЯВКИ НА ОБСЛУЖИВАНИЕ");
        std::cout << "  1. Просмотр всех заявок\n";
        std::cout << "  2. Создать заявку\n";
        std::cout << "  3. Изменить статус заявки\n";
        std::cout << "  4. Редактировать заявку\n";
        std::cout << "  5. Удалить заявку\n";
        std::cout << "  " << std::string(30, '-') << "\n";
        std::cout << "  0. Назад\n\n";
        
        int choice = readInt("  Выберите пункт меню: ", 0, 5);
        
        switch (choice) {
            case 1: showOrders(); break;
            case 2: createOrder(); break;
            case 3: changeOrderStatus(); break;
            case 4: editOrder(); break;
            case 5: deleteOrder(); break;
            case 0: return;
        }
    }
}

// ====================== ПОИСК И ФИЛЬТРАЦИЯ ======================

void searchMenu() {
    while (true) {
        showHeader("ПОИСК И ФИЛЬТРАЦИЯ");
        std::cout << "  1. Поиск по ФИО клиента\n";
        std::cout << "  2. Фильтр по дате\n";
        std::cout << "  3. Фильтр по статусу\n";
        std::cout << "  " << std::string(30, '-') << "\n";
        std::cout << "  0. Назад\n\n";
        
        int choice = readInt("  Выберите пункт меню: ", 0, 3);
        DataManager& dm = DataManager::getInstance();
        
        switch (choice) {
            case 1: {
                std::string name = readLine("  Введите ФИО или часть ФИО: ");

                // 1. Найти всех клиентов по ФИО
                auto clients = dm.searchClientsByName(name);

                if (clients.empty()) {
                    std::cout << "\n  Клиенты не найдены.\n";
                    pause();
                    break;
                }

                // 2. Для каждого клиента найти его заказы
                std::vector<ServiceOrder> foundOrders;
                for (const auto& client : clients) {
                    auto clientOrders = dm.getOrdersByClientId(client.getId());
                    for (const auto& order : clientOrders) {
                        foundOrders.push_back(order);
                    }
                }

                std::cout << "\n  " << std::string(65, '-') << "\n";
                std::cout << "  ЗАЯВКИ КЛИЕНТОВ С ФИО: \"" << name << "\"\n";
                std::cout << "  " << std::string(65, '-') << "\n";

                if (foundOrders.empty()) {
                    std::cout << "  Заявки не найдены.\n";
                }
                else {
                    std::cout << "  Найдено заявок: " << foundOrders.size() << "\n\n";
                    std::cout << "  " << std::left << std::setw(5) << "ID"
                        << std::setw(22) << "Клиент"
                        << std::setw(12) << "Дата"
                        << std::setw(12) << "Статус"
                        << "Сумма\n";
                    std::cout << "  " << std::string(60, '-') << "\n";
                    for (const auto& o : foundOrders) {
                        const Client* client = dm.findClientById(o.getClientId());
                        std::string clientName = client ? client->getName() : "???";
                        if (clientName.length() > 20) clientName = clientName.substr(0, 20) + "..";
                        std::cout << "  " << std::left << std::setw(5) << o.getId()
                            << std::setw(22) << clientName
                            << std::setw(12) << o.getDate()
                            << std::setw(12) << o.getStatusString()
                            << std::fixed << std::setprecision(2) << o.getTotalCost() << " BYN\n";
                    }
                }
                pause();
                break;
            }
            case 2: {
                std::string date = readLine("  Введите дату (ДД.ММ.ГГГГ): ");
                auto orders = dm.getOrdersByDate(date);
                
                std::cout << "\n  Заявки на " << date << ":\n";
                if (orders.empty()) {
                    std::cout << "  Заявки не найдены.\n";
                } else {
                    for (const auto& o : orders) {
                        std::cout << "  " << o.getInfo() << "\n";
                    }
                }
                pause();
                break;
            }
            case 3: {
                std::cout << "  Выберите статус:\n";
                std::cout << "  1. Новая\n";
                std::cout << "  2. В работе\n";
                std::cout << "  3. Выполнена\n";
                std::cout << "  4. Выдана\n";
                int statusChoice = readInt("  Ваш выбор: ", 1, 4);
                
                OrderStatus status;
                switch (statusChoice) {
                    case 1: status = OrderStatus::NEW; break;
                    case 2: status = OrderStatus::IN_PROGRESS; break;
                    case 3: status = OrderStatus::COMPLETED; break;
                    case 4: status = OrderStatus::ISSUED; break;
                    default: return;
                }
                
                auto orders = dm.getOrdersByStatus(status);
                std::cout << "\n  Результаты:\n";
                if (orders.empty()) {
                    std::cout << "  Заявки не найдены.\n";
                } else {
                    for (const auto& o : orders) {
                        std::cout << "  " << o.getInfo() << "\n";
                    }
                }
                pause();
                break;
            }
            case 0: return;
        }
    }
}

// ====================== ОТЧЁТЫ ======================

void reportsMenu() {
    while (true) {
        showHeader("ОТЧЁТЫ");
        std::cout << "  1. Квитанция на выполненные работы\n";
        std::cout << "  2. Отчёт по выручке за период\n";
        std::cout << "  3. Статистика по видам работ\n";
        std::cout << "  " << std::string(30, '-') << "\n";
        std::cout << "  0. Назад\n\n";
        
        int choice = readInt("  Выберите пункт меню: ", 0, 3);
        
        switch (choice) {
            case 1: {
                int orderId = readInt("  Введите ID заявки: ");
                std::string receipt = ReportGenerator::generateReceipt(orderId);
                std::cout << receipt;
                
                if (confirm("\n  Сохранить в HTML?")) {
                    std::string filename = "data/Квитанция_" + std::to_string(orderId) + ".html";
                    if (ReportGenerator::saveReceiptToHtml(orderId, filename)) {
                        std::cout << "  Сохранено в " << filename << "\n";
                    }
                }
                pause();
                break;
            }
            case 2: {
                std::string startDate = readLine("  Начальная дата (ДД.ММ.ГГГГ): ");
                std::string endDate = readLine("  Конечная дата (ДД.ММ.ГГГГ): ");
                
                std::string report = ReportGenerator::generateRevenueReport(startDate, endDate);
                std::cout << report;
                pause();
                break;
            }
            case 3: {
                std::string startDate = readLine("  Начальная дата (ДД.ММ.ГГГГ): ");
                std::string endDate = readLine("  Конечная дата (ДД.ММ.ГГГГ): ");
                
                std::string report = ReportGenerator::generateServiceStatistics(startDate, endDate);
                std::cout << report;
                pause();
                break;
            }
            case 0: return;
        }
    }
}

// ====================== АДМИНИСТРИРОВАНИЕ ======================

void servicesMenu() {
    while (true) {
        showHeader("СПРАВОЧНИК ВИДОВ РАБОТ");
        DataManager& dm = DataManager::getInstance();
        auto& services = dm.getServiceTypes();
        
        std::cout << "  Текущий прейскурант:\n";
        std::cout << "  " << std::string(55, '-') << "\n";
        for (const auto& s : services) {
            std::cout << "  " << std::left << std::setw(5) << s.getId()
                      << std::setw(38) << s.getName() << " "
                      << std::fixed << std::setprecision(2) << s.getPrice() << " BYN\n";
        }
        std::cout << "\n";
        std::cout << "  1. Добавить вид работы\n";
        std::cout << "  2. Редактировать\n";
        std::cout << "  3. Удалить\n";
        std::cout << "  " << std::string(30, '-') << "\n";
        std::cout << "  0. Назад\n\n";
        
        int choice = readInt("  Выберите пункт меню: ", 0, 3);
        
        switch (choice) {
            case 1: {
                std::string name = readLine("  Наименование: ");
                if (name.empty()) {
                    std::cout << "  ОШИБКА: Наименование не может быть пустым!\n";
                    pause();
                    break;
                }
                double price = readDouble("  Цена (BYN): ");
                
                int newId = ServiceType::getNextId();
                ServiceType service(newId, name, price);
                dm.addServiceType(service);
                dm.saveAll();
                
                std::cout << "  Добавлено! ID: " << newId << "\n";
                pause();
                break;
            }
            case 2: {
                int id = readInt("  Введите ID вида работы: ");
                const ServiceType* service = dm.findServiceTypeById(id);
                
                if (!service) {
                    std::cout << "  ОШИБКА: Вид работы не найден!\n";
                    pause();
                    break;
                }
                
                std::cout << "\n  Текущие данные:\n";
                std::cout << "  Наименование: " << service->getName() << "\n";
                std::cout << "  Цена: " << std::fixed << std::setprecision(2) 
                          << service->getPrice() << " BYN\n\n";
                
                std::cout << "  Введите новые данные (Enter - оставить без изменений):\n";
                
                std::string name = readLine("  Наименование: ");
                std::string priceStr = readLine("  Цена (BYN): ");
                
                ServiceType updated = *service;
                if (!name.empty()) updated.setName(name);
                if (!priceStr.empty()) updated.setPrice(std::stod(priceStr));
                
                dm.updateServiceType(id, updated);
                dm.saveAll();
                
                std::cout << "\n  Данные обновлены!\n";
                pause();
                break;
            }
            case 3: {
                int id = readInt("  Введите ID вида работы: ");
                const ServiceType* service = dm.findServiceTypeById(id);
                
                if (!service) {
                    std::cout << "  ОШИБКА: Вид работы не найден!\n";
                    pause();
                    break;
                }
                
                std::cout << "\n  Вид работы: " << service->getName() << "\n";
                std::cout << "  Цена: " << std::fixed << std::setprecision(2) 
                          << service->getPrice() << " BYN\n";
                
                if (confirm("\n  Подтвердите удаление")) {
                    dm.deleteServiceType(id);
                    dm.saveAll();
                    std::cout << "\n  Вид работы удалён.\n";
                } else {
                    std::cout << "\n  Операция отменена.\n";
                }
                pause();
                break;
            }
            case 0: return;
        }
    }
}

void usersMenu() {
    while (true) {
        showHeader("УПРАВЛЕНИЕ ПОЛЬЗОВАТЕЛЯМИ");
        DataManager& dm = DataManager::getInstance();
        auto& users = dm.getUsers();
        
        std::cout << "  Список пользователей:\n";
        std::cout << "  " << std::string(58, '-') << "\n";
        for (const auto& u : users) {
            
            std::cout << "  " << std::left << std::setw(5) << u.getId()
                      << std::setw(25) << u.getName()
                      << std::setw(15) << u.getLogin()
                      << u.getRoleString() << "\n";
        }
        std::cout << "\n";
        std::cout << "  1. Добавить пользователя\n";
        std::cout << "  2. Изменить пароль\n";
        std::cout << "  3. Удалить пользователя\n";
        std::cout << "  " << std::string(30, '-') << "\n";
        std::cout << "  0. Назад\n\n";
        
        int choice = readInt("  Выберите пункт меню: ", 0, 3);
        
        switch (choice) {
            case 1: {
                std::string name = readLine("  ФИО: ");
                std::string phone = readLine("  Телефон: ");
                std::string login = readLine("  Логин: ");
                std::string password = readPassword("  Пароль: ");
                std::string password2 = readPassword("  Повторите ввод Пароля: ");
                if (password != password2) {
                    std::cout << "  Пароли не совпали. ";
                    password2 = readPassword("  Повторите ввод Пароля: ");
                    if (password != password2) {
                        std::cout << "  Пароли не совпали. Пользователь не добавлен!\n";
                        pause();
                        return;
                    }
                }
                std::cout << "  Роль (1 - Оператор, 2 - Администратор): ";
                int roleChoice = readInt("", 1, 2);
                UserRole role = (roleChoice == 2) ? UserRole::ROLE_ADMIN : UserRole::ROLE_USER;
                
                int newId = Person::getNextId();
                User user(newId, name, phone, login, password, role);
                dm.addUser(user);
                dm.saveAll();
                
                std::cout << "  Пользователь добавлен!\n";
                pause();
                break;
            }
            case 2: { 
                int id = readInt("  Введите ID пользователя: ");
                const User* u = dm.findUserById(id);

                if (!u) {
                    std::cout << "  ОШИБКА: Пользователь не найден!\n";
                    pause();
                    break;
                }
                std::cout << "\n  Пользователь:";
                std::cout << "  " << u->getId()
                    << "  " << u->getName()
                    << "  " << u->getLogin()
                    << "  (" << u->getRoleString() << ")\n";

                std::string password = readPassword("  Новый Пароль: ");
                std::string password2 = readPassword("  Повторите ввод Пароля: ");
                if (password != password2) {
                    std::cout << "  Пароли не совпали. ";
                    password2 = readPassword("  Повторите ввод Пароля: ");
                    if (password != password2) {
                        std::cout << "  Пароли не совпали. Пароль не изменен!\n";
                        pause();
                        return;
                    }
                }

                User updated = *u;
                if (!password.empty()) updated.setPassword(password);
                dm.updateUser(id, updated);
                dm.saveAll();

                std::cout << "\n  Пароль изменен!\n";
                pause();
                break;
            }
            case 3: {
                int id = readInt("  Введите ID пользователя: ");
                const User* u = dm.findUserById(id);

                if (!u) {
                    std::cout << "  ОШИБКА: Пользователь не найден!\n";
                    pause();
                    break;
                }

                std::cout << "\n  Пользователь:";
                std::cout << "  " << u->getId()
                    << "  " << u->getName()
                    << "  " << u->getLogin()
                    << "  (" << u->getRoleString() << ")\n";
                if (confirm("\n  Подтвердите удаление")) {
                    dm.deleteUser(id);
                    dm.saveAll();
                    std::cout << "\n  Пользователь удалён.\n";
                }
                else {
                    std::cout << "\n  Операция отменена.\n";
                }
                pause();
                break;
            }
            case 0:
                return;
                break;
            default:
                std::cout << "  Функция в разработке\n";
                pause();
                return;
                break;
        }
    }
}

// ====================== ГЛАВНОЕ МЕНЮ ======================

void mainMenu() {
    while (true) {
        showHeader("ГЛАВНОЕ МЕНЮ");
        
        std::cout << "  1. Клиенты\n";
        std::cout << "  2. Автомобили\n";
        std::cout << "  3. Заявки на обслуживание\n";
        std::cout << "  4. Поиск и фильтрация\n";
        std::cout << "  5. Отчёты\n";
        
        if (currentUser && currentUser->getRole() == UserRole::ROLE_ADMIN) {
            std::cout << "  " << std::string(30, '-') << "\n";
            std::cout << "  6. Справочник видов работ\n";
            std::cout << "  7. Сотрудники\n";
            std::cout << "  8. Управление пользователями\n";
        }
        
        std::cout << "  " << std::string(30, '-') << "\n";
        std::cout << "  0. Выход из системы\n\n";
        
        int maxChoice = (currentUser && currentUser->getRole() == UserRole::ROLE_ADMIN) ? 8 : 5;
        int choice = readInt("  Выберите пункт меню: ", 0, maxChoice);
        
        switch (choice) {
            case 1: clientsMenu(); break;
            case 2: carsMenu(); break;
            case 3: ordersMenu(); break;
            case 4: searchMenu(); break;
            case 5: reportsMenu(); break;
            case 6: 
                if (currentUser && currentUser->getRole() == UserRole::ROLE_ADMIN) {
                    servicesMenu(); 
                }
                break;
            case 7:
                if (currentUser && currentUser->getRole() == UserRole::ROLE_ADMIN) {
                    std::cout << "  Меню сотрудников\n";
                    pause();
                }
                break;
            case 8:
                if (currentUser && currentUser->getRole() == UserRole::ROLE_ADMIN) {
                    usersMenu();
                }
                break;
            case 0:
                //if (confirm("  Выйти из системы?")) {
                DataManager::getInstance().saveAll();
                currentUser = nullptr;
                return;
                break;
        }
    }
}

// ====================== ТОЧКА ВХОДА ======================

int main() {
    setupConsole();
    
    // Загрузка данных
    DataManager& dm = DataManager::getInstance();
    dm.loadAll();
    
    // Авторизация
    if (login()) {
        mainMenu();
    }
    
    // Сохранение данных при выходе
    dm.saveAll();
    
    //std::cout << "\n  Спасибо за работу!\n\n";
    return 0;
}
