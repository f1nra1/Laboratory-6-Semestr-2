CREATE DATABASE company_db;

c company_db;

CREATE TABLE departments (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    location VARCHAR(100)
);

CREATE TABLE positions (
    id SERIAL PRIMARY KEY,
    title VARCHAR(100) NOT NULL,
    min_salary DECIMAL(10, 2),
    max_salary DECIMAL(10, 2)
);

CREATE TABLE employees (
    id SERIAL PRIMARY KEY,
    first_name VARCHAR(50) NOT NULL,
    last_name VARCHAR(50) NOT NULL,
    email VARCHAR(100) UNIQUE,
    phone VARCHAR(20),
    hire_date DATE NOT NULL,
    salary DECIMAL(10, 2),
    department_id INT REFERENCES departments(id),
    position_id INT REFERENCES positions(id)
);

INSERT INTO departments (name, location) VALUES 
('IT', 'Главный офис'),
('Бухгалтерия', 'Главный офис'),
('HR', 'Главный офис');

INSERT INTO positions (title, min_salary, max_salary) VALUES 
('Программист', 50000, 150000),
('Бухгалтер', 40000, 100000),
('Менеджер', 60000, 120000);