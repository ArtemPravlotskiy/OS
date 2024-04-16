//
// Created by pravl on 29.04.2025.
//

#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <chrono>
#include "../src/mainThread.cpp"

// Вспомогательная функция
void prepareTestEnv(int size, int markerCount) {
    arrSize = size;
    arr.assign(size, 0);
    activeCount = markerCount;
    pausedCount = 0;
    markers = std::vector<Marker>(markerCount);
    for (int i = 0; i < markerCount; ++i) {
        markers[i].id = i + 1;
        markers[i].terminate = false;
        markers[i].resume = false;
    }
    start = false;
}

TEST(MarkerThreadTest, ThreadsMarkArray) {
    prepareTestEnv(100, 3);

    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(markerThread, i);
    }

    {
        std::lock_guard<std::mutex> lk(startMutex);
        start = true;
    }
    startCV.notify_all();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    {
        std::lock_guard<std::mutex> lk(mainMutex);
        for (auto& marker : markers) {
            marker.terminate = true;
            marker.cv.notify_one();
        }
    }

    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }

    int markedElements = 0;
    {
        std::lock_guard<std::mutex> lk(mutex);
        for (int x : arr) {
            if (x != 0) ++markedElements;
        }
    }

    EXPECT_GT(markedElements, 0);
}

TEST(MarkerThreadTest, TerminatedThreadCleansMarkers) {
    prepareTestEnv(50, 1);

    std::thread t(markerThread, 0);
    {
        std::lock_guard<std::mutex> lk(startMutex);
        start = true;
    }
    startCV.notify_all();

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    {
        std::lock_guard<std::mutex> lk(markers[0].mtx);
        markers[0].terminate = true;
    }
    markers[0].cv.notify_one();
    if (t.joinable()) t.join();

    {
        std::lock_guard<std::mutex> lk(mutex);
        for (int val : arr) {
            EXPECT_NE(val, markers[0].id);
        }
    }
}

TEST(MarkerThreadTest, PauseAndResume) {
    prepareTestEnv(20, 2);
    std::vector<std::thread> threads;
    for (int i = 0; i < 2; ++i) {
        threads.emplace_back(markerThread, i);
    }

    {
        std::lock_guard<std::mutex> lk(startMutex);
        start = true;
    }
    startCV.notify_all();

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    {
        std::lock_guard<std::mutex> lk(mainMutex);
        pausedCount = 2;
    }

    for (int i = 0; i < 2; ++i) {
        {
            std::lock_guard<std::mutex> lk(markers[i].mtx);
            markers[i].resume = true;
        }
        markers[i].cv.notify_one();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    for (auto& marker : markers) {
        std::lock_guard<std::mutex> lk(marker.mtx);
        EXPECT_FALSE(marker.terminate);
    }

    for (int i = 0; i < 2; ++i) {
        {
            std::lock_guard<std::mutex> lk(markers[i].mtx);
            markers[i].terminate = true;
        }
        markers[i].cv.notify_one();
    }

    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }
}
