//
// Created by pravl on 23.02.2025.
//

#include <iostream>
#include <vector>
#include "fibonacci.h"

int main() {
    int n;

    std::cout << "Enter number of terms in the fibonacci sequence: ";
    std::cin >> n;

    try {
        const Fibonacci fib;
        std::vector<unsigned long> seq = fib.getSequence(n);
        std::cout << "Fibonacci sequence is: ";
        for (std::vector<unsigned long>::const_iterator it = seq.begin(); it != seq.end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}