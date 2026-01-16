#ifndef REPORT_GENERATOR_H
#define REPORT_GENERATOR_H

#include "DataManager.h"
#include <string>
#include <map>

using namespace std;

/**
 * Класс ReportGenerator
 * Формирует отчёты системы
 */
class ReportGenerator {
public:
    // Формирование квитанции для заявки
    static string generateReceipt(int orderId);
    
    // Формирование отчёта по выручке за период
    static string generateRevenueReport(const string& startDate, 
                                             const string& endDate);
    
    // Формирование статистики по видам работ
    static string generateServiceStatistics(const string& startDate,
                                                  const string& endDate);
    
    // Сохранение отчёта в HTML-файл
    static bool saveReceiptToHtml(int orderId, const string& filename);
    
    // Сохранение отчёта по выручке в HTML
    static bool saveRevenueReportToHtml(const string& startDate,
                                        const string& endDate,
                                        const string& filename);
    
    // Сохранение статистики в HTML
    static bool saveStatisticsToHtml(const string& startDate,
                                     const string& endDate,
                                     const string& filename);

private:
    // Вспомогательный метод для создания разделительной линии
    static string createLine(int length, char ch = '-');
    
    // Форматирование денежной суммы
    static string formatMoney(double amount);
    
    // Получение текущей даты
    static string getCurrentDate();
};

#endif // REPORT_GENERATOR_H
