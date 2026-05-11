#pragma once
#include <algorithm>
#include <cstddef>
#include <memory>
#include <type_traits>

namespace ct {

template <typename T, std::size_t SMALL_SIZE>
class SocowVector {
  static_assert(std::is_copy_constructible_v<T>, "T must have a copy constructor");
  static_assert(std::is_nothrow_move_constructible_v<T>, "T must have a non-throwing move constructor");
  static_assert(std::is_copy_assignable_v<T>, "T must have a copy assignment operator");
  static_assert(std::is_nothrow_move_assignable_v<T>, "T must have a non-throwing move assignment operator");
  static_assert(std::is_nothrow_swappable_v<T>, "T must have a non-throwing swap");

  static_assert(SMALL_SIZE > 0, "SMALL_SIZE must be positive");

public:
  using ValueType = T;
  using Reference = T&;
  using ConstReference = const T&;
  using Pointer = T*;
  using ConstPointer = const T*;
  using Iterator = T*;
  using ConstIterator = const T*;

  SocowVector()
      : size_(0)
      , is_big(false)
      , dynamic_storage_(nullptr) {}

  SocowVector(const SocowVector& other)
      : size_(other.size_)
      , is_big(other.is_big) {
    if (other.is_big) {
      dynamic_storage_ = other.dynamic_storage_;
      ++dynamic_storage_->ref_count_;
    } else {
      std::uninitialized_copy(other.get_data(), other.get_data() + other.size_, static_buff);
    }
  }

  SocowVector(SocowVector&& other) noexcept
      : size_(other.size_)
      , is_big(other.is_big) {
    if (other.is_big) {
      dynamic_storage_ = other.dynamic_storage_;
      other.is_big = false;
    } else {
      std::uninitialized_move(other.get_data(), other.get_data() + size(), get_data());
      std::destroy_n(other.get_data(), other.size());
    }
    other.size_ = 0;
  }

private:
  bool is_only() {
    return !is_big || dynamic_storage_->ref_count_ == 1;
  }

  SocowVector(std::size_t capacity)
      : size_(0)
      , is_big(capacity > SMALL_SIZE) {
    if (capacity > SMALL_SIZE) {
      dynamic_storage_ = alloc(capacity);
    }
  }

  template <typename U>
  void get_push_back(U&& value) {
    if (size() != get_capacity() && is_only()) {
      new (get_data() + size()) T(std::forward<U>(value));
      ++size_;
      return;
    }
    std::size_t new_cap = capacity();
    if (size() == get_capacity()) {
      new_cap = capacity() * 2;
    }
    SocowVector tmp(new_cap);
    if (is_only()) {
      new (tmp.get_data() + size()) T(std::forward<U>(value));
      std::uninitialized_move_n(get_data(), size(), tmp.get_data());
    } else {
      std::uninitialized_copy_n(get_data(), size(), tmp.get_data());
      new (tmp.get_data() + size()) T(std::forward<U>(value));
    }
    tmp.size_ = size_ + 1;
    swap(tmp);
    is_big = true;
  }

  template <typename U>
  Iterator get_insert(const ConstIterator& pos, U&& val) {
    std::size_t ind = pos - get_data();
    push_back(std::forward<U>(val));
    std::rotate(get_data() + ind, get_data() + size() - 1, get_data() + size());
    return get_data() + ind;
  }

public:
  ~SocowVector() {
    if (is_big) {
      --dynamic_storage_->ref_count_;
      if (dynamic_storage_->ref_count_ == 0) {
        std::destroy_n(get_data(), size());
        operator delete(dynamic_storage_, static_cast<std::align_val_t>(alignof(DynamicStorage)));
      }
    } else {
      std::destroy_n(get_data(), size());
    }
  }

  SocowVector& operator=(const SocowVector& other) {
    if (this != &other) {
      SocowVector tmp(other);
      clear();
      swap(tmp);
    }
    return *this;
  }

  SocowVector& operator=(SocowVector&& other) noexcept {
    if (this != &other) {
      clear();
      swap(other);
    }
    return *this;
  }

  void swap(SocowVector& other) noexcept {
    if (this == &other) {
      return;
    }
    if (!is_big && !other.is_big) {
      std::size_t mn = std::min(size(), other.size());
      std::swap_ranges(get_data(), get_data() + mn, other.get_data());

      if (size() > other.size_) {
        std::uninitialized_move_n(get_data() + mn, size() - mn, other.get_data() + mn);
        std::destroy_n(get_data() + mn, size() - mn);
      } else {
        std::uninitialized_move_n(other.get_data() + mn, other.size() - mn, get_data() + mn);
        std::destroy_n(other.get_data() + mn, other.size() - mn);
      }
    } else if (!is_big && other.is_big) {
      DynamicStorage* tmp = other.dynamic_storage_;
      other.dynamic_storage_ = nullptr;
      std::uninitialized_move(get_data(), get_data() + size(), other.static_buff);
      std::destroy_n(get_data(), size());
      other.is_big = false;
      dynamic_storage_ = tmp;
      is_big = true;
    } else if (is_big && !other.is_big) {
      other.swap(*this);
      return;
    } else {
      std::swap(dynamic_storage_, other.dynamic_storage_);
    }
    std::swap(size_, other.size_);
  }

  std::size_t size() const noexcept {
    return size_;
  }

