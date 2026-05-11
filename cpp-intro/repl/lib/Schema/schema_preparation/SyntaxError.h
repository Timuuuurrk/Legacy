#pragma once
#include <stdexcept>

struct SyntaxError : std::runtime_error {
  using std::runtime_error::runtime_error;
};
