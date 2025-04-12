//
// Created by pravl on 12.04.2025.
//

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

using namespace std;

int main() {
    int n;
    cout << "Enter array size: ";
    cin >> n;
    if (n <= 0) {
        cout << "Array size must be greater than 0" << endl;
        return 1;
    }

    vector <int> array(n);
    cout << "Enter array elements: ";
    for (int i = 0; i < n; ++i) {
        cin >> array[i];
    }

    int Min = 0, Max = 0;
    double avg = 0.0;

    thread MinMax([&]() {
        Min = array[0];
        Max = array[0];

        for (int i= 1; i < n; ++i) {
            if (Min > array[i]) {
                Min = array[i];
            }
            this_thread::sleep_for(chrono::milliseconds(7));

            if (Max < array[i]) {
                Max = array[i];
            }
            this_thread::sleep_for(chrono::milliseconds(7));
        }
    });

    thread Avg([&]() {
        int sum = 0;

        for (int i = 0; i < n; ++i) {
            sum += array[i];
            this_thread::sleep_for(chrono::milliseconds(12));
        }
        avg = static_cast<double>(sum) / n;
    });

    MinMax.join();
    Avg.join();

    for (int i = 0; i < n; ++i) {
        if (array[i] == Max || array[i] == Min) {
            array[i] = static_cast<int>(avg);
        }
    }

    cout << "New array elements: ";
    for (int i = 0; i < n; ++i) {
        cout << array[i] << " ";
    }

    return 0;
}