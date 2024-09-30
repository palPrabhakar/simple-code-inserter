#pragma once

#include <type_traits>

namespace test {
class Other {
public:
  Other(int x) : m_x(x) {}

  template <typename T> void foo(T t) {
    if (std::is_same_v<T, int>)
      m_x += t;
  }

  int get_x() { return m_x; }

private:
  int m_x;
};
} // namespace test
