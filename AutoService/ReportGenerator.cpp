#include "../include/ReportGenerator.h"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <map>
#include <algorithm>

// Вспомогательный метод для создания разделительной линии
std::string ReportGenerator::createLine(int length, char ch) {
    return std::string(length, ch);
}

// Форматирование денежной суммы
std::string ReportGenerator::formatMoney(double amount) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << amount << " BYN";
    return oss.str();
}

// Получение текущей даты
std::string ReportGenerator::getCurrentDate() {
    time_t now = time(0);
    std::ostringstream oss;
    #ifdef _WIN32
        tm ltm;
        localtime_s(&ltm, &now);
        oss << std::setfill('0') << std::setw(2) << ltm.tm_mday << "."
            << std::setfill('0') << std::setw(2) << (1 + ltm.tm_mon) << "."
            << (1900 + ltm.tm_year);
    #else
        tm* ltm = localtime(&now);
        oss << std::setfill('0') << std::setw(2) << ltm->tm_mday << "."
            << std::setfill('0') << std::setw(2) << (1 + ltm->tm_mon) << "."
            << (1900 + ltm->tm_year);
    #endif
    return oss.str();
}

// Формирование квитанции
std::string ReportGenerator::generateReceipt(int orderId) {
    DataManager& dm = DataManager::getInstance();
    const ServiceOrder* order = dm.findOrderById(orderId);
    
    if (!order) {
        return "ОШИБКА: Заявка не найдена!";
    }
    /*
    if (order->getStatus() != OrderStatus::COMPLETED && 
        order->getStatus() != OrderStatus::ISSUED) {
        return "ОШИБКА: Заявка ещё не выполнена!";
    }
    */
    
    const Client* client = dm.findClientById(order->getClientId());
    const Car* car = dm.findCarById(order->getCarId());
    const Employee* employee = dm.findEmployeeById(order->getEmployeeId());
    
    if (!client || !car) {
        return "ОШИБКА: Данные клиента или автомобиля не найдены!";
    }
    
    std::ostringstream oss;
    std::string line = createLine(70, '=');
    std::string thinLine = createLine(70, '-');
    
    oss << "\n" << line << "\n";
    oss << "                         КВИТАНЦИЯ № " << std::setfill('0') << std::setw(5) << orderId << "\n";
    oss << line << "\n";
    oss << "                    Автосервис \"АвтоМастер\"\n";
    oss << line << "\n\n";
    
    oss << "  Дата оформления: " << order->getDate() << "\n";
    oss << "  Дата выдачи:     " << getCurrentDate() << "\n\n";
    
    oss << thinLine << "\n";
    oss << "  ЗАКАЗЧИК\n";
    oss << thinLine << "\n";
    oss << "  ФИО:      " << client->getName() << "\n";
    oss << "  Телефон:  " << client->getPhone() << "\n";
    if (client->getDiscount() > 0) {
        oss << "  Скидка:   Постоянный клиент (" << std::fixed << std::setprecision(1) 
            << client->getDiscount() << "%)\n";
    }
    oss << "\n";
    
    oss << thinLine << "\n";
    oss << "  АВТОМОБИЛЬ\n";
    oss << thinLine << "\n";
    oss << "  Марка:    " << car->getBrand() << "\n";
    oss << "  Модель:   " << car->getModel() << "\n";
    oss << "  Год:      " << car->getYear() << "\n";
    oss << "  Госномер: " << car->getRegNumber() << "\n\n";
    
    oss << thinLine << "\n";
    oss << "  ВЫПОЛНЕННЫЕ РАБОТЫ\n";
    oss << thinLine << "\n";
    
    double subtotal = 0.0;
    int num = 1;
    std::vector<int> serviceIds = order->getServiceIds();
    
    oss << std::setfill(' ') << std::left;  // сбросить fill и установить left по умолчанию

    for (int serviceId : serviceIds) {
        const ServiceType* service = dm.findServiceTypeById(serviceId);
        if (service) {
            oss << "  " << num++ << ". " << std::left << std::setw(45) << service->getName()
                << std::right << std::setw(11) << formatMoney(service->getPrice()) << "\n";
            subtotal += service->getPrice();
        }
    }
    
    oss << thinLine << "\n\n";
    
    double discount = client->getDiscount();
    double discountAmount = subtotal * discount / 100.0;
    double total = subtotal - discountAmount;
    
    oss << std::right;
    oss << "  Итого:                                            " << formatMoney(subtotal) << "\n";
    
    if (discount > 0) {
        oss << "  Скидка (" << std::fixed << std::setprecision(1) << discount << "%):";
        oss << std::setw(43) << ("-" + formatMoney(discountAmount)) << "\n";
    }
    
    oss << "  " << createLine(60, '-') << "\n";
    oss << "  ИТОГО К ОПЛАТЕ:                                   " << formatMoney(total) << "\n";
    oss << "  " << createLine(60, '-') << "\n\n";
    
    if (employee) {
        oss << "  Исполнитель: " << employee->getName() << "\n\n";
    }
    
    oss << "  Гарантия: 30 дней на выполненные работы\n\n";
    
    oss << "  Работу сдал:  ___________________ / " 
        << (employee ? employee->getName().substr(0, employee->getName().find(' ')) : "________") << " /\n\n";
    oss << "  Работу принял, претензий не имею:\n\n";
    oss << "                ___________________ / " 
        << client->getName().substr(0, client->getName().find(' ')) << " /\n\n";
    
    oss << line << "\n";
    oss << "                 Благодарим за обращение!\n";
    oss << line << "\n";
    
    return oss.str();
}

