#include "menu.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <regex>
#include <locale>
#include <codecvt>
#include <cstring>
using namespace std;

// Функция для подсчета визуальной длины строки с учетом UTF-8
int visual_length(const string& str) {
    int len = 0;
    const char* s = str.c_str();
    while (*s) {
        if ((*s & 0xC0) != 0x80) len++;
        s++;
    }
    return len;
}

// Функция для правильного выравнивания строк с кириллицей
string align_string(const string& str, int width, bool left_align = true) {
    int visual_len = visual_length(str);
    int byte_len = str.length();
    int padding = width - visual_len;
    
    if (padding <= 0) {
        return str.substr(0, width);
    }
    
    if (left_align) {
        return str + string(padding, ' ');
    } else {
        return string(padding, ' ') + str;
    }
}

Menu::Menu(Database& database) : db(database) {}

void Menu::displayMainMenu() {
    cout << "\n========== Учет сотрудников ==========\n";
    cout << "1. Добавить сотрудника\n";
    cout << "2. Просмотр сотрудников\n";
    cout << "3. Обновить данные\n";
    cout << "4. Удалить сотрудника\n";
    cout << "5. Статистика по отделам\n";
    cout << "6. Поиск по отделу\n";
    cout << "7. Полная информация\n";
    cout << "8. Добавить отдел\n";
    cout << "9. Добавить должность\n";
    cout << "10. Очистить всех сотрудников\n";
    cout << "11. Удалить отдел\n";
    cout << "0. Выход\n";
    cout << "Выбор: ";
}

bool Menu::validateEmail(const string& email) {
    const regex pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return regex_match(email, pattern);
}

bool Menu::validatePhone(const string& phone) {
    const regex pattern(R"(^(\+7|8|7)?[0-9]{10}$)");
    return regex_match(phone, pattern);
}

void Menu::run() {
    int choice;
    
    while (true) {
        displayMainMenu();
        cin >> choice;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Некорректный ввод\n";
            continue;
        }
        
        switch (choice) {
            case 0:
                cout << "Выход из программы\n";
                return;
            case 1:
                handleAddEmployee();
                break;
            case 2:
                handleViewEmployees();
                break;
            case 3:
                handleUpdateEmployee();
                break;
            case 4:
                handleDeleteEmployee();
                break;
            case 5:
                handleDepartmentReport();
                break;
            case 6:
                handleSearch();
                break;
            case 7:
                handleFullInfo();
                break;
            case 8:
                handleAddDepartment();
                break;
            case 9:
                handleAddPosition();
                break;
            case 10:
                handleClearEmployees();
                break;
            case 11:
                handleDeleteDepartment();
                break;
            default:
                cout << "Неверный выбор\n";
        }
    }
}

void Menu::handleAddEmployee() {
    Employee emp;
    
    cout << "\n--- Добавление сотрудника ---\n";
    cout << "Имя: ";
    cin >> emp.first_name;
    cout << "Фамилия: ";
    cin >> emp.last_name;
    
    // Валидация email
    do {
        cout << "Email (должен быть уникальным): ";
        cin >> emp.email;
        if (!validateEmail(emp.email)) {
            cout << "Некорректный email! Формат: example@domain.com\n";
        }
    } while (!validateEmail(emp.email));
    
    // Валидация телефона
    do {
        cout << "Телефон: ";
        cin >> emp.phone;
        if (!validatePhone(emp.phone)) {
            cout << "Некорректный телефон! Примеры +79001234567\n";
        }
    } while (!validatePhone(emp.phone));
    
    cout << "Дата приема (YYYY-MM-DD): ";
    cin >> emp.hire_date;
    cout << "Зарплата: ";
    cin >> emp.salary;
    
    auto departments = db.getDepartments();
    cout << "\nОтделы:\n";
    for (const auto& dept : departments) {
        cout << dept.id << ". " << dept.name << " (" << dept.location << ")\n";
    }
    cout << "ID отдела: ";
    cin >> emp.department_id;
    
    auto positions = db.getPositions();
    cout << "\nДолжности:\n";
    for (const auto& pos : positions) {
        cout << pos.id << ". " << pos.title 
             << " (зарплата: " << pos.min_salary << " - " << pos.max_salary << ")\n";
    }
    cout << "ID должности: ";
    cin >> emp.position_id;
    
    if (db.addEmployee(emp)) {
        cout << "Сотрудник добавлен\n";
    }
}

