//
// Created by pravl on 14.04.2025.
//

#include <iostream>
#include <windows.h>

int* arr = NULL;
int arrSize = 0;


CRITICAL_SECTION criticalSection;

HANDLE hStartEvent = NULL;

struct MarkerParams {
    int id;
    HANDLE hResumeEvent;
    HANDLE hTerminateEvent;
    HANDLE hPauseEvent;
};

DWORD WINAPI MarkerThread(LPVOID lpParam) {
    MarkerParams* params = static_cast<MarkerParams *>(lpParam);

    WaitForSingleObject(hStartEvent, INFINITE);

    srand(params->id);

    int markedCount = 0;
    while (true) {
        int randNum = rand();
        int index = randNum % arrSize;

        EnterCriticalSection(&criticalSection);
        if (arr[index] == 0) {
            LeaveCriticalSection(&criticalSection);

            Sleep(5);

            EnterCriticalSection(&criticalSection);
            if (arr[index] == 0) {
                arr[index] = params->id;
                markedCount++;
            }
            LeaveCriticalSection(&criticalSection);

            Sleep(5);

            continue;
        }
        else {
            int impossibleIndex = index;
            LeaveCriticalSection(&criticalSection);

            std::cout << "Thread marker #" << params->id << ": Marked elements - " << markedCount
                      << "; Imposssible marked index - " << index << "\n";

            SetEvent(params->hPauseEvent);
        }
    }
}