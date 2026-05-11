#pragma once

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <numeric>

namespace ct {
template <typename T>
class Matrix {
  template <typename U>
  class BasicColIterator {
  public:
    template <typename U2>
    friend class BasicColIterator;

    template <typename U2>
    friend class Matrix;

    using iterator_category = std::random_access_iterator_tag;
    using iterator_concept = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using reference = U&;
    using pointer = U*;

    BasicColIterator() = default;

    ~BasicColIterator() = default;

    BasicColIterator(const BasicColIterator&) = default;

    BasicColIterator(BasicColIterator&&) = default;

    BasicColIterator& operator=(const BasicColIterator&) = default;

    BasicColIterator& operator=(BasicColIterator&&) = default;

    U& operator*() const {
      return *(cur_ + ind_ * cols_);
    }

    U& operator[](const std::size_t n) const {
      return *(*this + n);
    }

    U* operator->() {
      return (cur_ + ind_ * cols_);
    }

    operator BasicColIterator<const U>() const {
      return BasicColIterator<const U>(cur_, cols_, ind_);
    }

    BasicColIterator& operator++() {
      *this += 1;
      return *this;
    }

    BasicColIterator operator++(int) {
      BasicColIterator res(*this);
      *this += 1;
      return res;
    }

    BasicColIterator& operator--() {
      *this -= 1;
      return *this;
    }

    BasicColIterator operator--(int) {
      BasicColIterator res(*this);
      *this -= 1;
      return res;
    }

    BasicColIterator& operator+=(const difference_type n) {
      ind_ += n;
      return *this;
    }

    BasicColIterator& operator-=(const difference_type n) {
      *this += (-n);
      return *this;
    }

    template <typename U2>
    difference_type operator-(const BasicColIterator<U2>& other) const {
      return (ind_ - other.ind_);
    }

    template <typename U2>
    bool operator==(const BasicColIterator<U2>& other) const {
      return ind_ == other.ind_;
    }

    template <typename U2>
    bool operator!=(const BasicColIterator<U2>& other) const {
      return ind_ != other.ind_;
    }

    template <typename U2>
    bool operator<(const BasicColIterator<U2>& other) const {
      return ind_ < other.ind_;
    }

    template <typename U2>
    bool operator>(const BasicColIterator<U2>& other) const {
      return ind_ > other.ind_;
    }

    template <typename U2>
    bool operator<=(const BasicColIterator<U2>& other) const {
      return ind_ <= other.ind_;
    }

    template <typename U2>
    bool operator>=(const BasicColIterator<U2>& other) const {
      return ind_ >= other.ind_;
    }

    template <typename U2>
    bool operator==(const U2* other) const {
      return cur_ + ind_ == other;
    }

    friend BasicColIterator operator+(difference_type n, const BasicColIterator& our) {
      BasicColIterator ans(our);
      ans += n;
      return ans;
    }

    friend BasicColIterator operator+(const BasicColIterator& our, difference_type n) {
      BasicColIterator ans(our);
      ans += n;
      return ans;
    }

    friend BasicColIterator operator-(difference_type n, const BasicColIterator& our) {
      BasicColIterator ans(our);
      ans -= n;
      return ans;
    }

    friend BasicColIterator operator-(const BasicColIterator& our, difference_type n) {
      BasicColIterator ans(our);
      ans -= n;
      return ans;
    }

  private:
    BasicColIterator(U* from, const difference_type cols, const difference_type ind)
        : cur_(from)
        , cols_(cols)
        , ind_(ind) {}

    U* cur_;
    difference_type cols_;
    difference_type ind_;
  };

  template <typename U>
  class BaseRowView {
  public:
    template <typename U2>
    friend class BaseRowView;

    BaseRowView(U* from, const std::size_t cols)
        : cur_(from)
        , cols_(cols) {}

    U* begin() const {
      return cur_;
    }

    U* end() const {
      return cur_ + cols_;
    }

    const BaseRowView& operator*=(const U& scalar) const {
      for (std::size_t i = 0; i < cols_; ++i) {
        cur_[i] *= scalar;
      }
      return *this;
    }

  private:
    U* cur_;
    std::size_t cols_;
  };

  template <typename U>
  class BaseColView {
  public:
    template <typename U2>
    friend class BaseColView;

    BaseColView(U from, U end)
        : start_(from)
        , end_(end) {}

    U begin() const {
      return start_;
    }

    U end() const {
      return end_;
    }

    const BaseColView& operator*=(const T& scalar) const {
      for (U cur = begin(); cur != end(); ++cur) {
        *cur *= scalar;
      }
      return *this;
    }

  private:
    U start_;
    U end_;
  };

public:
  using ValueType = T;

  using Reference = T&;
  using ConstReference = const T&;

  using Pointer = T*;
  using ConstPointer = const T*;

  using Iterator = T*;
  using ConstIterator = const T*;

  using RowIterator = T*;
  using ConstRowIterator = const T*;

  using ColIterator = BasicColIterator<T>;
  using ConstColIterator = BasicColIterator<const T>;

  using RowView = BaseRowView<T>;
  using ConstRowView = BaseRowView<const T>;

  using ColView = BaseColView<ColIterator>;
  using ConstColView = BaseColView<ConstColIterator>;

  Matrix()
      : rows_(0)
      , cols_(0) {
    data_ = nullptr;
  }

