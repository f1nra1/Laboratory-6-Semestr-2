#include "menu.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>

using namespace std;

// Функция для подсчета визуальной длины строки с учетом UTF-8
static int visual_length(const string& str) {
    int len = 0;
    const char* s = str.c_str();
    while (*s) {
        if ((*s & 0xC0) != 0x80) len++;
        s++;
    }
    return len;
}

// Функция для правильного выравнивания строк с кириллицей
static string align_string(const string& str, int width, bool left_align = true) {
    int visual_len = visual_length(str);
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
    cout << "\n╔═══════════════════════════════════════════════╗\n";
    cout << "║        СИСТЕМА УЧЕТА СОТРУДНИКОВ             ║\n";
    cout << "╚═══════════════════════════════════════════════╝\n";
    cout << "\n┌─── СОТРУДНИКИ ───────────────────────────────┐\n";
    cout << "│ 1.  Добавить сотрудника                      │\n";
    cout << "│ 2.  Просмотр сотрудников                     │\n";
    cout << "│ 3.  Обновить данные сотрудника               │\n";
    cout << "│ 4.  Удалить сотрудника                       │\n";
    cout << "│ 5.  Полная информация о сотрудниках          │\n";
    cout << "└──────────────────────────────────────────────┘\n";
    cout << "\n┌─── ОТДЕЛЫ И ДОЛЖНОСТИ ───────────────────────┐\n";
    cout << "│ 6.  Добавить отдел                           │\n";
    cout << "│ 7.  Удалить отдел                            │\n";
    cout << "│ 8.  Добавить должность                       │\n";
    cout << "└──────────────────────────────────────────────┘\n";
    cout << "\n┌─── ПРОЕКТЫ ──────────────────────────────────┐\n";
    cout << "│ 9.  Добавить проект                          │\n";
    cout << "│ 10. Просмотр проектов                        │\n";
    cout << "│ 11. Назначить сотрудника на проект           │\n";
    cout << "│ 12. Удалить проект                           │\n";
    cout << "└──────────────────────────────────────────────┘\n";
    cout << "\n┌─── ОТЧЕТЫ И АНАЛИТИКА ───────────────────────┐\n";
    cout << "│ 13. Статистика по отделам                    │\n";
    cout << "│ 14. Поиск по отделу                          │\n";
    cout << "│ 15. Сотрудники и их проекты                  │\n";
    cout << "│ 16. Проекты со статистикой                   │\n";
    cout << "│ 17. Сотрудники с зарплатой выше средней      │\n";
    cout << "│ 18. Анализ бюджета отделов                   │\n";
    cout << "└──────────────────────────────────────────────┘\n";
    cout << "\n┌─── СЛУЖЕБНОЕ ────────────────────────────────┐\n";
    cout << "│ 19. Очистить всех сотрудников                │\n";
    cout << "│ 0.  Выход                                    │\n";
    cout << "└──────────────────────────────────────────────┘\n";
    cout << "\nВыбор: ";
}

bool Menu::validateEmail(const string& email) {
    return true;
}

bool Menu::validatePhone(const string& phone) {
    return true;
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
                cout << "\nВыход из программы. До свидания!\n";
                return;
            case 1: handleAddEmployee(); break;
            case 2: handleViewEmployees(); break;
            case 3: handleUpdateEmployee(); break;
            case 4: handleDeleteEmployee(); break;
            case 5: handleFullInfo(); break;
            case 6: handleAddDepartment(); break;
            case 7: handleDeleteDepartment(); break;
            case 8: handleAddPosition(); break;
            case 9: handleAddProject(); break;
            case 10: handleViewProjects(); break;
            case 11: handleAssignEmployeeToProject(); break;
            case 12: handleDeleteProject(); break;
            case 13: handleDepartmentReport(); break;
            case 14: handleSearch(); break;
            case 15: handleEmployeesWithProjects(); break;
            case 16: handleProjectsReport(); break;
            case 17: handleAboveAvgSalary(); break;
            case 18: handleBudgetAnalysis(); break;
            case 19: handleClearEmployees(); break;
            default:
                cout << "Неверный выбор\n";
        }
    }
}

// ==================== СОТРУДНИКИ ====================

