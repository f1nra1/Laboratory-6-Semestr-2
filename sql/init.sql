-- Подключаемся к БД
\c company_db;

-- Таблица 1: Отделы
CREATE TABLE departments (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    location VARCHAR(100),
    budget DECIMAL(12, 2),
    manager_id INT
);

-- Таблица 2: Должности
CREATE TABLE positions (
    id SERIAL PRIMARY KEY,
    title VARCHAR(100) NOT NULL,
    min_salary DECIMAL(10, 2),
    max_salary DECIMAL(10, 2),
    description TEXT
);

-- Таблица 3: Сотрудники
CREATE TABLE employees (
    id SERIAL PRIMARY KEY,
    first_name VARCHAR(50) NOT NULL,
    last_name VARCHAR(50) NOT NULL,
    email VARCHAR(100) UNIQUE,
    phone VARCHAR(20),
    hire_date DATE NOT NULL,
    salary DECIMAL(10, 2),
    department_id INT REFERENCES departments(id),
    position_id INT REFERENCES positions(id),
    manager_id INT REFERENCES employees(id)
);

-- Таблица 4: Проекты
CREATE TABLE projects (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    description TEXT,
    start_date DATE,
    end_date DATE,
    budget DECIMAL(12, 2),
    status VARCHAR(20) CHECK (status IN ('планирование', 'в работе', 'завершен', 'приостановлен')),
    department_id INT REFERENCES departments(id)
);

-- Таблица 5: Назначения сотрудников на проекты
CREATE TABLE employee_projects (
    id SERIAL PRIMARY KEY,
    employee_id INT REFERENCES employees(id) ON DELETE CASCADE,
    project_id INT REFERENCES projects(id) ON DELETE CASCADE,
    role VARCHAR(100),
    assigned_date DATE,
    hours_allocated DECIMAL(5, 2),
    UNIQUE(employee_id, project_id)
);

-- Добавляем внешний ключ для manager_id в departments
ALTER TABLE departments 
ADD CONSTRAINT fk_department_manager 
FOREIGN KEY (manager_id) REFERENCES employees(id);

-- Начальные данные
INSERT INTO departments (name, location, budget) VALUES 
('IT', 'Главный офис', 500000.00),
('Бухгалтерия', 'Главный офис', 200000.00),
('HR', 'Главный офис', 150000.00);

INSERT INTO positions (title, min_salary, max_salary, description) VALUES 
('Программист', 50000, 150000, 'Разработка ПО'),
('Бухгалтер', 40000, 100000, 'Ведение учета'),
('Менеджер', 60000, 120000, 'Управление персоналом');