// Формирование отчёта по выручке
std::string ReportGenerator::generateRevenueReport(const std::string& startDate,
                                                    const std::string& endDate) {
    DataManager& dm = DataManager::getInstance();
    std::vector<ServiceOrder> allOrders = dm.getOrders();
    
    std::ostringstream oss;
    std::string line = createLine(70, '=');
    std::string thinLine = createLine(70, '-');
    
    oss << "\n" << line << "\n";
    oss << "                  ОТЧЁТ ПО ВЫРУЧКЕ ЗА ПЕРИОД\n";
    oss << line << "\n\n";
    
    oss << "  Период: с " << startDate << " по " << endDate << "\n";
    oss << "  Дата формирования: " << getCurrentDate() << "\n\n";
    
    int totalOrders = 0;
    int completedOrders = 0;
    int inProgressOrders = 0;
    int newOrders = 0;
    double totalRevenue = 0.0;
    double totalDiscount = 0.0;
    
    for (const auto& order : allOrders) {
        // Простая проверка даты (формат ДД.ММ.ГГГГ)
        std::string orderDate = order.getDate();
        if (orderDate >= startDate && orderDate <= endDate) {
            totalOrders++;
            
            switch (order.getStatus()) {
                case OrderStatus::NEW:
                    newOrders++;
                    break;
                case OrderStatus::IN_PROGRESS:
                    inProgressOrders++;
                    break;
                case OrderStatus::COMPLETED:
                case OrderStatus::ISSUED:
                    completedOrders++;
                    // Считаем выручку только по выполненным заявкам
                    {
                        const Client* client = dm.findClientById(order.getClientId());
                        double orderTotal = 0.0;
                        for (int serviceId : order.getServiceIds()) {
                            const ServiceType* service = dm.findServiceTypeById(serviceId);
                            if (service) {
                                orderTotal += service->getPrice();
                            }
                        }
                        double discount = client ? client->getDiscount() : 0.0;
                        double discountAmount = orderTotal * discount / 100.0;
                        totalDiscount += discountAmount;
                        totalRevenue += (orderTotal - discountAmount);
                    }
                    break;
            }
        }
    }
    
    oss << thinLine << "\n";
    oss << "  СТАТИСТИКА ПО ЗАЯВКАМ\n";
    oss << thinLine << "\n";
    oss << "  Всего заявок за период:        " << std::setw(10) << totalOrders << "\n";
    oss << "  Из них:\n";
    oss << "    - выполнено и выдано:        " << std::setw(10) << completedOrders << "\n";
    oss << "    - в работе:                  " << std::setw(10) << inProgressOrders << "\n";
    oss << "    - новых:                     " << std::setw(10) << newOrders << "\n\n";
    
    oss << thinLine << "\n";
    oss << "  ФИНАНСОВЫЕ ПОКАЗАТЕЛИ\n";
    oss << thinLine << "\n";
    oss << "  Сумма предоставленных скидок:  " << std::setw(18) << formatMoney(totalDiscount) << "\n";
    oss << "  " << createLine(50, '-') << "\n";
    oss << "  ИТОГО ВЫРУЧКА:                 " << std::setw(18) << formatMoney(totalRevenue) << "\n\n";
    
    if (completedOrders > 0) {
        double avgCheck = totalRevenue / completedOrders;
        oss << "  Средний чек:                   " << std::setw(18) << formatMoney(avgCheck) << "\n";
    }
    
    oss << "\n" << line << "\n";
    
    return oss.str();
}

