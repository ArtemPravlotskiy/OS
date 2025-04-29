//
// Created by pravl on 29.04.2025.
//

#include "marker_logic.h"
#include <cstdlib>

bool TryMarkOnce(int* arr, int arrSize, int markerId, CRITICAL_SECTION* cs) {
    const int randNum = rand();
    const int index = randNum % arrSize;

    EnterCriticalSection(cs);
    if (arr[index] == 0) {
        LeaveCriticalSection(cs);

        Sleep(5);

        EnterCriticalSection(cs);
        if (arr[index] == 0) {
            arr[index] = markerId;
            LeaveCriticalSection(cs);
            return true;  // success
        }
        LeaveCriticalSection(cs);
        return false;
    } else {
        LeaveCriticalSection(cs);
        return false;
    }
}

