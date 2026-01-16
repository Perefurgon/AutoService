#include "include/ReportGenerator.h"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <map>
#include <algorithm>

// Вспомогательный метод для создания разделительной линии
string ReportGenerator::createLine(int length, char ch) {
    return string(length, ch);
}

// Форматирование денежной суммы
string ReportGenerator::formatMoney(double amount) {
    ostringstream oss;
    oss << fixed << setprecision(2) << amount << " BYN";
    return oss.str();
}

// Получение текущей даты
string ReportGenerator::getCurrentDate() {
    time_t now = time(0);
    ostringstream oss;
    #ifdef _WIN32
        tm ltm;
        localtime_s(&ltm, &now);
        oss << setfill('0') << setw(2) << ltm.tm_mday << "."
            << setfill('0') << setw(2) << (1 + ltm.tm_mon) << "."
            << (1900 + ltm.tm_year);
    #else
        tm* ltm = localtime(&now);
        oss << setfill('0') << setw(2) << ltm->tm_mday << "."
            << setfill('0') << setw(2) << (1 + ltm->tm_mon) << "."
            << (1900 + ltm->tm_year);
    #endif
    return oss.str();
}

// ============================================================================
// ПЕЧАТНАЯ ФОРМА: КВИТАНЦИЯ
// ============================================================================
string ReportGenerator::generateReceipt(int orderId) {
    DataManager& dm = DataManager::getInstance();
    const ServiceOrder* order = dm.findOrderById(orderId);
    
    if (!order) {
        return "ОШИБКА: Заявка не найдена!";
    }
    
    const Client* client = dm.findClientById(order->getClientId());
    const Car* car = dm.findCarById(order->getCarId());
    const Employee* employee = dm.findEmployeeById(order->getEmployeeId());
    
    if (!client || !car) {
        return "ОШИБКА: Данные клиента или автомобиля не найдены!";
    }
    
    ostringstream oss;
    string line = createLine(70, '=');
    string thinLine = createLine(63, '-');
    
    // Заголовок
    oss << "\n " << line << "\n";
    oss << "  КВИТАНЦИЯ\n";
    oss << "  Автосервис \"АвтоМастер\"\n";
    oss << " " << line << "\n";
    oss << "\n";
    
    // Информация о заявке
    oss << "  Заявка № " << orderId << " от " << order->getDate() << "\n";
    oss << "\n";
    
    // Данные клиента
    oss << "  Клиент: " << client->getName() << "\n";
    oss << "  Телефон: " << client->getPhone() << "\n";
    oss << "\n";
    
    // Данные автомобиля
    oss << "  Автомобиль: " << car->getBrand() << " " << car->getModel() << "\n";
    oss << "  Год выпуска: " << car->getYear() << "\n";
    oss << "  Госномер: " << car->getRegNumber() << "\n";
    oss << "\n";
    
    // Выполненные работы
    oss << " " << thinLine << "\n";
    oss << "  ВЫПОЛНЕННЫЕ РАБОТЫ:\n";
    oss << " " << thinLine << "\n";
    
    double subtotal = 0.0;
    int num = 1;
    vector<int> serviceIds = order->getServiceIds();
    
    for (int serviceId : serviceIds) {
        const ServiceType* service = dm.findServiceTypeById(serviceId);
        if (service) {
            string serviceName = service->getName();
            // Формируем строку с точками
            int dotsCount = 45 - static_cast<int>(serviceName.length());
            if (dotsCount < 3) dotsCount = 3;
            string dots(dotsCount, '.');
            
            oss << "  " << num++ << ". " << serviceName << " " << dots << " "
                << fixed << setprecision(2) << service->getPrice() << " BYN\n";
            subtotal += service->getPrice();
        }
    }
    
    oss << " " << thinLine << "\n";
    
    // Итоги
    double discount = client->getDiscount();
    double discountAmount = subtotal * discount / 100.0;
    double total = subtotal - discountAmount;
    
    oss << "  Итого: " << fixed << setprecision(2) << subtotal << " BYN\n";
    
    if (discount > 0) {
        oss << "  Скидка постоянного клиента (" << fixed << setprecision(0) 
            << discount << "%): -" << fixed << setprecision(2) << discountAmount << " BYN\n";
    }
    
    oss << " " << thinLine << "\n";
    oss << "  ИТОГО К ОПЛАТЕ: " << fixed << setprecision(2) << total << " BYN\n";
    oss << " " << thinLine << "\n";
    oss << "\n";
    
    // Исполнитель
    if (employee) {
        string fullName = employee->getName();
        string shortName = fullName;
        size_t space1 = fullName.find(' ');
        if (space1 != string::npos) {
            string surname = fullName.substr(0, space1);
            string rest = fullName.substr(space1 + 1);
            size_t space2 = rest.find(' ');
            if (space2 != string::npos) {
                shortName = surname + " " + rest[0] + "." + rest[space2 + 1] + ".";
            } else if (rest.length() > 0) {
                shortName = surname + " " + rest[0] + ".";
            }
        }
        oss << "  Исполнитель: " << shortName << "\n";
    }
    
    oss << "  Дата выдачи: " << getCurrentDate() << "\n";
    oss << "\n";
    oss << "  Подпись клиента: _______________\n";
    oss << "\n";
    
    return oss.str();
}

