//
// Created by pravl on 20.05.2025.
//

#ifndef UTILS_H
#define UTILS_H

#include <windows.h>

#pragma pack(push,1)
struct employee {
    int num;
    char name[10];
    double hours;
};
#pragma pack(pop)

enum RequestType {
    REQUEST_READ = 1,
    REQUEST_WRITE,
    REQUEST_WRITE_SUBMIT,
    REQUEST_RELEASE,
    REQUEST_EXIT
};

#pragma pack(push,1)
struct Request {
    DWORD type;
    int   id;
    employee data;
};
#pragma pack(pop)

const char* PIPE_NAME = "\\\\.\\pipe\\MyPipe";

#endif //UTILS_H
