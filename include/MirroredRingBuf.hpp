#pragma once
#include <array>
#include <cstddef>

template <class T, size_t Q_SIZE>
struct MirroredRingBuf {
    std::array<T, Q_SIZE * 2> buf{};

    size_t count = 0;
    size_t writeHead = 0;

    void write(T sample) {
        buf[writeHead] = sample;
        buf[writeHead + Q_SIZE] = sample;
        writeHead = (writeHead + 1) % Q_SIZE;
        count < Q_SIZE ? count++ : count;
    }

    operator T*() { return (writeHead + buf.data()); }

    size_t capacity() { return Q_SIZE; }
    size_t size() { return count; }

 private:
    size_t head = 0;
};
