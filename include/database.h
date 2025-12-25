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
    
    // Операции с сотрудниками
    vector<Employee> getAllEmployees();
    Employee getEmployeeById(int id);
    bool addEmployee(const Employee& emp);
    bool updateEmployee(const Employee& emp);
    bool deleteEmployee(int id);
    vector<Employee> getEmployeesByDepartment(int dept_id);
    
    // Операции с отделами
    vector<Department> getDepartments();
    bool addDepartment(const Department& dept);
    bool deleteDepartment(int id);
    int getEmployeeCountByDepartment(int dept_id);
    
    // Операции с должностями
    vector<Position> getPositions();
    bool addPosition(const Position& pos);
    
    // Операции с проектами
    vector<Project> getProjects();
    bool addProject(const Project& proj);
    bool deleteProject(int id);
    
    // Назначения на проекты
    bool assignEmployeeToProject(const EmployeeProject& assignment);
    bool removeEmployeeFromProject(int employee_id, int project_id);
    
    // Аналитические запросы
    double getAverageSalaryByDepartment(int dept_id);
    int getEmployeeCount();
    vector<EmployeeFull> getEmployeesFull();
    vector<DepartmentStats> getDepartmentStats();
    vector<EmployeeWithProject> getEmployeesWithProjects();
    vector<ProjectWithStats> getProjectsWithEmployees();
    vector<EmployeeAboveAvg> getAboveAvgSalaryEmployees();
    vector<DepartmentBudgetAnalysis> getDepartmentBudgetAnalysis();
    
    // Служебные
    bool clearAllEmployees();
};

#endif