#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include "../include/employee.h"

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

void printReportFile(const char* filename) {
    std::ifstream in(filename);
    if (!in) {
        std::cerr << "Error opening report file " << filename << "\n";
        return;
    }
    std::cout << "\nThe contents of the report file:\n";
    std::string line;
    while (std::getline(in, line)) {
        std::cout << line << "\n";
    }
    in.close();
}

void runCreator(const std::string& binFilename, int count) {
    std::ostringstream creatorCmd;
    creatorCmd << "lab01_98_creator.exe " << binFilename << " " << count;
    system(creatorCmd.str().c_str());
}

void runReporter(const std::string& binFilename, const std::string& reportFilename, double payRate) {
    std::ostringstream reporterCmd;
    reporterCmd << "lab01_98_reporter.exe " << binFilename << " " << reportFilename << " " << payRate;
    system(reporterCmd.str().c_str());
}

int main() {
    std::string binFilename;
    int count;
    std::cout << "Enter binary file name: ";
    std::cin >> binFilename;
    std::cout << "Enter number of employees: ";
    std::cin >> count;

    std::thread creatorThread(runCreator, binFilename, count);
    creatorThread.join();

    printBinaryFile(binFilename.c_str());

    std::string reportFilename;
    double payRate;
    std::cout << "\nEnter report file name: ";
    std::cin >> reportFilename;
    std::cout << "\nEnter pay rate: ";
    std::cin >> payRate;

    std::thread reporterThread(runReporter, binFilename, reportFilename, payRate);
    reporterThread.join();

    printReportFile(reportFilename.c_str());

    std::cout << "\nThe program has been successfully completed.\n";
    return 0;
}
