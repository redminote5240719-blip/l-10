#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

template <typename T>
class BoundedBlockingQueue {
public:
    explicit BoundedBlockingQueue(std::size_t capacity) : capacity_(capacity) {}

    void enqueue(T value) {
        std::unique_lock<std::mutex> lock(m_);
        notFull_.wait(lock, [this]() { return q_.size() < capacity_; });

        q_.push(std::move(value));

        notEmpty_.notify_one();
    }

    T dequeue() {
        std::unique_lock<std::mutex> lock(m_);
        notEmpty_.wait(lock, [this]() { return !q_.empty(); });

        T value = std::move(q_.front());
        q_.pop();

        notFull_.notify_one();
        return value;
    }

private:
    std::size_t capacity_;
    std::queue<T> q_;
    std::mutex m_;
    std::condition_variable notEmpty_;
    std::condition_variable notFull_;
};

int main() {
    const std::size_t capacity = 5;
    BoundedBlockingQueue<int> queue(capacity);

    const int producersCount = 2;
    const int consumersCount = 2;
    const int itemsPerProducer = 10;


    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    producers.reserve(producersCount);
    consumers.reserve(consumersCount);

    for (int p = 0; p < producersCount; ++p) {
        producers.emplace_back([p, &queue, itemsPerProducer]() {
            for (int i = 0; i < itemsPerProducer; ++i) {
                const int value = p * 100 + i;
                queue.enqueue(value);
                std::cout << "producer " << p << " -> " << value << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        });
    }

    for (int c = 0; c < consumersCount; ++c) {
        consumers.emplace_back([c, &queue]() {
            while (true) {
                int value = queue.dequeue();
                if (value == -1) {
                    std::cout << "consumer " << c << " got stop signal\n";
                    break;
                }
                std::cout << "consumer " << c << " <- " << value << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(8));
            }
        });
    }

    for (auto &t : producers) {
        t.join();
    }

    for (int i = 0; i < consumersCount; ++i) {
        queue.enqueue(-1);
    }

    for (auto &t : consumers) {
        t.join();
    }

    std::cout << "Done.\n";
    return 0;
}