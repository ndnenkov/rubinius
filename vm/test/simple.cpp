#include "vm.hpp"
#include "state.hpp"

int main() {
  rubinius::VM vm;
  vm.collect();
  return 0;
}