  Matrix(size_t rows, size_t cols)
      : Matrix() {
    if (rows * cols > 0) {
      rows_ = rows;
      cols_ = cols;
      data_ = new T[rows * cols]();
    }
  }

  template <size_t ROWS, size_t COLS>
  Matrix(const T (&init)[ROWS][COLS])
      : rows_(ROWS)
      , cols_(COLS) {
    data_ = new T[rows_ * cols_];
    for (std::size_t i = 0; i < rows_; ++i) {
      for (std::size_t j = 0; j < cols_; ++j) {
        data_[i * cols_ + j] = init[i][j];
      }
    }
  }

  Matrix(const Matrix& other)
      : rows_(other.rows_)
      , cols_(other.cols_) {
    data_ = nullptr;
    if (rows_ * cols_ > 0) {
      data_ = new T[rows_ * cols_];
      std::copy_n(other.data_, rows_ * cols_, data_);
    }
  }

  Matrix& operator=(const Matrix& other) {
    if (this != &other) {
      Matrix tmp(other);
      swap(tmp);
    }
    return *this;
  }

  ~Matrix() {
    delete[] data_;
  }

  // Iterators

  Iterator begin() {
    return Iterator(data());
  }

  ConstIterator begin() const {
    return ConstIterator(data());
  }

  Iterator end() {
    return Iterator(data() + rows() * cols());
  }

  ConstIterator end() const {
    return ConstIterator(data() + rows() * cols());
  }

  RowIterator row_begin(size_t row) {
    return RowIterator(data() + row * cols());
  }

  ConstRowIterator row_begin(size_t row) const {
    return ConstRowIterator(data() + row * cols());
  }

  RowIterator row_end(size_t row) {
    return RowIterator(data() + row * cols() + cols());
  }

  ConstRowIterator row_end(size_t row) const {
    return ConstRowIterator(data() + row * cols() + cols());
  }

  ColIterator col_begin(size_t col) {
    return ColIterator(data() + col, cols(), 0);
  }

  // Iterators

  ConstColIterator col_begin(size_t col) const {
    return ConstColIterator(data() + col, cols(), 0);
  }

  ColIterator col_end(size_t col) {
    return ColIterator(data() + col, cols(), rows());
  }

  ConstColIterator col_end(size_t col) const {
    return ConstColIterator(data() + col, cols(), rows());
  }

  // Views

  RowView row(size_t row) {
    return RowView(data() + row * cols(), cols());
  }

  ConstRowView row(size_t row) const {
    return ConstRowView(data() + row * cols(), cols());
  }

  ColView col(size_t col) {
    return ColView(col_begin(col), col_end(col));
  }

  ConstColView col(size_t col) const {
    return ConstColView(col_begin(col), col_end(col));
  }

  // Size

  size_t rows() const {
    return rows_;
  }

  size_t cols() const {
    return cols_;
  }

  size_t size() const {
    return rows() * cols();
  }

  bool empty() const {
    return size() == 0;
  }

  // Elements access

  Reference operator()(size_t row, size_t col) {
    return data()[row * cols() + col];
  }

  ConstReference operator()(size_t row, size_t col) const {
    return data()[row * cols() + col];
  }

  Pointer data() {
    return data_;
  }

  ConstPointer data() const {
    return data_;
  }

  // Comparison

  friend bool operator==(const Matrix& left, const Matrix& right) {
    if (left.rows_ != right.rows_ || left.cols_ != right.cols_) {
      return false;
    }
    return std::equal(left.begin(), left.end(), right.begin(), right.end());
  }

  friend bool operator!=(const Matrix& left, const Matrix& right) {
    return !(left == right);
  }

  // Arithmetic operations

  Matrix& operator+=(const Matrix& other) {
    std::transform(begin(), end(), other.begin(), begin(), std::plus<>{});
    return *this;
  }

  Matrix& operator-=(const Matrix& other) {
    std::transform(begin(), end(), other.begin(), begin(), std::minus<>{});
    return *this;
  }

  Matrix& operator*=(const Matrix& other) {
    Matrix tmp((*this) * other);
    swap(tmp);
    return *this;
  }

  Matrix& operator*=(ConstReference factor) {
    for (Iterator cur = begin(); cur != end(); ++cur) {
      *cur *= factor;
    }
    return *this;
  }

  friend Matrix operator+(const Matrix& left, const Matrix& right) {
    Matrix ans = left;
    ans += right;
    return ans;
  }

  friend Matrix operator-(const Matrix& left, const Matrix& right) {
    Matrix ans = left;
    ans -= right;
    return ans;
  }

  friend Matrix operator*(const Matrix& left, const Matrix& right) {
    Matrix ans(left.rows_, right.cols_);
    for (std::size_t i = 0; i < left.rows_; ++i) {
      for (std::size_t j = 0; j < right.cols_; ++j) {
        ans(i, j) = std::inner_product(left.row_begin(i), left.row_end(i), right.col_begin(j), T());
      }
    }
    return ans;
  }

  friend Matrix operator*(const Matrix& left, ConstReference right) {
    Matrix ans = left;
    ans *= right;
    return ans;
  }

  friend Matrix operator*(ConstReference left, const Matrix& right) {
    Matrix ans = right;
    ans *= left;
    return ans;
  }

  void swap(Matrix& other) noexcept {
    using std::swap;
    swap(rows_, other.rows_);
    swap(cols_, other.cols_);
    swap(data_, other.data_);
  }

private:
  T* data_;
  std::size_t rows_;
  std::size_t cols_;
};
} // namespace ct
