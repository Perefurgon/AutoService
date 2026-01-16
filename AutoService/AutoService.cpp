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
string readLine(const string& prompt) {
    cout << prompt;
    string line;
    getline(cin, line);
    return line;
}

// Чтение целого числа с валидацией
int readInt(const string& prompt, int minVal = INT_MIN, int maxVal = INT_MAX) {
    while (true) {
        cout << prompt;
        int value;
        if (cin >> value && value >= minVal && value <= maxVal) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Ошибка ввода! Повторите.\n";
    }
}

// Чтение числа с плавающей точкой
double readDouble(const string& prompt, double minVal = 0.0) {
    while (true) {
        cout << prompt;
        double value;
        if (cin >> value && value >= minVal) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Ошибка ввода! Повторите.\n";
    }
}

// Подтверждение действия
bool confirm(const string& message) {
    cout << message << " (Да/Нет): ";
    string answer;
    getline(cin, answer);
    return (answer == "д" || answer == "Д" || answer == "да" || answer == "Да" || answer == "ДА" || answer == "y" || answer == "Y");
}

// Получение текущей даты
string getCurrentDate() {
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
    return string(buffer);
}

// Отображение заголовка
void showHeader(const string& title) {
    clearScreen();
    cout << string(60, '=') << "\n";
    cout << "     ИНФОРМАЦИОННО-ПОИСКОВАЯ СИСТЕМА \"АВТОСЕРВИС\"\n";
    if (currentUser) {
        cout << "     Пользователь: " << currentUser->getName()
                  << " (" << currentUser->getRoleString() << ")\n";
    }
    cout << string(60, '=') << "\n\n";
    cout << "     " << title << "\n";
    //cout << string(60, '-') << "\n\n";
    cout << string(60, ' ') << "\n";
}

// ====================== АВТОРИЗАЦИЯ ======================

