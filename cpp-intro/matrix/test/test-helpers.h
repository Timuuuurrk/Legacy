#pragma once

#include "matrix.h"

#include <gtest/gtest.h>

namespace ct::test {

struct Element {
  void* operator new[](size_t count) {
    allocations += count / sizeof(Element);
    return ::operator new[](count);
  }

  Element() = default;

  Element(size_t value)
      : value(value) {}

  friend Element operator+(const Element& left, const Element& right) {
    return {left.value + right.value};
  }

  friend Element operator-(const Element& left, const Element& right) {
    return {left.value - right.value};
  }

  friend Element operator*(const Element& left, const Element& right) {
    return {left.value * right.value};
  }

  Element& operator+=(const Element& other) {
    value += other.value;
    return *this;
  }

  Element& operator-=(const Element& other) {
    value -= other.value;
    return *this;
  }

  Element& operator*=(const Element& other) {
    value *= other.value;
    return *this;
  }

  friend bool operator==(const Element&, const Element&) = default;

  friend std::ostream& operator<<(std::ostream& out, const Element& e) {
    return out << e.value;
  }

public:
  inline static size_t allocations;

  static void reset_allocations() {
    allocations = 0;
  }

public:
  size_t value;
};

inline bool operator==(Matrix<Element>::ConstIterator it, Matrix<Element>::ConstColIterator col_it) {
  return it == col_it.operator->();
}

inline void expect_allocations(size_t expected_allocations) {
  EXPECT_GE(expected_allocations, Element::allocations);
}

inline size_t elem(size_t i, size_t j) {
  return i * 101 + j * 7;
}

template <typename T>
void fill(Matrix<T>& a) {
  for (size_t i = 0; i < a.rows(); ++i) {
    for (size_t j = 0; j < a.cols(); ++j) {
      a(i, j) = elem(i, j);
    }
  }
}

template <typename T>
void expect_empty(const Matrix<T>& m) {
  EXPECT_EQ(0, m.rows());
  EXPECT_EQ(0, m.cols());
  EXPECT_EQ(0, m.size());
  EXPECT_TRUE(m.empty());
  EXPECT_EQ(nullptr, m.data());
}

template <typename T>
void expect_equal(const Matrix<T>& expected, const Matrix<T>& actual) {
  EXPECT_EQ(expected.rows(), actual.rows());
  EXPECT_EQ(expected.cols(), actual.cols());
  EXPECT_EQ(expected.size(), actual.size());
  EXPECT_EQ(expected.empty(), actual.empty());
  EXPECT_EQ(expected.data() == nullptr, actual.data() == nullptr);

  for (size_t i = 0; i < expected.rows(); ++i) {
    for (size_t j = 0; j < expected.cols(); ++j) {
      EXPECT_EQ(expected(i, j), actual(i, j)) << "  where i = " << i << ", j = " << j;
    }
  }
}

} // namespace ct::test
