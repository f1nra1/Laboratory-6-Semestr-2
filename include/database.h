#ifndef DATABASE_H
#define DATABASE_H

#include <pqxx/pqxx>
#include <vector>
#include <memory>
#include <map>
#include "employee.h"

using namespace std;
using namespace pqxx;

class Database {
private:
    unique_ptr<connection> conn;
    map<string, string> queries;
    
    void loadQueries();
    string getQuery(const string& queryName);
    
public:
    Database(const string& conn_string);
    ~Database();
    
    bool connect(const string& conn_string);
    void disconnect();
    
    vector<Employee> getAllEmployees();
    Employee getEmployeeById(int id);
    bool addEmployee(const Employee& emp);
    bool updateEmployee(const Employee& emp);
    bool deleteEmployee(int id);
    
    vector<Department> getDepartments();
    vector<Position> getPositions();
    bool addDepartment(const string& name, const string& location);
    bool addPosition(const string& title, double min_salary, double max_salary);
    
    vector<Employee> getEmployeesByDepartment(int dept_id);
    double getAverageSalaryByDepartment(int dept_id);
    int getEmployeeCount();
    vector<EmployeeFull> getEmployeesFull();
    vector<DepartmentStats> getDepartmentStats();
    
    bool clearAllEmployees();
    bool deleteDepartment(int id);
    int getEmployeeCountByDepartment(int dept_id);
};

#endif