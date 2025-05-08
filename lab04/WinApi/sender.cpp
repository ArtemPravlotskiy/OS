//
// Created by pravl on 08.05.2025.
//

#include <windows.h>
#include <iostream>
#include <string>

const int MSG_SIZE = 20;
struct RingHeader { LONG capacity, head, tail; };

std::string makeName(const std::string &base, const std::string &suffix) {
    return base + "_" + suffix;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: Sender.exe <filename>\n";
        return 1;
    }
    std::string filename = argv[1];
    // Opening file-mapping
    HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE hMap  = CreateFileMappingA(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
    LPVOID pView = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    // Открываем IPC-объекты с правом SYNCHRONIZE и MODIFY_STATE
    std::string base = filename;
    HANDLE hMtx      = OpenMutexA(SYNCHRONIZE, FALSE, makeName(base, "mtx").c_str());
    HANDLE hSemFree  = OpenSemaphoreA(SYNCHRONIZE|SEMAPHORE_MODIFY_STATE, FALSE, makeName(base, "free").c_str());
    HANDLE hSemFilled= OpenSemaphoreA(SYNCHRONIZE|SEMAPHORE_MODIFY_STATE, FALSE, makeName(base, "filled").c_str());
    HANDLE hReady    = OpenSemaphoreA(SEMAPHORE_MODIFY_STATE, FALSE, makeName(base, "ready").c_str());

    // 2. ready signal
    ReleaseSemaphore(hReady, 1, NULL);

    RingHeader* header = (RingHeader*)pView;
    char* dataArea = (char*)pView + sizeof(RingHeader);

    // cycle sender commands
    while (true) {
        std::cout << "Enter command (send/exit): ";
        std::string cmd;
        std::getline(std::cin, cmd);

        if (cmd == "send") {
            std::cout << "Enter message (<20 chars): ";
            char input[MSG_SIZE+1] = {0};

            std::cin.getline(input, MSG_SIZE+1);

            // wait free slots
            WaitForSingleObject(hSemFree, INFINITE);
            WaitForSingleObject(hMtx, INFINITE);

            // write to tail
            LONG idx = header->tail;
            memcpy(dataArea + idx*MSG_SIZE, input, MSG_SIZE);
            header->tail = (idx+1) % header->capacity;

            ReleaseMutex(hMtx);
            ReleaseSemaphore(hSemFilled, 1, NULL);
        } else if (cmd == "exit") {
            break;
        }
    }

    // Cleanup
    UnmapViewOfFile(pView);
    CloseHandle(hMap);
    CloseHandle(hFile);
    CloseHandle(hMtx);
    CloseHandle(hSemFree);
    CloseHandle(hSemFilled);
    CloseHandle(hReady);
    return 0;
}