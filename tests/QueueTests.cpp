#include <stdexcept>
#include <string>

#include <catch2/catch_test_macros.hpp>

#include "CppFromScratch/Queue.hpp"

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

TEST_CASE("Queue preserves FIFO order", "[Queue]") {
  Queue<int> queue(3);

  queue.push(10);
  queue.push(20);
  queue.push(30);

  REQUIRE(queue.pop() == 10);
  REQUIRE(queue.pop() == 20);
  REQUIRE(queue.pop() == 30);
}

TEST_CASE("Queue rejects overflow", "[Queue]") {
  Queue<int> queue(2);

  queue.push(10);
  queue.push(20);

  REQUIRE_THROWS_AS(queue.push(30), std::overflow_error);

  REQUIRE(queue.pop() == 10);
  REQUIRE(queue.pop() == 20);
}

TEST_CASE("Queue rejects underflow", "[Queue]") {
  Queue<int> queue(2);

  REQUIRE_THROWS_AS(queue.pop(), std::underflow_error);

  queue.push(42);

  REQUIRE(queue.pop() == 42);
  REQUIRE_THROWS_AS(queue.pop(), std::underflow_error);
}

TEST_CASE("Queue supports repeated push and pop", "[Queue]") {
  Queue<int> queue(1);

  for (int i = 0; i < 100; ++i) {
    queue.push(i);
    REQUIRE(queue.pop() == i);
  }
}

TEST_CASE("Queue wraps around correctly", "[Queue]") {
  Queue<int> queue(3);

  queue.push(10);
  queue.push(20);

  REQUIRE(queue.pop() == 10);

  queue.push(30);
  queue.push(40);

  REQUIRE(queue.pop() == 20);
  REQUIRE(queue.pop() == 30);
  REQUIRE(queue.pop() == 40);
}

TEST_CASE("Queue supports multiple wraparounds", "[Queue]") {
  Queue<int> queue(3);

  for (int cycle = 0; cycle < 100; ++cycle) {
    queue.push(cycle * 3);
    queue.push(cycle * 3 + 1);
    queue.push(cycle * 3 + 2);

    REQUIRE(queue.pop() == cycle * 3);
    REQUIRE(queue.pop() == cycle * 3 + 1);
    REQUIRE(queue.pop() == cycle * 3 + 2);
  }
}

TEST_CASE("Queue preserves order across partial wraparound", "[Queue]") {
  Queue<int> queue(4);

  queue.push(10);
  queue.push(20);
  queue.push(30);
  queue.push(40);

  REQUIRE(queue.pop() == 10);
  REQUIRE(queue.pop() == 20);

  queue.push(50);
  queue.push(60);

  REQUIRE_THROWS_AS(queue.push(70), std::overflow_error);

  REQUIRE(queue.pop() == 30);
  REQUIRE(queue.pop() == 40);
  REQUIRE(queue.pop() == 50);
  REQUIRE(queue.pop() == 60);
}

TEST_CASE("Queue supports zero capacity", "[Queue]") {
  Queue<int> queue(0);

  REQUIRE_THROWS_AS(queue.push(1), std::overflow_error);
  REQUIRE_THROWS_AS(queue.pop(), std::underflow_error);
}

TEST_CASE("Queue supports non-default-constructible types", "[Queue]") {
  Tracked::reset();

  {
    Queue<Tracked> queue(2);

    queue.push(Tracked(42));

    REQUIRE(queue.pop().m_value == 42);
    REQUIRE(Tracked::s_alive == 0);
  }

  REQUIRE(Tracked::s_constructed == Tracked::s_destroyed);
}

TEST_CASE("Queue destroys remaining elements", "[Queue]") {
  Tracked::reset();

  {
    Queue<Tracked> queue(3);

    queue.push(Tracked(10));
    queue.push(Tracked(20));
    queue.push(Tracked(30));

    REQUIRE(Tracked::s_alive == 3);
  }

  REQUIRE(Tracked::s_alive == 0);
  REQUIRE(Tracked::s_constructed == Tracked::s_destroyed);
}

TEST_CASE("Queue destroys popped elements", "[Queue]") {
  Tracked::reset();

  {
    Queue<Tracked> queue(3);

    queue.push(Tracked(10));
    queue.push(Tracked(20));

    REQUIRE(Tracked::s_alive == 2);

    {
      Tracked value = queue.pop();

      REQUIRE(value.m_value == 10);
      REQUIRE(Tracked::s_alive == 2);
    }

    REQUIRE(Tracked::s_alive == 1);
  }

  REQUIRE(Tracked::s_alive == 0);
  REQUIRE(Tracked::s_constructed == Tracked::s_destroyed);
}

TEST_CASE("Queue destroys wrapped elements", "[Queue]") {
  Tracked::reset();

  {
    Queue<Tracked> queue(3);

    queue.push(Tracked(10));
    queue.push(Tracked(20));
    queue.push(Tracked(30));

    REQUIRE(queue.pop().m_value == 10);
    REQUIRE(queue.pop().m_value == 20);

    queue.push(Tracked(40));
    queue.push(Tracked(50));

    REQUIRE(Tracked::s_alive == 3);
  }

  REQUIRE(Tracked::s_alive == 0);
  REQUIRE(Tracked::s_constructed == Tracked::s_destroyed);
}

TEST_CASE("Queue remains valid after failed push", "[Queue]") {
  Queue<ThrowOnCopy> queue(2);
  ThrowOnCopy value(42);

  ThrowOnCopy::s_shouldThrow = true;

  REQUIRE_THROWS_AS(queue.push(value), std::runtime_error);

  ThrowOnCopy::s_shouldThrow = false;

  queue.push(value);

  REQUIRE(queue.pop().m_value == 42);
  REQUIRE_THROWS_AS(queue.pop(), std::underflow_error);
}

TEST_CASE("Queue remains valid after failed pop", "[Queue]") {
  Queue<ThrowOnCopy> queue(2);

  queue.push(ThrowOnCopy(42));

  ThrowOnCopy::s_shouldThrow = true;

  REQUIRE_THROWS_AS(queue.pop(), std::runtime_error);

  ThrowOnCopy::s_shouldThrow = false;

  REQUIRE(queue.pop().m_value == 42);
  REQUIRE_THROWS_AS(queue.pop(), std::underflow_error);
}

TEST_CASE("Queue supports string values", "[Queue]") {
  Queue<std::string> queue(3);

  queue.push("Hello");
  queue.push("World");

  REQUIRE(queue.pop() == "Hello");

  queue.push("C++");

  REQUIRE(queue.pop() == "World");
  REQUIRE(queue.pop() == "C++");
}
