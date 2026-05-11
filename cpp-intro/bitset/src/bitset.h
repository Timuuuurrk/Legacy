#pragma once

#include "bitset-iterator.h"
#include "bitset-reference.h"

#include <cstddef>
#include <string_view>

namespace ct {
template <typename T>
class BitsetReference;
template <typename T>
class BitsetIterator;
template <typename T>
class BitsetView;

class BitSet {
public:
  using Word = MyWord;

private:
  std::size_t size_;
  Word* data_;
  void shift(std::size_t count, bool is_left);

public:
  using Value = bool;
  using Reference = BitsetReference<Word>;
  using ConstReference = BitsetReference<const Word>;
  using Iterator = BitsetIterator<Word>;
  using ConstIterator = BitsetIterator<const Word>;
  using View = BitsetView<Word>;
  using ConstView = BitsetView<const Word>;

  friend class BitsetView<Word>;
  friend class BitsetView<const Word>;

  friend void swap(BitSet& our, BitSet& other);

  static constexpr std::size_t NPOS = -1;

  BitSet();

  BitSet(std::size_t size, bool value);

  BitSet(const BitSet& other);

  explicit BitSet(std::string_view str);

  explicit BitSet(const ConstView& other);

  BitSet(const ConstIterator& first, const ConstIterator& last);

  BitSet& operator=(const BitSet& other) &;

  BitSet& operator=(const ConstView& other) &;

  ~BitSet();

  void swap(BitSet& other);

  std::size_t size() const;

  bool empty() const;

  Reference operator[](std::size_t index);

  ConstReference operator[](std::size_t index) const;

  Iterator begin();

  ConstIterator begin() const;

  Iterator end();

  ConstIterator end() const;

  BitSet& operator&=(const ConstView& other) &;

  BitSet& operator|=(const ConstView& other) &;

  BitSet& operator^=(const ConstView& other) &;

  BitSet& operator<<=(std::size_t count) &;

  BitSet& operator>>=(std::size_t count) &;

  BitSet operator>>(std::size_t count) const;

  BitSet operator<<(std::size_t count) const;

  BitSet& flip() &;

  BitSet& set() &;

  BitSet& reset() &;

  bool all() const;

  bool any() const;

  std::size_t count() const;

  operator ConstView() const;

  operator View();

  View subview(std::size_t offset = 0, std::size_t count = NPOS);

  ConstView subview(std::size_t offset = 0, std::size_t count = NPOS) const;

  BitSet operator&(const BitSet& other) const;

  BitSet operator|(const BitSet& other) const;

  BitSet operator^(const BitSet& other) const;

  BitSet operator~() const;
};

bool operator==(const BitSet& left, const BitSet& right);

bool operator!=(const BitSet& left, const BitSet& right);

std::string to_string(const BitSet& our);

std::ostream& operator<<(std::ostream& os, const BitSet& our);
} // namespace ct

#include "bitset-view.h"
