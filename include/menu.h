#ifndef MENU_H
#define MENU_H

#include "database.h"

class Menu {
private:
    Database& db;
    
    void displayMainMenu();
    void handleAddEmployee();
    void handleViewEmployees();
    void handleUpdateEmployee();
    void handleDeleteEmployee();
    void handleDepartmentReport();
    void handleSearch();
    void handleFullInfo();
    void handleAddDepartment();
    void handleAddPosition();
    void handleClearEmployees();
    void handleDeleteDepartment();
    
    bool validateEmail(const string& email);
    bool validatePhone(const string& phone);
    
public:
    Menu(Database& database);
    void run();
};

#endif