void Menu::handleAddEmployee() {
    Employee emp;
    emp.manager_id = 0;
    
    cout << "\n┌─── ДОБАВЛЕНИЕ СОТРУДНИКА ────────────────────┐\n";
    cout << "Имя: ";
    cin >> emp.first_name;
    cout << "Фамилия: ";
    cin >> emp.last_name;
    cout << "Email (уникальный): ";
    cin >> emp.email;
    cout << "Телефон: ";
    cin >> emp.phone;
    cout << "Дата приема (YYYY-MM-DD): ";
    cin >> emp.hire_date;
    cout << "Зарплата: ";
    cin >> emp.salary;
    
    auto departments = db.getDepartments();
    cout << "\nОтделы:\n";
    for (const auto& dept : departments) {
        cout << "  " << dept.id << ". " << dept.name 
             << " (" << dept.location << ")\n";
    }
    cout << "ID отдела: ";
    cin >> emp.department_id;
    
    auto positions = db.getPositions();
    cout << "\nДолжности:\n";
    for (const auto& pos : positions) {
        cout << "  " << pos.id << ". " << pos.title 
             << " (" << pos.min_salary << " - " << pos.max_salary << ")\n";
    }
    cout << "ID должности: ";
    cin >> emp.position_id;
    
    cout << "ID менеджера (0 если нет): ";
    cin >> emp.manager_id;
    
    if (db.addEmployee(emp)) {
        cout << "✓ Сотрудник успешно добавлен\n";
    } else {
        cout << "✗ Ошибка при добавлении\n";
    }
}

void Menu::handleViewEmployees() {
    auto employees = db.getAllEmployees();
    
    cout << "\n┌─── СПИСОК СОТРУДНИКОВ ───────────────────────────────────────────────────────────────────────────┐\n";
    cout << "+-----+----------------+----------------+------------------------------+---------------+------------+\n";
    cout << "| ID  | Имя            | Фамилия        | Email                        | Телефон       | Зарплата   |\n";
    cout << "+-----+----------------+----------------+------------------------------+---------------+------------+\n";
    
    for (const auto& emp : employees) {
        emp.display();
    }
    cout << "+-----+----------------+----------------+------------------------------+---------------+------------+\n";
    cout << "Всего сотрудников: " << db.getEmployeeCount() << endl;
}

void Menu::handleUpdateEmployee() {
    int id;
    cout << "\n┌─── ОБНОВЛЕНИЕ ДАННЫХ СОТРУДНИКА ─────────────┐\n";
    cout << "ID сотрудника: ";
    cin >> id;
    
    Employee emp;
    emp.id = id;
    
    cout << "Новое имя: ";
    cin >> emp.first_name;
    cout << "Новая фамилия: ";
    cin >> emp.last_name;
    cout << "Email: ";
    cin >> emp.email;
    cout << "Телефон: ";
    cin >> emp.phone;
    cout << "Зарплата: ";
    cin >> emp.salary;
    cout << "ID отдела: ";
    cin >> emp.department_id;
    cout << "ID должности: ";
    cin >> emp.position_id;
    cout << "ID менеджера (0 если нет): ";
    cin >> emp.manager_id;
    
    if (db.updateEmployee(emp)) {
        cout << "✓ Данные обновлены\n";
    } else {
        cout << "✗ Ошибка обновления\n";
    }
}

void Menu::handleDeleteEmployee() {
    int id;
    cout << "\n┌─── УДАЛЕНИЕ СОТРУДНИКА ──────────────────────┐\n";
    cout << "ID для удаления: ";
    cin >> id;
    
    if (db.deleteEmployee(id)) {
        cout << "✓ Сотрудник удален\n";
    } else {
        cout << "✗ Ошибка удаления\n";
    }
}

void Menu::handleFullInfo() {
    auto employees = db.getEmployeesFull();
    
    cout << "\n┌─── ПОЛНАЯ ИНФОРМАЦИЯ О СОТРУДНИКАХ ──────────────────────────────────────────────────────────────┐\n";
    cout << "+-----+-------------+-------------+--------------------+---------------+------------+\n";
    cout << "| ID  | Имя         | Фамилия     | Отдел              | Должность     | Зарплата   |\n";
    cout << "+-----+-------------+-------------+--------------------+---------------+------------+\n";
    
    for (const auto& emp : employees) {
        emp.display();
    }
    cout << "+-----+-------------+-------------+--------------------+---------------+------------+\n";
}

