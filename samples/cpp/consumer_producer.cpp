#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <stop_token>

std::mutex mtx;
std::condition_variable cvP;
std::condition_variable cvC;
std::vector<int> buffer;

void producer(std::stop_token stok) {
    int i = 0;
    while (!stok.stop_requested()) {
        std::unique_lock<std::mutex> lock(mtx);
        cvP.wait(lock, [] {
            return buffer.size() < 2;
        });
        buffer.push_back(i);
        std::cout << "Produced: " << buffer.back() << std::endl;
        i++;
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        lock.unlock();
        cvC.notify_one();
    }
}

void consumer(std::stop_token stok) {
    while (!stok.stop_requested()) {
        std::unique_lock<std::mutex> lock(mtx);
        cvC.wait(lock, [] {
            return buffer.size() > 0;
        });
        std::cout << "Consumed: " << buffer.back() << std::endl;
        buffer.pop_back();
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        lock.unlock();
        cvP.notify_one();
    }
}

int main() {
    std::jthread pT(producer);
    std::jthread cT0(consumer);
    std::jthread cT1(consumer);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    pT.request_stop();
    cT0.request_stop();
    cT1.request_stop();

    return 0;
}
