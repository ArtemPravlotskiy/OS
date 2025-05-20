//
// Created by pravl on 20.05.2025.
//

#include <windows.h>
#include <iostream>
#include <string>
#include "utils.h"

int main() {
    // connecting to the channel
    HANDLE hPipe;
    while (true) {
        hPipe = CreateFile(
            PIPE_NAME,
            GENERIC_READ | GENERIC_WRITE,
            0, NULL, OPEN_EXISTING, 0, NULL);
        if (hPipe != INVALID_HANDLE_VALUE) break;
        if (GetLastError() != ERROR_PIPE_BUSY) {
            std::cerr << "Couldn't open channel\n";
            return 1;
        }
        // waiting for 5 seconds.
        if (!WaitNamedPipe(PIPE_NAME, 5000)) {
            std::cerr << "There are no available channel instances\n";
            return 1;
        }
    }

    std::cout << "Connected to the server.\n";

    bool running = true;
    while (running) {
        std::cout << "\nSelect an operation:\n"
                << "1. Read the entry\n"
                << "2. Modify an entry\n"
                << "3. Exit\nYour choice: ";
        int choice;
        std::cin >> choice;
        Request req;
        DWORD cb;

        switch (choice) {
            case 1: // read
                req.type = REQUEST_READ;
                std::cout << "Enter the ID: ";
                std::cin >> req.id;
                WriteFile(hPipe, &req, sizeof(req), &cb, NULL);
                employee er;
                ReadFile(hPipe, &er, sizeof(er), &cb, NULL);
                std::cout << "Received: ID=" << er.num
                        << " Name=" << er.name
                        << " Hours=" << er.hours << "\n";
                // releasing
                req.type = REQUEST_RELEASE;
                WriteFile(hPipe, &req, sizeof(req), &cb, NULL);
                break;

            case 2: // record
                req.type = REQUEST_WRITE;
                std::cout << "Enter the ID: ";
                std::cin >> req.id;
                WriteFile(hPipe, &req, sizeof(req), &cb, NULL);
                ReadFile(hPipe, &er, sizeof(er), &cb, NULL);
                std::cout << "Current record: ID=" << er.num
                        << " Name=" << er.name
                        << " Hours=" << er.hours << "\n";
                // entering new data
                std::cout << "New name and hours: ";
                std::cin >> er.name >> er.hours;
                // sending the modified record
                req.type = REQUEST_WRITE_SUBMIT;
                req.data = er;
                WriteFile(hPipe, &req, sizeof(req), &cb, NULL);
                // releasing
                req.type = REQUEST_RELEASE;
                WriteFile(hPipe, &req, sizeof(req), &cb, NULL);
                break;

            case 3:
                // exit signal
                req.type = REQUEST_EXIT;
                WriteFile(hPipe, &req, sizeof(req), &cb, NULL);
                running = false;
                break;

            default:
                std::cout << "Incorrect choice\n";
        }
    }

    CloseHandle(hPipe);
    std::cout << "The client is shutting down.\n";
    return 0;
}
