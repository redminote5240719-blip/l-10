#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex m1;
std::mutex m2;

int main() {
    std::thread t1([]() {
        std::cout << "t1: lock m1\n";
        m1.lock();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        std::cout << "t1: lock m2 (may deadlock)\n";
        m2.lock();

        std::cout << "t1: acquired both\n";
        m2.unlock();
        m1.unlock();
    });

    std::thread t2([]() {
        std::cout << "t2: lock m2\n";
        m2.lock();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        std::cout << "t2: lock m1 (may deadlock)\n";
        m1.lock();

        std::cout << "t2: acquired both\n";
        m1.unlock();
        m2.unlock();
    });

    t1.join();
    t2.join();

    std::cout << "Done (if no deadlock occurred).\n";
    return 0;
}