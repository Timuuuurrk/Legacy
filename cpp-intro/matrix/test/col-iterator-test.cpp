#include "matrix.h"
#include "test-helpers.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <utility>

namespace ct::test {

class ColIteratorTest : public ::testing::Test {
protected:
  using EMatrix = Matrix<Element>;

  EMatrix m = EMatrix({
      {1, 2, 3},
      {4, 5, 6},
      {11, 12, 13},
      {14, 15, 16},
  });
};

namespace {

template <typename It>
It linear_advance(It it, std::iter_difference_t<It> n) {
  while (n > 0) {
    --n;
    ++it;
  }
  while (n < 0) {
    ++n;
    --it;
  }
  return it;
}

std::ptrdiff_t as_diff(std::size_t n) {
  return static_cast<std::ptrdiff_t>(n);
}

} // namespace

TEST_F(ColIteratorTest, non_const_to_const) {
  EMatrix::ColIterator begin = m.col_begin(1);
  EMatrix::ConstColIterator cbegin = begin;
  EXPECT_EQ(std::as_const(m).col_begin(1), cbegin);
  EXPECT_EQ(begin, cbegin);

  EMatrix::ColIterator end = m.col_end(1);
  EMatrix::ConstColIterator cend = end;
  EXPECT_EQ(std::as_const(m).col_end(1), cend);
  EXPECT_EQ(end, cend);
}

TEST_F(ColIteratorTest, copy_ctor) {
  EMatrix::ColIterator it1 = m.col_begin(1);
  EMatrix::ColIterator it2 = it1;
  EXPECT_EQ(it1, it2);

  ++it2;
  EXPECT_NE(it1, it2);
}

TEST_F(ColIteratorTest, default_construct) {
  EMatrix::ColIterator it;
  it = m.col_begin(1);
  EMatrix::ConstColIterator cit;
  cit = m.col_begin(1);
  EXPECT_EQ(it, cit);
}

TEST_F(ColIteratorTest, indirection) {
  auto it = m.col_begin(1);
  auto end = std::as_const(m).col_end(1);

  for (size_t i = 0; i < m.rows(); ++i) {
    ASSERT_NE(it, end);
    EXPECT_EQ(m(i, 1), *it);
    *it = 42;
    ++it;
  }
  EXPECT_EQ(it, end);

  std::for_each(m.col_begin(1), m.col_end(1), [](Element x) { EXPECT_EQ(42, x); });
}

TEST_F(ColIteratorTest, member_access) {
  auto it = m.col_begin(1);
  auto end = std::as_const(m).col_end(1);

  for (size_t i = 0; i < m.rows(); ++i) {
    ASSERT_NE(it, end);
    EXPECT_EQ(m(i, 1).value, it->value);
    it->value = 42;
    ++it;
  }
  EXPECT_EQ(it, end);

  std::for_each(m.col_begin(1), m.col_end(1), [](Element x) { EXPECT_EQ(42, x); });
}

TEST_F(ColIteratorTest, increment) {
  auto it = std::as_const(m).col_begin(1);

  EXPECT_EQ(&m(1, 1), ++it);
  EXPECT_EQ(&m(1, 1), it);

  EXPECT_EQ(&m(1, 1), it++);
  EXPECT_EQ(&m(2, 1), it);
}

TEST_F(ColIteratorTest, decrement) {
  auto it = std::as_const(m).col_end(1);

  EXPECT_EQ(&m(3, 1), --it);
  EXPECT_EQ(&m(3, 1), it);

  EXPECT_EQ(&m(3, 1), it--);
  EXPECT_EQ(&m(2, 1), it);
}

TEST_F(ColIteratorTest, add_diff) {
  auto it = std::as_const(m).col_begin(1);

  EXPECT_EQ(m.col_end(1), it + m.rows());

  for (std::ptrdiff_t i = 0; i <= as_diff(m.rows()); ++i) {
    EXPECT_EQ(linear_advance(it, i), it + i);
    EXPECT_EQ(linear_advance(it, i), i + it);

    auto it2 = it;
    it2 += i;
    EXPECT_EQ(linear_advance(it, i), it2);
  }
}

TEST_F(ColIteratorTest, sub_diff) {
  auto it = std::as_const(m).col_end(1);

  EXPECT_EQ(m.col_begin(1), it - m.rows());

  for (std::ptrdiff_t i = 0; i <= as_diff(m.rows()); ++i) {
    EXPECT_EQ(linear_advance(it, -i), it - i);

    auto it2 = it;
    it2 -= i;
    EXPECT_EQ(linear_advance(it, -i), it2);
  }
}

TEST_F(ColIteratorTest, add_neg_diff) {
  auto it = std::as_const(m).col_end(1);

  EXPECT_EQ(m.col_begin(1), it + -as_diff(m.rows()));

  for (std::ptrdiff_t i = 0; i <= as_diff(m.rows()); ++i) {
    EXPECT_EQ(linear_advance(it, -i), it + -i);
    EXPECT_EQ(linear_advance(it, -i), -i + it);

    auto it2 = it;
    it2 += -i;
    EXPECT_EQ(linear_advance(it, -i), it2);
  }
}

TEST_F(ColIteratorTest, sub_neg_diff) {
  auto it = std::as_const(m).col_begin(1);

  EXPECT_EQ(m.col_end(1), it - -as_diff(m.rows()));

  for (std::ptrdiff_t i = 0; i <= as_diff(m.rows()); ++i) {
    EXPECT_EQ(linear_advance(it, i), it - -i);

    auto it2 = it;
    it2 -= -i;
    EXPECT_EQ(linear_advance(it, i), it2);
  }
}

TEST_F(ColIteratorTest, iter_diff) {
  auto it = std::as_const(m).col_begin(1);
  auto end = m.col_end(1);

  EXPECT_EQ(0, it - it);
  EXPECT_EQ(0, end - end);
  EXPECT_EQ(m.rows(), end - it);
  EXPECT_EQ(-as_diff(m.rows()), it - end);

  ++it;
  --end;

  EXPECT_EQ(m.rows() - 2, end - it);
  EXPECT_EQ(-as_diff(m.rows() - 2), it - end);
}

TEST_F(ColIteratorTest, subscript) {
  auto it = std::as_const(m).col_begin(1);

  EXPECT_EQ(m(0, 1), it[0]);
  EXPECT_EQ(m(1, 1), it[1]);

  EXPECT_EQ(&m(0, 1), &it[0]);
  EXPECT_EQ(&m(1, 1), &it[1]);

  ++it;

  EXPECT_EQ(m(0, 1), it[-1]);
  EXPECT_EQ(m(1, 1), it[0]);
  EXPECT_EQ(m(2, 1), it[1]);

  EXPECT_EQ(&m(0, 1), &it[-1]);
  EXPECT_EQ(&m(1, 1), &it[0]);
  EXPECT_EQ(&m(2, 1), &it[1]);
}

TEST_F(ColIteratorTest, compare) {
  auto it1 = std::as_const(m).col_begin(1);
  auto it2 = it1;

  EXPECT_EQ(it1, it2);
  EXPECT_LE(it1, it2);
  EXPECT_GE(it1, it2);

  EXPECT_FALSE(it1 < it2);
  EXPECT_FALSE(it1 > it2);

  ++it2;
  EXPECT_NE(it1, it2);
  EXPECT_LE(it1, it2);
  EXPECT_LT(it1, it2);
  EXPECT_GE(it2, it1);
  EXPECT_GT(it2, it1);

  auto end = m.col_end(1);

  EXPECT_EQ(end, end);
  EXPECT_LE(end, end);
  EXPECT_GE(end, end);

  EXPECT_FALSE(end < end);
  EXPECT_FALSE(end > end);

  EXPECT_NE(it2, end);
  EXPECT_LT(it2, end);
  EXPECT_GT(end, it2);
}

} // namespace ct::test
