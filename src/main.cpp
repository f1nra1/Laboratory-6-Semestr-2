#include <iostream>
#include "database.h"
#include "menu.h"

using namespace std;

int main() {
    const string connection_string = "dbname=company_db host=localhost";
    
    Database db(connection_string);
    
    cout << "Подключение к базе данных...\n";
    
    Menu menu(db);
    menu.run();
    
    return 0;
}