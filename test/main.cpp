#include <iostream>

#include "../kutils.hpp"

using namespace std;
using namespace io;

template <typename T>
struct A {
    int bar = 1;
    void foo() {
        print("DEFAULT FOOOOO");
    }
};

template<>
void A<int>::foo() {
    print("I'M A FOO!!!!");
}

int main(int argc, char *argv[]) {
    A<float> b;
    b.foo();
    print(b.bar);
    A<int> c;
    c.foo();
    return 0;
}