// ============================================================================
// ПЕЧАТНАЯ ФОРМА: ОТЧЁТ ПО ВЫРУЧКЕ ЗА ПЕРИОД
// ============================================================================
string ReportGenerator::generateRevenueReport(const string& startDate,
                                                    const string& endDate) {
    DataManager& dm = DataManager::getInstance();
    vector<ServiceOrder> allOrders = dm.getOrders();
    
    ostringstream oss;
    string line = createLine(70, '=');
    string thinLine = createLine(63, '-');
    
    // Заголовок
    oss << "\n " << line << "\n";
    oss << "  ОТЧЁТ ПО ВЫРУЧКЕ ЗА ПЕРИОД\n";
    oss << " " << line << "\n";
    oss << "\n";
    
    // Период
    oss << "  Период: с " << startDate << " по " << endDate << "\n";
    oss << "\n";
    
    // Подсчёт статистики
    int totalOrders = 0;
    int completedOrders = 0;
    int inProgressOrders = 0;
    int newOrders = 0;
    double totalWithoutDiscount = 0.0;
    double totalDiscount = 0.0;
    
    for (const auto& order : allOrders) {
        string orderDate = order.getDate();
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
                    {
                        const Client* client = dm.findClientById(order.getClientId());
                        double orderTotal = 0.0;
                        for (int serviceId : order.getServiceIds()) {
                            const ServiceType* service = dm.findServiceTypeById(serviceId);
                            if (service) {
                                orderTotal += service->getPrice();
                            }
                        }
                        totalWithoutDiscount += orderTotal;
                        double discount = client ? client->getDiscount() : 0.0;
                        double discountAmount = orderTotal * discount / 100.0;
                        totalDiscount += discountAmount;
                    }
                    break;
            }
        }
    }
    
    double totalRevenue = totalWithoutDiscount - totalDiscount;
    
    // Статистика по заявкам
    oss << " " << thinLine << "\n";
    oss << "\n";
    oss << "  Всего заявок за период: " << totalOrders << "\n";
    oss << "  Из них выполнено: " << completedOrders << "\n";
    oss << "  В работе: " << inProgressOrders << "\n";
    oss << "  Новых: " << newOrders << "\n";
    oss << "\n";
    oss << " " << thinLine << "\n";
    oss << "\n";
    
    // Финансовые показатели
    oss << "  Выручка по выполненным заявкам:\n";
    oss << "\n";
    oss << "  Сумма без учёта скидок: " << fixed << setprecision(2) << totalWithoutDiscount << " BYN\n";
    oss << "  Сумма предоставленных скидок: " << fixed << setprecision(2) << totalDiscount << " BYN\n";
    oss << " " << thinLine << "\n";
    oss << "  ИТОГО ВЫРУЧКА: " << fixed << setprecision(2) << totalRevenue << " BYN\n";
    oss << "\n";
    oss << " " << thinLine << "\n";
    
    // Средний чек
    if (completedOrders > 0) {
        double avgCheck = totalRevenue / completedOrders;
        oss << "  Средний чек: " << fixed << setprecision(2) << avgCheck << " BYN\n";
    }
    oss << "\n";
    
    return oss.str();
}