// ==================== ОТДЕЛЫ ====================

void Menu::handleAddDepartment() {
    Department dept;
    
    cout << "\n┌─── ДОБАВЛЕНИЕ ОТДЕЛА ────────────────────────┐\n";
    cout << "Название отдела: ";
    cin.ignore();
    getline(cin, dept.name);
    cout << "Расположение: ";
    getline(cin, dept.location);
    cout << "Бюджет: ";
    cin >> dept.budget;
    cout << "ID менеджера (0 если нет): ";
    cin >> dept.manager_id;
    
    if (db.addDepartment(dept)) {
        cout << "✓ Отдел добавлен\n";
    } else {
        cout << "✗ Ошибка добавления\n";
    }
}

void Menu::handleDeleteDepartment() {
    cout << "\n┌─── УДАЛЕНИЕ ОТДЕЛА ──────────────────────────┐\n";
    
    auto departments = db.getDepartments();
    cout << "\nСписок отделов:\n";
    for (const auto& dept : departments) {
        cout << "  " << dept.id << ". " << dept.name 
             << " (" << dept.location << ")\n";
    }
    
    int dept_id;
    cout << "\nID отдела для удаления (0 для отмены): ";
    cin >> dept_id;
    
    if (dept_id == 0) {
        cout << "Операция отменена\n";
        return;
    }
    
    int employee_count = db.getEmployeeCountByDepartment(dept_id);
    
    if (employee_count > 0) {
        cout << "\n⚠ ВНИМАНИЕ: В отделе " << employee_count << " сотрудник(ов)!\n";
        cout << "Удалить отдел? (y/n): ";
        char confirm;
        cin >> confirm;
        if (confirm != 'y' && confirm != 'Y') {
            cout << "Операция отменена\n";
            return;
        }
    }
    
    if (db.deleteDepartment(dept_id)) {
        cout << "✓ Отдел удален\n";
    } else {
        cout << "✗ Ошибка удаления\n";
    }
}

void Menu::handleDepartmentReport() {
    auto stats = db.getDepartmentStats();
    
    cout << "\n┌─── СТАТИСТИКА ПО ОТДЕЛАМ ────────────────────────────────────────────────────────────────────────┐\n";
    cout << "+----------------------+--------+---------------+---------------+---------------+---------------+\n";
    cout << "| Отдел                | Кол-во | Ср. зарплата  | Мин.          | Макс.         | Общая         |\n";
    cout << "+----------------------+--------+---------------+---------------+---------------+---------------+\n";
    
    for (const auto& stat : stats) {
        cout << "| " << align_string(stat.name, 20) << " | ";
        cout << align_string(to_string(stat.employee_count), 6, false) << " | ";
        
        stringstream ss_avg, ss_min, ss_max, ss_total;
        ss_avg << fixed << setprecision(2) << stat.avg_salary;
        ss_min << fixed << setprecision(2) << stat.min_salary;
        ss_max << fixed << setprecision(2) << stat.max_salary;
        ss_total << fixed << setprecision(2) << stat.total_salary;
        
        cout << align_string(ss_avg.str(), 13, false) << " | ";
        cout << align_string(ss_min.str(), 13, false) << " | ";
        cout << align_string(ss_max.str(), 13, false) << " | ";
        cout << align_string(ss_total.str(), 13, false) << " |" << endl;
    }
    cout << "+----------------------+--------+---------------+---------------+---------------+---------------+\n";
}

// ==================== ДОЛЖНОСТИ ====================

void Menu::handleAddPosition() {
    Position pos;
    
    cout << "\n┌─── ДОБАВЛЕНИЕ ДОЛЖНОСТИ ─────────────────────┐\n";
    cout << "Название должности: ";
    cin.ignore();
    getline(cin, pos.title);
    cout << "Минимальная зарплата: ";
    cin >> pos.min_salary;
    cout << "Максимальная зарплата: ";
    cin >> pos.max_salary;
    cout << "Описание: ";
    cin.ignore();
    getline(cin, pos.description);
    
    if (db.addPosition(pos)) {
        cout << "✓ Должность добавлена\n";
    } else {
        cout << "✗ Ошибка добавления\n";
    }
}

