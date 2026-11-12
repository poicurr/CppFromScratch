#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>

template <typename T>
class Stack {
public:
  explicit Stack(size_t capacity)
      : m_position(0u), m_capacity(capacity), m_allocator(),
        m_data(m_allocator.allocate(capacity)) {}

  ~Stack() {
    std::destroy_n(m_data, m_position);
    m_allocator.deallocate(m_data, m_capacity);
  }

  Stack(const Stack &) = delete;
  Stack &operator=(const Stack &) = delete;

  void push(T value) {
    if (m_position >= m_capacity)
      throw std::overflow_error("stack overflow");

    std::construct_at(m_data + m_position, value);
    ++m_position;
  }

  T pop() {
    if (m_position == 0u)
      throw std::underflow_error("stack is empty");

    T result = m_data[m_position - 1];
    std::destroy_at(m_data + m_position - 1);
    --m_position;
    return result;
  }

private:
  size_t m_position;
  size_t m_capacity;
  std::allocator<T> m_allocator;
  T *m_data;
};
