//
// Created by Luis Holanda on 2019-04-15.
//

#ifndef LAB3_MPMCQUEUE_HPP
#define LAB3_MPMCQUEUE_HPP

#pragma once
#include <memory>

// A basic multi-producer, multi-consumer bounded queue.
template<typename T>
class MPMCQueue {
public:
    MPMCQueue(size_t size)
        : buffer(reinterpret_cast<cell_t*>(new aligned_cell_t[size]))
        , buffer_mask(size - 1)
        , producer_pos(0)
        , consumer_pos(0)
    {
        assert(size >= 2 and (size & size - 1) == 0);

        for (size_t i = 0; i != size; ++i) {
            buffer[i].stamp.store(i, std::memory_order_relaxed);
        }
    }

    ~MPMCQueue() {
        delete []buffer;
    }

    // Tries to add an element to the queue.
    // Returns false if the queue is full.
    bool enqueue(T const& data) {
        for(;;) {
            size_t pos = producer_pos.load(std::memory_order_relaxed);
            cell_t* cell = &buffer[pos & buffer_mask];
            size_t stamp = cell->stamp.load(std::memory_order_acquire);

            intptr_t diff = static_cast<intptr_t>(stamp) - static_cast<intptr_t>(pos);

            // This slot is empty.
            if (diff == 0) {
                // Move head to claim the slot.
                if (producer_pos.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) {
                    cell->data = data;
                    cell->stamp.store(stamp + 1, std::memory_order_release);
                    return true;
                }
            } else if (diff < 0) {
                // The expected slot is occupied, therefore the buffer is full.
                return false;
            }
        }
    }

    // Tries to remove an element from the queue.
    // Returns false if the queue is empty.
    bool dequeue(T &data) {
        for(;;) {
            size_t pos = consumer_pos.load(std::memory_order_relaxed);
            cell_t* cell = &buffer[pos & buffer_mask];
            size_t stamp = cell->stamp.load(std::memory_order_acquire);

            intptr_t diff = static_cast<intptr_t>(stamp) - static_cast<intptr_t>(pos + 1);

            // The slot is taken.
            if (diff == 0) {
                // Move head to claim the slot.
                if (consumer_pos.compare_exchange_weak(pos, pos + 1, std::memory_order_acquire)) {
                   data = cell->data;
                   cell->stamp.store(pos + buffer_mask + 1, std::memory_order_release);
                   return true;
                }
            } else if (diff < 0) {
                // The expected slot is empty, therefore the buffer is empty.
                return false;
            }
        }
    }
private:
    struct cell_t {
        std::atomic_size_t stamp;
        T data;
    };

    // Correct aligned cell type.
    typedef typename std::aligned_storage<sizeof(cell_t), std::alignment_of<cell_t>::value>::type aligned_cell_t;

    // Make sure that each field is in a different cache line.
    // Prevents cache invalidation.
    alignas(64) cell_t* const buffer;
    alignas(64) size_t const buffer_mask;
    alignas(64) std::atomic_size_t producer_pos;
    alignas(64) std::atomic_size_t consumer_pos;

    void operator=(const MPMCQueue&) {};
};

#endif //LAB3_MPMCQUEUE_HPP
