#include "matrix.h"
#include "test-helpers.h"

#include <gtest/gtest.h>

namespace ct::test {

class ViewsTest : public ::testing::Test {
protected:
  void SetUp() override {
    Element::reset_allocations();
  }
};

TEST_F(ViewsTest, row_view) {
  constexpr size_t ROWS = 40;
  constexpr size_t COLS = 100;
  constexpr size_t CHOSEN_ROW = 2;

  Matrix<Element> a(ROWS, COLS);
  fill(a);

  const Matrix<Element>::RowView row_view = a.row(CHOSEN_ROW);

  for (size_t i = 0; Element x : row_view) {
    EXPECT_EQ(a(CHOSEN_ROW, i), x);
    ++i;
  }

  for (size_t i = 0; const Element& x : row_view) {
    EXPECT_EQ(a.data() + COLS * CHOSEN_ROW + i, &x);
    EXPECT_EQ(a(CHOSEN_ROW, i), x);
    ++i;
  }

  for (size_t i = 0; Element & x : row_view) {
    EXPECT_EQ(a.data() + COLS * CHOSEN_ROW + i, &x);
    EXPECT_EQ(a(CHOSEN_ROW, i), x);
    x += 2;
    ++i;
  }

  for (size_t i = 0; Element x : a) {
    size_t row = i / COLS;
    size_t col = i % COLS;
    if (row == CHOSEN_ROW) {
      EXPECT_EQ(elem(row, col) + 2, x);
    } else {
      EXPECT_EQ(elem(row, col), x);
    }
    ++i;
  }
}

TEST_F(ViewsTest, row_view_multiply) {
  constexpr size_t ROWS = 40;
  constexpr size_t COLS = 100;
  constexpr size_t CHOSEN_ROW = 2;

  Matrix<Element> a(ROWS, COLS);
  fill(a);

  const Matrix<Element>::RowView row_view = a.row(CHOSEN_ROW);

  (row_view *= 5) *= 2;

  for (size_t i = 0; Element x : a) {
    size_t row = i / COLS;
    size_t col = i % COLS;
    if (row == CHOSEN_ROW) {
      EXPECT_EQ(elem(row, col) * 10, x);
    } else {
      EXPECT_EQ(elem(row, col), x);
    }
    ++i;
  }
}

TEST_F(ViewsTest, const_row_view) {
  constexpr size_t ROWS = 40;
  constexpr size_t COLS = 100;
  constexpr size_t CHOSEN_ROW = 2;

  Matrix<Element> a(ROWS, COLS);
  fill(a);

  Matrix<Element>::ConstRowView const_row_view = std::as_const(a).row(CHOSEN_ROW);

  for (size_t i = 0; Element x : const_row_view) {
    EXPECT_EQ(a(CHOSEN_ROW, i), x);
    ++i;
  }

  for (size_t i = 0; const Element& x : const_row_view) {
    EXPECT_EQ(a.data() + COLS * CHOSEN_ROW + i, &x);
    EXPECT_EQ(a(CHOSEN_ROW, i), x);
    ++i;
  }
}

TEST_F(ViewsTest, col_view) {
  constexpr size_t ROWS = 40;
  constexpr size_t COLS = 100;
  constexpr size_t CHOSEN_COL = 2;

  Matrix<Element> a(ROWS, COLS);
  fill(a);

  const Matrix<Element>::ColView col_view = a.col(CHOSEN_COL);

  for (size_t i = 0; Element x : col_view) {
    EXPECT_EQ(a(i, CHOSEN_COL), x);
    ++i;
  }

  for (size_t i = 0; const Element& x : col_view) {
    EXPECT_EQ(a.data() + CHOSEN_COL + COLS * i, &x);
    EXPECT_EQ(a(i, CHOSEN_COL), x);
    ++i;
  }

  for (size_t i = 0; Element & x : col_view) {
    EXPECT_EQ(a.data() + CHOSEN_COL + COLS * i, &x);
    EXPECT_EQ(a(i, CHOSEN_COL), x);
    x += 2;
    ++i;
  }

  for (size_t i = 0; Element x : a) {
    size_t row = i / COLS;
    size_t col = i % COLS;
    if (col == CHOSEN_COL) {
      EXPECT_EQ(elem(row, col) + 2, x);
    } else {
      EXPECT_EQ(elem(row, col), x);
    }
    ++i;
  }
}

TEST_F(ViewsTest, col_view_multiply) {
  constexpr size_t ROWS = 40;
  constexpr size_t COLS = 100;
  constexpr size_t CHOSEN_COL = 2;

  Matrix<Element> a(ROWS, COLS);
  fill(a);

  const Matrix<Element>::ColView col_view = a.col(CHOSEN_COL);

  (col_view *= 5) *= 2;

  for (size_t i = 0; Element x : a) {
    size_t row = i / COLS;
    size_t col = i % COLS;
    if (col == CHOSEN_COL) {
      EXPECT_EQ(elem(row, col) * 10, x);
    } else {
      EXPECT_EQ(elem(row, col), x);
    }
    ++i;
  }
}

TEST_F(ViewsTest, const_col_view) {
  constexpr size_t ROWS = 40;
  constexpr size_t COLS = 100;
  constexpr size_t CHOSEN_COL = 2;

  Matrix<Element> a(ROWS, COLS);
  fill(a);

  Matrix<Element>::ConstColView const_col_view = std::as_const(a).col(CHOSEN_COL);

  for (size_t i = 0; Element x : const_col_view) {
    EXPECT_EQ(a(i, CHOSEN_COL), x);
    ++i;
  }

  for (size_t i = 0; const Element& x : const_col_view) {
    EXPECT_EQ(a.data() + CHOSEN_COL + COLS * i, &x);
    EXPECT_EQ(a(i, CHOSEN_COL), x);
    ++i;
  }
}

} // namespace ct::test