// Чтение пароля с маскировкой звездочками
string readPassword(const string& prompt) {
    cout << prompt;

    string password;
    char ch;
    bool showStars = true; // флаг для показа звездочек

    while (true) {
        ch = _getch();

        if (ch == 13 || ch == 10) { // Enter
            cout << endl;
            break;
        }
        else if (ch == 8 || ch == 127) { // Backspace
            if (!password.empty()) {
                password.pop_back();
                if (showStars) {
                    cout << "\b \b";
                }
            }
        }
        else if (ch == 3 || ch == 27) { // Ctrl+C или Escape
            password.clear();
            cout << endl;
            return password;
        }
        else if (ch == 9) { // Tab - переключение видимости пароля
            showStars = !showStars;
            // Перерисовываем строку
            cout << "\r" << prompt;
            for (size_t i = 0; i < password.length(); i++) {
                cout << (showStars ? '*' : password[i]);
            }
        }
        else if (ch >= 32 && ch <= 126) { // печатные символы
            password.push_back(ch);
            cout << (showStars ? '*' : ch);
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
        cout << string(60, '=') << "\n";
        cout << "       ИНФОРМАЦИОННО-ПОИСКОВАЯ СИСТЕМА \"АВТОСЕРВИС\"\n";
        cout << "                   Авторизация\n";
        cout << string(60, '=') << "\n\n";
        
        if (attempts > 0) {
            cout << "  Осталось попыток: " << (MAX_ATTEMPTS - attempts) << "\n\n";
        }
        
        string login = readLine("  Логин: ");
        string password = readPassword("  Пароль: ");
        
        const User* user = dm.findUserByLogin(login);
        
        if (user == nullptr) {
            cout << "\n  ОШИБКА: Пользователь не найден!\n";
            attempts++;
            pause();
            continue;
        }
        
        if (!user->checkPassword(password)) {
            cout << "\n  ОШИБКА: Неверный пароль!\n";
            attempts++;
            pause();
            continue;
        }
        
        currentUser = user;
        cout << "\n  Добро пожаловать, " << user->getName() << "!\n";
        pause();
        return true;
    }
    
    cout << "\n  Доступ заблокирован. Обратитесь к администратору.\n";
    pause();
    return false;
}

// ====================== РАБОТА С КЛИЕНТАМИ ======================

void showClients() {
    showHeader("СПИСОК КЛИЕНТОВ");
    DataManager& dm = DataManager::getInstance();
    auto& clients = dm.getClients();
    
    if (clients.empty()) {
        cout << "  Список клиентов пуст.\n";
    } else {
        cout << left << setw(6) << "  ID"
                  << setw(38) << "ФИО"
                  << setw(18) << "Телефон"
                  << "Скидка\n";
        cout << "  " << string(65, '-') << "\n";
        
        for (const auto& c : clients) {
            cout << "  " << left << setw(5) << c.getId()
                      << setw(38) << c.getName()
                      << setw(18) << c.getPhone()
                      << fixed << setprecision(1) << c.getDiscount() << "%\n";
        }
        cout << "\n  Всего клиентов: " << clients.size() << "\n";
    }
    pause();
}

void addClient() {
    showHeader("ДОБАВЛЕНИЕ КЛИЕНТА");
    DataManager& dm = DataManager::getInstance();
    
    string name = readLine("  ФИО: ");
    if (name.length() < 3) {
        cout << "  ОШИБКА: ФИО должно содержать минимум 3 символа!\n";
        pause();
        return;
    }
    
    string phone = readLine("  Телефон: ");
    double discount = readDouble("  Скидка (%): ", 0.0);
    
    if (discount > 100) {
        cout << "  ОШИБКА: Скидка не может превышать 100%!\n";
        pause();
        return;
    }
    
    int newId = Person::getNextId();
    Client client(newId, name, phone, discount);
    dm.addClient(client);
    dm.saveAll();
    
    cout << "\n  Клиент успешно добавлен! ID: " << newId << "\n";
    pause();
}

void editClient() {
    showHeader("РЕДАКТИРОВАНИЕ КЛИЕНТА");
    DataManager& dm = DataManager::getInstance();
    
    int id = readInt("  Введите ID клиента: ");
    const Client* client = dm.findClientById(id);
    
    if (!client) {
        cout << "  ОШИБКА: Клиент не найден!\n";
        pause();
        return;
    }
    
    cout << "\n  Текущие данные:\n";
    cout << "  ФИО: " << client->getName() << "\n";
    cout << "  Телефон: " << client->getPhone() << "\n";
    cout << "  Скидка: " << client->getDiscount() << "%\n\n";
    
    cout << "  Введите новые данные (Enter - оставить без изменений):\n";
    
    string name = readLine("  ФИО: ");
    string phone = readLine("  Телефон: ");
    string discountStr = readLine("  Скидка (%): ");
    
    Client updated = *client;
    if (!name.empty()) updated.setName(name);
    if (!phone.empty()) updated.setPhone(phone);
    if (!discountStr.empty()) updated.setDiscount(stod(discountStr));
    
    dm.updateClient(id, updated);
    dm.saveAll();
    
    cout << "\n  Данные клиента обновлены!\n";
    pause();
}

void deleteClient() {
    showHeader("УДАЛЕНИЕ КЛИЕНТА");
    DataManager& dm = DataManager::getInstance();
    
    int id = readInt("  Введите ID клиента: ");
    const Client* client = dm.findClientById(id);
    
    if (!client) {
        cout << "  ОШИБКА: Клиент не найден!\n";
        pause();
        return;
    }
    
    cout << "\n  Клиент: " << client->getName() << "\n";
    cout << "  ВНИМАНИЕ: Будут удалены все связанные данные!\n\n";
    
    if (confirm("  Подтвердите удаление")) {
        dm.deleteClient(id);
        dm.saveAll();
        cout << "\n  Клиент удалён.\n";
    } else {
        cout << "\n  Операция отменена.\n";
    }
    pause();
}

void clientsMenu() {
    while (true) {
        showHeader("КЛИЕНТЫ");
        cout << "  1. Просмотр всех клиентов\n";
        cout << "  2. Добавить клиента\n";
        cout << "  3. Редактировать клиента\n";
        cout << "  4. Удалить клиента\n";
        cout << "  " << string(30, '-') << "\n";
        cout << "  0. Назад\n\n";
        
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
        cout << "  Список автомобилей пуст.\n";
    } else {
        cout << left << setw(6) << "  ID"
                  << setw(15) << "Марка"
                  << setw(15) << "Модель"
                  << setw(8) << "Год"
                  << setw(15) << "Госномер"
                  << "Владелец\n";
        cout << "  " << string(70, '-') << "\n";
        
        for (const auto& car : cars) {
            const Client* owner = dm.findClientById(car.getClientId());
            string ownerName = owner ? owner->getName() : "Неизвестен";
            // Сокращаем имя до фамилии
            size_t spacePos = ownerName.find(' ');
            if (spacePos != string::npos) {
                ownerName = ownerName.substr(0, spacePos);
            }
            
            cout << "  " << left << setw(5) << car.getId()
                      << setw(15) << car.getBrand()
                      << setw(15) << car.getModel()
                      << setw(8) << car.getYear()
                      << setw(15) << car.getRegNumber()
                      << ownerName << "\n";
        }
        cout << "\n  Всего автомобилей: " << cars.size() << "\n";
    }
    pause();
}

void addCar() {
    showHeader("ДОБАВЛЕНИЕ АВТОМОБИЛЯ");
    DataManager& dm = DataManager::getInstance();
    
    // Показываем список клиентов
    auto& clients = dm.getClients();
    if (clients.empty()) {
        cout << "  Сначала добавьте клиента!\n";
        pause();
        return;
    }
    
    cout << "  Список клиентов:\n";
    for (const auto& c : clients) {
        cout << "  " << c.getId() << ". " << c.getName() << "\n";
    }
    cout << "\n";
    
    int clientId = readInt("  ID клиента: ");
    if (!dm.findClientById(clientId)) {
        cout << "  ОШИБКА: Клиент не найден!\n";
        pause();
        return;
    }
    
    string brand = readLine("  Марка: ");
    string model = readLine("  Модель: ");
    int year = readInt("  Год выпуска: ", 1900, 2100);
    string regNumber = readLine("  Госномер: ");
    
    int newId = Car::getNextId();
    Car car(newId, brand, model, year, regNumber, clientId);
    dm.addCar(car);
    dm.saveAll();
    
    cout << "\n  Автомобиль добавлен! ID: " << newId << "\n";
    pause();
}

void carsMenu() {
    while (true) {
        showHeader("АВТОМОБИЛИ");
        cout << "  1. Просмотр всех автомобилей\n";
        cout << "  2. Добавить автомобиль\n";
        cout << "  3. Редактировать автомобиль\n";
        cout << "  4. Удалить автомобиль\n";
        cout << "  " << string(30, '-') << "\n";
        cout << "  0. Назад\n\n";
        
        int choice = readInt("  Выберите пункт меню: ", 0, 4);
        
        switch (choice) {
            case 1: showCars(); break;
            case 2: addCar(); break;
            case 3: 
                cout << "  Функция редактирования автомобиля\n";
                pause();
                break;
            case 4:
                cout << "  Функция удаления автомобиля\n";
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
        cout << "  Список заявок пуст.\n";
    } else {
        cout << left << setw(5) << "  ID"
                  << setw(25) << "Клиент"
                  << setw(20) << "Автомобиль"
                  << setw(12) << "Дата"
                  << setw(12) << "Статус"
                  << "Сумма\n";
        cout << "  " << string(81, '-') << "\n";
        
        for (const auto& order : orders) {
            const Client* client = dm.findClientById(order.getClientId());
            const Car* car = dm.findCarById(order.getCarId());
            
            string clientName = client ? client->getName() : "?";
            size_t spacePos = clientName.find(' ');
            if (spacePos != string::npos) {
                //clientName = clientName.substr(0, spacePos);
            }
            
            string carInfo = car ? car->getShortInfo() : "?";
            
            cout << "  " << left << setw(3) << order.getId()
                      << setw(25) << clientName.substr(0, 24)
                      << setw(20) << carInfo.substr(0, 19)
                      << setw(12) << order.getDate()
                      << setw(12) << order.getStatusString()
                      << fixed << setprecision(2) << order.getTotalCost() << " BYN\n";
        }
        cout << "\n  Всего заявок: " << orders.size() << "\n";
    }
    pause();
}

void createOrder() {
    showHeader("СОЗДАНИЕ ЗАЯВКИ");
    DataManager& dm = DataManager::getInstance();
    
    // Шаг 1: Выбор клиента
    auto& clients = dm.getClients();
    if (clients.empty()) {
        cout << "  Сначала добавьте клиента!\n";
        pause();
        return;
    }
    
    cout << "  ШАГ 1: Выбор клиента\n";
    cout << "  " << string(40, '-') << "\n";
    for (const auto& c : clients) {
        cout << "  " << c.getId() << ". " << c.getName() << "\n";
    }
    
    int clientId = readInt("\n  ID клиента: ");
    const Client* client = dm.findClientById(clientId);
    if (!client) {
        cout << "  ОШИБКА: Клиент не найден!\n";
        pause();
        return;
    }
    
    // Шаг 2: Выбор автомобиля
    cout << "\n  ШАГ 2: Выбор автомобиля\n";
    cout << "  " << string(40, '-') << "\n";
    auto clientCars = dm.getCarsByClientId(clientId);
    if (clientCars.empty()) {
        cout << "  У клиента нет автомобилей. Добавьте автомобиль.\n";
        pause();
        return;
    }
    
    for (const auto& car : clientCars) {
        cout << "  " << car.getId() << ". " << car.getInfo() << "\n";
    }
    
    int carId = readInt("\n  ID автомобиля: ");
    const Car* car = dm.findCarById(carId);
    if (!car || car->getClientId() != clientId) {
        cout << "  ОШИБКА: Автомобиль не найден!\n";
        pause();
        return;
    }
    
    // Шаг 3: Выбор услуг
    cout << "\n  ШАГ 3: Выбор услуг\n";
    cout << "  " << string(40, '-') << "\n";
    auto& services = dm.getServiceTypes();
    for (const auto& s : services) {
        cout << "  " << s.getId() << ". " << s.getName() 
                  << " - " << fixed << setprecision(2) << s.getPrice() << " BYN\n";
    }
    
    cout << "\n  Введите ID услуг через запятую: ";
    string servicesInput;
    getline(cin, servicesInput);
    
    vector<int> serviceIds;
    istringstream iss(servicesInput);
    string token;
    while (getline(iss, token, ',')) {
        try {
            int serviceId = stoi(token);
            if (dm.findServiceTypeById(serviceId)) {
                serviceIds.push_back(serviceId);
            }
        } catch (...) {}
    }
    
    if (serviceIds.empty()) {
        cout << "  ОШИБКА: Выберите хотя бы одну услугу!\n";
        pause();
        return;
    }
    
    // Шаг 4: Выбор исполнителя и даты
    cout << "\n  ШАГ 4: Исполнитель и дата\n";
    cout << "  " << string(40, '-') << "\n";
    auto& employees = dm.getEmployees();
    for (const auto& e : employees) {
        cout << "  " << e.getId() << ". " << e.getName() << " (" << e.getPosition() << ")\n";
    }
    
    int employeeId = readInt("\n  ID исполнителя: ");
    if (!dm.findEmployeeById(employeeId)) {
        cout << "  ОШИБКА: Сотрудник не найден!\n";
        pause();
        return;
    }
    
    string date = readLine("  Дата (ДД.ММ.ГГГГ) [Enter - сегодня]: ");
    if (date.empty()) {
        date = getCurrentDate();
    }
    
    // Расчёт стоимости
    double totalCost = dm.calculateOrderCost(serviceIds, client->getDiscount());
    
    // Подтверждение
    cout << "\n  " << string(40, '=') << "\n";
    cout << "  ИТОГО:\n";
    cout << "  Клиент: " << client->getName() << "\n";
    cout << "  Автомобиль: " << car->getInfo() << "\n";
    cout << "  Услуг: " << serviceIds.size() << "\n";
    cout << "  Скидка: " << client->getDiscount() << "%\n";
    cout << "  Стоимость: " << fixed << setprecision(2) << totalCost << " BYN\n";
    cout << "  " << string(40, '=') << "\n";
    
    if (confirm("\n  Создать заявку?")) {
        int newId = ServiceOrder::getNextId();
        ServiceOrder order(newId, clientId, carId, employeeId, date, 
                           OrderStatus::NEW, serviceIds, totalCost);
        dm.addOrder(order);
        dm.saveAll();
        
        cout << "\n  Заявка создана! Номер: " << newId << "\n";
    } else {
        cout << "\n  Операция отменена.\n";
    }
    pause();
}

void changeOrderStatus() {
    showHeader("ИЗМЕНЕНИЕ СТАТУСА ЗАЯВКИ");
    DataManager& dm = DataManager::getInstance();
    
    int id = readInt("  Введите ID заявки: ");
    const ServiceOrder* order = dm.findOrderById(id);
    
    if (!order) {
        cout << "  ОШИБКА: Заявка не найдена!\n";
        pause();
        return;
    }
    
    cout << "\n  Заявка #" << id << "\n";
    cout << "  Текущий статус: " << order->getStatusString() << "\n\n";
    
    cout << "  Выберите новый статус:\n";
    cout << "  1. Новая\n";
    cout << "  2. В работе\n";
    cout << "  3. Выполнена\n";
    cout << "  4. Выдана\n\n";
    
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
    
    cout << "\n  Статус изменён на: " << ServiceOrder::statusToString(newStatus) << "\n";
    pause();
}

void editOrder() {
    showHeader("РЕДАКТИРОВАНИЕ ЗАЯВКИ");
    DataManager& dm = DataManager::getInstance();

    auto& orders = dm.getOrders();
    if (orders.empty()) {
        cout << "  Нет заявок для редактирования.\n";
        pause();
        return;
    }

    cout << "  Список заявок:\n";
    cout << "  " << string(70, '-') << "\n";
    cout << "  " << left << setw(5) << "ID"
        << setw(20) << "Клиент"
        << setw(15) << "Авто"
        << setw(12) << "Дата"
        << setw(12) << "Статус" << "\n";
    cout << "  " << string(70, '-') << "\n";

    for (const auto& o : orders) {
        const Client* client = dm.findClientById(o.getClientId());
        const Car* car = dm.findCarById(o.getCarId());
        string clientName = client ? client->getName() : "???";
        string carInfo = car ? (car->getBrand() + " " + car->getModel()) : "???";
        if (clientName.length() > 18) clientName = clientName.substr(0, 18) + "..";
        if (carInfo.length() > 13) carInfo = carInfo.substr(0, 13) + "..";

        cout << "  " << left << setw(5) << o.getId()
            << setw(20) << clientName
            << setw(15) << carInfo
            << setw(12) << o.getDate()
            << setw(12) << o.getStatusString() << "\n";
    }
    cout << "\n";

    int id = readInt("  Введите ID заявки: ");
    const ServiceOrder* order = dm.findOrderById(id);

    if (!order) {
        cout << "  ОШИБКА: Заявка не найдена!\n";
        pause();
        return;
    }

    const Client* client = dm.findClientById(order->getClientId());
    const Car* car = dm.findCarById(order->getCarId());
    const Employee* employee = dm.findEmployeeById(order->getEmployeeId());

    cout << "\n  " << string(50, '=') << "\n";
    cout << "  ТЕКУЩИЕ ДАННЫЕ ЗАЯВКИ #" << id << "\n";
    cout << "  " << string(50, '=') << "\n";
    cout << "  Клиент:     " << (client ? client->getName() : "???") << "\n";
    cout << "  Автомобиль: " << (car ? car->getInfo() : "???") << "\n";
    cout << "  Исполнитель: " << (employee ? employee->getName() : "???") << "\n";
    cout << "  Дата:       " << order->getDate() << "\n";
    cout << "  Статус:     " << order->getStatusString() << "\n";
    cout << "  Услуги:     ";

    auto serviceIds = order->getServiceIds();
    for (size_t i = 0; i < serviceIds.size(); i++) {
        const ServiceType* service = dm.findServiceTypeById(serviceIds[i]);
        if (service) {
            cout << service->getName();
            if (i < serviceIds.size() - 1) cout << ", ";
        }
    }
    cout << "\n";
    cout << "  Стоимость:  " << fixed << setprecision(2) << order->getTotalCost() << " BYN\n";
    cout << "  " << string(50, '-') << "\n\n";

    cout << "  Что редактировать?\n";
    cout << "  1. Исполнителя\n";
    cout << "  2. Дату\n";
    cout << "  3. Услуги\n";
    cout << "  0. Отмена\n\n";

    int editChoice = readInt("  Ваш выбор: ", 0, 3);

    if (editChoice == 0) {
        cout << "  Отменено.\n";
        pause();
        return;
    }

    ServiceOrder updated = *order;

    switch (editChoice) {
    case 1: {
        cout << "\n  Доступные сотрудники:\n";
        auto& employees = dm.getEmployees();
        for (const auto& e : employees) {
            cout << "  " << e.getId() << ". " << e.getName() << " (" << e.getPosition() << ")\n";
        }

        int newEmployeeId = readInt("\n  ID нового исполнителя: ");
        if (!dm.findEmployeeById(newEmployeeId)) {
            cout << "  ОШИБКА: Сотрудник не найден!\n";
            pause();
            return;
        }
        updated.setEmployeeId(newEmployeeId);
        break;
    }
    case 2: {
        string newDate = readLine("  Новая дата (ДД.ММ.ГГГГ): ");
        if (newDate.empty()) {
            cout << "  ОШИБКА: Дата не может быть пустой!\n";
            pause();
            return;
        }
        updated.setDate(newDate);
        break;
    }
    case 3: {
        cout << "\n  Доступные услуги:\n";
        auto& services = dm.getServiceTypes();
        for (const auto& s : services) {
            cout << "  " << left << setw(3) << s.getId() << ". "
                << setw(35) << s.getName()
                << fixed << setprecision(2) << s.getPrice() << " BYN\n";
        }

        string servicesInput = readLine("\n  ID услуг через запятую: ");
        vector<int> newServiceIds;
        stringstream ss(servicesInput);
        string token;

        while (getline(ss, token, ',')) {
            try {
                int serviceId = stoi(token);
                if (dm.findServiceTypeById(serviceId)) {
                    newServiceIds.push_back(serviceId);
                }
            }
            catch (...) {}
        }

        if (newServiceIds.empty()) {
            cout << "  ОШИБКА: Не выбраны корректные услуги!\n";
            pause();
            return;
        }

        // Clear old services and add new
        vector<int> oldIds = updated.getServiceIds();
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

    cout << "\n  Заявка обновлена!\n";
    pause();
}

void deleteOrder() {
    showHeader("УДАЛЕНИЕ ЗАЯВКИ");
    DataManager& dm = DataManager::getInstance();

    int id = readInt("  Введите ID заявки: ");
    const ServiceOrder* order = dm.findOrderById(id);

    if (!order) {
        cout << "  ОШИБКА: Заявка не найдена!\n";
        pause();
        return;
    }

    const Client* client = dm.findClientById(order->getClientId());

    cout << "\n  Заявка #" << id << "\n";
    cout << "  Клиент: " << (client ? client->getName() : "???") << "\n";
    cout << "  Дата: " << order->getDate() << "\n";
    cout << "  Статус: " << order->getStatusString() << "\n";
    cout << "  Сумма: " << fixed << setprecision(2) << order->getTotalCost() << " BYN\n";

    if (confirm("\n  Подтвердите удаление")) {
        dm.deleteOrder(id);
        dm.saveAll();
        cout << "\n  Заявка удалена.\n";
    }
    else {
        cout << "\n  Операция отменена.\n";
    }
    pause();
}

void ordersMenu() {
    while (true) {
        showHeader("ЗАЯВКИ НА ОБСЛУЖИВАНИЕ");
        cout << "  1. Просмотр всех заявок\n";
        cout << "  2. Создать заявку\n";
        cout << "  3. Изменить статус заявки\n";
        cout << "  4. Редактировать заявку\n";
        cout << "  5. Удалить заявку\n";
        cout << "  " << string(30, '-') << "\n";
        cout << "  0. Назад\n\n";
        
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
        cout << "  1. Поиск по ФИО клиента\n";
        cout << "  2. Фильтр по дате\n";
        cout << "  3. Фильтр по статусу\n";
        cout << "  " << string(30, '-') << "\n";
        cout << "  0. Назад\n\n";
        
        int choice = readInt("  Выберите пункт меню: ", 0, 3);
        DataManager& dm = DataManager::getInstance();
        
        switch (choice) {
            case 1: {
                string name = readLine("  Введите ФИО или часть ФИО: ");

                // 1. Найти всех клиентов по ФИО
                auto clients = dm.searchClientsByName(name);

                if (clients.empty()) {
                    cout << "\n  Клиенты не найдены.\n";
                    pause();
                    break;
                }

                // 2. Для каждого клиента найти его заказы
                vector<ServiceOrder> foundOrders;
                for (const auto& client : clients) {
                    auto clientOrders = dm.getOrdersByClientId(client.getId());
                    for (const auto& order : clientOrders) {
                        foundOrders.push_back(order);
                    }
                }

                cout << "\n  " << string(65, '-') << "\n";
                cout << "  ЗАЯВКИ КЛИЕНТОВ С ФИО: \"" << name << "\"\n";
                cout << "  " << string(65, '-') << "\n";

                if (foundOrders.empty()) {
                    cout << "  Заявки не найдены.\n";
                }
                else {
                    cout << "  Найдено заявок: " << foundOrders.size() << "\n\n";
                    cout << "  " << left << setw(5) << "ID"
                        << setw(22) << "Клиент"
                        << setw(12) << "Дата"
                        << setw(12) << "Статус"
                        << "Сумма\n";
                    cout << "  " << string(60, '-') << "\n";
                    for (const auto& o : foundOrders) {
                        const Client* client = dm.findClientById(o.getClientId());
                        string clientName = client ? client->getName() : "???";
                        if (clientName.length() > 20) clientName = clientName.substr(0, 20) + "..";
                        cout << "  " << left << setw(5) << o.getId()
                            << setw(22) << clientName
                            << setw(12) << o.getDate()
                            << setw(12) << o.getStatusString()
                            << fixed << setprecision(2) << o.getTotalCost() << " BYN\n";
                    }
                }
                pause();
                break;
            }
            case 2: {
                string date = readLine("  Введите дату (ДД.ММ.ГГГГ): ");
                auto orders = dm.getOrdersByDate(date);
                
                cout << "\n  Заявки на " << date << ":\n";
                if (orders.empty()) {
                    cout << "  Заявки не найдены.\n";
                } else {
                    for (const auto& o : orders) {
                        cout << "  " << o.getInfo() << "\n";
                    }
                }
                pause();
                break;
            }
            case 3: {
                cout << "  Выберите статус:\n";
                cout << "  1. Новая\n";
                cout << "  2. В работе\n";
                cout << "  3. Выполнена\n";
                cout << "  4. Выдана\n";
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
                cout << "\n  Результаты:\n";
                if (orders.empty()) {
                    cout << "  Заявки не найдены.\n";
                } else {
                    for (const auto& o : orders) {
                        cout << "  " << o.getInfo() << "\n";
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
        cout << "  1. Квитанция на выполненные работы\n";
        cout << "  2. Отчёт по выручке за период\n";
        cout << "  3. Статистика по видам работ\n";
        cout << "  " << string(30, '-') << "\n";
        cout << "  0. Назад\n\n";
        
        int choice = readInt("  Выберите пункт меню: ", 0, 3);
        
        switch (choice) {
            case 1: {
                int orderId = readInt("  Введите ID заявки: ");
                string receipt = ReportGenerator::generateReceipt(orderId);
                cout << receipt;
                
                if (confirm("\n  Сохранить в HTML?")) {
                    string filename = "data/Квитанция_" + to_string(orderId) + ".html";
                    if (ReportGenerator::saveReceiptToHtml(orderId, filename)) {
                        cout << "  Сохранено в " << filename << "\n";
                    }
                }
                pause();
                break;
            }
            case 2: {
                string startDate = readLine("  Начальная дата (ДД.ММ.ГГГГ): ");
                string endDate = readLine("  Конечная дата (ДД.ММ.ГГГГ): ");
                
                string report = ReportGenerator::generateRevenueReport(startDate, endDate);
                cout << report;
                pause();
                break;
            }
            case 3: {
                string startDate = readLine("  Начальная дата (ДД.ММ.ГГГГ): ");
                string endDate = readLine("  Конечная дата (ДД.ММ.ГГГГ): ");
                
                string report = ReportGenerator::generateServiceStatistics(startDate, endDate);
                cout << report;
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
        
        cout << "  Текущий прейскурант:\n";
        cout << "  " << string(55, '-') << "\n";
        for (const auto& s : services) {
            cout << "  " << left << setw(5) << s.getId()
                      << setw(38) << s.getName() << " "
                      << fixed << setprecision(2) << s.getPrice() << " BYN\n";
        }
        cout << "\n";
        cout << "  1. Добавить вид работы\n";
        cout << "  2. Редактировать\n";
        cout << "  3. Удалить\n";
        cout << "  " << string(30, '-') << "\n";
        cout << "  0. Назад\n\n";
        
        int choice = readInt("  Выберите пункт меню: ", 0, 3);
        
        switch (choice) {
            case 1: {
                string name = readLine("  Наименование: ");
                if (name.empty()) {
                    cout << "  ОШИБКА: Наименование не может быть пустым!\n";
                    pause();
                    break;
                }
                double price = readDouble("  Цена (BYN): ");
                
                int newId = ServiceType::getNextId();
                ServiceType service(newId, name, price);
                dm.addServiceType(service);
                dm.saveAll();
                
                cout << "  Добавлено! ID: " << newId << "\n";
                pause();
                break;
            }
            case 2: {
                int id = readInt("  Введите ID вида работы: ");
                const ServiceType* service = dm.findServiceTypeById(id);
                
                if (!service) {
                    cout << "  ОШИБКА: Вид работы не найден!\n";
                    pause();
                    break;
                }
                
                cout << "\n  Текущие данные:\n";
                cout << "  Наименование: " << service->getName() << "\n";
                cout << "  Цена: " << fixed << setprecision(2) 
                          << service->getPrice() << " BYN\n\n";
                
                cout << "  Введите новые данные (Enter - оставить без изменений):\n";
                
                string name = readLine("  Наименование: ");
                string priceStr = readLine("  Цена (BYN): ");
                
                ServiceType updated = *service;
                if (!name.empty()) updated.setName(name);
                if (!priceStr.empty()) updated.setPrice(stod(priceStr));
                
                dm.updateServiceType(id, updated);
                dm.saveAll();
                
                cout << "\n  Данные обновлены!\n";
                pause();
                break;
            }
            case 3: {
                int id = readInt("  Введите ID вида работы: ");
                const ServiceType* service = dm.findServiceTypeById(id);
                
                if (!service) {
                    cout << "  ОШИБКА: Вид работы не найден!\n";
                    pause();
                    break;
                }
                
                cout << "\n  Вид работы: " << service->getName() << "\n";
                cout << "  Цена: " << fixed << setprecision(2) 
                          << service->getPrice() << " BYN\n";
                
                if (confirm("\n  Подтвердите удаление")) {
                    dm.deleteServiceType(id);
                    dm.saveAll();
                    cout << "\n  Вид работы удалён.\n";
                } else {
                    cout << "\n  Операция отменена.\n";
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
        
        cout << "  Список пользователей:\n";
        cout << "  " << string(58, '-') << "\n";
        for (const auto& u : users) {
            
            cout << "  " << left << setw(5) << u.getId()
                      << setw(25) << u.getName()
                      << setw(15) << u.getLogin()
                      << u.getRoleString() << "\n";
        }
        cout << "\n";
        cout << "  1. Добавить пользователя\n";
        cout << "  2. Изменить пароль\n";
        cout << "  3. Удалить пользователя\n";
        cout << "  " << string(30, '-') << "\n";
        cout << "  0. Назад\n\n";
        
        int choice = readInt("  Выберите пункт меню: ", 0, 3);
        
        switch (choice) {
            case 1: {
                string name = readLine("  ФИО: ");
                string phone = readLine("  Телефон: ");
                string login = readLine("  Логин: ");
                string password = readPassword("  Пароль: ");
                string password2 = readPassword("  Повторите ввод Пароля: ");
                if (password != password2) {
                    cout << "  Пароли не совпали. ";
                    password2 = readPassword("  Повторите ввод Пароля: ");
                    if (password != password2) {
                        cout << "  Пароли не совпали. Пользователь не добавлен!\n";
                        pause();
                        return;
                    }
                }
                cout << "  Роль (1 - Пользователь, 2 - Администратор): ";
                int roleChoice = readInt("", 1, 2);
                UserRole role = (roleChoice == 2) ? UserRole::ROLE_ADMIN : UserRole::ROLE_USER;
                
                int newId = Person::getNextId();
                User user(newId, name, phone, login, password, role);
                dm.addUser(user);
                dm.saveAll();
                
                cout << "  Пользователь добавлен!\n";
                pause();
                break;
            }
            case 2: { 
                int id = readInt("  Введите ID пользователя: ");
                const User* u = dm.findUserById(id);

                if (!u) {
                    cout << "  ОШИБКА: Пользователь не найден!\n";
                    pause();
                    break;
                }
                cout << "\n  Пользователь:";
                cout << "  " << u->getId()
                    << "  " << u->getName()
                    << "  " << u->getLogin()
                    << "  (" << u->getRoleString() << ")\n";

                string password = readPassword("  Новый Пароль: ");
                string password2 = readPassword("  Повторите ввод Пароля: ");
                if (password != password2) {
                    cout << "  Пароли не совпали. ";
                    password2 = readPassword("  Повторите ввод Пароля: ");
                    if (password != password2) {
                        cout << "  Пароли не совпали. Пароль не изменен!\n";
                        pause();
                        return;
                    }
                }

                User updated = *u;
                if (!password.empty()) updated.setPassword(password);
                dm.updateUser(id, updated);
                dm.saveAll();

                cout << "\n  Пароль изменен!\n";
                pause();
                break;
            }
            case 3: {
                int id = readInt("  Введите ID пользователя: ");
                const User* u = dm.findUserById(id);

                if (!u) {
                    cout << "  ОШИБКА: Пользователь не найден!\n";
                    pause();
                    break;
                }

                cout << "\n  Пользователь:";
                cout << "  " << u->getId()
                    << "  " << u->getName()
                    << "  " << u->getLogin()
                    << "  (" << u->getRoleString() << ")\n";
                if (confirm("\n  Подтвердите удаление")) {
                    dm.deleteUser(id);
                    dm.saveAll();
                    cout << "\n  Пользователь удалён.\n";
                }
                else {
                    cout << "\n  Операция отменена.\n";
                }
                pause();
                break;
            }
            case 0:
                return;
                break;
            default:
                cout << "  Функция в разработке\n";
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
        
        cout << "  1. Клиенты\n";
        cout << "  2. Автомобили\n";
        cout << "  3. Заявки на обслуживание\n";
        cout << "  4. Поиск и фильтрация\n";
        cout << "  5. Отчёты\n";
        
        if (currentUser && currentUser->getRole() == UserRole::ROLE_ADMIN) {
            cout << "  " << string(30, '-') << "\n";
            cout << "  6. Справочник видов работ\n";
            cout << "  7. Сотрудники\n";
            cout << "  8. Управление пользователями\n";
        }
        
        cout << "  " << string(30, '-') << "\n";
        cout << "  0. Выход из системы\n\n";
        
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
                    cout << "  Меню сотрудников\n";
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
    
    //cout << "\n  Спасибо за работу!\n\n";
    return 0;
}
