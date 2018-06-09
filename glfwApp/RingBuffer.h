#pragma once
#include <cassert>

// A sinmple fixed size ring buffer.
template <class T, int N>
struct RingBuffer
{
    RingBuffer() : _tail(0), _head(0), _currentSize(0) {}
    bool empty() const { return _currentSize == 0; }
    bool full() const { return _currentSize == N; }
    size_t size() const { return _currentSize; }
    T const& operator[](size_t idx) { return _array[(_tail + idx) % N]; }

    void push_back(T v)
    {
        if (_currentSize == N) _head = (_head + 1) % N;
        _array[_tail] = v;
        _tail = (_tail + 1) % N;
        _currentSize++;
    }

    void pop_front()
    {
        assert(!empty());
        _head = (_head + 1) % N;
        _currentSize--;
    }

    T front() const { return _array[_head]; }

   private:
    T _array[N];
    size_t _tail;
    size_t _head;
    size_t _currentSize = 0;
};
