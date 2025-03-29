#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

struct employee {
    int num;
    char name[10];
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

int main() {
    std::string binFilename;
    int count;
    std::cout << "Enter binary file name: ";
    std::cin >> binFilename;
    std::cout << "Enter number of employees: ";
    std::cin >> count;

    std::ostringstream creatorCmd;
    creatorCmd << "lab01_98_creator.exe " << binFilename << " " << count;

    STARTUPINFO siCreator;
    PROCESS_INFORMATION piCreator;
    ZeroMemory(&siCreator, sizeof(siCreator));
    siCreator.cb = sizeof(siCreator);
    ZeroMemory(&piCreator, sizeof(piCreator));

    if (!CreateProcess(NULL,
        const_cast<char*>(creatorCmd.str().c_str()),
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &siCreator,
        &piCreator))
    {
        std::cerr << " Error creating process creator. Error code: "
            << GetLastError() << "\n";
        return 1;
    }

    WaitForSingleObject(piCreator.hProcess, INFINITE);
    CloseHandle(piCreator.hProcess);
    CloseHandle(piCreator.hThread);

    printBinaryFile(binFilename.c_str());

    std::string reportFilename;
    double payRate;
    std::cout << "\nEnter report file name: ";
    std::cin >> reportFilename;
    std::cout << "\nEnter pay rate: ";
    std::cin >> payRate;

    std::ostringstream reporterCmd;
    reporterCmd << "lab01_98_reporter.exe " << binFilename << " " << reportFilename << " " << payRate << "\n";

    STARTUPINFO siReporter;
    PROCESS_INFORMATION piReporter;
    ZeroMemory(&siReporter, sizeof(siReporter));
    siReporter.cb = sizeof(siReporter);
    ZeroMemory(&piReporter, sizeof(piReporter));

    if (!CreateProcess(NULL,
        const_cast<char*>(reporterCmd.str().c_str()),
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &siReporter,
        &piReporter))
    {
        std::cerr << "Error creating report process. Error code: " << GetLastError() << "\n";
        return 1;
    }

    WaitForSingleObject(piReporter.hProcess, INFINITE);
    CloseHandle(piReporter.hProcess);
    CloseHandle(piReporter.hThread);

    printReportFile(reportFilename.c_str());

    std::cout << "\nThe program has been successfully completed.\n";
    return 0;
}