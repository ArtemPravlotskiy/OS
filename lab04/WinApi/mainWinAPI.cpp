//
// Created by pravl on 08.05.2025.
//

#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>

const int MSG_SIZE = 20;


struct RingHeader {
    LONG capacity;    // max num of records
    LONG head;        // reading index
    LONG tail;        // record index
};

std::string makeName(const std::string &base, const std::string &suffix) {
    return base + "_" + suffix;
}

int main() {
    // Task 1
    std::cout << "Enter binary filename: ";
    std::string filename;
    std::getline(std::cin, filename);

    std::cout << "Enter number of records (capacity): ";
    LONG capacity;
    std::cin >> capacity;
    std::cin.ignore(); // read '\n'

    // Task 1_1
    // Creating/opening the file
    HANDLE hFile = CreateFileA(
        filename.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Cannot create file\n";
        return 1;
    }

    // Calculating the file size: header + capacity * MSG_SIZE bytes
    DWORD fileSize = sizeof(RingHeader) + capacity * MSG_SIZE;
    // Expanding the file
    SetFilePointer(hFile, fileSize, NULL, FILE_BEGIN);
    SetEndOfFile(hFile);

    // 2. Create FileMapping
    HANDLE hMap = CreateFileMappingA(
        hFile,
        NULL,
        PAGE_READWRITE,
        0,
        0,
        NULL);
    if (!hMap) {
        std::cerr << "Cannot create file mapping\n";
        return 1;
    }

    // display in memory
    LPVOID pView = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (!pView) {
        std::cerr << "Cannot map view\n";
        return 1;
    }

    // init header
    RingHeader *header = (RingHeader*)pView;
    header->capacity = capacity;
    header->head = 0;
    header->tail = 0;

    // 2. Enter number of Sender processes
    std::cout << "Enter number of Sender processes: ";
    int senderCount;
    std::cin >> senderCount;
    std::cin.ignore();

    // Generating the names of synchro objects
    std::string base = filename;
    std::string mtxName = makeName(base, "mtx");
    std::string semFreeName = makeName(base, "free");
    std::string semFilledName = makeName(base, "filled");
    std::string readySemName = makeName(base, "ready");

    // Create Mutex
    HANDLE hMtx = CreateMutexA(NULL, FALSE, mtxName.c_str());
    // Create Semaphores
    HANDLE hSemFree = CreateSemaphoreA(NULL, capacity, capacity, semFreeName.c_str());
    HANDLE hSemFilled = CreateSemaphoreA(NULL, 0, capacity, semFilledName.c_str());
    // Semaphore for ready: init value 0, max senderCount
    HANDLE hReady = CreateSemaphoreA(NULL, 0, senderCount, readySemName.c_str());

    // 3. Start Senders
    for (int i = 0; i < senderCount; ++i) {
        std::ostringstream cmd;
        cmd << "Sender.exe " << filename;
        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        if (!CreateProcessA(NULL, (LPSTR)cmd.str().c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            std::cerr << "Cannot start Sender " << i << "\n";
            return 1;
        }
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }

    // 4. Waiting ready signals from all Sender
    std::cout << "Waiting for senders readiness...\n";
    for (int i = 0; i < senderCount; ++i) {
        WaitForSingleObject(hReady, INFINITE);
    }
    std::cout << "All senders are ready.\n";

    // 5. Receiver command cycle
    while (true) {
        std::cout << "Enter command (read/exit): ";
        std::string cmd;
        std::getline(std::cin, cmd);
        if (cmd == "read") {
            // wait if there are no filled ones
            WaitForSingleObject(hSemFilled, INFINITE);
            // take access
            WaitForSingleObject(hMtx, INFINITE);

            // Read the message
            char * dataArea = (char*)pView + sizeof(RingHeader);
            char msg[MSG_SIZE + 1] = {0};
            LONG idx = header->head;
            memcpy(msg, dataArea + idx * MSG_SIZE, MSG_SIZE);
            header->head = (idx + 1) % header->capacity;

            ReleaseMutex(hMtx);
            // Increase the number of available
            ReleaseSemaphore(hSemFree, 1, NULL);

            std::cout << "Received: " << msg << "\n";
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