#include <cstddef>
#include <stdexcept>
#include <string>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "CppFromScratch/DynamicArray.hpp"

namespace {

struct Tracked {
  static inline int s_alive = 0;
  static inline int s_constructed = 0;
  static inline int s_destroyed = 0;

  int m_value;

  Tracked() = delete;

  explicit Tracked(int value) : m_value(value) {
    ++s_alive;
    ++s_constructed;
  }

  Tracked(const Tracked &other) : m_value(other.m_value) {
    ++s_alive;
    ++s_constructed;
  }

  Tracked &operator=(const Tracked &other) {
    m_value = other.m_value;
    return *this;
  }

  ~Tracked() {
    --s_alive;
    ++s_destroyed;
  }

  static void reset() {
    s_alive = 0;
    s_constructed = 0;
    s_destroyed = 0;
  }
};

struct ThrowOnCopy {
  static inline int s_copiesBeforeThrow = -1;

  int m_value;

  explicit ThrowOnCopy(int value) : m_value(value) {}

  ThrowOnCopy(const ThrowOnCopy &other) : m_value(other.m_value) {
    if (s_copiesBeforeThrow == 0)
      throw std::runtime_error("copy failed");

    if (s_copiesBeforeThrow > 0)
      --s_copiesBeforeThrow;
  }
};

struct ThrowOnAssign {
  static inline bool s_shouldThrow = false;
  static inline int s_alive = 0;

  int m_value;

  explicit ThrowOnAssign(int value) : m_value(value) { ++s_alive; }

  ThrowOnAssign(const ThrowOnAssign &other) : m_value(other.m_value) { ++s_alive; }

  ThrowOnAssign &operator=(const ThrowOnAssign &other) {
    if (s_shouldThrow)
      throw std::runtime_error("assignment failed");

    m_value = other.m_value;
    return *this;
  }

  ~ThrowOnAssign() { --s_alive; }
};

} // namespace

TEST_CASE("DynamicArray starts empty", "[DynamicArray]") {
  DynamicArray<int> array;

  REQUIRE(array.begin() == array.end());
}

TEST_CASE("DynamicArray supports zero initial capacity", "[DynamicArray]") {
  DynamicArray<int> array(0);

  array.pushBack(10);
  array.pushBack(20);

  REQUIRE(array[0] == 10);
  REQUIRE(array[1] == 20);
  REQUIRE(array.end() - array.begin() == 2);
}

TEST_CASE("DynamicArray preserves pushBack order", "[DynamicArray]") {
  DynamicArray<int> array(4);

  array.pushBack(10);
  array.pushBack(20);
  array.pushBack(30);

  REQUIRE(array[0] == 10);
  REQUIRE(array[1] == 20);
  REQUIRE(array[2] == 30);
  REQUIRE(array.end() - array.begin() == 3);
}

TEST_CASE("DynamicArray grows beyond initial capacity", "[DynamicArray]") {
  DynamicArray<int> array(2);

  for (int i = 0; i < 100; ++i)
    array.pushBack(i);

  REQUIRE(array.end() - array.begin() == 100);

  for (int i = 0; i < 100; ++i)
    REQUIRE(array[static_cast<size_t>(i)] == i);
}

TEST_CASE("DynamicArray preserves pushFront order", "[DynamicArray]") {
  DynamicArray<int> array(4);

  array.pushFront(10);
  array.pushFront(20);
  array.pushFront(30);

  REQUIRE(array[0] == 30);
  REQUIRE(array[1] == 20);
  REQUIRE(array[2] == 10);
}

TEST_CASE("DynamicArray pushFront handles zero capacity", "[DynamicArray]") {
  DynamicArray<int> array(0);

  array.pushFront(10);
  array.pushFront(20);

  REQUIRE(array[0] == 20);
  REQUIRE(array[1] == 10);
}

TEST_CASE("DynamicArray grows during pushFront", "[DynamicArray]") {
  DynamicArray<int> array(1);

  for (int i = 0; i < 100; ++i)
    array.pushFront(i);

  REQUIRE(array.end() - array.begin() == 100);

  for (int i = 0; i < 100; ++i)
    REQUIRE(array[static_cast<size_t>(i)] == 99 - i);
}

TEST_CASE("DynamicArray supports mixed insertion", "[DynamicArray]") {
  DynamicArray<int> array(2);

  array.pushBack(20);
  array.pushBack(30);
  array.pushFront(10);
  array.pushBack(40);
  array.pushFront(0);

  REQUIRE(array.end() - array.begin() == 5);

  for (int i = 0; i < 5; ++i)
    REQUIRE(array[static_cast<size_t>(i)] == i * 10);
}

TEST_CASE("DynamicArray supports mutable indexing", "[DynamicArray]") {
  DynamicArray<int> array;

  array.pushBack(10);
  array[0] = 42;

  REQUIRE(array[0] == 42);

  static_assert(std::is_same_v<decltype(array[0]), int &>);
}

TEST_CASE("DynamicArray rejects out-of-range indices", "[DynamicArray]") {
  DynamicArray<int> array(2);

  REQUIRE_THROWS_AS(array[0], std::out_of_range);

  array.pushBack(10);

  REQUIRE(array[0] == 10);
  REQUIRE_THROWS_AS(array[1], std::out_of_range);
  REQUIRE_THROWS_AS(array[100], std::out_of_range);
}

