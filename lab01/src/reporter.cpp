//
// Created by pravl on 24.03.2025.
//
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <cstring>
#include "../include/employee.h"

struct reportEntry {
    int num;
    char name[10];
    double hours;
    double salary;
};

bool compareByNum(const reportEntry &a, const reportEntry &b) {
    return a.num < b.num;
}

void printReport (std::ostream& out, const char* binFilename, const std::vector<reportEntry> &entries) {
    out << "File report \"" << binFilename << "\"\n";

    out << std::left
        << std::setw(10) << "Employee #"
        << std::setw(10) << "Name"
        << std::setw(10) << "Hours"
        << std::setw(10) << "Salary"
        << "\n";

    out << std::string(40, '-') << "\n";

    for (size_t i = 0; i < entries.size(); i++) {
        out << std::left
            << std::setw(10) << entries[i].num
            << std::setw(10) << entries[i].name
            << std::setw(10) << entries[i].hours
            << std::setw(10) << std::fixed << std::setprecision(2)
            << std::setw(10) << entries[i].salary
            << "\n";
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: reporter <binary_filename> <report_filename> <pay_rate>\n";
        return 1;
    }

    const char* binFilename = argv[1];
    const char* reportFilename = argv[2];
    double payRate = std::atof(argv[3]);
    if (payRate <= 0) {
        std::cerr << "Pay rate must be greater than 0\n";
        return 1;
    }

    std::ifstream in (binFilename, std::ios::binary);
    if (!in) {
        std::cerr << "Cannot open binary file " << binFilename << " for reading.\n";
        return 1;
    }

    std::vector<reportEntry> entries;
    employee emp;
    while (in.read(reinterpret_cast<char*>(&emp), sizeof(emp))) {
        reportEntry entry;
        entry.num = emp.num;
        std::strncpy(entry.name, emp.name, 10);
        entry.hours = emp.hours;
        entry.salary = emp.hours * payRate;
        entries.push_back(entry);
    }
    in.close();

    std::sort(entries.begin(), entries.end(), compareByNum);

    std::ofstream out (reportFilename);
    if (!out) {
        std::cerr << "Cannot open report file " << reportFilename << " for writing.\n";
        return 1;
    }

    printReport(out, binFilename, entries);

    out.close();
    std::cout << "Report successfully saved in " << reportFilename << "\n";
    return 0;
}