//
// Created by pravl on 24.03.2025.
//

#include <iostream>
#include <fstream>
#include <cstdlib>

struct employee {
    int num;            // id num
    char name[10];      // name
    double hours;       // number of hours worked
};

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: creator <binary_filename> <number_of_records>\n";
        return 1;
    }

    const char* filename = argv[1];
    int count = std::atoi(argv[2]);
    if (count <= 0) {
        std::cerr << "Invalid number of records\n";
        return 1;
    }

    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        std::cerr << "Error opening output file\n";
        return 1;
    }

    employee emp;
    for (int i = 0; i < count; ++i) {
        std::cout << "Enter info for employee " << i + 1 << ":\n";
        std::cout << "Employee number: ";
        std::cin >> emp.num;
        std::cout << "Employee name (up to 9 characters, without spaces): ";
        std::cin >> emp.name;
        std::cout << "Employee hours: ";
        std::cin >> emp.hours;

        out.write(reinterpret_cast<char*>(&emp), sizeof(employee));
        if (!out) {
            std::cerr << "Error writing to output file\n";
            return 1;
        }
    }

    out.close();
    std::cout << "binary file " << filename << " created successfully.\n";
    return 0;
}