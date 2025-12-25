-- ЗАПРОС 1: Все сотрудники (базовый SELECT)
-- GET_ALL_EMPLOYEES
SELECT * FROM employees ORDER BY id;

-- ЗАПРОС 2: Сотрудник по ID (WHERE)
-- GET_EMPLOYEE_BY_ID
SELECT * FROM employees WHERE id = $1;

-- ЗАПРОС 3: Сотрудники с полной информацией (INNER JOIN + LEFT JOIN)
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

-- ЗАПРОС 4: Статистика по отделам (GROUP BY + агрегатные функции + LEFT JOIN)
-- DEPARTMENT_STATS
SELECT 
    d.id,
    d.name,
    d.location,
    COUNT(e.id) as employee_count,
    COALESCE(AVG(e.salary), 0) as avg_salary,
    COALESCE(MAX(e.salary), 0) as max_salary,
    COALESCE(MIN(e.salary), 0) as min_salary,
    COALESCE(SUM(e.salary), 0) as total_salary
FROM departments d
LEFT JOIN employees e ON d.id = e.department_id
GROUP BY d.id, d.name, d.location
ORDER BY d.id;

-- ЗАПРОС 5: Сотрудники работающие над проектами (сложный JOIN)
-- GET_EMPLOYEES_WITH_PROJECTS
SELECT 
    e.id,
    e.first_name || ' ' || e.last_name as full_name,
    p.name as project_name,
    ep.role,
    ep.hours_allocated,
    proj.status as project_status,
    d.name as department_name
FROM employees e
INNER JOIN employee_projects ep ON e.id = ep.employee_id
INNER JOIN projects proj ON ep.project_id = proj.id
LEFT JOIN positions p ON e.position_id = p.id
LEFT JOIN departments d ON e.department_id = d.id
ORDER BY e.id, proj.name;

-- ЗАПРОС 6: Проекты с информацией о сотрудниках (обратный JOIN)
-- GET_PROJECTS_WITH_EMPLOYEES
SELECT 
    p.id,
    p.name as project_name,
    p.status,
    p.budget,
    d.name as department_name,
    COUNT(ep.employee_id) as employee_count,
    SUM(ep.hours_allocated) as total_hours
FROM projects p
LEFT JOIN departments d ON p.department_id = d.id
LEFT JOIN employee_projects ep ON p.id = ep.project_id
GROUP BY p.id, p.name, p.status, p.budget, d.name
ORDER BY p.id;

-- ЗАПРОС 7: Сотрудники с зарплатой выше средней по отделу (подзапрос + HAVING)
-- GET_ABOVE_AVG_SALARY_EMPLOYEES
SELECT 
    e.id,
    e.first_name,
    e.last_name,
    e.salary,
    d.name as department_name,
    dept_avg.avg_sal as department_avg_salary
FROM employees e
JOIN departments d ON e.department_id = d.id
JOIN (
    SELECT department_id, AVG(salary) as avg_sal
    FROM employees
    WHERE department_id IS NOT NULL
    GROUP BY department_id
) dept_avg ON e.department_id = dept_avg.department_id
WHERE e.salary > dept_avg.avg_sal
ORDER BY e.salary DESC;

-- ЗАПРОС 8: Загруженность сотрудников (агрегация с HAVING)
-- GET_EMPLOYEE_WORKLOAD
SELECT 
    e.id,
    e.first_name || ' ' || e.last_name as full_name,
    COUNT(ep.project_id) as project_count,
    SUM(ep.hours_allocated) as total_hours,
    AVG(ep.hours_allocated) as avg_hours_per_project
FROM employees e
LEFT JOIN employee_projects ep ON e.id = ep.employee_id
GROUP BY e.id, e.first_name, e.last_name
HAVING COUNT(ep.project_id) > 0
ORDER BY total_hours DESC;

-- ЗАПРОС 9: Отделы без активных проектов (LEFT JOIN + WHERE NULL)
-- GET_DEPARTMENTS_WITHOUT_PROJECTS
SELECT 
    d.id,
    d.name,
    d.location,
    d.budget
FROM departments d
LEFT JOIN projects p ON d.id = p.department_id AND p.status IN ('в работе', 'планирование')
WHERE p.id IS NULL;

-- ЗАПРОС 10: Иерархия сотрудников (самоссылка)
-- GET_EMPLOYEE_HIERARCHY
SELECT 
    e.id,
    e.first_name || ' ' || e.last_name as employee_name,
    m.first_name || ' ' || m.last_name as manager_name,
    p.title as position
