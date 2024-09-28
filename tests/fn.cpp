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

int bad(int y) { return y+4; }

int main() {
  auto b = bar(2);
  return 0;
}
