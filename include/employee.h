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
    int manager_id;
    
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
    double budget;
    int manager_id;
};

struct Position {
    int id;
    string title;
    double min_salary;
    double max_salary;
    string description;
};

struct DepartmentStats {
    int id;
    string name;
    string location;
    int employee_count;
    double avg_salary;
    double max_salary;
    double min_salary;
    double total_salary;
};

struct Project {
    int id;
    string name;
    string description;
    string start_date;
    string end_date;
    double budget;
    string status;
    int department_id;
};

struct EmployeeProject {
    int id;
    int employee_id;
    int project_id;
    string role;
    string assigned_date;
    double hours_allocated;
};

struct EmployeeWithProject {
    int employee_id;
    string full_name;
    string project_name;
    string role;
    double hours_allocated;
    string project_status;
    string department_name;
    
    void display() const;
};

struct ProjectWithStats {
    int id;
    string project_name;
    string status;
    double budget;
    string department_name;
    int employee_count;
    double total_hours;
    
    void display() const;
};

struct EmployeeAboveAvg {
    int id;
    string first_name;
    string last_name;
    double salary;
    string department_name;
    double department_avg_salary;
    
    void display() const;
};

struct DepartmentBudgetAnalysis {
    int id;
    string name;
    double department_budget;
    double annual_salary_cost;
    double projects_budget;
    double remaining_budget;
    
    void display() const;
};

#endif