  std::size_t capacity() const noexcept {
    return is_big ? dynamic_storage_->capacity_ : SMALL_SIZE;
  }

  bool empty() const noexcept {
    return size() == 0;
  }

  T& operator[](const std::size_t& i) {
    if (is_big) {
      unshare();
    }
    return get_data()[i];
  }

  const T& operator[](const std::size_t& i) const noexcept {
    return get_data()[i];
  }

  T& front() {
    return data()[0];
  }

  const T& front() const noexcept {
    return data()[0];
  }

  T& back() {
    return data()[size() - 1];
  }

  const T& back() const noexcept {
    return data()[size() - 1];
  }

  T* data() {
    unshare();
    return get_data();
  }

  const T* data() const noexcept {
    return get_data();
  }

  T* begin() {
    return data();
  }

  const T* begin() const noexcept {
    return data();
  }

  T* end() {
    return data() + size();
  }

  const T* end() const noexcept {
    return data() + size();
  }

  void push_back(const T& value) {
    get_push_back(value);
  }

  void push_back(T&& value) {
    get_push_back(std::move(value));
  }

  void pop_back() {
    erase(get_data() + size() - 1, get_data() + size());
  }

  void reserve(const std::size_t new_capacity_) {
    std::size_t new_cap = new_capacity_;
    if (new_capacity_ <= get_capacity()) {
      if (new_capacity_ <= SMALL_SIZE && get_capacity() > SMALL_SIZE && size() <= SMALL_SIZE) {
        new_cap = SMALL_SIZE;
      } else {
        return;
      }
    }
    SocowVector tmp(new_cap);
    if (is_only()) {
      std::uninitialized_move(get_data(), get_data() + size(), tmp.get_data());
    } else {
      std::uninitialized_copy(get_data(), get_data() + size(), tmp.get_data());
    }
    tmp.size_ = size();
    swap(tmp);
    is_big = new_cap > SMALL_SIZE;
  }

  void shrink_to_fit() {
    if (!is_big || size_ == get_capacity()) {
      return;
    }
    std::size_t new_cap = size();
    if (size() <= SMALL_SIZE) {
      new_cap = SMALL_SIZE;
    }
    SocowVector tmp(new_cap);
    if (dynamic_storage_->ref_count_ == 1) {
      std::uninitialized_move(get_data(), get_data() + size(), tmp.get_data());
    } else {
      std::uninitialized_copy(get_data(), get_data() + size(), tmp.get_data());
    }
    tmp.size_ = size_;
    swap(tmp);
    is_big = size_ > SMALL_SIZE;
  }

  T* insert(const ConstIterator& pos, const T& val) {
    return get_insert(pos, val);
  }

  T* insert(const ConstIterator& pos, T&& val) {
    return get_insert(pos, std::move(val));
  }

  T* erase(const ConstIterator& pos) {
    return erase(pos, pos + 1);
  }

  T* erase(const T* first, const T* last) {
    std::size_t st = first - get_data();
    std::size_t end_ = last - get_data();
    const std::size_t cnt = end_ - st;
    if (is_only()) {
      for (std::size_t i = st; i < size() - cnt; ++i) {
        std::swap(get_data()[i], get_data()[i + cnt]);
      }
      std::destroy_n(get_data() + size() - cnt, cnt);
      size_ -= cnt;
    } else {
      std::size_t need_alloc = capacity();
      if (size() <= SMALL_SIZE + cnt) {
        need_alloc = SMALL_SIZE;
      }
      SocowVector tmp(need_alloc);
      for (std::size_t i = 0; i < st; ++i) {
        tmp.push_back(get_data()[i]);
      }
      for (std::size_t i = end_; i < size_; ++i) {
        tmp.push_back(get_data()[i]);
      }
      swap(tmp);
    }
    return get_data() + st;
  }

  void clear() {
    if (is_only()) {
      std::destroy_n(get_data(), size());
    } else {
      --dynamic_storage_->ref_count_;
      is_big = false;
    }
    size_ = 0;
  }

private:
  struct DynamicStorage {
    std::size_t ref_count_;
    std::size_t capacity_;
    T data_[0];

    explicit DynamicStorage(const std::size_t capacity)
        : ref_count_(1)
        , capacity_(capacity) {}
  };

  DynamicStorage* alloc(std::size_t capacity) {
    void* raw = operator new(
        sizeof(DynamicStorage) + capacity * sizeof(T),
        static_cast<std::align_val_t>(alignof(DynamicStorage))
    );
    return new (raw) DynamicStorage(capacity);
  }

  std::size_t size_;
  bool is_big;

  union {
    ValueType static_buff[SMALL_SIZE];
    DynamicStorage* dynamic_storage_;
  };

  Iterator get_data() noexcept {
    return is_big ? dynamic_storage_->data_ : static_buff;
  }

  ConstIterator get_data() const noexcept {
    return is_big ? dynamic_storage_->data_ : static_buff;
  }

  std::size_t get_capacity() {
    return is_big ? dynamic_storage_->capacity_ : SMALL_SIZE;
  }

  void unshare() {
    if (is_only()) {
      return;
    }
    SocowVector tmp(capacity());
    std::uninitialized_copy_n(get_data(), size(), tmp.get_data());
    tmp.size_ = size_;
    swap(tmp);
  }
};
} // namespace ct
