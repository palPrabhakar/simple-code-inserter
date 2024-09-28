#include "./fn.h"
#include <iostream>

class Toy {
  void do_something(int y) {
    x = y;
    x = x * 2;
  }
  int get_x() { return x; }

private:
  int x;
};
void foo(int a, int b) { int x = a + b; }

int bar(int x) { return x * 2; }

int main() {
  auto b = bar(2);
  auto o = testing::Other(4);
  o.foo(0.0f);
  o.foo(6);
  std::cout << o.get_x() << std::endl;

  return 0;
}
