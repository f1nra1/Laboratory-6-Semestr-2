#include "employee.h"
#include <iomanip>
#include <iostream>
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

void Employee::display() const {
    cout << "| " << setw(3) << id << " | ";
    cout << align_string(first_name, 14) << " | ";
    cout << align_string(last_name, 14) << " | ";
    cout << align_string(email, 28) << " | ";
    cout << align_string(phone, 13) << " | ";
    
    stringstream ss;
    ss << fixed << setprecision(2) << salary;
    cout << align_string(ss.str(), 10, false) << " |" << endl;
}

void EmployeeFull::display() const {
    cout << "| " << setw(3) << id << " | ";
    cout << align_string(first_name, 11) << " | ";
    cout << align_string(last_name, 11) << " | ";
    cout << align_string(department_name, 18) << " | ";
    cout << align_string(position_title, 13) << " | ";
    
    stringstream ss;
    ss << fixed << setprecision(2) << salary;
    cout << align_string(ss.str(), 10, false) << " |" << endl;
}

void EmployeeWithProject::display() const {
    cout << "| " << setw(3) << employee_id << " | ";
    cout << align_string(full_name, 20) << " | ";
    cout << align_string(project_name, 25) << " | ";
    cout << align_string(role, 18) << " | ";
    
    stringstream ss;
    ss << fixed << setprecision(1) << hours_allocated;
    cout << align_string(ss.str(), 8, false) << " | ";
    cout << align_string(project_status, 12) << " |" << endl;
}

void ProjectWithStats::display() const {
    cout << "| " << setw(3) << id << " | ";
    cout << align_string(project_name, 25) << " | ";
    cout << align_string(status, 12) << " | ";
    
    stringstream ss_budget, ss_hours;
    ss_budget << fixed << setprecision(2) << budget;
    ss_hours << fixed << setprecision(1) << total_hours;
    
    cout << align_string(ss_budget.str(), 12, false) << " | ";
    cout << align_string(to_string(employee_count), 8, false) << " | ";
    cout << align_string(ss_hours.str(), 10, false) << " |" << endl;
}

void EmployeeAboveAvg::display() const {
    cout << "| " << setw(3) << id << " | ";
    cout << align_string(first_name + " " + last_name, 20) << " | ";
    cout << align_string(department_name, 18) << " | ";
    
    stringstream ss_sal, ss_avg;
    ss_sal << fixed << setprecision(2) << salary;
    ss_avg << fixed << setprecision(2) << department_avg_salary;
    
    cout << align_string(ss_sal.str(), 10, false) << " | ";
    cout << align_string(ss_avg.str(), 10, false) << " |" << endl;
}

void DepartmentBudgetAnalysis::display() const {
    cout << "| " << align_string(name, 18) << " | ";
    
    stringstream ss_budget, ss_salary, ss_proj, ss_rem;
    ss_budget << fixed << setprecision(2) << department_budget;
    ss_salary << fixed << setprecision(2) << annual_salary_cost;
    ss_proj << fixed << setprecision(2) << projects_budget;
    ss_rem << fixed << setprecision(2) << remaining_budget;
    
    cout << align_string(ss_budget.str(), 12, false) << " | ";
    cout << align_string(ss_salary.str(), 12, false) << " | ";
    cout << align_string(ss_proj.str(), 12, false) << " | ";
    cout << align_string(ss_rem.str(), 12, false) << " |" << endl;
}