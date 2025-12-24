#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <string>
#include <iostream>

using namespace std;

struct Employee {
    int id;
    string first_name;
    string last_name;
    string email;
    string phone;
    string hire_date;
    double salary;
    int department_id;
    int position_id;
    
    void display() const;
};

struct EmployeeFull {
    int id;
    string first_name;
    string last_name;
    string email;
    string phone;
    string hire_date;
    double salary;
    string department_name;
    string position_title;
    
    void display() const;
};

struct Department {
    int id;
    string name;
    string location;
};

struct Position {
    int id;
    string title;
    double min_salary;
    double max_salary;
};

struct DepartmentStats {
    int id;
    string name;
    int employee_count;
    double avg_salary;
    double max_salary;
    double min_salary;
};

#endif