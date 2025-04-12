//
// Created by pravl on 12.04.2025.
//

#include <vector>
#include <thread>
#include <chrono>
#include <stdexcept>

using namespace std;

std::vector<int> processArray(const std::vector<int>& array) {
    int n = array.size();
    if (n <= 0) {
        throw std::invalid_argument("Array size must be greater than 0");
    }

    int minVal = array[0];
    int maxVal = array[0];
    double avg = 0.0;

    int sum = 0;
    std::thread minMaxThread([&]() {
        minVal = array[0];
        maxVal = array[0];
        for (int i = 1; i < n; ++i) {
            if (array[i] < minVal) {
                minVal = array[i];
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(7));
            if (array[i] > maxVal) {
                maxVal = array[i];
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(7));
        }
    });

    std::thread avgThread([&]() {
        for (int i = 0; i < n; ++i) {
            sum += array[i];
            std::this_thread::sleep_for(std::chrono::milliseconds(12));
        }
    });

    minMaxThread.join();
    avgThread.join();

    avg = static_cast<double>(sum) / n;

    std::vector<int> newArray = array;
    for (int i = 0; i < n; ++i) {
        if (newArray[i] == minVal || newArray[i] == maxVal) {
            newArray[i] = static_cast<int>(avg);
        }
    }

    return newArray;
}