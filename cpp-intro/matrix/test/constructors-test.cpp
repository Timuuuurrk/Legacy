#include "matrix.h"
#include "test-helpers.h"

#include <gtest/gtest.h>

namespace ct::test {

class ConstructorsTest : public ::testing::Test {
protected:
  void SetUp() override {
    Element::reset_allocations();
  }
};

TEST_F(ConstructorsTest, default_ctor) {
  Matrix<Element> a;

  expect_empty(a);
  expect_allocations(0);
}

TEST_F(ConstructorsTest, zeros_ctor) {
  constexpr size_t ROWS = 40;
  constexpr size_t COLS = 100;
  constexpr size_t SIZE = ROWS * COLS;

  Matrix<Element> a(ROWS, COLS);

  EXPECT_EQ(ROWS, a.rows());
  EXPECT_EQ(COLS, a.cols());
  EXPECT_EQ(SIZE, a.size());
  EXPECT_FALSE(a.empty());
  EXPECT_NE(nullptr, a.data());

  for (size_t i = 0; i < ROWS; ++i) {
    for (size_t j = 0; j < COLS; ++j) {
      EXPECT_EQ(0, a(i, j));
    }
  }

  expect_allocations(SIZE);
}

TEST_F(ConstructorsTest, zeros_ctor_empty) {
  Matrix<Element> a(0, 0);

  expect_empty(a);
  expect_allocations(0);
}

TEST_F(ConstructorsTest, zeros_ctor_weird) {
  Matrix<Element> a(10, 0);
  expect_empty(a);

  Matrix<Element> b(0, 10);
  expect_empty(b);

  expect_allocations(0);
}

TEST_F(ConstructorsTest, init_ctor) {
  Matrix<Element> a({
      {10, 20, 30},
      {40, 50, 60},
  });

  EXPECT_EQ(2, a.rows());
  EXPECT_EQ(3, a.cols());
  EXPECT_EQ(6, a.size());
  EXPECT_FALSE(a.empty());
  EXPECT_NE(nullptr, a.data());

  EXPECT_EQ(10, a(0, 0));
  EXPECT_EQ(20, a(0, 1));
  EXPECT_EQ(30, a(0, 2));
  EXPECT_EQ(40, a(1, 0));
  EXPECT_EQ(50, a(1, 1));
  EXPECT_EQ(60, a(1, 2));

  expect_allocations(6);
}

TEST_F(ConstructorsTest, copy_ctor) {
  constexpr size_t ROWS = 40;
  constexpr size_t COLS = 100;
  constexpr size_t SIZE = ROWS * COLS;

  Matrix<Element> a(ROWS, COLS);

  expect_allocations(SIZE);

  fill(a);

  expect_allocations(SIZE);

  Matrix<Element> b = a;

  expect_equal(a, b);

  expect_allocations(SIZE * 2);
}

TEST_F(ConstructorsTest, copy_ctor_empty) {
  Matrix<Element> a;
  Matrix<Element> b = a;

  expect_empty(a);
  expect_empty(b);

  expect_allocations(0);
}

TEST_F(ConstructorsTest, copy_assignment) {
  constexpr size_t ROWS_A = 40;
  constexpr size_t COLS_A = 100;
  constexpr size_t SIZE_A = ROWS_A * COLS_A;

  constexpr size_t ROWS_B = 15;
  constexpr size_t COLS_B = 15;
  constexpr size_t SIZE_B = ROWS_B * COLS_B;

  Matrix<Element> b(ROWS_B, COLS_B);

  {
    Matrix<Element> a(ROWS_A, COLS_A);

    expect_allocations(SIZE_A + SIZE_B);

    fill(a);

    expect_allocations(SIZE_A + SIZE_B);

    b = a;

    expect_equal(a, b);

    expect_allocations(SIZE_A * 2 + SIZE_B);
  }

  for (size_t i = 0; i < ROWS_A; ++i) {
    for (size_t j = 0; j < COLS_A; ++j) {
      EXPECT_EQ(elem(i, j), b(i, j));
    }
  }
}

TEST_F(ConstructorsTest, self_copy_assignment) {
  constexpr size_t ROWS = 40;
  constexpr size_t COLS = 100;
  constexpr size_t SIZE = ROWS * COLS;

  Matrix<Element> a(ROWS, COLS);

  expect_allocations(SIZE);

  fill(a);

  expect_allocations(SIZE);

  a = a;

  expect_allocations(SIZE);

  for (size_t i = 0; i < ROWS; ++i) {
    for (size_t j = 0; j < COLS; ++j) {
      EXPECT_EQ(elem(i, j), a(i, j));
    }
  }
}

TEST_F(ConstructorsTest, copy_assignment_empty) {
  constexpr size_t ROWS_B = 40;
  constexpr size_t COLS_B = 100;
  constexpr size_t SIZE_B = ROWS_B * COLS_B;

  Matrix<Element> a;
  Matrix<Element> b(ROWS_B, COLS_B);
  b = a;

  expect_empty(a);
  expect_empty(b);

  expect_allocations(SIZE_B);
}

} // namespace ct::test
