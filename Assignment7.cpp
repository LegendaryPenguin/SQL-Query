// EECS 348 Assignment 7
// Author: Nischay Rawal
// Created: 04/15/2025
// Description: Connects to EECS MySQL server and performs 13 SQL queries via C++
// Inputs: SQL Database
// Output: Console output of query results
// Sources: MySQL Connector/C++ documentation, ChatGPT assistance

#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

using namespace std;
using namespace sql;

int main() {
    try {
        // Establish connection to the MySQL server
        mysql::MySQL_Driver *driver = mysql::get_mysql_driver_instance();
        Connection *con = driver->connect("tcp://mysql.eecs.ku.edu", "348s25_n791r289", "ohvei9Ae");
        con->setSchema("348s25_n791r289");

        // Create a statement object for executing queries
        Statement *stmt = con->createStatement();
        ResultSet *res;

        // Query 1: Retrieve students majoring in Information Systems (IS)
        cout << "1. Students majoring in IS (Information Systems):\n";
        res = stmt->executeQuery("SELECT StdFirstName, StdLastName FROM Student WHERE StdMajor = 'IS'");
        while (res->next()) {
            cout << " - " << res->getString("StdFirstName") << " " << res->getString("StdLastName") << endl;
        }

        // Query 2: Retrieve students enrolled in more than two courses
        cout << "\n2. Students enrolled in more than two courses:\n";
        res = stmt->executeQuery(
            "SELECT S.StdFirstName, S.StdLastName FROM Student S "
            "JOIN Enrollment E ON S.StdNo = E.StdNo "
            "GROUP BY S.StdNo HAVING COUNT(E.OfferNo) > 2");
        while (res->next()) {
            cout << " - " << res->getString("StdFirstName") << " " << res->getString("StdLastName") << endl;
        }

        // Query 3: Retrieve faculty in Physics teaching for more than 5 years
        cout << "\n3. Faculty in Physics teaching more than 5 years:\n";
        res = stmt->executeQuery(
            "SELECT FacFirstName, FacLastName FROM Faculty "
            "WHERE FacDept = 'PHYS' AND YEAR(CURDATE()) - YEAR(FacHireDate) > 5");
        while (res->next()) {
            cout << " - " << res->getString("FacFirstName") << " " << res->getString("FacLastName") << endl;
        }

        // Query 4: Retrieve departments with more than 50 students
        cout << "\n4. Departments with more than 50 students:\n";
        res = stmt->executeQuery(
            "SELECT StdMajor, COUNT(*) AS Total FROM Student GROUP BY StdMajor HAVING Total > 50");
        while (res->next()) {
            cout << " - " << res->getString("StdMajor") << ": " << res->getInt("Total") << endl;
        }

        // Query 5: Retrieve courses with 'Data' in the title taught by Dr. Johnson
        cout << "\n5. Courses with 'Data' in title taught by Dr. Johnson:\n";
        res = stmt->executeQuery(
            "SELECT C.CourseNo, C.CrsDesc FROM Course C "
            "JOIN Offering O ON C.CourseNo = O.CourseNo "
            "JOIN Faculty F ON O.FacNo = F.FacNo "
            "WHERE C.CrsDesc LIKE '%Data%' AND F.FacLastName = 'JOHNSON'");
        while (res->next()) {
            cout << " - " << res->getString("CourseNo") << ": " << res->getString("CrsDesc") << endl;
        }

        // Query 6: Retrieve students not enrolled in SP24 or FA23
        cout << "\n6. Students not enrolled in SP24 or FA23:\n";
        res = stmt->executeQuery(
            "SELECT StdFirstName, StdLastName FROM Student "
            "WHERE StdNo NOT IN ("
            "SELECT DISTINCT E.StdNo FROM Enrollment E JOIN Offering O ON E.OfferNo = O.OfferNo "
            "WHERE O.OffTerm IN ('SP24', 'FA23'))");
        while (res->next()) {
            cout << " - " << res->getString("StdFirstName") << " " << res->getString("StdLastName") << endl;
        }

        // Query 7: Retrieve the second-highest GPA
        cout << "\n7. Second-highest GPA:\n";
        res = stmt->executeQuery("SELECT DISTINCT StdGPA FROM Student ORDER BY StdGPA DESC LIMIT 1 OFFSET 1");
        if (res->next()) {
            cout << " - GPA: " << res->getDouble("StdGPA") << endl;
        }

        // Query 8: Retrieve students who are TAs with GPA > 3.5
        cout << "\n8. Students who are TAs with GPA > 3.5:\n";
        res = stmt->executeQuery(
            "SELECT DISTINCT S.StdFirstName, S.StdLastName FROM Student S "
            "JOIN Offering O ON S.StdNo = O.FacNo " // Assuming student is TA via FacNo mapping
            "WHERE S.StdGPA > 3.5");
        while (res->next()) {
            cout << " - " << res->getString("StdFirstName") << " " << res->getString("StdLastName") << endl;
        }

        // Query 9: Retrieve students enrolled after 2022
        cout << "\n9. Students enrolled after 2022:\n";
        res = stmt->executeQuery(
            "SELECT S.StdFirstName, S.StdLastName, O.CourseNo FROM Student S "
            "JOIN Enrollment E ON S.StdNo = E.StdNo "
            "JOIN Offering O ON E.OfferNo = O.OfferNo "
            "WHERE O.OffYear > 2022");
        while (res->next()) {
            cout << " - " << res->getString("StdFirstName") << " " << res->getString("StdLastName")
                 << " → Course: " << res->getString("CourseNo") << endl;
        }

        // Query 10: Retrieve the top 3 highest-paid faculty
        cout << "\n10. Top 3 highest-paid faculty:\n";
        res = stmt->executeQuery(
            "SELECT FacFirstName, FacLastName, FacSalary FROM Faculty ORDER BY FacSalary DESC LIMIT 3");
        while (res->next()) {
            cout << " - " << res->getString("FacFirstName") << " " << res->getString("FacLastName")
                 << " → $" << res->getDouble("FacSalary") << endl;
        }

        // Query 11: Insert a new student record for Alice Smith
        cout << "\n11. Insert Alice Smith:\n";
        stmt->execute(
            "INSERT INTO Student (StdNo, StdFirstName, StdLastName, StdCity, StdState, StdZip, StdMajor, StdClass, StdGPA) "
            "VALUES ('888-88-8888', 'ALICE', 'SMITH', 'TOPEKA', 'KS', '66610', 'CS', 'JR', 3.85)");
        res = stmt->executeQuery("SELECT * FROM Student WHERE StdNo = '888-88-8888'");
        while (res->next()) {
            cout << " - Inserted: " << res->getString("StdFirstName") << " " << res->getString("StdLastName") << endl;
        }

        // Query 12: Update Bob Norbert's city and zip code to Overland Park, KS
        cout << "\n12. Update Bob Norbert to Overland Park, KS:\n";
        stmt->execute("UPDATE Student SET StdCity = 'OVERLAND PARK', StdZip = '66212' WHERE StdLastName = 'NORBERT'");
        res = stmt->executeQuery("SELECT * FROM Student WHERE StdLastName = 'NORBERT'");
        while (res->next()) {
            cout << " - Updated: " << res->getString("StdFirstName") << " " << res->getString("StdLastName")
                 << " → " << res->getString("StdCity") << ", " << res->getString("StdZip") << endl;
        }

        // Clean up resources
        delete res;
        delete stmt;
        delete con;
    } catch (SQLException &e) {
        // Handle SQL exceptions
        cerr << "SQL Error: " << e.what() << endl;
    }

    return 0;
}