// ==================== ПРОЕКТЫ ====================

void Menu::handleAddProject() {
    Project proj;
    
    cout << "\n┌─── ДОБАВЛЕНИЕ ПРОЕКТА ───────────────────────┐\n";
    cout << "Название проекта: ";
    cin.ignore();
    getline(cin, proj.name);
    cout << "Описание: ";
    getline(cin, proj.description);
    cout << "Дата начала (YYYY-MM-DD): ";
    cin >> proj.start_date;
    cout << "Дата окончания (YYYY-MM-DD): ";
    cin >> proj.end_date;
    cout << "Бюджет: ";
    cin >> proj.budget;
    
    cout << "\nСтатус:\n";
    cout << "  1. планирование\n";
    cout << "  2. в работе\n";
    cout << "  3. завершен\n";
    cout << "  4. приостановлен\n";
    cout << "Выбор: ";
    int status_choice;
    cin >> status_choice;
    
    switch (status_choice) {
        case 1: proj.status = "планирование"; break;
        case 2: proj.status = "в работе"; break;
        case 3: proj.status = "завершен"; break;
        case 4: proj.status = "приостановлен"; break;
        default: proj.status = "планирование";
    }
    
    auto departments = db.getDepartments();
    cout << "\nОтделы:\n";
    for (const auto& dept : departments) {
        cout << "  " << dept.id << ". " << dept.name << "\n";
    }
    cout << "ID отдела: ";
    cin >> proj.department_id;
    
    if (db.addProject(proj)) {
        cout << "✓ Проект добавлен\n";
    } else {
        cout << "✗ Ошибка добавления\n";
    }
}

void Menu::handleViewProjects() {
    auto projects = db.getProjects();
    
    cout << "\n┌─── СПИСОК ПРОЕКТОВ ──────────────────────────────────────────────────────────────────────────────┐\n";
    for (const auto& proj : projects) {
        cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        cout << "ID: " << proj.id << "\n";
        cout << "Название: " << proj.name << "\n";
        cout << "Описание: " << proj.description << "\n";
        cout << "Период: " << proj.start_date << " — " << proj.end_date << "\n";
        cout << "Бюджет: " << fixed << setprecision(2) << proj.budget << "\n";
        cout << "Статус: " << proj.status << "\n";
    }
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
}

void Menu::handleDeleteProject() {
    int id;
    cout << "\n┌─── УДАЛЕНИЕ ПРОЕКТА ─────────────────────────┐\n";
    cout << "ID проекта для удаления: ";
    cin >> id;
    
    if (db.deleteProject(id)) {
        cout << "✓ Проект удален\n";
    } else {
        cout << "✗ Ошибка удаления\n";
    }
}

void Menu::handleAssignEmployeeToProject() {
    EmployeeProject assignment;
    
    cout << "\n┌─── НАЗНАЧЕНИЕ НА ПРОЕКТ ─────────────────────┐\n";
    
    auto employees = db.getAllEmployees();
    cout << "\nСотрудники:\n";
    for (const auto& emp : employees) {
        cout << "  " << emp.id << ". " << emp.first_name 
             << " " << emp.last_name << "\n";
    }
    cout << "ID сотрудника: ";
    cin >> assignment.employee_id;
    
    auto projects = db.getProjects();
    cout << "\nПроекты:\n";
    for (const auto& proj : projects) {
        cout << "  " << proj.id << ". " << proj.name << "\n";
    }
    cout << "ID проекта: ";
    cin >> assignment.project_id;
    
    cout << "Роль в проекте: ";
    cin.ignore();
    getline(cin, assignment.role);
    
    cout << "Дата назначения (YYYY-MM-DD): ";
    cin >> assignment.assigned_date;
    
    cout << "Часов выделено: ";
    cin >> assignment.hours_allocated;
    
    if (db.assignEmployeeToProject(assignment)) {
        cout << "✓ Сотрудник назначен на проект\n";
    } else {
        cout << "✗ Ошибка назначения\n";
    }
}

