#include "employee.h"
#include <iomanip>
#include <iostream>

using namespace std;

void Employee::display() const {
    cout << "| " << setw(3) << id << " | ";
    cout.width(14); cout << left << first_name << " | ";
    cout.width(14); cout << left << last_name << " | ";
    cout.width(28); cout << left << email << " | ";
    cout.width(13); cout << left << phone << " | ";
    cout.width(10); cout << right << fixed << setprecision(2) << salary << " |" << endl;
}

void EmployeeFull::display() const {
    cout << "| " << setw(3) << id << " | ";
    cout.width(11); cout << left << first_name << " | ";
    cout.width(11); cout << left << last_name << " | ";
    cout.width(18); cout << left << department_name << " | ";
    cout.width(13); cout << left << position_title << " | ";
    cout.width(10); cout << right << fixed << setprecision(2) << salary << " |" << endl;
}