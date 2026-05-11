#pragma once
#include "constraints.h"

#include <cstddef>
using namespace my_constraints;

namespace ct {

template <typename U>
class BitsetView;

template <typename T>
class BitsetReference {
  T* ref;
  std::size_t ind;

public:
  friend class BitsetView<T>;

  BitsetReference(T* data_, const std::size_t ind_)
      : ref(data_ + ind_ / sz_type)
      , ind(ind_ % sz_type) {}

  BitsetReference(const BitsetReference& other) = default;

  BitsetReference& operator=(const BitsetReference& other) = default;

  ~BitsetReference() = default;

  BitsetReference& operator=(const bool other) {
    if (!other) {
      *ref &= ~(static_cast<T>(1) << ind);
    } else {
      *ref |= (static_cast<T>(1) << ind);
    }
    return *this;
  }

  operator BitsetReference<const T>() const {
    return BitsetReference<const T>(ref, ind);
  }

  operator bool() const {
    return (*ref >> ind & static_cast<T>(1)) != 0u;
  }

  void operator&() = delete;

  BitsetReference& flip() {
    *ref ^= (static_cast<T>(1) << ind);
    return *this;
  }
};

} // namespace ct
