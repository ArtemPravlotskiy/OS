//
// Created by pravl on 14.04.2025.
//

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <random>

// common data
std::vector <int> arr;
int arrSize = 0;
std::mutex mutex;

// start signal
bool start = false;
std::mutex startMutex;
std::condition_variable startCV;

int activeCount = 0;
int pausedCount = 0;
std::mutex mainMutex;
std::condition_variable mainCV;

struct Marker {
    int id = 0;
    int markedCount = 0;
    bool terminate = false;
    bool resume = false;
    std::mutex mtx;
    std::condition_variable cv;
};
std::vector<Marker> markers;

void markerThread(const int idx) {
    Marker &self = markers[idx];

    // wait start signal
    {
        std::unique_lock<std::mutex> lk(startMutex);
        startCV.wait(lk, [] { return start; });
    }

    // init generator
    std::mt19937 rng(self.id);
    std::uniform_int_distribution<int> dist(0, arrSize - 1);

    while (true) {
        const int index = dist(rng);

        // try to mark element
        {
            std::unique_lock<std::mutex> lk(mutex);
            if (arr[index] == 0) {
                lk.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(5));

                std::unique_lock<std::mutex> lk2(mutex);
                if (arr[index] == 0) {
                    arr[index] = self.id;
                    ++self.markedCount;
                }

                lk2.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                continue;
            }
        }

        printf("Thread marker #%d: Marked elements - %d; Impossible marked index - %d\n",
                    self.id, self.markedCount, index);

        {
            std::lock_guard<std::mutex> ml(mainMutex);
            ++pausedCount;
            if (pausedCount == activeCount) {
                mainCV.notify_one();
            }
        }

        std::unique_lock <std::mutex> lk(self.mtx);
        self.cv.wait(lk, [&]{ return self.resume || self.terminate; });
        if (self.terminate) {
            // clear our markers
            std::lock_guard<std::mutex> lk3(mutex);
            for (auto& x : arr) {
                if (x == self.id) {
                    x = 0;
                }
            }
            break;
        }
        self.resume = false;
    }
}

void PrintArray() {
    std::lock_guard<std::mutex> lk(mutex);
    std::cout << "Array elements: ";
    for (auto x : arr) {
        std::cout << x << " ";
    }
    std::cout << "\n";
}

int main() {
    std::cout << "Enter array size: ";
    std::cin >> arrSize;
    arr.assign(arrSize, 0);

    int count;
    std::cout << "Enter marker thread count: ";
    std::cin >> count;
    activeCount = count;
    markers = std::vector<Marker>(count);
    for (int i = 0; i < count; ++i) {
        markers[i].id = i + 1;
    }

    // start thread
    std::vector<std::thread> threads;
    for (int i = 0; i < count; ++i) {
        threads.emplace_back(markerThread, i);
    }

    // signal for start work
    {
        std::lock_guard<std::mutex> lk(startMutex);
        start = true;
    }
    startCV.notify_all();

    // main logic
    while (activeCount > 0) {
        // wait all threads pause
        {
            std::unique_lock<std::mutex> lk(mainMutex);
            mainCV.wait(lk, []{ return pausedCount == activeCount; });
        }

        PrintArray();

        int termId;
        std::cout << "Enter term id: ";
        std::cin >> termId;

        if (termId < 1 || termId > count || markers[termId - 1].terminate) {
            std::cout << "Invalid id or already terminated. Continuing!\n";
        } else {
            {
                std::lock_guard<std::mutex> lk(markers[termId - 1].mtx);
                markers[termId - 1].terminate = true;
            }
            markers[termId - 1].cv.notify_one();

            if (threads[termId - 1].joinable()) {
                threads[termId - 1].join();
            }
            --activeCount;
            std::cout << "Thread #" << termId << " terminated!\n";

            PrintArray();
        }

        {
            std::lock_guard<std::mutex> lk(mainMutex);
            pausedCount = 0;
        }
        for (int i = 0; i < count; ++i) {
            if (i + 1 == termId) {
                continue;
            }
            {
                std::lock_guard<std::mutex> lk(markers[i].mtx);
                markers[i].resume = true;
            }
            markers[i].cv.notify_one();
        }
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    std::cout << "All marker threads finished!\n";

    return 0;
}
