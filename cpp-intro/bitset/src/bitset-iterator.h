#pragma once
#include "bitset-reference.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>

namespace ct {
template <typename T>
class BitsetIterator {
  T* data;
  std::size_t ind;

public:
  template <typename U>
  friend class BitsetIterator;
  template <typename U>
  friend class BitsetView;
  friend class BitSet;

  template <typename U>
  friend std::ostream& operator<<(std::ostream& out, const BitsetView<U>& our);

  using iterator_category = std::random_access_iterator_tag;
  using iterator_concept = std::random_access_iterator_tag;
  using value_type = bool;
  using reference = BitsetReference<T>;
  using difference_type = std::ptrdiff_t;
  using pointer = void;

  BitsetIterator() = default;

  ~BitsetIterator() = default;

  BitsetIterator(const BitsetIterator&) = default;

  BitsetIterator(BitsetIterator&& other) = default;

  BitsetIterator(T* data_, const std::size_t ind_)
      : data(data_)
      , ind(ind_) {}

  BitsetIterator& operator=(const BitsetIterator& other) = default;

  BitsetIterator& operator=(BitsetIterator&& other) = default;

  BitsetReference<T> operator*() const {
    return {data + ind / sz_type, ind % sz_type};
  }

  BitsetReference<T> operator[](const std::size_t pos) const {
    return {data + (ind + pos) / sz_type, (ind + pos) % sz_type};
  }

  BitsetIterator& operator+=(const std::size_t len) {
    ind += len;
    return *this;
  }

  BitsetIterator& operator-=(const std::size_t len) {
    ind -= len;
    return *this;
  }

  BitsetIterator& operator++() {
    *this += 1;
    return *this;
  }

  BitsetIterator& operator--() {
    *this -= 1;
    return *this;
  }

  BitsetIterator operator++(int) {
    const BitsetIterator it(*this);
    *this += 1;
    return it;
  }

  BitsetIterator operator--(int) {
    const BitsetIterator it(*this);
    *this -= 1;
    return it;
  }

  template <typename U>
  difference_type operator-(const BitsetIterator<U>& other) const {
    return ind - other.ind;
  }

  template <typename U>
  bool operator==(const BitsetIterator<U>& other) const {
    return data == other.data && ind == other.ind;
  }

  template <typename U>
  bool operator!=(const BitsetIterator<U>& other) const {
    return !(*this == other);
  }

  auto operator<=>(const BitsetIterator& other) const {
    return ind <=> other.ind;
  }

  operator BitsetIterator<const T>() const {
    return BitsetIterator<const T>(data, ind);
  }

  friend BitsetIterator operator+(int32_t len, const BitsetIterator& our) {
    BitsetIterator it(our);
    it += len;
    return it;
  }

  friend BitsetIterator operator+(const BitsetIterator& our, int32_t len) {
    BitsetIterator it(our);
    it += len;
    return it;
  }

  friend BitsetIterator operator-(const int32_t len, const BitsetIterator& our) {
    BitsetIterator it(our);
    it -= len;
    return it;
  }

  friend BitsetIterator operator-(const BitsetIterator& our, const int32_t len) {
    BitsetIterator it(our);
    it -= len;
    return it;
  }

  void swap(BitsetIterator& other) {
    std::swap(data, other.data);
    std::swap(ind, other.ind);
  }

private:
  MyWord readBits(const std::size_t cnt) const {
    const size_t begin_block = ind / sz_type;
    const size_t begin_ind = ind % sz_type;
    const size_t end_ind = ind + cnt - 1;
    const size_t end_block = end_ind / sz_type;
    if (begin_block != end_block) {
      // cnt>=sz_type
      const size_t first_bits = sz_type - begin_ind;
      const size_t second_bits = cnt - first_bits;
      const MyWord msk1 = (1ULL << first_bits) - 1;
      const MyWord msk2 = (1ULL << second_bits) - 1;
      const MyWord first_ans = (data[begin_block] >> begin_ind) & msk1;
      const MyWord second_ans = (data[end_block]) & msk2;
      return first_ans | (second_ans << first_bits);
    }
    MyWord msk = mask_max;
    if (cnt != sz_type) {
      msk = ((1ULL << cnt) - 1);
    }
    return (data[begin_block] >> begin_ind) & msk;
  }

  template <typename U = T, typename = std::enable_if_t<!std::is_const_v<U>>>
  void writeBits(const MyWord val, const size_t cnt) {
    const size_t begin_block = ind / sz_type;
    const size_t begin_ind = ind % sz_type;
    const size_t end_ind = ind + cnt - 1;
    const size_t end_block = end_ind / sz_type;
    if (begin_block != end_block) {
      // cnt>=sz_type
      const size_t first_bits = sz_type - begin_ind;
      const size_t second_bits = cnt - first_bits;
      const MyWord msk1 = (1ULL << first_bits) - 1;
      const MyWord msk2 = (1ULL << second_bits) - 1;

      data[begin_block] &= ~(msk1 << begin_ind);
      data[begin_block] |= (val & msk1) << begin_ind;

      data[end_block] &= ~msk2;
      data[end_block] |= (val >> first_bits) & msk2;
    } else {
      MyWord msk = mask_max;
      if (cnt != sz_type) {
        msk = ((1ULL << cnt) - 1);
      }
      data[begin_block] &= ~(msk << begin_ind);
      data[begin_block] |= (val & msk) << begin_ind;
    }
  }
};

} // namespace ct