// ============================================================================
// ПЕЧАТНАЯ ФОРМА: СТАТИСТИКА ПО ВИДАМ РАБОТ
// ============================================================================
string ReportGenerator::generateServiceStatistics(const string& startDate,
                                                        const string& endDate) {
    DataManager& dm = DataManager::getInstance();
    vector<ServiceOrder> allOrders = dm.getOrders();
    
    // Собираем статистику: serviceId -> {count, totalAmount}
    map<int, pair<int, double>> stats;
    
    for (const auto& order : allOrders) {
        string orderDate = order.getDate();
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
    
    ostringstream oss;
    string line = createLine(70, '=');
    string thinLine = createLine(62, '-');
    
    // Заголовок
    oss << "\n " << line << "\n";
    oss << "  СТАТИСТИКА ПО ВИДАМ РАБОТ\n";
    oss << "  Период: c " << startDate << " по " << endDate << "\n";
    oss << " " << line << "\n";
    oss << "\n";
    
    // Заголовок таблицы
    oss << "  " << left << setw(3) << "№"
        << setw(32) << "Вид работы"
        << right << setw(8) << "Кол-во"
        << setw(12) << "Сумма"
        << setw(10) << "Доля, %" << "\n";
    oss << " " << thinLine << "\n";
    
    double grandTotal = 0.0;
    int totalCount = 0;
    
    // Сначала посчитаем общую сумму для процентов
    for (const auto& stat : stats) {
        grandTotal += stat.second.second;
        totalCount += stat.second.first;
    }
    
    // Собираем в вектор для сортировки
    vector<tuple<int, string, int, double>> sortedStats;
    for (const auto& stat : stats) {
        const ServiceType* service = dm.findServiceTypeById(stat.first);
        if (service) {
            sortedStats.push_back({stat.first, service->getName(), 
                                   stat.second.first, stat.second.second});
        }
    }
    
    // Сортируем по сумме (по убыванию)
    sort(sortedStats.begin(), sortedStats.end(),
        [](const auto& a, const auto& b) {
            return get<3>(a) > get<3>(b);
        });
    
    int num = 1;
    for (const auto& item : sortedStats) {
        string serviceName = get<1>(item);
        if (serviceName.length() > 30) {
            serviceName = serviceName.substr(0, 28) + "..";
        }
        
        double percent = grandTotal > 0 ? (get<3>(item) / grandTotal) * 100.0 : 0.0;
        
        oss << "  " << left << setw(3) << num++
            << setw(32) << serviceName
            << right << setw(8) << get<2>(item)
            << setw(12) << fixed << setprecision(2) << get<3>(item)
            << setw(9) << fixed << setprecision(1) << percent << "%" << "\n";
    }
    
    oss << " " << thinLine << "\n";
    
    // Итого
    oss << "  " << left << setw(35) << "ИТОГО:"
        << right << setw(8) << totalCount
        << setw(12) << fixed << setprecision(2) << grandTotal
        << setw(9) << "100.0" << "%" << "\n";
    oss << "\n";
    
    return oss.str();
}

// ============================================================================
// HTML: КВИТАНЦИЯ
// ============================================================================
bool ReportGenerator::saveReceiptToHtml(int orderId, const string& filename) {
    DataManager& dm = DataManager::getInstance();
    const ServiceOrder* order = dm.findOrderById(orderId);
    
    if (!order) return false;
    
    const Client* client = dm.findClientById(order->getClientId());
    const Car* car = dm.findCarById(order->getCarId());
    const Employee* employee = dm.findEmployeeById(order->getEmployeeId());
    
    if (!client || !car) return false;
    
    ofstream file(filename);
    if (!file.is_open()) return false;
    
    file << "<!DOCTYPE html>\n<html lang=\"ru\">\n<head>\n";
    file << "<meta charset=\"windows-1251\">\n";
    file << "<title>Квитанция №" << orderId << "</title>\n";
    file << "<style>\n";
    file << "body { font-family: 'Courier New', monospace; max-width: 600px; margin: 20px auto; padding: 20px; }\n";
    file << ".line { border-top: 2px solid #000; margin: 10px 0; }\n";
    file << ".thin-line { border-top: 1px dashed #000; margin: 10px 0; }\n";
    file << ".center { text-align: center; }\n";
    file << ".header { font-weight: bold; font-size: 16px; }\n";
    file << ".work-item { display: flex; justify-content: space-between; }\n";
    file << ".total { font-weight: bold; }\n";
    file << "</style>\n</head>\n<body>\n";
    
    // Заголовок
    file << "<div class=\"line\"></div>\n";
    file << "<div class=\"center header\">КВИТАНЦИЯ</div>\n";
    file << "<div class=\"center\">Автосервис \"АвтоМастер\"</div>\n";
    file << "<div class=\"line\"></div>\n";
    
    // Информация о заявке
    file << "<p>Заявка № " << orderId << " от " << order->getDate() << "</p>\n";
    
    // Данные клиента
    file << "<p>Клиент: " << client->getName() << "<br>\n";
    file << "Телефон: " << client->getPhone() << "</p>\n";
    
    // Данные авто
    file << "<p>Автомобиль: " << car->getBrand() << " " << car->getModel() << "<br>\n";
    file << "Год выпуска: " << car->getYear() << "<br>\n";
    file << "Госномер: " << car->getRegNumber() << "</p>\n";
    
    // Выполненные работы
    file << "<div class=\"thin-line\"></div>\n";
    file << "<div class=\"header\">ВЫПОЛНЕННЫЕ РАБОТЫ:</div>\n";
    file << "<div class=\"thin-line\"></div>\n";
    
    double subtotal = 0.0;
    int num = 1;
    for (int serviceId : order->getServiceIds()) {
        const ServiceType* service = dm.findServiceTypeById(serviceId);
        if (service) {
            file << "<div class=\"work-item\"><span>" << num++ << ". " << service->getName() 
                 << "</span><span>" << fixed << setprecision(2) << service->getPrice() << " BYN</span></div>\n";
            subtotal += service->getPrice();
        }
    }
    
    file << "<div class=\"thin-line\"></div>\n";
    
    // Итоги
    double discount = client->getDiscount();
    double discountAmount = subtotal * discount / 100.0;
    double total = subtotal - discountAmount;
    
    file << "<p>Итого: " << fixed << setprecision(2) << subtotal << " BYN</p>\n";
    if (discount > 0) {
        file << "<p>Скидка постоянного клиента (" << fixed << setprecision(0) 
             << discount << "%): -" << fixed << setprecision(2) << discountAmount << " BYN</p>\n";
    }
    file << "<div class=\"thin-line\"></div>\n";
    file << "<p class=\"total\">ИТОГО К ОПЛАТЕ: " << fixed << setprecision(2) << total << " BYN</p>\n";
    file << "<div class=\"thin-line\"></div>\n";
    
    // Исполнитель
    if (employee) {
        string fullName = employee->getName();
        string shortName = fullName;
        size_t space1 = fullName.find(' ');
        if (space1 != string::npos) {
            string surname = fullName.substr(0, space1);
            string rest = fullName.substr(space1 + 1);
            size_t space2 = rest.find(' ');
            if (space2 != string::npos) {
                shortName = surname + " " + rest[0] + "." + rest[space2 + 1] + ".";
            } else if (rest.length() > 0) {
                shortName = surname + " " + rest[0] + ".";
            }
        }
        file << "<p>Исполнитель: " << shortName << "</p>\n";
    }
    
    file << "<p>Дата выдачи: " << getCurrentDate() << "</p>\n";
    file << "<p>Подпись клиента: _______________</p>\n";
    
    file << "</body>\n</html>\n";
    file.close();
    
    return true;
}

// ============================================================================
// HTML: ОТЧЁТ ПО ВЫРУЧКЕ
// ============================================================================
bool ReportGenerator::saveRevenueReportToHtml(const string& startDate,
                                               const string& endDate,
                                               const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) return false;
    
    DataManager& dm = DataManager::getInstance();
    vector<ServiceOrder> allOrders = dm.getOrders();
    
    int totalOrders = 0, completedOrders = 0, inProgressOrders = 0, newOrders = 0;
    double totalWithoutDiscount = 0.0, totalDiscount = 0.0;
    
    for (const auto& order : allOrders) {
        if (order.getDate() >= startDate && order.getDate() <= endDate) {
            totalOrders++;
            switch (order.getStatus()) {
                case OrderStatus::NEW: newOrders++; break;
                case OrderStatus::IN_PROGRESS: inProgressOrders++; break;
                case OrderStatus::COMPLETED:
                case OrderStatus::ISSUED:
                    completedOrders++;
                    {
                        const Client* client = dm.findClientById(order.getClientId());
                        double orderTotal = 0.0;
                        for (int serviceId : order.getServiceIds()) {
                            const ServiceType* service = dm.findServiceTypeById(serviceId);
                            if (service) orderTotal += service->getPrice();
                        }
                        totalWithoutDiscount += orderTotal;
                        double discount = client ? client->getDiscount() : 0.0;
                        totalDiscount += orderTotal * discount / 100.0;
                    }
                    break;
            }
        }
    }
    
    double totalRevenue = totalWithoutDiscount - totalDiscount;
    
    file << "<!DOCTYPE html>\n<html lang=\"ru\">\n<head>\n";
    file << "<meta charset=\"windows-1251\">\n";
    file << "<title>Отчёт по выручке</title>\n";
    file << "<style>\n";
    file << "body { font-family: 'Courier New', monospace; max-width: 600px; margin: 20px auto; padding: 20px; }\n";
    file << ".line { border-top: 2px solid #000; margin: 10px 0; }\n";
    file << ".thin-line { border-top: 1px dashed #000; margin: 10px 0; }\n";
    file << ".center { text-align: center; }\n";
    file << ".header { font-weight: bold; font-size: 16px; }\n";
    file << ".total { font-weight: bold; }\n";
    file << "</style>\n</head>\n<body>\n";
    
    file << "<div class=\"line\"></div>\n";
    file << "<div class=\"center header\">ОТЧЁТ ПО ВЫРУЧКЕ ЗА ПЕРИОД</div>\n";
    file << "<div class=\"line\"></div>\n";
    
    file << "<p>Период: с " << startDate << " по " << endDate << "</p>\n";
    
    file << "<div class=\"thin-line\"></div>\n";
    file << "<p>Всего заявок за период: " << totalOrders << "<br>\n";
    file << "Из них выполнено: " << completedOrders << "<br>\n";
    file << "В работе: " << inProgressOrders << "<br>\n";
    file << "Новых: " << newOrders << "</p>\n";
    file << "<div class=\"thin-line\"></div>\n";
    
    file << "<p>Выручка по выполненным заявкам:</p>\n";
    file << "<p>Сумма без учёта скидок: " << fixed << setprecision(2) << totalWithoutDiscount << " BYN<br>\n";
    file << "Сумма предоставленных скидок: " << fixed << setprecision(2) << totalDiscount << " BYN</p>\n";
    file << "<div class=\"thin-line\"></div>\n";
    file << "<p class=\"total\">ИТОГО ВЫРУЧКА: " << fixed << setprecision(2) << totalRevenue << " BYN</p>\n";
    file << "<div class=\"thin-line\"></div>\n";
    
    if (completedOrders > 0) {
        file << "<p>Средний чек: " << fixed << setprecision(2) << (totalRevenue / completedOrders) << " BYN</p>\n";
    }
    
    file << "</body>\n</html>\n";
    file.close();
    return true;
}