// Формирование статистики по видам работ
std::string ReportGenerator::generateServiceStatistics(const std::string& startDate,
                                                        const std::string& endDate) {
    DataManager& dm = DataManager::getInstance();
    std::vector<ServiceOrder> allOrders = dm.getOrders();
    
    // Собираем статистику: serviceId -> {count, totalAmount}
    std::map<int, std::pair<int, double>> stats;
    
    for (const auto& order : allOrders) {
        std::string orderDate = order.getDate();
        if (orderDate >= startDate && orderDate <= endDate) {
            if (order.getStatus() == OrderStatus::COMPLETED || 
                order.getStatus() == OrderStatus::ISSUED) {
                for (int serviceId : order.getServiceIds()) {
                    const ServiceType* service = dm.findServiceTypeById(serviceId);
                    if (service) {
                        stats[serviceId].first++;
                        stats[serviceId].second += service->getPrice();
                    }
                }
            }
        }
    }
    
    std::ostringstream oss;
    std::string line = createLine(75, '=');
    std::string thinLine = createLine(75, '-');
    
    oss << "\n" << line << "\n";
    oss << "                   СТАТИСТИКА ПО ВИДАМ РАБОТ\n";
    oss << line << "\n\n";
    
    oss << "  Период: с " << startDate << " по " << endDate << "\n";
    oss << "  Дата формирования: " << getCurrentDate() << "\n\n";
    
    oss << thinLine << "\n";
    oss << "  " << std::left << std::setw(5) << "№"
        << std::setw(40) << "Вид работы"
        << std::right << std::setw(10) << "Кол-во"
        << std::setw(15) << "Сумма" << "\n";
    oss << thinLine << "\n";
    
    double grandTotal = 0.0;
    int totalCount = 0;
    int num = 1;
    
    // Собираем в вектор для сортировки
    std::vector<std::tuple<int, std::string, int, double>> sortedStats;
    for (const auto& stat : stats) {
        const ServiceType* service = dm.findServiceTypeById(stat.first);
        if (service) {
            sortedStats.push_back({stat.first, service->getName(), 
                                   stat.second.first, stat.second.second});
        }
    }
    
    // Сортируем по количеству (по убыванию)
    std::sort(sortedStats.begin(), sortedStats.end(),
        [](const auto& a, const auto& b) {
            return std::get<2>(a) > std::get<2>(b);
        });
    
    for (const auto& item : sortedStats) {
        oss << "  " << std::left << std::setw(5) << num++
            << std::setw(40) << std::get<1>(item)
            << std::right << std::setw(10) << std::get<2>(item)
            << std::setw(15) << formatMoney(std::get<3>(item)) << "\n";
        totalCount += std::get<2>(item);
        grandTotal += std::get<3>(item);
    }
    
    oss << thinLine << "\n";
    oss << "  " << std::left << std::setw(45) << "ИТОГО:"
        << std::right << std::setw(10) << totalCount
        << std::setw(15) << formatMoney(grandTotal) << "\n";
    oss << thinLine << "\n\n";
    
    // Процентное распределение
    if (grandTotal > 0) {
        oss << "  РАСПРЕДЕЛЕНИЕ ПО ДОЛЕ ВЫРУЧКИ:\n\n";
        for (const auto& item : sortedStats) {
            double percent = (std::get<3>(item) / grandTotal) * 100.0;
            int barLength = static_cast<int>(percent / 2);
            oss << "  " << std::left << std::setw(25) << std::get<1>(item).substr(0, 24)
                << " " << std::string(barLength, '#')
                << " " << std::fixed << std::setprecision(1) << percent << "%\n";
        }
    }
    
    oss << "\n" << line << "\n";
    
    return oss.str();
}

