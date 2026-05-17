#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

int counter = 0;
std::mutex counterMutex;

int main() {

    const int threadsCount = 3;

    std::vector<std::thread> threads;
    threads.reserve(threadsCount);

    for (int thread_id = 1; thread_id <= threadsCount; ++thread_id) {
        threads.emplace_back([thread_id]() {
            for (int k = 0; k < 5; ++k) {
                int snapshot = 0;
                {
                    std::lock_guard<std::mutex> lock(counterMutex);
                    counter += thread_id;
                    snapshot = counter;
                }

                std::cout << "thread_id=" << thread_id
                          << " step=" << k
                          << " counter=" << snapshot << "\n";

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }

    for (auto &t : threads) {
        t.join();
    }

    const int expected = (1 + 2 + 3) * 5; // кожен потік 5 разів додає свій id
    std::cout << "Final counter: " << counter << " (expected " << expected << ")\n";
    return 0;
}