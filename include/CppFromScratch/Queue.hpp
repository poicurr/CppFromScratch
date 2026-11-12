#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>

template <typename T>
class Queue {
public:
  explicit Queue(size_t capacity)
      : m_capacity(capacity), m_head(0u), m_size(0u), m_allocator(),
        m_data(m_allocator.allocate(capacity)) {}
  ~Queue() {
    while (m_size != 0u) {
      std::destroy_at(m_data + m_head);
      m_head = (m_head + 1) % m_capacity;
      --m_size;
    }
    m_allocator.deallocate(m_data, m_capacity);
  }

  Queue(const Queue &) = delete;
  Queue &operator=(const Queue &) = delete;

  void push(T value) {
    if (m_size >= m_capacity)
      throw std::overflow_error("capacity overflow");
    std::construct_at(m_data + (m_head + m_size) % m_capacity, value);
    ++m_size;
  }

  T pop() {
    if (m_size == 0u)
      throw std::underflow_error("queue is empty");
    T result = m_data[m_head];
    std::destroy_at(m_data + m_head);
    m_head = (m_head + 1) % m_capacity;
    --m_size;
    return result;
  }

private:
  size_t m_capacity;
  size_t m_head;
  size_t m_size;
  std::allocator<T> m_allocator;
  T *m_data;
};
