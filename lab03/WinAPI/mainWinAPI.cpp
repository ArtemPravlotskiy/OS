//
// Created by pravl on 14.04.2025.
//

#include <iostream>
#include <windows.h>
#include "marker_logic.h"

int* arr = NULL;
int arrSize = 0;


CRITICAL_SECTION criticalSection;

HANDLE StartEvent = NULL;

struct MarkerParams {
    int id;
    HANDLE hResumeEvent;
    HANDLE hTerminateEvent;
    HANDLE hPauseEvent;
};

DWORD WINAPI MarkerThread(LPVOID lpParam) {
    MarkerParams* params = static_cast<MarkerParams *>(lpParam);

    WaitForSingleObject(StartEvent, INFINITE);

    srand(params->id);
    int markedCount = 0;

    while (true) {
        if (TryMarkOnce(arr, arrSize, params->id, &criticalSection)) {
            markedCount++;
            Sleep(5);
            continue;
        }

        EnterCriticalSection(&criticalSection);
        int impossibleIndex = -1;
        for (int i = 0; i < arrSize; ++i) {
            if (arr[i] != 0 && arr[i] != params->id) {
                impossibleIndex = i;
                break;
            }
        }
        printf("Thread marker #%d: Marked elements - %d; Imposssible marked index - %d\n",
               params->id, markedCount, impossibleIndex);
        LeaveCriticalSection(&criticalSection);

        SetEvent(params->hPauseEvent);

        const HANDLE events[2] = {params->hTerminateEvent, params->hResumeEvent};
        DWORD dwWait = WaitForMultipleObjects(2, events, FALSE, INFINITE);

        ResetEvent(params->hPauseEvent);
        ResetEvent(params->hResumeEvent);

        if (dwWait == WAIT_OBJECT_0) {
            EnterCriticalSection(&criticalSection);
            for (int i = 0; i < arrSize; ++i) {
                if (arr[i] == params->id) {
                    arr[i] = 0;
                }
            }
            LeaveCriticalSection(&criticalSection);
            break;
        }
    }

    return 0;
}

void PrintArray() {
    EnterCriticalSection(&criticalSection);
    std::cout << "Array elements: ";
    for (int i = 0; i < arrSize; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << "\n";
    LeaveCriticalSection(&criticalSection);
}

int main() {
    InitializeCriticalSection(&criticalSection);

    std::cout << "Enter array size: ";
    std::cin >> arrSize;
    arr = new int[arrSize];
    for (int i = 0; i < arrSize; ++i) {
        arr[i] = 0;
    }

    int markerCount = 0;
    std::cout << "Enter marker thread count: ";
    std::cin >> markerCount;

    HANDLE* threads = new HANDLE[markerCount];
    MarkerParams* params = new MarkerParams[markerCount];

    StartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    for (int i = 0; i < markerCount; ++i) {
        params[i].id = i + 1;
        params[i].hResumeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        params[i].hTerminateEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        params[i].hPauseEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        threads[i] = CreateThread(NULL, 0, MarkerThread, &params[i], 0, NULL);
    }

    SetEvent(StartEvent);

    int activeCount = markerCount;
    bool* active = new bool[markerCount];
    for (int i = 0; i < markerCount; ++i) {
        active[i] = true;
    }

    while (activeCount > 0) {
        int waitingCount = 0;
        for (int i = 0; i < markerCount; ++i) {
            if (active[i]) {
                WaitForSingleObject(params[i].hPauseEvent, INFINITE);
                waitingCount++;
            }
        }

        EnterCriticalSection(&criticalSection);
        PrintArray();

        int termId = 0;
        std::cout << "Enter term ID: ";
        std::cin >> termId;
        LeaveCriticalSection(&criticalSection);

        if (termId < 1 || termId > markerCount || !active[termId - 1]) {
            std::cout << "Invalid term ID or thread has already terminated. Continue.\n";
        } else {
            SetEvent(params[termId - 1].hTerminateEvent);

            WaitForSingleObject(threads[termId - 1], INFINITE);

            active[termId - 1] = false;
            activeCount--;

            CloseHandle(threads[termId - 1]);
            CloseHandle(params[termId - 1].hResumeEvent);
            CloseHandle(params[termId - 1].hTerminateEvent);
            CloseHandle(params[termId - 1].hPauseEvent);

            std::cout << "Thread #" << termId << " terminated.\n";
        }

        PrintArray();
        for (int i = 0; i < markerCount; ++i) {
            if (active[i]) {
                SetEvent(params[i].hResumeEvent);
            }
        }
    }

    delete[] arr;
    delete[] threads;
    delete[] params;
    delete[] active;
    CloseHandle(StartEvent);
    DeleteCriticalSection(&criticalSection);

    std::cout << "All threads terminated.\n";

    return 0;
}