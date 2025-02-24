//
// Created by pravl on 23.02.2025.
//

#include <fibonacci.h>
#include <stdexcept>

Fibonacci::Fibonacci() {
}

Fibonacci::~Fibonacci() {
}

void Fibonacci::validateInput(const int &n) const {
    if (n <= 0) {
        throw std::invalid_argument("n must be positive");
    }
}

unsigned long Fibonacci::nextNum(const unsigned long &a, const unsigned long &b) const {
    return a + b;
}

std::vector<unsigned long> Fibonacci::getSequence(int n) const {
    validateInput(n);
    std::vector<unsigned long> sequence;
    sequence.reserve(n);

    if (n == 0) {
        return sequence;
    }
    sequence.push_back(0);
    if (n == 1) {
        return sequence;
    }
    sequence.push_back(1);

    for (int i = 2; i < n; ++i) {
        sequence.push_back(nextNum(sequence[i-1], sequence[i-2]));
    }
    return sequence;
}
