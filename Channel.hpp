//
// Created by Luis Holanda on 2019-04-16.
//

#ifndef LAB3_CHANNEL_HPP
#define LAB3_CHANNEL_HPP

#include <atomic>
#include <optional>
#include "SemapQueue.hpp"

template<typename T>
class Channel {
public:
    explicit Channel(size_t size)
        : open(true)
        , queue(size){};

    Channel(Channel<T> const& copy) = default;

    virtual ~Channel() = default;

    bool recv(T& data) noexcept;
    bool send(T const & data) noexcept;
    void close() noexcept;

private:
    alignas(64) bool open;
    std::mutex open_mtx;
    SemapQueue<std::optional<T>> queue;
};

template<typename T>
bool Channel<T>::recv(T& data) noexcept {
    std::optional<T> dataopt = std::nullopt;

    this->queue.dequeue(dataopt);

    if (!dataopt.has_value()) {
        this->queue.enqueue(std::nullopt);
        return false;
    }

    data = *dataopt;
    return true;
}

template<typename T>
bool Channel<T>::send(T const& data) noexcept {
    auto dataopt = std::make_optional(data);

    {
        std::lock_guard lock(this->open_mtx);
        if (!this->open) {
            return false;
        }

        this->queue.enqueue(dataopt);
    }

    return true;
}

template<typename T>
void Channel<T>::close() noexcept {
    std::lock_guard lock(this->open_mtx);

    this->open = false;

    this->queue.enqueue(std::nullopt);
}

#endif //LAB3_CHANNEL_HPP
