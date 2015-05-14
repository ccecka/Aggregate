#include <iostream>
#include <typeinfo>
#include <complex>

#include "aggregate.hpp"

struct my_struct {
};

float& operator+=(float& a, const my_struct&) {
  return a *= 3;
}

std::string operator+(float a, const my_struct&) {
  return std::string("Hello") + std::to_string(a);
}


using agg::aggregate;


int main() {
  auto x = aggregate<float,3>{{}};
  std::cout << x << std::endl;

  x += 2.5;
  std::cout << x << std::endl;

  x = {{}};
  x += aggregate<int,3>{1,2,3};
  std::cout << x << std::endl;

  auto zi = x + 3;
  std::cout << zi << std::endl;

  auto zd = x + 3.14;
  std::cout << zd << std::endl;

  x += my_struct();
  std::cout << x << std::endl;

  ++x;
  std::cout << x << std::endl;

  x++;
  std::cout << x << std::endl;

  auto addy = &x;
  std::cout << addy << std::endl;

  auto zs = x + my_struct();
  std::cout << zs << std::endl;

  auto vvc = x + std::complex<float>{3,2};
  std::cout << vvc << std::endl;

  x = {1,2,3};
  auto vv = aggregate<aggregate<float,3>,3>{x,x,x};
  std::cout << vv << std::endl;

  auto xvv = x + vv;
  std::cout << xvv << std::endl;

  auto vvx = vv + x;
  std::cout << vvx << std::endl;

  auto vv2 = vv + vv;
  std::cout << vv2 << std::endl;

  return 0;
}
