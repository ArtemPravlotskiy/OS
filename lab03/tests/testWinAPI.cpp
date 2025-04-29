//
// Created by pravl on 29.04.2025.
//

#include <windows.h>
#include <gtest/gtest.h>
#include "../WinAPI/marker_logic.h"

TEST(TryMarkOnceTest, MarksIfSlotEmpty) {
    int arr[10] = {0};
    CRITICAL_SECTION cs;
    InitializeCriticalSection(&cs);
    srand(1);

    bool result = TryMarkOnce(arr, 10, 5, &cs);

    DeleteCriticalSection(&cs);

    ASSERT_TRUE(result);
    int count = 0;
    for (int i = 0; i < 10; ++i) {
        if (arr[i] == 5) count++;
    }
    ASSERT_EQ(count, 1);
}

TEST(TryMarkOnceTest, SkipsIfSlotOccupied) {
    int arr[10];
    for (int i = 0; i < 10; ++i) arr[i] = 5;
    CRITICAL_SECTION cs;
    InitializeCriticalSection(&cs);
    srand(2);

    bool result = TryMarkOnce(arr, 10, 7, &cs);

    DeleteCriticalSection(&cs);
    ASSERT_FALSE(result);
}