// Сохранение квитанции в HTML
bool ReportGenerator::saveReceiptToHtml(int orderId, const std::string& filename) {
    DataManager& dm = DataManager::getInstance();
    const ServiceOrder* order = dm.findOrderById(orderId);
    
    if (!order) return false;
    
    const Client* client = dm.findClientById(order->getClientId());
    const Car* car = dm.findCarById(order->getCarId());
    const Employee* employee = dm.findEmployeeById(order->getEmployeeId());
    
    if (!client || !car) return false;
    
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    file << "<!DOCTYPE html>\n<html lang=\"ru\">\n<head>\n";
    file << "<title>Квитанция №" << std::setfill('0') << std::setw(5) << orderId << "</title>\n";
    file << "<style>\n";
    file << "body { font-family: Arial, sans-serif; max-width: 800px; margin: 0 auto; padding: 10px; }\n";
    file << "h1 { text-align: center; font-size: 1.2em; }\n";
    file << "table { width: 100%; border-collapse: collapse; margin: 10px 0; }\n";
    file << "th, td { border: 1px solid #ddd; padding: 4px; text-align: left; }\n";
    file << "th { background-color: #f2f2f2; }\n";
    file << ".total { font-weight: bold; font-size: 1.2em; }\n";
    file << ".header { text-align: center; margin-bottom: 10px; }\n";
    file << "</style>\n</head>\n<body>\n";
    
    file << "<div class=\"header\">\n";
    file << "<h1>КВИТАНЦИЯ № " << std::setfill('0') << std::setw(5) << orderId << "</h1>\n";
    file << "<p>Автосервис \"АвтоМастер\"";
    file << "</div>\n";
    
    file << "<h3>Заказчик</h3>\n";
    file << "<p>ФИО: " << client->getName() << "<br>\n";
    file << "Телефон: " << client->getPhone() << "</p>\n";
    
    file << "<h3>Автомобиль</h3>\n";
    file << "<p>" << car->getBrand() << " " << car->getModel() << " (" << car->getYear() << ")<br>\n";
    file << "Госномер: " << car->getRegNumber() << "</p>\n";
    
    file << "<h3>Выполненные работы</h3>\n";
    file << "<table>\n<tr><th>№</th><th>Наименование</th><th>Стоимость</th></tr>\n";
    
    double subtotal = 0.0;
    int num = 1;
    for (int serviceId : order->getServiceIds()) {
        const ServiceType* service = dm.findServiceTypeById(serviceId);
        if (service) {
            file << "<tr><td>" << num++ << "</td><td>" << service->getName() 
                 << "</td><td>" << formatMoney(service->getPrice()) << "</td></tr>\n";
            subtotal += service->getPrice();
        }
    }
    
    file << "</table>\n";
    
    double discount = client->getDiscount();
    double discountAmount = subtotal * discount / 100.0;
    double total = subtotal - discountAmount;
    
    file << "<p>Подытог: " << formatMoney(subtotal) << "</p>\n";
    if (discount > 0) {
        file << "<p>Скидка (" << discount << "%): -" << formatMoney(discountAmount) << "</p>\n";
    }
    file << "<p class=\"total\">ИТОГО К ОПЛАТЕ: " << formatMoney(total) << "</p>\n";
    
    if (employee) {
        file << "<p>Исполнитель: " << employee->getName() << "</p>\n";
    }
    
    file << "<p>Дата: " << getCurrentDate() << "</p>\n";
    
    file << "</body>\n</html>\n";
    file.close();
    
    return true;
}