TEST_CASE("DynamicArray supports range-based for", "[DynamicArray]") {
  DynamicArray<int> array(2);

  for (int i = 1; i <= 5; ++i)
    array.pushBack(i);

  int sum = 0;

  for (const int value : array)
    sum += value;

  REQUIRE(sum == 15);

  for (int &value : array)
    value *= 2;

  REQUIRE(array[0] == 2);
  REQUIRE(array[4] == 10);
}

TEST_CASE("DynamicArray supports const iteration", "[DynamicArray]") {
  DynamicArray<int> array(2);

  array.pushBack(10);
  array.pushBack(20);

  const auto &ref = array;

  int sum = 0;

  for (const int &value : ref)
    sum += value;

  REQUIRE(sum == 30);
}

TEST_CASE("DynamicArray supports strings", "[DynamicArray]") {
  DynamicArray<std::string> array(1);

  array.pushBack("World");
  array.pushFront("Hello");
  array.pushBack("C++");

  REQUIRE(array[0] == "Hello");
  REQUIRE(array[1] == "World");
  REQUIRE(array[2] == "C++");
}

TEST_CASE("DynamicArray supports non-default-constructible types", "[DynamicArray]") {
  Tracked::reset();

  {
    DynamicArray<Tracked> array(2);

    array.pushBack(Tracked(10));
    array.pushFront(Tracked(20));

    REQUIRE(array[0].m_value == 20);
    REQUIRE(array[1].m_value == 10);
    REQUIRE(Tracked::s_alive == 2);
  }

  REQUIRE(Tracked::s_alive == 0);
  REQUIRE(Tracked::s_constructed == Tracked::s_destroyed);
}

TEST_CASE("DynamicArray destroys all live elements", "[DynamicArray]") {
  Tracked::reset();

  {
    DynamicArray<Tracked> array(1);

    for (int i = 0; i < 20; ++i)
      array.pushBack(Tracked(i));

    REQUIRE(Tracked::s_alive == 20);
  }

  REQUIRE(Tracked::s_alive == 0);
  REQUIRE(Tracked::s_constructed == Tracked::s_destroyed);
}

TEST_CASE("DynamicArray destroys shifted and reallocated elements", "[DynamicArray]") {
  Tracked::reset();

  {
    DynamicArray<Tracked> array(2);

    for (int i = 0; i < 10; ++i)
      array.pushFront(Tracked(i));

    REQUIRE(Tracked::s_alive == 10);

    for (int i = 0; i < 10; ++i)
      REQUIRE(array[static_cast<size_t>(i)].m_value == 9 - i);
  }

  REQUIRE(Tracked::s_alive == 0);
  REQUIRE(Tracked::s_constructed == Tracked::s_destroyed);
}

TEST_CASE("DynamicArray preserves values after failed reallocation", "[DynamicArray]") {
  ThrowOnCopy::s_copiesBeforeThrow = -1;

  DynamicArray<ThrowOnCopy> array(2);

  array.pushBack(ThrowOnCopy(10));
  array.pushBack(ThrowOnCopy(20));

  // First relocation copy succeeds; second throws.
  ThrowOnCopy::s_copiesBeforeThrow = 1;

  REQUIRE_THROWS_AS(array.pushBack(ThrowOnCopy(30)), std::runtime_error);

  ThrowOnCopy::s_copiesBeforeThrow = -1;

  REQUIRE(array.end() - array.begin() == 2);
  REQUIRE(array[0].m_value == 10);
  REQUIRE(array[1].m_value == 20);

  array.pushBack(ThrowOnCopy(30));

  REQUIRE(array[2].m_value == 30);
}

TEST_CASE("DynamicArray remains usable after failed pushBack", "[DynamicArray]") {
  ThrowOnCopy::s_copiesBeforeThrow = -1;

  DynamicArray<ThrowOnCopy> array(3);
  array.pushBack(ThrowOnCopy(10));

  ThrowOnCopy::s_copiesBeforeThrow = 0;

  REQUIRE_THROWS_AS(array.pushBack(ThrowOnCopy(20)), std::runtime_error);

  ThrowOnCopy::s_copiesBeforeThrow = -1;

  REQUIRE(array.end() - array.begin() == 1);
  REQUIRE(array[0].m_value == 10);

  array.pushBack(ThrowOnCopy(30));

  REQUIRE(array[1].m_value == 30);
}

TEST_CASE("DynamicArray cleans up after failed pushFront", "[DynamicArray]") {
  ThrowOnAssign::s_shouldThrow = false;
  ThrowOnAssign::s_alive = 0;

  {
    DynamicArray<ThrowOnAssign> array(4);

    array.pushBack(ThrowOnAssign(10));
    array.pushBack(ThrowOnAssign(20));

    REQUIRE(ThrowOnAssign::s_alive == 2);

    ThrowOnAssign::s_shouldThrow = true;

    REQUIRE_THROWS_AS(array.pushFront(ThrowOnAssign(5)), std::runtime_error);

    ThrowOnAssign::s_shouldThrow = false;

    REQUIRE(array.end() - array.begin() == 2);
    REQUIRE(ThrowOnAssign::s_alive == 2);

    array.pushFront(ThrowOnAssign(5));

    REQUIRE(array[0].m_value == 5);
    REQUIRE(array[1].m_value == 10);
    REQUIRE(array[2].m_value == 20);
  }

  REQUIRE(ThrowOnAssign::s_alive == 0);
}
