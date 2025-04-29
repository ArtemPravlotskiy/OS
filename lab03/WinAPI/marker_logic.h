//
// Created by pravl on 29.04.2025.
//

#ifndef MARKER_LOGIC_H
#define MARKER_LOGIC_H

#include <windows.h>

bool TryMarkOnce(int* arr, int arrSize, int markerId, CRITICAL_SECTION* cs);

#endif //MARKER_LOGIC_H
