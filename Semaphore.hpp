//
// Created by Luis Holanda on 2019-04-15.
//

#ifndef LAB3_SEMAPHORE_HPP
#define LAB3_SEMAPHORE_HPP

#include <mutex>
#include <condition_variable>

class Semaphore {
public:
    explicit Semaphore(size_t count)
        : count(count)
    {}

    virtual ~Semaphore() = default;

    void post() noexcept {
        {
            std::unique_lock<std::mutex> lock(mtx);
            count++;
        }
        cv.notify_one();
    }

    void wait() noexcept {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]() { return count != 0; });
        count--;
    }

private:
    std::mutex mtx;
    std::condition_variable cv;
    size_t count;
};


#endif //LAB3_SEMAPHORE_HPP
