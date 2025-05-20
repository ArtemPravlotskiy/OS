//
// Created by pravl on 20.05.2025.
//

#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include "utils.h"

#pragma comment(lib, "advapi32.lib")

SRWLOCK *g_locks = NULL; // array of locks on record
int g_recCount = 0; // number of entries in the file
std::string g_filename; // file name

void print_file() {
    FILE *f = fopen(g_filename.c_str(), "rb");
    if (!f) {
        std::cerr << "Couldn't open the file\n";
        return;
    }
    employee e;
    std::cout << "--- File Contents ---\n";
    while (fread(&e, sizeof(e), 1, f) == 1) {
        std::cout << "ID=" << e.num
                << "  Name=" << e.name
                << "  Hours=" << e.hours << "\n";
    }
    fclose(f);
}

DWORD WINAPI InstanceThread(LPVOID lpvParam) {
    HANDLE hPipe = (HANDLE) lpvParam;
    Request req;
    bool running = true;

    int lockedMode = 0; // 0 = none, 1 = shared, 2 = exclusive
    int lockedIdx  = -1;

    while (running) {
        DWORD cbRead = 0;
        if (!ReadFile(hPipe, &req, sizeof(req), &cbRead, NULL) || cbRead == 0)
            break;

        switch (req.type) {
            case REQUEST_READ: {
                int idx = req.id;
                if (idx < 0 || idx >= g_recCount) break;
                AcquireSRWLockShared(&g_locks[idx]);
                lockedMode = 1; lockedIdx = idx;
                // reading the record
                FILE *f = fopen(g_filename.c_str(), "rb");
                fseek(f, idx * sizeof(employee), SEEK_SET);
                employee e;
                fread(&e, sizeof(e), 1, f);
                fclose(f);
                // sending to the client
                DWORD cbWritten;
                WriteFile(hPipe, &e, sizeof(e), &cbWritten, NULL);
                break;
            }
            case REQUEST_WRITE: {
                int idx = req.id;
                if (idx < 0 || idx >= g_recCount) break;
                AcquireSRWLockExclusive(&g_locks[idx]);
                lockedMode = 2; lockedIdx = idx;
                // read the record and send
                FILE *f = fopen(g_filename.c_str(), "rb");
                fseek(f, idx * sizeof(employee), SEEK_SET);
                employee e;
                fread(&e, sizeof(e), 1, f);
                fclose(f);
                DWORD cbW;
                WriteFile(hPipe, &e, sizeof(e), &cbW, NULL);
                break;
            }
            case REQUEST_WRITE_SUBMIT: {
                int idx = req.id;
                // recording new data
                FILE *f = fopen(g_filename.c_str(), "rb+");
                fseek(f, idx * sizeof(employee), SEEK_SET);
                fwrite(&req.data, sizeof(employee), 1, f);
                fclose(f);
                break;
            }
            case REQUEST_RELEASE: {
                if (lockedIdx>=0) {
                    if (lockedMode == 1) {
                        ReleaseSRWLockShared(&g_locks[lockedIdx]);
                    } else if (lockedMode == 2) {
                        ReleaseSRWLockExclusive(&g_locks[lockedIdx]);
                    }
                }
                lockedMode = 0;
                lockedIdx  = -1;
                break;
            }
            case REQUEST_EXIT: {
                running = false;
                break;
            }
        }
    }

    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    return 0;
}

int main() {
    // 1. Entering the name and employees
    std::cout << "Enter the name of the binary file: ";
    std::getline(std::cin, g_filename);

    std::cout << "How many employees? ";
    std::cin >> g_recCount;
    std::vector<employee> vec(g_recCount);
    for (int i = 0; i < g_recCount; i++) {
        vec[i].num = i;
        std::cout << "Employee #" << i << ": name and hours: ";
        std::cin >> vec[i].name >> vec[i].hours;
    }
    // writing the file
    FILE *f = fopen(g_filename.c_str(), "wb");
    for (int i = 0; i < g_recCount; i++)
        fwrite(&vec[i], sizeof(employee), 1, f);
    fclose(f);

    // output
    print_file();

    // creating locks
    g_locks = new SRWLOCK[g_recCount];
    for (int i = 0; i < g_recCount; i++)
        InitializeSRWLock(&g_locks[i]);

    // 2. clients
    int clientCount;
    std::cout << "How many clients to launch? ";
    std::cin >> clientCount;

    std::vector<HANDLE> th(clientCount);
    for (int i = 0; i < clientCount; i++) {
        HANDLE hPipe = CreateNamedPipe(
            PIPE_NAME,
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            0, 0, 0, NULL);
        if (hPipe == INVALID_HANDLE_VALUE) {
            std::cerr << "CreateNamedPipe failed\n";
            return 1;
        }

        // Waiting for the client to connect
        BOOL ok = ConnectNamedPipe(hPipe, NULL);
        if (!ok && GetLastError() != ERROR_PIPE_CONNECTED) {
            std::cerr<<"ConnectNamedPipe failed, GLE="<<GetLastError()<<"\n";
            CloseHandle(hPipe);
            return 1;
        }

        // creating a handler thread
        th[i] = CreateThread(NULL, 0, InstanceThread, hPipe, 0,NULL);
    }

    // waiting for the completion of all clients
    WaitForMultipleObjects(clientCount, &th[0], TRUE, INFINITE);

    // the final output
    std::cout << "\n--- Modified file ---\n";
    print_file();

    std::cout << "Press Enter to exit...";
    std::cin.ignore();
    std::cin.get();
    return 0;
}
