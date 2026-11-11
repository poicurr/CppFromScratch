#include <stdexcept>
#include <string>

#include <catch2/catch_test_macros.hpp>

#include "CppFromScratch/Stack.hpp"

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
  static inline bool s_shouldThrow = false;

  int m_value;

  explicit ThrowOnCopy(int value) : m_value(value) {}

  ThrowOnCopy(const ThrowOnCopy &other) : m_value(other.m_value) {
    if (s_shouldThrow)
      throw std::runtime_error("copy failed");
  }
};

} // namespace

TEST_CASE("Stack preserves LIFO order", "[Stack]") {
  Stack<int> stack(3);

  stack.push(10);
  stack.push(20);
  stack.push(30);

  REQUIRE(stack.pop() == 30);
  REQUIRE(stack.pop() == 20);
  REQUIRE(stack.pop() == 10);
}

TEST_CASE("Stack rejects overflow", "[Stack]") {
  Stack<int> stack(2);

  stack.push(10);
  stack.push(20);

  REQUIRE_THROWS_AS(stack.push(30), std::overflow_error);

  REQUIRE(stack.pop() == 20);
  REQUIRE(stack.pop() == 10);
}

TEST_CASE("Stack rejects underflow", "[Stack]") {
  Stack<int> stack(2);

  REQUIRE_THROWS_AS(stack.pop(), std::underflow_error);

  stack.push(42);

  REQUIRE(stack.pop() == 42);
  REQUIRE_THROWS_AS(stack.pop(), std::underflow_error);
}

TEST_CASE("Stack supports repeated push and pop", "[Stack]") {
  Stack<int> stack(1);

  for (int i = 0; i < 100; ++i) {
    stack.push(i);
    REQUIRE(stack.pop() == i);
  }
}

TEST_CASE("Stack supports zero capacity", "[Stack]") {
  Stack<int> stack(0);

  REQUIRE_THROWS_AS(stack.push(1), std::overflow_error);
  REQUIRE_THROWS_AS(stack.pop(), std::underflow_error);
}

TEST_CASE("Stack supports non-default-constructible types", "[Stack]") {
  Stack<Tracked> stack(2);

  Tracked::reset();

  stack.push(Tracked(42));

  REQUIRE(stack.pop().m_value == 42);
  REQUIRE(Tracked::s_alive == 0);
}

TEST_CASE("Stack destroys remaining elements", "[Stack]") {
  Tracked::reset();

  {
    Stack<Tracked> stack(3);

    stack.push(Tracked(10));
    stack.push(Tracked(20));
    stack.push(Tracked(30));

    REQUIRE(Tracked::s_alive == 3);
  }

  REQUIRE(Tracked::s_alive == 0);
  REQUIRE(Tracked::s_constructed == Tracked::s_destroyed);
}

TEST_CASE("Stack destroys popped elements", "[Stack]") {
  Tracked::reset();

  {
    Stack<Tracked> stack(3);

    stack.push(Tracked(10));
    stack.push(Tracked(20));

    REQUIRE(Tracked::s_alive == 2);

    {
      Tracked value = stack.pop();
      REQUIRE(value.m_value == 20);
      REQUIRE(Tracked::s_alive == 2);
    }

    REQUIRE(Tracked::s_alive == 1);
  }

  REQUIRE(Tracked::s_alive == 0);
  REQUIRE(Tracked::s_constructed == Tracked::s_destroyed);
}

TEST_CASE("Stack remains valid after failed push", "[Stack]") {
  Stack<ThrowOnCopy> stack(2);

  ThrowOnCopy value(42);

  ThrowOnCopy::s_shouldThrow = true;
  REQUIRE_THROWS_AS(stack.push(value), std::runtime_error);

  ThrowOnCopy::s_shouldThrow = false;

  stack.push(value);
  REQUIRE(stack.pop().m_value == 42);
  REQUIRE_THROWS_AS(stack.pop(), std::underflow_error);
}

TEST_CASE("Stack supports string values", "[Stack]") {
  Stack<std::string> stack(3);

  stack.push("Hello");
  stack.push("World");

  REQUIRE(stack.pop() == "World");
  REQUIRE(stack.pop() == "Hello");
}
