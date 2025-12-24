-- Все сотрудники
-- GET_ALL_EMPLOYEES
SELECT * FROM employees ORDER BY id;

-- Сотрудник по ID
-- GET_EMPLOYEE_BY_ID
SELECT * FROM employees WHERE id = $1;

-- Добавить сотрудника
-- ADD_EMPLOYEE
INSERT INTO employees (first_name, last_name, email, phone, hire_date, salary, department_id, position_id) 
VALUES ($1, $2, $3, $4, $5, $6, $7, $8);

-- Добавить отдел
-- ADD_DEPARTMENT
INSERT INTO departments (name, location) VALUES ($1, $2);

-- Добавить должность
-- ADD_POSITION  
INSERT INTO positions (title, min_salary, max_salary) VALUES ($1, $2, $3);

-- Удалить отдел
-- DELETE_DEPARTMENT
DELETE FROM departments WHERE id = $1;

-- Проверить, есть ли сотрудники в отделе
-- CHECK_DEPARTMENT_EMPLOYEES
SELECT COUNT(*) FROM employees WHERE department_id = $1;

-- Обновить сотрудника
-- UPDATE_EMPLOYEE
UPDATE employees SET 
    first_name = $2, 
    last_name = $3, 
    email = $4, 
    phone = $5, 
    salary = $6, 
    department_id = $7, 
    position_id = $8 
WHERE id = $1;

-- Удалить сотрудника
-- DELETE_EMPLOYEE
DELETE FROM employees WHERE id = $1;

-- Все отделы
-- GET_DEPARTMENTS
SELECT * FROM departments;

-- Все должности  
-- GET_POSITIONS
SELECT * FROM positions;

-- Сотрудники по отделу
-- GET_EMPLOYEES_BY_DEPARTMENT
SELECT * FROM employees WHERE department_id = $1;

-- Средняя зарплата по отделу
-- AVG_SALARY_BY_DEPARTMENT
SELECT AVG(salary) FROM employees WHERE department_id = $1;

-- Количество сотрудников
-- COUNT_EMPLOYEES
SELECT COUNT(*) FROM employees;

-- Очистить всех сотрудников
-- CLEAR_EMPLOYEES
TRUNCATE TABLE employees RESTART IDENTITY;

-- Сотрудники с полной информацией
-- GET_EMPLOYEES_FULL
SELECT 
    e.id, e.first_name, e.last_name, e.email, e.phone, 
    e.hire_date, e.salary,
    d.name as department_name,
    p.title as position_title
FROM employees e
LEFT JOIN departments d ON e.department_id = d.id
LEFT JOIN positions p ON e.position_id = p.id
ORDER BY e.id;

-- Статистика по отделам
-- DEPARTMENT_STATS
SELECT 
    d.id,
    d.name,
    COUNT(e.id) as employee_count,
    AVG(e.salary) as avg_salary,
    MAX(e.salary) as max_salary,
    MIN(e.salary) as min_salary
FROM departments d
LEFT JOIN employees e ON d.id = e.department_id
GROUP BY d.id, d.name;