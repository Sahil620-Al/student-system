#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

using namespace std;

sql::mysql::MySQL_Driver *driver;
sql::Connection *con;

void connectDB() {
    driver = sql::mysql::get_mysql_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "root", "1234");
    con->setSchema("student_db");
}

// ADD
void addStudent() {
    int id, marks;
    string name;

    cout << "Enter ID: ";
    cin >> id;

    cin.ignore();

    cout << "Enter Name: ";
    getline(cin, name);

    cout << "Enter Marks: ";
    cin >> marks;

    if (marks < 0 || marks > 100) {
        cout << "Invalid marks!\n";
        return;
    }

    // Check duplicate ID
    sql::PreparedStatement *check;
    sql::ResultSet *res;

    check = con->prepareStatement("SELECT id FROM students WHERE id=?");
    check->setInt(1, id);
    res = check->executeQuery();

    if (res->next()) {
        cout << "ID already exists!\n";
        delete res;
        delete check;
        return;
    }

    delete res;
    delete check;

    // Insert
    sql::PreparedStatement *pstmt;
    pstmt = con->prepareStatement("INSERT INTO students(id, name, marks) VALUES (?, ?, ?)");

    pstmt->setInt(1, id);
    pstmt->setString(2, name);
    pstmt->setInt(3, marks);

    pstmt->execute();

    cout << "Student Added...\n";

    delete pstmt;
}

//  VIEW
void viewStudents() {
    sql::PreparedStatement *pstmt;
    sql::ResultSet *res;

    pstmt = con->prepareStatement("SELECT * FROM students");
    res = pstmt->executeQuery();

    bool found = false;

    cout << "\nID | Name | Marks\n";
    cout << "----------------------\n";

    while (res->next()) {
        found = true;
        cout << res->getInt("id") << " | "
             << res->getString("name") << " | "
             << res->getInt("marks") << endl;
    }

    if (!found) {
        cout << "No records found!\n";
    }

    delete res;
    delete pstmt;
}

// SEARCH
void searchStudent() {
    int id;
    cout << "Enter ID: ";
    cin >> id;

    sql::PreparedStatement *pstmt;
    sql::ResultSet *res;

    pstmt = con->prepareStatement("SELECT * FROM students WHERE id=?");
    pstmt->setInt(1, id);

    res = pstmt->executeQuery();

    if (res->next()) {
        cout << "\nID: " << res->getInt("id") << endl;
        cout << "Name: " << res->getString("name") << endl;
        cout << "Marks: " << res->getInt("marks") << endl;
    } else {
        cout << "Student not found!\n";
    }

    delete res;
    delete pstmt;
}

// UPDATE
void updateStudent() {
    int id, marks;
    string name;

    cout << "Enter ID to update: ";
    cin >> id;

    cin.ignore();

    cout << "Enter new name: ";
    getline(cin, name);

    cout << "Enter new marks: ";
    cin >> marks;

    if (marks < 0 || marks > 100) {
        cout << "Invalid marks!\n";
        return;
    }

    sql::PreparedStatement *pstmt;

    pstmt = con->prepareStatement("UPDATE students SET name=?, marks=? WHERE id=?");

    pstmt->setString(1, name);
    pstmt->setInt(2, marks);
    pstmt->setInt(3, id);

    int rows = pstmt->executeUpdate();

    if (rows > 0) {
        cout << "Student updated...\n";
    } else {
        cout << "ID not found!\n";
    }

    delete pstmt;
}

// DELETE
void deleteStudent() {
    int id;
    cout << "Enter ID to delete: ";
    cin >> id;

    sql::PreparedStatement *pstmt;

    pstmt = con->prepareStatement("DELETE FROM students WHERE id=?");
    pstmt->setInt(1, id);

    int rows = pstmt->executeUpdate();

    if (rows > 0) {
        cout << "Student deleted...\n";
    } else {
        cout << "No student found!\n";
    }

    delete pstmt;
}

int main() {
    connectDB();

    int choice;

    do {
        cout << "\n===== STUDENT MANAGEMENT SYSTEM =====\n";
        cout << "1. Add\n2. View\n3. Search\n4. Update\n5. Delete\n6. Exit\n";
        cout << "=====================================\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addStudent(); break;
            case 2: viewStudents(); break;
            case 3: searchStudent(); break;
            case 4: updateStudent(); break;
            case 5: deleteStudent(); break;
            case 6: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice!\n";
        }

    } while (choice != 6);

    delete con;
    return 0;
}