#pragma once
#include <stdexcept>

struct AutoCompletionError : std::runtime_error {
  using std::runtime_error::runtime_error;
};
