#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

int counter = 0;

int main() {
  

    const int threadsCount = 3;

    std::vector<std::thread> threads;
    threads.reserve(threadsCount);

    for (int thread_id = 1; thread_id <= threadsCount; ++thread_id) {
        threads.emplace_back([thread_id]() {
            for (int k = 0; k < 5; ++k) {
                counter += thread_id;

                std::cout << "thread_id=" << thread_id
                          << " step=" << k
                          << " counter=" << counter << "\n";

    
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }

    for (auto &t : threads) {
        t.join();
    }

    std::cout << "Final counter (NON-deterministic due to Data Race): " << counter << "\n";
    return 0;
}