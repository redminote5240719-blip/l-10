#include <iostream>
#include <thread>
#include <vector>

int main() {

    const std::vector<int> values{2, 5, -7};

    std::vector<std::thread> threads;
    threads.reserve(values.size());

    for (int i = 0; i < static_cast<int>(values.size()); ++i) {
        const int x = values[i];
        threads.emplace_back([x, i]() {
            const int sq = x * x;
            std::cout << "thread " << i << ": " << x << "^2 = " << sq << "\n";
        });
    }

    for (auto &t : threads) {
        t.join();
    }

    std::cout << "Done.\n";
    return 0;
}