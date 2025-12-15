#ifndef REPORT_GENERATOR_H
#define REPORT_GENERATOR_H

#include "DataManager.h"
#include <string>
#include <map>

/**
 * Класс ReportGenerator
 * Формирует отчёты системы
 */
class ReportGenerator {
public:
    // Формирование квитанции для заявки
    static std::string generateReceipt(int orderId);
    
    // Формирование отчёта по выручке за период
    static std::string generateRevenueReport(const std::string& startDate, 
                                             const std::string& endDate);
    
    // Формирование статистики по видам работ
    static std::string generateServiceStatistics(const std::string& startDate,
                                                  const std::string& endDate);
    
    // Сохранение отчёта в HTML-файл
    static bool saveReceiptToHtml(int orderId, const std::string& filename);
    
    // Сохранение отчёта по выручке в HTML
    static bool saveRevenueReportToHtml(const std::string& startDate,
                                        const std::string& endDate,
                                        const std::string& filename);
    
    // Сохранение статистики в HTML
    static bool saveStatisticsToHtml(const std::string& startDate,
                                     const std::string& endDate,
                                     const std::string& filename);

private:
    // Вспомогательный метод для создания разделительной линии
    static std::string createLine(int length, char ch = '-');
    
    // Форматирование денежной суммы
    static std::string formatMoney(double amount);
    
    // Получение текущей даты
    static std::string getCurrentDate();
};

#endif // REPORT_GENERATOR_H
