#pragma once

template <typename T>
class LinkedList {
public:
  LinkedList() : m_first(nullptr), m_last(nullptr) {}
  ~LinkedList() { clear(); }

  LinkedList(const LinkedList &) = delete;
  LinkedList &operator=(const LinkedList &) = delete;

  struct Node {
    explicit Node(T input) : value(input), prev(nullptr), next(nullptr) {}

    T value;
    Node *prev;
    Node *next;
  };

  Node *begin() { return m_first; }
  Node *end() { return nullptr; }
  const Node *begin() const { return m_first; }
  const Node *end() const { return nullptr; }

  void push_front(T value) { insert(begin(), value); }
  void push_back(T value) { insert(end(), value); }

private:
  void insert(Node *where, T value) {
    Node *newNode = new Node(value);
    newNode->prev = where ? where->prev : m_last;
    newNode->next = where;

    if (newNode->prev)
      newNode->prev->next = newNode;
    if (where)
      where->prev = newNode;
    if (where == m_first)
      m_first = newNode;
    if (!where)
      m_last = newNode;
  }

  void erase(Node *where) {
    Node *prev = where->prev;
    Node *next = where->next;

    if (prev)
      prev->next = next;
    if (next)
      next->prev = prev;
    if (where == m_first)
      m_first = next;
    if (where == m_last)
      m_last = prev;

    delete where;
  }

  void clear() {
    while (m_first)
      erase(m_first);
  }

  Node *m_first;
  Node *m_last;
};
