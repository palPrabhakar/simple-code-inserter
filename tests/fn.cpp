#include "./fn.h"

namespace {
class Toy {
public:
  Toy() {}
  virtual void foo() {};
  void do_something(int y) {
    x = y;
    x = x * 2;
  }
  int get_x() { return x; }

private:
  int x;
};

class SpToy : public Toy {
  SpToy(int t) : m_t(t) {}

private:
  int m_t;
};

} // namespace

void foo(int &a, int &b) {
  if ((a + b % 2) == 0) {
    return;
  }

  a = a + b;
  b = b * b * 2;
}

int bar(int x) {
  if (x % 2 == 0)
    return x * 2;
  else
    return x * x;
}

int lol(int y) { return y + 4; }

int main() {
  auto b = bar(2);
  auto a = test::Other(2);
  a.get_x();
  a.foo(2);
  a.foo(2.0f);

  for (int i = 0; i < 2; ++i) {
    i = i + 2;
  }

  return 0;
}
