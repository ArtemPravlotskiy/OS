//
// Created by pravl on 07.04.2025.
//

#include <iostream>
#include <windows.h>

using namespace std;

int* array = NULL;
int arraySize = 0;
int Min = 0, Max = 0;
double avg = 0.0;

DWORD WINAPI ThreadMinMax(LPVOID lpParam) {
    if (arraySize <= 0) {
        return 0;
    }

    Min = array[0];
    Max = array[0];

    for (int i = 1; i < arraySize; ++i) {
        if (array[i] < Min) {
            Min = array[i];
        }
        Sleep(7);

        if (array[i] > Max) {
            Max = array[i];
        }
        Sleep(7);
    }

    return 0;
}

DWORD WINAPI ThreadAvg(LPVOID lpParam) {
    if (arraySize <= 0) {
        return 0;
    }

    int sum = 0;
    for (int i = 0; i < arraySize; ++i) {
        sum += array[i];
        Sleep(12);
    }
    avg = static_cast<double>(sum) / arraySize;

    return 0;
}

int main() {
    cout << "Enter array size: ";
    cin >> arraySize;
    if (arraySize < 1) {
        cout << "Invalid array size!" << endl;
    }

    array = new int[arraySize];
    cout << "Enter array elements: ";
    for (int i = 0; i < arraySize; ++i) {
        cin >> array[i];
    }

    HANDLE hThreadMinMax = CreateThread(NULL, 0, ThreadMinMax, NULL, 0, NULL);
    HANDLE hThreadAvg = CreateThread(NULL, 0, ThreadAvg, NULL, 0, NULL);

    if (hThreadMinMax == NULL || hThreadAvg == NULL) {
        cout << "Error creating thread!" << endl;
        delete[] array;
        return 1;
    }

    WaitForSingleObject(hThreadMinMax, INFINITE);
    WaitForSingleObject(hThreadAvg, INFINITE);

    for (int i = 0; i < arraySize; ++i) {
        if (array[i] == Min || array[i] == Max) {
            array[i] = static_cast<int>(avg);
        }
    }

    cout << "New array is: ";
    for (int i = 0; i < arraySize; ++i) {
        cout << array[i] << " ";
    }

    delete[] array;
    CloseHandle(hThreadMinMax);
    CloseHandle(hThreadAvg);

    return 0;
}

