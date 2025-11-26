#include <doptions/doptions.hpp>
#include <iostream>
#include <stdexcept>
#include "doptions/option.hpp"

struct MyStruct {
  std::string wa;
  bool waz = false;
};

REGISTER_TYPE(MyStruct) {
  if (!(str.at(0) == '[') || !(str.at(str.size() - 1) == ']')) {
    throw std::runtime_error("Cannot parse " + str);
  }
  size_t comma = str.find(',');
  std::string wa = str.substr(1, comma);
  std::string waz = str.substr(comma + 1, 1);
  bool wazv = waz == "v";
  return {.wa = wa, .waz = wazv};
}

auto main() -> int {
  std::cout << "doptions library example\n\n";

  int number = 0;
  MyStruct stc = {};

  auto option = doptions::Option<int>::createOption("-n,--number", &number);
  auto option2 = doptions::Option<MyStruct>::createOption("-v,--veee", &stc);

  std::cout << "Created option for int: " << option->longName() << "\n";
  std::cout << "Created option for MyStruct: " << option2->longName() << "\n";

  return 0;
}
