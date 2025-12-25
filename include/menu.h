#ifndef MENU_H
#define MENU_H

#include "database.h"

class Menu {
private:
    Database& db;
    
    void displayMainMenu();
    
    // Меню для сотрудников
    void handleAddEmployee();
    void handleViewEmployees();
    void handleUpdateEmployee();
    void handleDeleteEmployee();
    void handleFullInfo();
    
    // Меню для отделов
    void handleAddDepartment();
    void handleDeleteDepartment();
    void handleDepartmentReport();
    
    // Меню для должностей
    void handleAddPosition();
    
    // Меню для проектов
    void handleAddProject();
    void handleViewProjects();
    void handleDeleteProject();
    void handleAssignEmployeeToProject();
    void handleProjectsReport();
    
    // Аналитические отчеты
    void handleSearch();
    void handleEmployeesWithProjects();
    void handleAboveAvgSalary();
    void handleBudgetAnalysis();
    
    // Служебные
    void handleClearEmployees();
    bool validateEmail(const string& email);
    bool validatePhone(const string& phone);
    
public:
    Menu(Database& database);
    void run();
};

#endif