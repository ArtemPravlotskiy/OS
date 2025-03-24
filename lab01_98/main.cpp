#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

struct employee {
    int num;
    char name[21];
    double hours;
};

void printBinaryFile(const char* filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        std::cerr << "Error opening binary file " << filename << "\n";
        return;
    }
    std::cout << "\nThe contents of the binary file:\n";
    employee emp;
    while (in.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        std::cout << "Number: " << emp.num
                  << ", Name: " << emp.name
                  << ", Hours: " << emp.hours << "\n";
    }
    in.close();
}

void printReportFile() {
    
}