FROM employees e
LEFT JOIN employees m ON e.manager_id = m.id
LEFT JOIN positions p ON e.position_id = p.id
ORDER BY e.id;

-- ЗАПРОС 11: Топ-3 самых дорогих проектов по отделам (WINDOW FUNCTION)
-- GET_TOP_PROJECTS_BY_DEPARTMENT
SELECT 
    department_name,
    project_name,
    budget,
    rank
FROM (
    SELECT 
        d.name as department_name,
        p.name as project_name,
        p.budget,
        RANK() OVER (PARTITION BY d.id ORDER BY p.budget DESC) as rank
    FROM projects p
    JOIN departments d ON p.department_id = d.id
) ranked
WHERE rank <= 3
ORDER BY department_name, rank;

-- ЗАПРОС 12: Сотрудники с опытом работы (EXTRACT + DATE)
-- GET_EMPLOYEES_WITH_EXPERIENCE
SELECT 
    e.id,
    e.first_name || ' ' || e.last_name as full_name,
    e.hire_date,
    EXTRACT(YEAR FROM AGE(CURRENT_DATE, e.hire_date)) as years_of_service,
    CASE 
        WHEN EXTRACT(YEAR FROM AGE(CURRENT_DATE, e.hire_date)) < 1 THEN 'Новичок'
        WHEN EXTRACT(YEAR FROM AGE(CURRENT_DATE, e.hire_date)) < 3 THEN 'Младший'
        WHEN EXTRACT(YEAR FROM AGE(CURRENT_DATE, e.hire_date)) < 5 THEN 'Средний'
        ELSE 'Старший'
    END as experience_level
FROM employees e
ORDER BY years_of_service DESC;

-- ЗАПРОС 13: Бюджеты и затраты отделов (сложная агрегация)
-- GET_DEPARTMENT_BUDGET_ANALYSIS
SELECT 
    d.id,
    d.name,
    d.budget as department_budget,
    COALESCE(SUM(e.salary * 12), 0) as annual_salary_cost,
    COALESCE(SUM(p.budget), 0) as projects_budget,
    d.budget - COALESCE(SUM(e.salary * 12), 0) - COALESCE(SUM(p.budget), 0) as remaining_budget
FROM departments d
LEFT JOIN employees e ON d.id = e.department_id
LEFT JOIN projects p ON d.id = p.department_id
GROUP BY d.id, d.name, d.budget
ORDER BY remaining_budget ASC;

-- Дополнительные CRUD операции для поддержки новых таблиц:

-- ADD_EMPLOYEE
INSERT INTO employees (first_name, last_name, email, phone, hire_date, salary, department_id, position_id, manager_id) 
VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9);

-- UPDATE_EMPLOYEE
UPDATE employees SET 
    first_name = $2, 
    last_name = $3, 
    email = $4, 
    phone = $5, 
    salary = $6, 
    department_id = $7, 
    position_id = $8,
    manager_id = $9
WHERE id = $1;

-- DELETE_EMPLOYEE
DELETE FROM employees WHERE id = $1;

-- ADD_DEPARTMENT
INSERT INTO departments (name, location, budget, manager_id) VALUES ($1, $2, $3, $4);

-- ADD_POSITION  
INSERT INTO positions (title, min_salary, max_salary, description) VALUES ($1, $2, $3, $4);

-- ADD_PROJECT
INSERT INTO projects (name, description, start_date, end_date, budget, status, department_id) 
VALUES ($1, $2, $3, $4, $5, $6, $7);

-- ASSIGN_EMPLOYEE_TO_PROJECT
INSERT INTO employee_projects (employee_id, project_id, role, assigned_date, hours_allocated)
VALUES ($1, $2, $3, $4, $5);

-- GET_DEPARTMENTS
SELECT * FROM departments ORDER BY id;

-- GET_POSITIONS
SELECT * FROM positions ORDER BY id;

-- GET_PROJECTS
SELECT * FROM projects ORDER BY id;

-- GET_EMPLOYEES_BY_DEPARTMENT
SELECT * FROM employees WHERE department_id = $1;

-- AVG_SALARY_BY_DEPARTMENT
SELECT AVG(salary) FROM employees WHERE department_id = $1;

-- COUNT_EMPLOYEES
SELECT COUNT(*) FROM employees;

-- CHECK_DEPARTMENT_EMPLOYEES
SELECT COUNT(*) FROM employees WHERE department_id = $1;

-- DELETE_DEPARTMENT
DELETE FROM departments WHERE id = $1;

-- CLEAR_EMPLOYEES
TRUNCATE TABLE employee_projects, employees RESTART IDENTITY CASCADE;