void Menu::handleProjectsReport() {
    auto projects = db.getProjectsWithEmployees();
    
    cout << "\n┌─── ПРОЕКТЫ СО СТАТИСТИКОЙ ───────────────────────────────────────────────────────────────────────┐\n";
    cout << "+-----+---------------------------+--------------+--------------+----------+------------+\n";
    cout << "| ID  | Проект                    | Статус       | Бюджет       | Сотр.    | Часов      |\n";
    cout << "+-----+---------------------------+--------------+--------------+----------+------------+\n";
    
    for (const auto& proj : projects) {
        proj.display();
    }
    cout << "+-----+---------------------------+--------------+--------------+----------+------------+\n";
}

// ==================== АНАЛИТИКА ====================

void Menu::handleSearch() {
    int dept_id;
    cout << "\n┌─── ПОИСК ПО ОТДЕЛУ ──────────────────────────┐\n";
    cout << "ID отдела: ";
    cin >> dept_id;
    
    auto employees = db.getEmployeesByDepartment(dept_id);
    
    cout << "\n+-----+----------------+----------------+------------------------------+---------------+------------+\n";
    cout << "| ID  | Имя            | Фамилия        | Email                        | Телефон       | Зарплата   |\n";
    cout << "+-----+----------------+----------------+------------------------------+---------------+------------+\n";
    
    for (const auto& emp : employees) {
        emp.display();
    }
    cout << "+-----+----------------+----------------+------------------------------+---------------+------------+\n";
    cout << "Найдено сотрудников: " << employees.size() << endl;
}

void Menu::handleEmployeesWithProjects() {
    auto data = db.getEmployeesWithProjects();
    
    cout << "\n┌─── СОТРУДНИКИ И ИХ ПРОЕКТЫ ──────────────────────────────────────────────────────────────────────┐\n";
    cout << "+-----+----------------------+---------------------------+--------------------+----------+--------------+\n";
    cout << "| ID  | Сотрудник            | Проект                    | Роль               | Часов    | Статус       |\n";
    cout << "+-----+----------------------+---------------------------+--------------------+----------+--------------+\n";
    
    for (const auto& item : data) {
        item.display();
    }
    cout << "+-----+----------------------+---------------------------+--------------------+----------+--------------+\n";
}

void Menu::handleAboveAvgSalary() {
    auto employees = db.getAboveAvgSalaryEmployees();
    
    cout << "\n┌─── СОТРУДНИКИ С ЗАРПЛАТОЙ ВЫШЕ СРЕДНЕЙ ПО ОТДЕЛУ ────────────────────────────────────────────────┐\n";
    cout << "+-----+----------------------+--------------------+------------+------------+\n";
    cout << "| ID  | Сотрудник            | Отдел              | Зарплата   | Средняя    |\n";
    cout << "+-----+----------------------+--------------------+------------+------------+\n";
    
    for (const auto& emp : employees) {
        emp.display();
    }
    cout << "+-----+----------------------+--------------------+------------+------------+\n";
}

void Menu::handleBudgetAnalysis() {
    auto analysis = db.getDepartmentBudgetAnalysis();
    
    cout << "\n┌─── АНАЛИЗ БЮДЖЕТА ОТДЕЛОВ ───────────────────────────────────────────────────────────────────────┐\n";
    cout << "+--------------------+--------------+--------------+--------------+--------------+\n";
    cout << "| Отдел              | Бюджет       | ЗП (год)     | Проекты      | Остаток      |\n";
    cout << "+--------------------+--------------+--------------+--------------+--------------+\n";
    
    for (const auto& item : analysis) {
        item.display();
    }
    cout << "+--------------------+--------------+--------------+--------------+--------------+\n";
}

void Menu::handleClearEmployees() {
    char confirm;
    cout << "\n⚠ ВНИМАНИЕ! Удалить ВСЕХ сотрудников? (y/n): ";
    cin >> confirm;
    
    if (confirm == 'y' || confirm == 'Y') {
        if (db.clearAllEmployees()) {
            cout << "✓ Все сотрудники удалены\n";
        }
    } else {
        cout << "Операция отменена\n";
    }
}