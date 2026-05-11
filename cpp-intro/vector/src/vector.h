#pragma once

#include <algorithm>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace ct {

template <typename T>
class Vector {
  static_assert(std::is_nothrow_move_constructible_v<T> || std::is_copy_constructible_v<T>);

  T* data_;
  std::size_t size_;
  std::size_t capacity_;

public:
  using ValueType = T;

  using Reference = T&;
  using ConstReference = const T&;

  using Pointer = T*;
  using ConstPointer = const T*;

  using Iterator = Pointer;
  using ConstIterator = ConstPointer;

public:
  // O(1) nothrow
  Vector() noexcept
      : data_(nullptr)
      , size_(0)
      , capacity_(0) {}

private:
  explicit Vector(std::size_t new_cap)
      : data_(static_cast<T*>(operator new(new_cap * sizeof(T), static_cast<std::align_val_t>(alignof(T)))))
      , size_(0)
      , capacity_(new_cap) {}

  void destroy() {
    for (std::size_t k = size(); k > 0; --k) {
      data()[k - 1].~T();
    }
    operator delete(data(), static_cast<std::align_val_t>(alignof(T)));
  }

  template <typename U>
  void get_push_back(U&& value) {
    if (size_ == capacity_) {
      capacity_ = capacity() > 0 ? capacity() * 2 : 1;
      Vector new_data(capacity());
      std::size_t kol = 0;
      new (new_data.data() + size()) T(std::forward<U>(value));
      try {
        for (; kol < size(); ++kol) {
          new (new_data.data() + kol) T(std::move_if_noexcept(data()[kol]));
          ++new_data.size_;
        }
      } catch (...) {
        new_data[size()].~T();
        throw;
      }
      swap(new_data);
    } else {
      new (data() + size()) T(std::forward<U>(value));
    }
    ++size_;
  }

  template <typename U>
  Iterator get_insert(ConstIterator pos, U&& value) {
    std::size_t ind = pos - begin();
    push_back(std::forward<U>(value));
    for (std::size_t i = size() - 1; i > ind; --i) {
      std::swap(data_[i], data_[i - 1]);
    }
    return begin() + ind;
  }

public:
  // O(N) strong
  Vector(const Vector& other)
      : data_(
            other.size() > 0
                ? static_cast<T*>(operator new(other.size_ * sizeof(T), static_cast<std::align_val_t>(alignof(T))))
                : nullptr
        )
      , size_(0)
      , capacity_(other.size()) {
    std::size_t i = 0;
    if (other.data_ != nullptr) {
      try {
        for (; i < other.size(); i++) {
          new (data() + i) T(other.data_[i]);
          ++size_;
        }
      } catch (...) {
        destroy();
        throw;
      }
    }
  }

  // O(1) strong
  Vector(Vector&& other)
      : data_(other.data())
      , size_(other.size())
      , capacity_(other.capacity()) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
  }

  // O(N) strong
  Vector& operator=(const Vector& other) {
    if (this != &other) {
      Vector tmp(other);
      swap(tmp);
    }
    return *this;
  }

  // O(1) strong
  Vector& operator=(Vector&& other) {
    Vector tmp(std::move(other));
    swap(tmp);
    return *this;
  }

  // O(N) nothrow
  ~Vector() noexcept {
    if (data() != nullptr) {
      for (std::size_t i = size(); i > 0; --i) {
        data()[i - 1].~T();
      }
    }
    operator delete(data(), static_cast<std::align_val_t>(alignof(T)));
  }

  // O(1) nothrow
  Reference operator[](size_t index) {
    return data()[index];
  }

  // O(1) nothrow
  ConstReference operator[](size_t index) const {
    return data()[index];
  }

  // O(1) nothrow
  Pointer data() noexcept {
    return data_;
  }

  // O(1) nothrow
  ConstPointer data() const noexcept {
    return data_;
  }

  // O(1) nothrow
  size_t size() const noexcept {
    return size_;
  }

  // O(1) nothrow
  Reference front() {
    return data()[0];
  }

  // O(1) nothrow
  ConstReference front() const {
    return data()[0];
  }

  // O(1) nothrow
  Reference back() {
    return data()[size() - 1];
  }

  // O(1) nothrow
  ConstReference back() const {
    return data()[size() - 1];
  }

  // O(1)* strong
  void push_back(const T& value) {
    get_push_back(value);
  }

  // O(1)* strong if move nothrow
  void push_back(T&& value) {
    get_push_back(std::move(value));
  }

  // O(1) nothrow
  void pop_back() {
    data()[size() - 1].~T();
    --size_;
  }

  // O(1) nothrow
  bool empty() const noexcept {
    return size() == 0;
  }

  // O(1) nothrow
  size_t capacity() const noexcept {
    return capacity_;
  }

  // O(N) strong
  void reserve(const std::size_t new_capacity) {
    if (new_capacity > capacity_) {
      capacity_ = new_capacity;
      Vector new_data_(capacity_);
      std::size_t i = 0;
      for (; i < size_; ++i) {
        new (new_data_.data() + i) T(std::move_if_noexcept(data_[i]));
        ++new_data_.size_;
      }
      destroy();
      data_ = new_data_.data_;
      new_data_.data_ = nullptr;
    }
  }

  // O(N) strong
  void shrink_to_fit() {
    if (size() == capacity()) {
      return;
    }
    if (size() == 0) {
      operator delete(data(), static_cast<std::align_val_t>(alignof(T)));
      data_ = nullptr;
      capacity_ = 1;
      return;
    }
    capacity_ = 0;
    reserve(size());
  }

  // O(N) nothrow
  void clear() noexcept {
    if (data()) {
      for (std::size_t i = size(); i > 0; --i) {
        data()[i - 1].~T();
      }
      size_ = 0;
    }
  }

  // O(1) nothrow
  void swap(Vector& other) noexcept {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
  }

  // O(1) nothrow
  Iterator begin() noexcept {
    return data();
  }

  // O(1) nothrow
  Iterator end() noexcept {
    return data() + size();
  }

  // O(1) nothrow
  ConstIterator begin() const noexcept {
    return data();
  }

  // O(1) nothrow
  ConstIterator end() const noexcept {
    return data() + size();
  }

  // O(N) strong
  Iterator insert(ConstIterator pos, const T& value) {
    return get_insert(pos, value);
  }

  // O(N) strong if move nothrow
  Iterator insert(ConstIterator pos, T&& value) {
    return get_insert(pos, std::move(value));
  }

  // O(N) nothrow(swap)
  Iterator erase(ConstIterator pos) {
    return erase(pos, pos + 1);
  }

  // O(N) nothrow(swap)
  Iterator erase(ConstIterator first, ConstIterator last) {
    const std::size_t len = last - first;
    std::size_t ind = first - begin();
    for (std::size_t i = ind; i < size() - len; ++i) {
      std::swap(data_[i], data_[i + len]);
    }
    for (std::size_t i = 0; i < len; ++i) {
      pop_back();
    }
    return begin() + ind;
  }
};

} // namespace ct
