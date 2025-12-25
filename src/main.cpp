#include <iostream>
#include "database.h"
#include "menu.h"

using namespace std;

int main() {
    const string connection_string = "dbname=company_db host=localhost";
    
    cout << "╔═══════════════════════════════════════════════╗\n";
    cout << "║   СИСТЕМА УПРАВЛЕНИЯ БАЗОЙ ДАННЫХ КОМПАНИИ   ║\n";
    cout << "╚═══════════════════════════════════════════════╝\n\n";
    
    cout << "Подключение к базе данных...\n";
    
    try {
        Database db(connection_string);
        
        cout << "✓ Подключение успешно!\n";
        cout << "\nИспользуемые таблицы:\n";
        cout << "  • departments     - отделы компании\n";
        cout << "  • positions       - должности\n";
        cout << "  • employees       - сотрудники\n";
        cout << "  • projects        - проекты\n";
        cout << "  • employee_projects - назначения на проекты\n";
        
        Menu menu(db);
        menu.run();
        
    } catch (const exception& e) {
        cerr << "\n✗ Критическая ошибка: " << e.what() << endl;
        cerr << "\nПроверьте:\n";
        cerr << "  1. PostgreSQL запущен\n";
        cerr << "  2. База данных company_db создана\n";
        cerr << "  3. Параметры подключения правильные\n";
        cerr << "  4. У пользователя есть права доступа\n";
        return 1;
    }
    
    return 0;
}