// Сохранение отчёта по выручке в HTML
bool ReportGenerator::saveRevenueReportToHtml(const std::string& startDate,
                                               const std::string& endDate,
                                               const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    // Аналогично generateRevenueReport, но в HTML формате
    DataManager& dm = DataManager::getInstance();
    std::vector<ServiceOrder> allOrders = dm.getOrders();
    
    int totalOrders = 0, completedOrders = 0;
    double totalRevenue = 0.0;
    
    for (const auto& order : allOrders) {
        if (order.getDate() >= startDate && order.getDate() <= endDate) {
            totalOrders++;
            if (order.getStatus() == OrderStatus::COMPLETED || 
                order.getStatus() == OrderStatus::ISSUED) {
                completedOrders++;
                totalRevenue += order.getTotalCost();
            }
        }
    }
    
    file << "<!DOCTYPE html>\n<html lang=\"ru\">\n<head>\n";
    file << "<meta charset=\"UTF-8\">\n<title>Отчёт по выручке</title>\n";
    file << "<style>body{font-family:Arial;max-width:800px;margin:0 auto;padding:20px;}</style>\n";
    file << "</head>\n<body>\n";
    file << "<h1>Отчёт по выручке</h1>\n";
    file << "<p>Период: " << startDate << " - " << endDate << "</p>\n";
    file << "<p>Всего заявок: " << totalOrders << "</p>\n";
    file << "<p>Выполнено: " << completedOrders << "</p>\n";
    file << "<p><strong>Итого выручка: " << formatMoney(totalRevenue) << "</strong></p>\n";
    file << "</body>\n</html>\n";
    
    file.close();
    return true;
}

// Сохранение статистики в HTML
bool ReportGenerator::saveStatisticsToHtml(const std::string& startDate,
                                           const std::string& endDate,
                                           const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    file << "<!DOCTYPE html>\n<html lang=\"ru\">\n<head>\n";
    file << "<meta charset=\"UTF-8\">\n<title>Статистика по видам работ</title>\n";
    file << "<style>body{font-family:Arial;max-width:800px;margin:0 auto;padding:20px;}";
    file << "table{width:100%;border-collapse:collapse;}th,td{border:1px solid #ddd;padding:8px;}</style>\n";
    file << "</head>\n<body>\n";
    file << "<h1>Статистика по видам работ</h1>\n";
    file << "<p>Период: " << startDate << " - " << endDate << "</p>\n";
    
    // Сбор статистики аналогично generateServiceStatistics
    DataManager& dm = DataManager::getInstance();
    std::map<int, std::pair<int, double>> stats;
    
    for (const auto& order : dm.getOrders()) {
        if (order.getDate() >= startDate && order.getDate() <= endDate) {
            if (order.getStatus() == OrderStatus::COMPLETED || 
                order.getStatus() == OrderStatus::ISSUED) {
                for (int serviceId : order.getServiceIds()) {
                    const ServiceType* service = dm.findServiceTypeById(serviceId);
                    if (service) {
                        stats[serviceId].first++;
                        stats[serviceId].second += service->getPrice();
                    }
                }
            }
        }
    }
    
    file << "<table>\n<tr><th>Вид работы</th><th>Количество</th><th>Сумма</th></tr>\n";
    for (const auto& stat : stats) {
        const ServiceType* service = dm.findServiceTypeById(stat.first);
        if (service) {
            file << "<tr><td>" << service->getName() << "</td><td>" << stat.second.first
                 << "</td><td>" << formatMoney(stat.second.second) << "</td></tr>\n";
        }
    }
    file << "</table>\n</body>\n</html>\n";
    
    file.close();
    return true;
}
