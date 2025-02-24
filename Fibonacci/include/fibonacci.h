//
// Created by pravl on 23.02.2025.
//

#ifndef FIBONACCI_H
#define FIBONACCI_H

#include <vector>

class Fibonacci {
public:
    Fibonacci();
    ~Fibonacci();

    std::vector<unsigned long> getSequence(int n) const;

protected:
    unsigned long nextNum(const unsigned long &a, const unsigned long &b) const;

private:
    void validateInput(const int &n) const;
};

#endif //FIBONACCI_H