// ============================================================================
// HTML: СТАТИСТИКА ПО ВИДАМ РАБОТ
// ============================================================================
bool ReportGenerator::saveStatisticsToHtml(const string& startDate,
                                           const string& endDate,
                                           const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) return false;
    
    DataManager& dm = DataManager::getInstance();
    map<int, pair<int, double>> stats;
    double grandTotal = 0.0;
    int totalCount = 0;
    
    for (const auto& order : dm.getOrders()) {
        if (order.getDate() >= startDate && order.getDate() <= endDate) {
            if (order.getStatus() == OrderStatus::COMPLETED || 
                order.getStatus() == OrderStatus::ISSUED) {
                for (int serviceId : order.getServiceIds()) {
                    const ServiceType* service = dm.findServiceTypeById(serviceId);
                    if (service) {
                        stats[serviceId].first++;
                        stats[serviceId].second += service->getPrice();
                        grandTotal += service->getPrice();
                        totalCount++;
                    }
                }
            }
        }
    }
    
    // Сортировка
    vector<tuple<int, string, int, double>> sortedStats;
    for (const auto& stat : stats) {
        const ServiceType* service = dm.findServiceTypeById(stat.first);
        if (service) {
            sortedStats.push_back({stat.first, service->getName(), stat.second.first, stat.second.second});
        }
    }
    sort(sortedStats.begin(), sortedStats.end(),
        [](const auto& a, const auto& b) { return get<3>(a) > get<3>(b); });
    
    file << "<!DOCTYPE html>\n<html lang=\"ru\">\n<head>\n";
    file << "<meta charset=\"windows-1251\">\n";
    file << "<title>Статистика по видам работ</title>\n";
    file << "<style>\n";
    file << "body { font-family: 'Courier New', monospace; max-width: 700px; margin: 20px auto; padding: 20px; }\n";
    file << ".line { border-top: 2px solid #000; margin: 10px 0; }\n";
    file << ".thin-line { border-top: 1px dashed #000; margin: 10px 0; }\n";
    file << ".center { text-align: center; }\n";
    file << ".header { font-weight: bold; font-size: 16px; }\n";
    file << "table { width: 100%; border-collapse: collapse; margin: 10px 0; }\n";
    file << "th, td { padding: 5px; text-align: left; border-bottom: 1px solid #ddd; }\n";
    file << "th { background-color: #f0f0f0; }\n";
    file << ".num { text-align: right; }\n";
    file << ".total-row { font-weight: bold; }\n";
    file << "</style>\n</head>\n<body>\n";
    
    file << "<div class=\"line\"></div>\n";
    file << "<div class=\"center header\">СТАТИСТИКА ПО ВИДАМ РАБОТ</div>\n";
    file << "<div class=\"center\">Период: c " << startDate << " по " << endDate << "</div>\n";
    file << "<div class=\"line\"></div>\n";
    
    file << "<table>\n";
    file << "<tr><th>№</th><th>Вид работы</th><th class=\"num\">Кол-во</th><th class=\"num\">Сумма</th><th class=\"num\">Доля, %</th></tr>\n";
    
    int num = 1;
    for (const auto& item : sortedStats) {
        double percent = grandTotal > 0 ? (get<3>(item) / grandTotal) * 100.0 : 0.0;
        file << "<tr><td>" << num++ << "</td><td>" << get<1>(item) 
             << "</td><td class=\"num\">" << get<2>(item) 
             << "</td><td class=\"num\">" << fixed << setprecision(2) << get<3>(item)
             << "</td><td class=\"num\">" << fixed << setprecision(1) << percent << "%</td></tr>\n";
    }
    
    file << "<tr class=\"total-row\"><td></td><td>ИТОГО:</td><td class=\"num\">" << totalCount
         << "</td><td class=\"num\">" << fixed << setprecision(2) << grandTotal
         << "</td><td class=\"num\">100.0%</td></tr>\n";
    file << "</table>\n";
    
    file << "</body>\n</html>\n";
    file.close();
    return true;
}
