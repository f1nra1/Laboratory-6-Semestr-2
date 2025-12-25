#include "database.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

Database::Database(const string& conn_string) {
    loadQueries();
    connect(conn_string);
}

Database::~Database() {
    disconnect();
}

void Database::loadQueries() {
    vector<string> paths = {
        "sql/queries.sql",
        "../sql/queries.sql", 
        "../../sql/queries.sql",
        "queries.sql",
        "/Users/arseny/Desktop/Laba6/sql/queries.sql"
};
    
    ifstream file;
    bool found = false;
    
    for (const auto& path : paths) {
        file.open(path);
        if (file.is_open()) {
            cout << "Загружены запросы из: " << path << endl;
            found = true;
            break;
        }
    }
    
    if (!found) {
        cerr << "ОШИБКА: Не удалось найти файл queries_extended.sql\n";
        exit(1);
    }
    
    string line;
    string currentQuery;
    string queryName;
    
    while (getline(file, line)) {
        if (line.find("-- ") == 0 && line.find("ЗАПРОС") == string::npos) {
            if (!queryName.empty() && !currentQuery.empty()) {
                queries[queryName] = currentQuery;
                currentQuery.clear();
            }
            queryName = line.substr(3);
        } else if (!line.empty() && line[0] != '-') {
            currentQuery += line + " ";
        }
    }
    
    if (!queryName.empty() && !currentQuery.empty()) {
        queries[queryName] = currentQuery;
    }
    
    file.close();
    cout << "Загружено запросов: " << queries.size() << endl;
}

string Database::getQuery(const string& queryName) {
    auto it = queries.find(queryName);
    if (it != queries.end()) {
        return it->second;
    }
    cerr << "ВНИМАНИЕ: Запрос '" << queryName << "' не найден!" << endl;
    return "";
}

bool Database::connect(const string& conn_string) {
    try {
        conn = make_unique<connection>(conn_string);
        cout << "Подключение к БД успешно!" << endl;
        return conn->is_open();
    } catch (const exception& e) {
        cerr << "Ошибка подключения: " << e.what() << endl;
        return false;
    }
}

void Database::disconnect() {
    if (conn && conn->is_open()) {
        conn->close();
    }
}

// ==================== СОТРУДНИКИ ====================

