#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include "CppFromScratch/LinkedList.hpp"

TEST_CASE("Empty list has no nodes", "[LinkedList]") {
  LinkedList<int> list;
  REQUIRE(list.begin() == list.end());
}

TEST_CASE("Appending maintains forward and backward links", "[LinkedList]") {
  LinkedList<int> list;
  list.push_back(1);
  list.push_back(2);
  list.push_back(3);

  const auto *first = list.begin();
  REQUIRE(first != nullptr);
  REQUIRE(first->value == 1);
  REQUIRE(first->prev == nullptr);

  const auto *second = first->next;
  REQUIRE(second != nullptr);
  REQUIRE(second->value == 2);
  REQUIRE(second->prev == first);

  const auto *third = second->next;
  REQUIRE(third != nullptr);
  REQUIRE(third->value == 3);
  REQUIRE(third->prev == second);
  REQUIRE(third->next == nullptr);
}

TEST_CASE("Prepending maintains forward order", "[LinkedList]") {
  LinkedList<int> list;
  list.push_front(3);
  list.push_front(2);
  list.push_front(1);

  int expected = 1;
  for (const auto *node = list.begin(); node != list.end(); node = node->next)
    REQUIRE(node->value == expected++);
  REQUIRE(expected == 4);
}

TEST_CASE("Front and back insertions can be interleaved", "[LinkedList]") {
  LinkedList<int> list;
  list.push_back(2);
  list.push_front(1);
  list.push_back(3);
  list.push_front(0);

  const LinkedList<int> &readOnly = list;
  const auto *node = readOnly.begin();
  for (int expected = 0; expected < 4; ++expected) {
    REQUIRE(node != nullptr);
    REQUIRE(node->value == expected);
    node = node->next;
  }
  REQUIRE(node == readOnly.end());
}

TEST_CASE("Owning list cannot be copied", "[LinkedList]") {
  STATIC_REQUIRE_FALSE(std::is_copy_constructible_v<LinkedList<int>>);
  STATIC_REQUIRE_FALSE(std::is_copy_assignable_v<LinkedList<int>>);
}