void Menu::handleViewEmployees() {
    auto employees = db.getAllEmployees();
    
    cout << "\n--- Список сотрудников ---\n";
    cout << "+-----+----------------+----------------+------------------------------+---------------+------------+\n";
    cout << "| ID  | Имя            | Фамилия        | Email                        | Телефон       | Зарплата   |\n";
    cout << "+-----+----------------+----------------+------------------------------+---------------+------------+\n";
    
    for (const auto& emp : employees) {
        cout << "| " << setw(3) << emp.id << " | ";
        cout << align_string(emp.first_name, 14) << " | ";
        cout << align_string(emp.last_name, 14) << " | ";
        cout << align_string(emp.email, 28) << " | ";
        cout << align_string(emp.phone, 13) << " | ";
        
        stringstream ss;
        ss << fixed << setprecision(2) << emp.salary;
        cout << align_string(ss.str(), 10, false) << " |" << endl;
    }
    cout << "+-----+----------------+----------------+------------------------------+---------------+------------+\n";
    cout << "Всего сотрудников: " << db.getEmployeeCount() << endl;
}

void Menu::handleUpdateEmployee() {
    int id;
    cout << "ID сотрудника: ";
    cin >> id;
    
    Employee emp;
    emp.id = id;
    
    cout << "Новое имя: ";
    cin >> emp.first_name;
    cout << "Новая фамилия: ";
    cin >> emp.last_name;
    
    do {
        cout << "Email: ";
        cin >> emp.email;
        if (!validateEmail(emp.email)) {
            cout << "Некорректный email!\n";
        }
    } while (!validateEmail(emp.email));
    
    do {
        cout << "Телефон: ";
        cin >> emp.phone;
        if (!validatePhone(emp.phone)) {
            cout << "Некорректный телефон! Пример: +79001234567\n";
        }
    } while (!validatePhone(emp.phone));
    
    cout << "Зарплата: ";
    cin >> emp.salary;
    cout << "ID отдела: ";
    cin >> emp.department_id;
    cout << "ID должности: ";
    cin >> emp.position_id;
    
    if (db.updateEmployee(emp)) {
        cout << "Обновлено\n";
    }
}

void Menu::handleDeleteEmployee() {
    int id;
    cout << "ID для удаления: ";
    cin >> id;
    
    if (db.deleteEmployee(id)) {
        cout << "Удалено\n";
    }
}

void Menu::handleDepartmentReport() {
    auto stats = db.getDepartmentStats();
    
    cout << "\n--- Статистика по отделам ---\n";
    cout << "+----------------------+--------+---------------+---------------+---------------+\n";
    cout << "| Отдел                | Кол-во | Ср. зарплата  | Мин.          | Макс.         |\n";
    cout << "+----------------------+--------+---------------+---------------+---------------+\n";
    
    for (const auto& stat : stats) {
        cout << "| " << align_string(stat.name, 20) << " | ";
        cout << align_string(to_string(stat.employee_count), 6, false) << " | ";
        
        stringstream ss_avg, ss_min, ss_max;
        ss_avg << fixed << setprecision(2) << stat.avg_salary;
        ss_min << fixed << setprecision(2) << stat.min_salary;
        ss_max << fixed << setprecision(2) << stat.max_salary;
        
        cout << align_string(ss_avg.str(), 13, false) << " | ";
        cout << align_string(ss_min.str(), 13, false) << " | ";
        cout << align_string(ss_max.str(), 13, false) << " |" << endl;
    }
    cout << "+----------------------+--------+---------------+---------------+---------------+\n";
}

void Menu::handleSearch() {
    int dept_id;
    cout << "ID отдела: ";
    cin >> dept_id;
    
    auto employees = db.getEmployeesByDepartment(dept_id);
    
    cout << "\nСотрудники отдела:\n";
    cout << "+-----+----------------+----------------+------------------------------+---------------+------------+\n";
    cout << "| ID  | Имя            | Фамилия        | Email                        | Телефон       | Зарплата   |\n";
    cout << "+-----+----------------+----------------+------------------------------+---------------+------------+\n";
    
    for (const auto& emp : employees) {
        emp.display();
    }
    cout << "+-----+----------------+----------------+------------------------------+---------------+------------+\n";
}