vector<Employee> Database::getAllEmployees() {
    vector<Employee> employees;
    try {
        nontransaction N(*conn);
        result R(N.exec(getQuery("GET_ALL_EMPLOYEES")));
        
        for (auto c : R) {
            Employee emp;
            emp.id = c[0].as<int>();
            emp.first_name = c[1].as<string>();
            emp.last_name = c[2].as<string>();
            emp.email = c[3].is_null() ? "" : c[3].as<string>();
            emp.phone = c[4].is_null() ? "" : c[4].as<string>();
            emp.hire_date = c[5].as<string>();
            emp.salary = c[6].as<double>();
            emp.department_id = c[7].is_null() ? 0 : c[7].as<int>();
            emp.position_id = c[8].is_null() ? 0 : c[8].as<int>();
            emp.manager_id = c[9].is_null() ? 0 : c[9].as<int>();
            employees.push_back(emp);
        }
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
    return employees;
}

bool Database::addEmployee(const Employee& emp) {
    try {
        work W(*conn);
        
        W.exec(getQuery("ADD_EMPLOYEE"), 
            pqxx::params{emp.first_name, emp.last_name, emp.email, emp.phone,
                        emp.hire_date, emp.salary, emp.department_id, 
                        emp.position_id, emp.manager_id});
        W.commit();
        cout << "Сотрудник успешно добавлен\n";
        return true;
    } catch (const pqxx::unique_violation& e) {
        cerr << "Ошибка: Email уже существует\n";
        return false;
    } catch (const pqxx::foreign_key_violation& e) {
        cerr << "Ошибка: Несуществующий ID отдела, должности или менеджера\n";
        return false;
    } catch (const exception& e) {
        cerr << "Ошибка добавления: " << e.what() << endl;
        return false;
    }
}

bool Database::updateEmployee(const Employee& emp) {
    try {
        work W(*conn);
        W.exec(getQuery("UPDATE_EMPLOYEE"),
            pqxx::params{emp.id, emp.first_name, emp.last_name, emp.email,
                        emp.phone, emp.salary, emp.department_id, 
                        emp.position_id, emp.manager_id});
        W.commit();
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка обновления: " << e.what() << endl;
        return false;
    }
}

bool Database::deleteEmployee(int id) {
    try {
        work W(*conn);
        W.exec(getQuery("DELETE_EMPLOYEE"), pqxx::params{id});
        W.commit();
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка удаления: " << e.what() << endl;
        return false;
    }
}

vector<Employee> Database::getEmployeesByDepartment(int dept_id) {
    vector<Employee> employees;
    try {
        nontransaction N(*conn);
        result R(N.exec(getQuery("GET_EMPLOYEES_BY_DEPARTMENT"), pqxx::params{dept_id}));
        
        for (auto c : R) {
            Employee emp;
            emp.id = c[0].as<int>();
            emp.first_name = c[1].as<string>();
            emp.last_name = c[2].as<string>();
            emp.email = c[3].is_null() ? "" : c[3].as<string>();
            emp.phone = c[4].is_null() ? "" : c[4].as<string>();
            emp.hire_date = c[5].as<string>();
            emp.salary = c[6].as<double>();
            emp.department_id = c[7].as<int>();
            emp.position_id = c[8].as<int>();
            emp.manager_id = c[9].is_null() ? 0 : c[9].as<int>();
            employees.push_back(emp);
        }
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
    return employees;
}

// ==================== ОТДЕЛЫ ====================

vector<Department> Database::getDepartments() {
    vector<Department> departments;
    try {
        nontransaction N(*conn);
        result R(N.exec(getQuery("GET_DEPARTMENTS")));
        
        for (auto row : R) {
            Department dept;
            dept.id = row[0].as<int>();
            dept.name = row[1].as<string>();
            dept.location = row[2].is_null() ? "" : row[2].as<string>();
            dept.budget = row[3].is_null() ? 0 : row[3].as<double>();
            dept.manager_id = row[4].is_null() ? 0 : row[4].as<int>();
            departments.push_back(dept);
        }
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
    return departments;
}

bool Database::addDepartment(const Department& dept) {
    try {
        work W(*conn);
        W.exec(getQuery("ADD_DEPARTMENT"), 
               pqxx::params{dept.name, dept.location, dept.budget, dept.manager_id});
        W.commit();
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка добавления отдела: " << e.what() << endl;
        return false;
    }
}

bool Database::deleteDepartment(int id) {
    try {
        work W(*conn);
        W.exec(getQuery("DELETE_DEPARTMENT"), pqxx::params{id});
        W.commit();
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка удаления отдела: " << e.what() << endl;
        return false;
    }
}

int Database::getEmployeeCountByDepartment(int dept_id) {
    try {
        nontransaction N(*conn);
        result R(N.exec(getQuery("CHECK_DEPARTMENT_EMPLOYEES"), pqxx::params{dept_id}));
        return R[0][0].as<int>();
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        return -1;
    }
}

// ==================== ДОЛЖНОСТИ ====================

vector<Position> Database::getPositions() {
    vector<Position> positions;
    try {
        nontransaction N(*conn);
        result R(N.exec(getQuery("GET_POSITIONS")));
        
        for (auto row : R) {
            Position pos;
            pos.id = row[0].as<int>();
            pos.title = row[1].as<string>();
            pos.min_salary = row[2].is_null() ? 0 : row[2].as<double>();
            pos.max_salary = row[3].is_null() ? 0 : row[3].as<double>();
            pos.description = row[4].is_null() ? "" : row[4].as<string>();
            positions.push_back(pos);
        }
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
    return positions;
}

bool Database::addPosition(const Position& pos) {
    try {
        work W(*conn);
        W.exec(getQuery("ADD_POSITION"), 
               pqxx::params{pos.title, pos.min_salary, pos.max_salary, pos.description});
        W.commit();
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка добавления должности: " << e.what() << endl;
        return false;
    }
}

// ==================== ПРОЕКТЫ ====================

vector<Project> Database::getProjects() {
    vector<Project> projects;
    try {
        nontransaction N(*conn);
        result R(N.exec(getQuery("GET_PROJECTS")));
        
        for (auto row : R) {
            Project proj;
            proj.id = row[0].as<int>();
            proj.name = row[1].as<string>();
            proj.description = row[2].is_null() ? "" : row[2].as<string>();
            proj.start_date = row[3].is_null() ? "" : row[3].as<string>();
            proj.end_date = row[4].is_null() ? "" : row[4].as<string>();
            proj.budget = row[5].is_null() ? 0 : row[5].as<double>();
            proj.status = row[6].is_null() ? "" : row[6].as<string>();
            proj.department_id = row[7].is_null() ? 0 : row[7].as<int>();
            projects.push_back(proj);
        }
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
    return projects;
}

bool Database::addProject(const Project& proj) {
    try {
        work W(*conn);
        W.exec(getQuery("ADD_PROJECT"), 
               pqxx::params{proj.name, proj.description, proj.start_date, 
                          proj.end_date, proj.budget, proj.status, proj.department_id});
        W.commit();
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка добавления проекта: " << e.what() << endl;
        return false;
    }
}

bool Database::deleteProject(int id) {
    try {
        work W(*conn);
        W.exec("DELETE FROM projects WHERE id = $1", pqxx::params{id});
        W.commit();
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка удаления проекта: " << e.what() << endl;
        return false;
    }
}

// ==================== НАЗНАЧЕНИЯ ====================

bool Database::assignEmployeeToProject(const EmployeeProject& assignment) {
    try {
        work W(*conn);
        W.exec(getQuery("ASSIGN_EMPLOYEE_TO_PROJECT"), 
               pqxx::params{assignment.employee_id, assignment.project_id, 
                          assignment.role, assignment.assigned_date, 
                          assignment.hours_allocated});
        W.commit();
        return true;
    } catch (const pqxx::unique_violation& e) {
        cerr << "Ошибка: Сотрудник уже назначен на этот проект\n";
        return false;
    } catch (const exception& e) {
        cerr << "Ошибка назначения: " << e.what() << endl;
        return false;
    }
}

bool Database::removeEmployeeFromProject(int employee_id, int project_id) {
    try {
        work W(*conn);
        W.exec("DELETE FROM employee_projects WHERE employee_id = $1 AND project_id = $2",
               pqxx::params{employee_id, project_id});
        W.commit();
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        return false;
    }
}

// ==================== АНАЛИТИКА ====================

double Database::getAverageSalaryByDepartment(int dept_id) {
    try {
        nontransaction N(*conn);
        result R(N.exec(getQuery("AVG_SALARY_BY_DEPARTMENT"), pqxx::params{dept_id}));
        if (!R.empty() && !R[0][0].is_null()) {
            return R[0][0].as<double>();
        }
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
    return 0.0;
}

int Database::getEmployeeCount() {
    try {
        nontransaction N(*conn);
        result R(N.exec(getQuery("COUNT_EMPLOYEES")));
        return R[0][0].as<int>();
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
    return 0;
}

vector<EmployeeFull> Database::getEmployeesFull() {
    vector<EmployeeFull> employees;
    try {
        nontransaction N(*conn);
        result R(N.exec(getQuery("GET_EMPLOYEES_FULL")));
        
        for (auto c : R) {
            EmployeeFull emp;
            emp.id = c[0].as<int>();
            emp.first_name = c[1].as<string>();
            emp.last_name = c[2].as<string>();
            emp.email = c[3].is_null() ? "" : c[3].as<string>();
            emp.phone = c[4].is_null() ? "" : c[4].as<string>();
            emp.hire_date = c[5].as<string>();
            emp.salary = c[6].as<double>();
            emp.department_name = c[7].is_null() ? "Не указан" : c[7].as<string>();
            emp.position_title = c[8].is_null() ? "Не указана" : c[8].as<string>();
            employees.push_back(emp);
        }
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
    return employees;
}

vector<DepartmentStats> Database::getDepartmentStats() {
    vector<DepartmentStats> stats;
    try {
        nontransaction N(*conn);
        result R(N.exec(getQuery("DEPARTMENT_STATS")));
        
        for (auto row : R) {
            DepartmentStats stat;
            stat.id = row[0].as<int>();
            stat.name = row[1].as<string>();
            stat.location = row[2].is_null() ? "" : row[2].as<string>();
            stat.employee_count = row[3].as<int>();
            stat.avg_salary = row[4].as<double>();
            stat.max_salary = row[5].as<double>();
            stat.min_salary = row[6].as<double>();
            stat.total_salary = row[7].as<double>();
            stats.push_back(stat);
        }
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
    return stats;
}

vector<EmployeeWithProject> Database::getEmployeesWithProjects() {
    vector<EmployeeWithProject> result;
    try {
        nontransaction N(*conn);
        pqxx::result R(N.exec(getQuery("GET_EMPLOYEES_WITH_PROJECTS")));
        
        for (auto row : R) {
            EmployeeWithProject ewp;
            ewp.employee_id = row[0].as<int>();
            ewp.full_name = row[1].as<string>();
            ewp.project_name = row[2].as<string>();
            ewp.role = row[3].is_null() ? "" : row[3].as<string>();
            ewp.hours_allocated = row[4].is_null() ? 0 : row[4].as<double>();
            ewp.project_status = row[5].is_null() ? "" : row[5].as<string>();
            ewp.department_name = row[6].is_null() ? "" : row[6].as<string>();
            result.push_back(ewp);
        }
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
    return result;
}

vector<ProjectWithStats> Database::getProjectsWithEmployees() {
    vector<ProjectWithStats> result;
    try {
        nontransaction N(*conn);
        pqxx::result R(N.exec(getQuery("GET_PROJECTS_WITH_EMPLOYEES")));
        
        for (auto row : R) {
            ProjectWithStats pws;
            pws.id = row[0].as<int>();
            pws.project_name = row[1].as<string>();
            pws.status = row[2].is_null() ? "" : row[2].as<string>();
            pws.budget = row[3].is_null() ? 0 : row[3].as<double>();
            pws.department_name = row[4].is_null() ? "" : row[4].as<string>();
            pws.employee_count = row[5].as<int>();
            pws.total_hours = row[6].is_null() ? 0 : row[6].as<double>();
            result.push_back(pws);
        }
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
    return result;
}

vector<EmployeeAboveAvg> Database::getAboveAvgSalaryEmployees() {
    vector<EmployeeAboveAvg> result;
    try {
        nontransaction N(*conn);
        pqxx::result R(N.exec(getQuery("GET_ABOVE_AVG_SALARY_EMPLOYEES")));
        
        for (auto row : R) {
            EmployeeAboveAvg eaa;
            eaa.id = row[0].as<int>();
            eaa.first_name = row[1].as<string>();
            eaa.last_name = row[2].as<string>();
            eaa.salary = row[3].as<double>();
            eaa.department_name = row[4].as<string>();
            eaa.department_avg_salary = row[5].as<double>();
            result.push_back(eaa);
        }
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
    return result;
}

vector<DepartmentBudgetAnalysis> Database::getDepartmentBudgetAnalysis() {
    vector<DepartmentBudgetAnalysis> result;
    try {
        nontransaction N(*conn);
        pqxx::result R(N.exec(getQuery("GET_DEPARTMENT_BUDGET_ANALYSIS")));
        
        for (auto row : R) {
            DepartmentBudgetAnalysis dba;
            dba.id = row[0].as<int>();
            dba.name = row[1].as<string>();
            dba.department_budget = row[2].is_null() ? 0 : row[2].as<double>();
            dba.annual_salary_cost = row[3].as<double>();
            dba.projects_budget = row[4].as<double>();
            dba.remaining_budget = row[5].as<double>();
            result.push_back(dba);
        }
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
    return result;
}

bool Database::clearAllEmployees() {
    try {
        work W(*conn);
        W.exec(getQuery("CLEAR_EMPLOYEES"));
        W.commit();
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка очистки: " << e.what() << endl;
        return false;
    }
}