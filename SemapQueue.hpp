//
// Created by Luis Holanda on 2019-04-15.
//

#ifndef LAB3_SEMAPQUEUE_HPP
#define LAB3_SEMAPQUEUE_HPP

#include "MPMCQueue.hpp"
#include "Semaphore.hpp"

/// Semaphore wrapped MPMC queue.
/// Producers waits when the queue is full, and consumers waits
/// when the queue is empty.
template<typename T>
class SemapQueue {
public:
    explicit SemapQueue(size_t size)
        : queue(size)
        , cons_sem(size)
        , prod_sem(0)
    {}

    virtual ~SemapQueue() = default;

    /// Adds a new item to the queue.
    /// If the queue is full, waits until a space is available.
    void enqueue(T const& data) noexcept {
        for(;;) {
            // Wait for a space.
            cons_sem.wait();

            if (queue.enqueue(data)) {
                // An item was produced.
                prod_sem.post();
                return;
            }
        }
    }

    /// Removes an item from the queue.
    /// If the queue is empty, waits until an item is available.
    void dequeue(T &data) noexcept {
        for(;;) {
            // Wait for a new item.
            prod_sem.wait();

            if(queue.dequeue(data)) {
                // An item was consumed, therefore an empty space was added.
                cons_sem.post();
                return;
            }
        }
    }
private:
    MPMCQueue<T> queue;

    // Synchronization semaphores.
    // In a given point, we should have `cons_sem + prod_sem = capacity(queue)`.

    // Nº of available empty spaces in the queue.
    Semaphore cons_sem;
    // Nº of available items in the queue.
    Semaphore prod_sem;
};


#endif //LAB3_SEMAPQUEUE_HPP