void Menu::handleFullInfo() {
    auto employees = db.getEmployeesFull();
    
    cout << "\n--- Полная информация ---\n";
    cout << "+-----+-------------+-------------+--------------------+---------------+------------+\n";
    cout << "| ID  | Имя         | Фамилия     | Отдел              | Должность     | Зарплата   |\n";
    cout << "+-----+-------------+-------------+--------------------+---------------+------------+\n";
    
    for (const auto& emp : employees) {
        cout << "| " << setw(3) << emp.id << " | ";
        cout << align_string(emp.first_name, 11) << " | ";
        cout << align_string(emp.last_name, 11) << " | ";
        cout << align_string(emp.department_name, 18) << " | ";
        cout << align_string(emp.position_title, 13) << " | ";
        
        stringstream ss;
        ss << fixed << setprecision(2) << emp.salary;
        cout << align_string(ss.str(), 10, false) << " |" << endl;
    }
    cout << "+-----+-------------+-------------+--------------------+---------------+------------+\n";
}
void Menu::handleAddDepartment() {
    string name, location;
    
    cout << "\n--- Добавление отдела ---\n";
    cout << "Название отдела: ";
    cin.ignore();
    getline(cin, name);
    cout << "Расположение: ";
    getline(cin, location);
    
    if (db.addDepartment(name, location)) {
        cout << "Отдел добавлен\n";
    }
}

void Menu::handleAddPosition() {
    string title;
    double min_salary, max_salary;
    
    cout << "\n--- Добавление должности ---\n";
    cout << "Название должности: ";
    cin.ignore();
    getline(cin, title);
    cout << "Минимальная зарплата: ";
    cin >> min_salary;
    cout << "Максимальная зарплата: ";
    cin >> max_salary;
    
    if (db.addPosition(title, min_salary, max_salary)) {
        cout << "Должность добавлена\n";
    }
}

void Menu::handleClearEmployees() {
    char confirm;
    cout << "\nВы уверены, что хотите удалить ВСЕХ сотрудников? (y/n): ";
    cin >> confirm;
    
    if (confirm == 'y' || confirm == 'Y') {
        if (db.clearAllEmployees()) {
            cout << "Все сотрудники удалены\n";
        }
    } else {
        cout << "Операция отменена\n";
    }
}

void Menu::handleDeleteDepartment() {
    cout << "\n--- Удаление отдела ---\n";
    
    // Показываем список отделов
    auto departments = db.getDepartments();
    cout << "\nСписок отделов:\n";
    for (const auto& dept : departments) {
        cout << dept.id << ". " << dept.name << " (" << dept.location << ")\n";
    }
    
    int dept_id;
    cout << "\nВведите ID отдела для удаления (0 для отмены): ";
    cin >> dept_id;
    
    if (dept_id == 0) {
        cout << "Операция отменена\n";
        return;
    }
    
    // Проверяем, есть ли сотрудники в отделе
    int employee_count = db.getEmployeeCountByDepartment(dept_id);
    
    if (employee_count > 0) {
        cout << "\nВНИМАНИЕ: В этом отделе работает " << employee_count << " сотрудник(ов)!\n";
        cout << "Удаление отдела приведет к проблемам с данными сотрудников.\n";
        cout << "Сначала переведите сотрудников в другие отделы или удалите их.\n";
        cout << "\nВсе равно удалить отдел? (y/n): ";
        
        char confirm;
        cin >> confirm;
        
        if (confirm != 'y' && confirm != 'Y') {
            cout << "Операция отменена\n";
            return;
        }
    } else {
        cout << "\nВы уверены, что хотите удалить этот отдел? (y/n): ";
        char confirm;
        cin >> confirm;
        
        if (confirm != 'y' && confirm != 'Y') {
            cout << "Операция отменена\n";
            return;
        }
    }
    
    if (db.deleteDepartment(dept_id)) {
        cout << "Отдел успешно удален\n";
    } else {
        cout << "Не удалось удалить отдел. Возможно, он не существует.\n";
    }
}