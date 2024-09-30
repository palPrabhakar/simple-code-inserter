#include "./fn.h"

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

int lol(int y) { return y + 4; }

int main() {
  auto b = bar(2);
  auto a = test::Other(2);
  a.get_x();
  a.foo(2);
  a.foo(2.0f);
  
  for(int i = 0; i < 2; ++i) {
    i = i +2;
  }

  return 0;
}
