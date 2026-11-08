#include <iostream>

#include "CppFromScratch/LinkedList.hpp"

int main() {
  LinkedList<int> list;
  list.push_back(1);
  list.push_back(2);
  list.push_front(0);

  for (const auto *node = list.begin(); node != list.end(); node = node->next)
    std::cout << node->value << '\n';
}
