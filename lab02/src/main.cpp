//
// Created by pravl on 07.04.2025.
//

#include <iostream>
#include <windows.h>

using namespace std;

int* arr = NULL;
int arraySize = 0;
int Min = 0, Max = 0;
double avg = 0.0;

DWORD WINAPI ThreadMinMax(LPVOID lpParam) {
    if (arraySize <= 0) {
        return 1;
    }

    Min = arr[0];
    Max = arr[0];

    for (int i = 1; i < arraySize; ++i) {
        if (arr[i] < Min) {
            Min = arr[i];
        }
        Sleep(7);

        if (arr[i] > Max) {
            Max = arr[i];
        }
        Sleep(7);
    }
    std::cout << "Min: " << Min << "\nMax: " << Max << "\n";

    return 0;
}

DWORD WINAPI ThreadAvg(LPVOID lpParam) {
    if (arraySize <= 0) {
        return 1;
    }

    int sum = 0;
    for (int i = 0; i < arraySize; ++i) {
        sum += arr[i];
        Sleep(12);
    }
    avg = static_cast<double>(sum) / arraySize;
    std::cout << "Avg: " << avg << "\n";

    return 0;
}

int main() {
    cout << "Enter array size: ";
    cin >> arraySize;
    if (arraySize < 1) {
        cout << "Invalid array size!" << endl;
    }

    arr = new int[arraySize];
    cout << "Enter array elements: ";
    for (int i = 0; i < arraySize; ++i) {
        cin >> arr[i];
    }

    HANDLE hThreadMinMax = CreateThread(NULL, 0, ThreadMinMax, NULL, 0, NULL);
    HANDLE hThreadAvg = CreateThread(NULL, 0, ThreadAvg, NULL, 0, NULL);

    if (hThreadMinMax == NULL || hThreadAvg == NULL) {
        cout << "Error creating thread!" << endl;
        delete[] arr;
        return 1;
    }

    WaitForSingleObject(hThreadMinMax, INFINITE);
    WaitForSingleObject(hThreadAvg, INFINITE);

    for (int i = 0; i < arraySize; ++i) {
        if (arr[i] == Min || arr[i] == Max) {
            arr[i] = static_cast<int>(avg);
        }
    }

    cout << "New array is: ";
    for (int i = 0; i < arraySize; ++i) {
        cout << arr[i] << " ";
    }

    delete[] arr;
    CloseHandle(hThreadMinMax);
    CloseHandle(hThreadAvg);

    return 0;
}

