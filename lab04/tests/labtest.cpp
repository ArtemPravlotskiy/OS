//
// Created by pravl on 25.05.2025.
//

#include <gtest/gtest.h>
#include <string>
#include <windows.h>

const int MSG_SIZE = 20;

struct RingHeader {
    long long capacity;    // max num of records
    long long head;        // reading index
    long long tail;        // record index
};

std::string MakeName(const std::string &base, const std::string &suffix) {
    return base + "_" + suffix;
}

inline long advanceIndex(long idx, long capacity) {
    return (idx + 1) % capacity;
}

TEST(UtilsTest, MakeName_Concatenation) {
    std::string base = "datafile";
    std::string suffix = "mtx";
    std::string result = MakeName(base, suffix);
    EXPECT_EQ(result, "datafile_mtx");
}

struct RingHeaderTest {
    RingHeader hdr;
    void init(long capacity) {
        hdr.capacity = capacity;
        hdr.head = 0;
        hdr.tail = 0;
    }
    void push() {
        long idx = hdr.tail;
        hdr.tail = (idx + 1) % hdr.capacity;
    }
    void pop() {
        long idx = hdr.head;
        hdr.head = (idx + 1) % hdr.capacity;
    }
};

TEST(RingHeaderTestSuite, Initialization) {
    RingHeaderTest rb;
    rb.init(5);
    EXPECT_EQ(rb.hdr.capacity, 5);
    EXPECT_EQ(rb.hdr.head, 0);
    EXPECT_EQ(rb.hdr.tail, 0);
}

TEST(RingHeaderTestSuite, TailWrapAround) {
    RingHeaderTest rb;
    rb.init(3);
    rb.push(); EXPECT_EQ(rb.hdr.tail, 1);
    rb.push(); EXPECT_EQ(rb.hdr.tail, 2);
    rb.push(); EXPECT_EQ(rb.hdr.tail, 0);
}

TEST(RingHeaderTestSuite, HeadWrapAround) {
    RingHeaderTest rb;
    rb.init(2);
    // дважды продвигаем head: 0→1, 1→0
    rb.pop(); EXPECT_EQ(rb.hdr.head, 1);
    rb.pop(); EXPECT_EQ(rb.hdr.head, 0);
}

TEST(FileMappingIntegration, HeaderAndMessagePersistence) {
    const char* fname = "test_integration.bin";
    const long capacity = 3;
    const int msgSize = MSG_SIZE;
    const DWORD fileSize = sizeof(RingHeader) + capacity * msgSize;

    // 1) Удаляем старый файл и создаём новый
    std::remove(fname);
    HANDLE hFile = CreateFileA(
        fname,
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    ASSERT_NE(hFile, INVALID_HANDLE_VALUE) << "Не удалось создать файл";

    // 2) Устанавливаем размер
    DWORD pos = SetFilePointer(hFile, fileSize, nullptr, FILE_BEGIN);
    ASSERT_NE(pos, INVALID_SET_FILE_POINTER) << "SetFilePointer failed";
    BOOL eof_ok = SetEndOfFile(hFile);
    ASSERT_TRUE(eof_ok) << "SetEndOfFile failed";

    // 3) Создаём mapping и мапим
    HANDLE hMap = CreateFileMappingA(hFile, nullptr, PAGE_READWRITE, 0, 0, nullptr);
    ASSERT_NE(hMap, nullptr) << "CreateFileMappingA failed";
    LPVOID view = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    ASSERT_NE(view, nullptr) << "MapViewOfFile failed";

    // 4) Инициализируем заголовок и пишем три сообщения
    auto hdr = reinterpret_cast<RingHeader*>(view);
    hdr->capacity = capacity;
    hdr->head = 0;
    hdr->tail = 0;
    char* dataArea = reinterpret_cast<char*>(view) + sizeof(RingHeader);
    const char* msgs[] = {"first", "second", "third"};

    for (int i = 0; i < 3; ++i) {
        long idx = hdr->tail;
        memcpy(dataArea + idx * msgSize, msgs[i], strlen(msgs[i]) + 1);
        hdr->tail = advanceIndex(hdr->tail, hdr->capacity);
    }

    UnmapViewOfFile(view);
    CloseHandle(hMap);
    CloseHandle(hFile);

    // 5) Снова открываем и мапим для проверки
    hFile = CreateFileA(fname, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    ASSERT_NE(hFile, INVALID_HANDLE_VALUE);
    hMap = CreateFileMappingA(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
    ASSERT_NE(hMap, nullptr) << "CreateFileMappingA failed on read-only mapping";
    view = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
    ASSERT_NE(view, nullptr) << "MapViewOfFile failed for read-only view";

    hdr = reinterpret_cast<RingHeader*>(view);
    EXPECT_EQ(hdr->capacity, capacity);
    EXPECT_EQ(hdr->head, 0);
    EXPECT_EQ(hdr->tail, 0);  // после трёх записей для capacity=3 должно вернуться в 0

    dataArea = reinterpret_cast<char*>(view) + sizeof(RingHeader);
    char buffer[msgSize + 1] = {};

    // Читаем по порядку три сообщения
    for (int i = 0; i < 3; ++i) {
        memcpy(buffer, dataArea + i * msgSize, msgSize);
        EXPECT_STREQ(buffer, msgs[i]);
    }

    // 6) Очистка
    UnmapViewOfFile(view);
    CloseHandle(hMap);
    CloseHandle(hFile);
    std::remove(fname);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}