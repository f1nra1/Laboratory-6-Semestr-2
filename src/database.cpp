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
        "/Users/arseny/Desktop/Laba6/sql/queries.sql"  // Абсолютный путь
    };
    
    ifstream file;
    bool found = false;
    
    for (const auto& path : paths) {
        file.open(path);
        if (file.is_open()) {
            found = true;
            break;
        }
    }
    
    if (!found) {
        cerr << "ОШИБКА: Не удалось найти файл queries.sql\n";
        cerr << "Убедитесь, что файл находится в директории sql/\n";
        exit(1);
    }
    
    string line;
    string currentQuery;
    string queryName;
    
    while (getline(file, line)) {
        if (line.find("-- ") == 0 && line.find("-- ") == line.rfind("-- ")) {
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
}

string Database::getQuery(const string& queryName) {
    auto it = queries.find(queryName);
    if (it != queries.end()) {
        return it->second;
    }
    return "";
}

bool Database::connect(const string& conn_string) {
    try {
        conn = make_unique<connection>(conn_string);
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
        
        // Добавим отладочную информацию
        cout << "Добавление сотрудника: " 
             << emp.first_name << " " 
             << emp.last_name << " " 
             << emp.email << endl;
        
        W.exec(getQuery("ADD_EMPLOYEE"), 
            pqxx::params{emp.first_name, emp.last_name, emp.email, emp.phone,
                        emp.hire_date, emp.salary, emp.department_id, emp.position_id});
        W.commit();
        cout << "Сотрудник успешно добавлен в БД\n";
        return true;
    } catch (const pqxx::unique_violation& e) {
        cerr << "Ошибка: Email уже существует в базе данных\n";
        return false;
    } catch (const pqxx::foreign_key_violation& e) {
        cerr << "Ошибка: Несуществующий ID отдела или должности\n";
        return false;
    } catch (const exception& e) {
        cerr << "Ошибка добавления: " << e.what() << endl;
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

bool Database::updateEmployee(const Employee& emp) {
    try {
        work W(*conn);
        W.exec(getQuery("UPDATE_EMPLOYEE"),
            pqxx::params{emp.id, emp.first_name, emp.last_name, emp.email,
                        emp.phone, emp.salary, emp.department_id, emp.position_id});
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
            departments.push_back(dept);
        }
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
    return departments;
}

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
            positions.push_back(pos);
        }
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
    return positions;
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
            employees.push_back(emp);
        }
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
    return employees;
}

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
            stat.employee_count = row[2].as<int>();
            stat.avg_salary = row[3].is_null() ? 0 : row[3].as<double>();
            stat.max_salary = row[4].is_null() ? 0 : row[4].as<double>();
            stat.min_salary = row[5].is_null() ? 0 : row[5].as<double>();
            stats.push_back(stat);
        }
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
    return stats;
}

bool Database::addDepartment(const string& name, const string& location) {
    try {
        work W(*conn);
        W.exec(getQuery("ADD_DEPARTMENT"), pqxx::params{name, location});
        W.commit();
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка добавления отдела: " << e.what() << endl;
        return false;
    }
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

bool Database::addPosition(const string& title, double min_salary, double max_salary) {
    try {
        work W(*conn);
        W.exec(getQuery("ADD_POSITION"), pqxx::params{title, min_salary, max_salary});
        W.commit();
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка добавления должности: " << e.what() << endl;
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