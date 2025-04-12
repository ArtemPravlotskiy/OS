//
// Created by pravl on 07.04.2025.
//

#include <iostream>
#include <windows.h>

using namespace std;

int* array = nullptr;
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
