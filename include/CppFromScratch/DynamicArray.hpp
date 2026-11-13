#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>

template <class T>
class DynamicArray {
public:
  explicit DynamicArray(size_t capacity = 8u)
      : m_size(0u), m_capacity(capacity), m_allocator(), m_data(m_allocator.allocate(capacity)) {}
  ~DynamicArray() {
    std::destroy_n(m_data, m_size);
    m_allocator.deallocate(m_data, m_capacity);
  }

  DynamicArray(const DynamicArray &) = delete;
  DynamicArray &operator=(const DynamicArray &) = delete;

  void pushBack(T value) {
    if (m_size == m_capacity)
      reallocate();
    std::construct_at(m_data + m_size, value);
    ++m_size;
  }
  void pushFront(T value) {
    if (m_size == m_capacity)
      reallocate();

    if (m_size == 0) {
      std::construct_at(m_data + m_size, value);
      ++m_size;
      return;
    }

    std::construct_at(m_data + m_size, m_data[m_size - 1]);
    try {
      for (size_t i = m_size - 1; i != 0; --i) {
        m_data[i] = m_data[i - 1];
      }
      m_data[0] = value;
    } catch (...) {
      std::destroy_at(m_data + m_size);
      throw;
    }
    ++m_size;
  }

  T *begin() { return m_data; }
  T *end() { return m_data + m_size; }
  const T *begin() const { return m_data; }
  const T *end() const { return m_data + m_size; }

  T &operator[](size_t index) {
    if (index >= m_size)
      throw std::out_of_range("index out of range");
    return m_data[index];
  }

  const T &operator[](size_t index) const {
    if (index >= m_size)
      throw std::out_of_range("index out of range");
    return m_data[index];
  }

private:
  void reallocate() {
    const size_t MAX_CAPACITY = std::allocator_traits<decltype(m_allocator)>::max_size(m_allocator);

    size_t newCapacity;
    if (m_capacity == MAX_CAPACITY) {
      throw std::length_error("reallocate failed: exceed MAX_CAPACITY");
    } else if (m_capacity > MAX_CAPACITY / 2) {
      newCapacity = MAX_CAPACITY;
    } else {
      newCapacity = m_capacity == 0 ? 1 : m_capacity * 2;
    }

    T *newData = m_allocator.allocate(newCapacity);
    size_t constructedCount = 0;
    try {
      for (size_t i = 0u; i < m_size; ++i) {
        std::construct_at(newData + i, m_data[i]);
        ++constructedCount;
      }
    } catch (...) {
      std::destroy_n(newData, constructedCount);
      m_allocator.deallocate(newData, newCapacity);
      throw;
    }
    std::destroy_n(m_data, m_size);
    m_allocator.deallocate(m_data, m_capacity);
    m_capacity = newCapacity;
    m_data = newData;
  }

  size_t m_size;
  size_t m_capacity;
  std::allocator<T> m_allocator;
  T *m_data;
};
