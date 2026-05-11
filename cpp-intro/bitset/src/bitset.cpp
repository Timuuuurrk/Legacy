
#include "bitset.h"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <string_view>

class string_view;

namespace ct {
using Word = BitSet::Word;
using Reference = BitsetReference<Word>;
using ConstReference = BitsetReference<const Word>;
using View = BitsetView<Word>;
using ConstView = BitsetView<const Word>;

BitSet::BitSet()
    : size_(0)
    , data_(nullptr) {}

BitSet::BitSet(const std::size_t size, const bool value)
    : size_(size) {
  const std::size_t cnt_blocks = (size + sz_type - 1) / sz_type;
  data_ = (size == 0 ? nullptr : new Word[cnt_blocks]());

  if (size_ != 0) {
    const Word fill_val = value ? ~static_cast<Word>(0) : static_cast<Word>(0);
    std::fill_n(data_, cnt_blocks, fill_val);

    if (value && size_ % sz_type != 0) {
      data_[cnt_blocks - 1] &= (static_cast<Word>(1) << (size_ % sz_type)) - 1;
    }
  }
}

BitSet::BitSet(const BitSet& other)
    : size_(other.size_) {
  const std::size_t sz = (size_ + sz_type - 1) / sz_type;
  data_ = sz > 0 ? new Word[sz]() : nullptr;
  if (sz > 0) {
    std::copy_n(other.data_, sz, data_);
  }
}

BitSet::BitSet(const std::string_view str)
    : size_(str.size()) {
  const std::size_t sz = size_ == 0 ? 0 : (size_ + sz_type - 1) / sz_type;
  data_ = sz > 0 ? new Word[sz]() : nullptr;
  if (sz > 0) {
    for (std::size_t i = 0; i < str.size(); ++i) {
      const std::size_t block = i / sz_type;
      const std::size_t ind = i % sz_type;
      data_[block] |= (static_cast<Word>(str[i] == '0' ? 0 : 1) << ind);
    }
  }
}

BitSet::BitSet(const ConstView& other)
    : size_(other.size()) {
  const std::size_t sz = (size_ + sz_type - 1) / sz_type;
  data_ = new Word[sz]();
  if (sz > 0) {
    ConstIterator it = other.begin();
    std::size_t left = other.size();
    std::size_t ind = 0;
    while (left >= sz_type) {
      data_[ind] = it.readBits(sz_type);
      ++ind;
      it += sz_type;
      left -= sz_type;
    }
    if (left > 0) {
      data_[ind] = it.readBits(left);
    }
  }
}

BitSet::BitSet(const ConstIterator& first, const ConstIterator& last)
    : BitSet(ConstView(first, last)) {}

BitSet& BitSet::operator=(const BitSet& other) & {
  if (this == &other) {
    return *this;
  }
  BitSet tmp(other);
  swap(tmp);
  return *this;
}

BitSet& BitSet::operator=(const ConstView& other) & {
  BitSet my(other);
  swap(my);
  return *this;
}

BitSet::~BitSet() {
  delete[] data_;
}

void BitSet::swap(BitSet& other) {
  using std::swap;
  std::swap(size_, other.size_);
  std::swap(data_, other.data_);
}

std::size_t BitSet::size() const {
  return size_;
}

bool BitSet::empty() const {
  return size_ == 0;
}

Reference BitSet::operator[](const std::size_t index) {
  const std::size_t block = index / sz_type;
  const std::size_t ind = index % sz_type;

  return Reference(data_ + block, ind);
}

ConstReference BitSet::operator[](const std::size_t index) const {
  const std::size_t block = index / sz_type;
  const std::size_t ind = index % sz_type;

  return ConstReference(data_ + block, ind);
}

BitSet::operator ConstView() const {
  return ConstView(*this);
}

BitSet::operator View() {
  return View(*this);
}

BitSet::Iterator BitSet::begin() {
  return Iterator(data_, 0);
}

BitSet::ConstIterator BitSet::begin() const {
  return ConstIterator(data_, 0);
}

BitSet::Iterator BitSet::end() {
  return Iterator(data_, size_);
}

BitSet::ConstIterator BitSet::end() const {
  return ConstIterator(data_, size_);
}

BitSet& BitSet::operator&=(const ConstView& other) & {
  const View our(data_, 0, size_);
  our &= other;
  return *this;
}

BitSet& BitSet::operator|=(const ConstView& other) & {
  const View our(data_, 0, size_);
  our |= other;
  return *this;
}

BitSet& BitSet::operator^=(const ConstView& other) & {
  const View our(data_, 0, size_);
  our ^= other;
  return *this;
}

void BitSet::shift(const std::size_t count, const bool is_left) {
  if (is_left) {
    if ((size_ + sz_type - 1) / sz_type == (size_ + count + sz_type - 1) / sz_type) {
      size_ += count;
      return;
    }
    BitSet ans(size_ + count, false);
    if (data_ != nullptr) {
      std::copy_n(data_, (size_ + sz_type - 1) / sz_type, ans.data_);
    }
    for (std::size_t i = (size_ + sz_type - 1) / sz_type + 1; i < (size_ + sz_type - 1 + count) / sz_type; ++i) {
      ans.data_[i] = static_cast<Word>(0);
    }
    *this = ans;
  } else {
    const std::size_t cnt = (size_ + sz_type - 1 - std::min(count, size_)) / sz_type;
    BitSet ans(size_ - std::min(count, size_), false);
    if (cnt != 0) {
      if ((size_ - count) % sz_type == 0) {
        std::copy_n(data_, cnt, ans.data_);
      } else {
        std::copy_n(data_, cnt - 1, ans.data_);
        ans.data_[cnt - 1] = data_[cnt - 1] & ((1ULL << ((size_ - count) % sz_type)) - 1);
      }
    }
    *this = ans;
  }
}

BitSet& BitSet::operator<<=(const std::size_t count) & {
  shift(count, true);
  return *this;
}

BitSet& BitSet::operator>>=(const std::size_t count) & {
  shift(count, false);
  return *this;
}

BitSet BitSet::operator<<(const std::size_t count) const {
  BitSet buf(*this);
  buf <<= count;
  return buf;
}

BitSet BitSet::operator>>(const std::size_t count) const {
  BitSet buf(*this);
  buf >>= count;
  return buf;
}

BitSet& BitSet::flip() & {
  View our(data_, 0, size_);
  our.flip();
  return *this;
}

BitSet& BitSet::set() & {
  View our(data_, 0, size_);
  our.set();
  return *this;
}

BitSet& BitSet::reset() & {
  View our(data_, 0, size_);
  our.reset();
  return *this;
}

bool BitSet::all() const {
  const View our(data_, 0, size_);
  return our.all();
}

bool BitSet::any() const {
  const View our(data_, 0, size_);
  return our.any();
}

std::size_t BitSet::count() const {
  const View our(data_, 0, size_);
  return our.count();
}

BitSet::View BitSet::subview(const std::size_t offset, const std::size_t count) {
  BitSet::View our(*this);
  return our.subview(offset, count);
}

BitSet::ConstView BitSet::subview(const std::size_t offset, const std::size_t count) const {
  BitSet::ConstView our(*this);
  return our.subview(offset, count);
}

BitSet BitSet::operator&(const BitSet& other) const {
  BitSet ans(*this);
  ans &= other;
  return ans;
}

BitSet BitSet::operator|(const BitSet& other) const {
  BitSet ans(*this);
  ans |= other;
  return ans;
}

BitSet BitSet::operator^(const BitSet& other) const {
  BitSet ans(*this);
  ans ^= other;
  return ans;
}

BitSet BitSet::operator~() const {
  BitSet first(size_, true);
  const ConstView second(*this);
  first ^= second;
  return first;
}

std::string to_string(const BitSet& our) {
  const ConstView ans(our);
  return to_string(ans);
}

std::ostream& operator<<(std::ostream& os, const BitSet& our) {
  const BitsetView<const Word> tmp(our);
  os << tmp;
  return os;
}

void swap(BitSet& our, BitSet& other) {
  using std::swap;
  swap(our.size_, other.size_);
  swap(our.data_, other.data_);
}

bool operator!=(const BitSet& our, const BitSet& other) {
  return !(our == other);
}

bool operator==(const BitSet& our, const BitSet& other) {
  const ConstView cur(other);
  const ConstView ou(our);
  return ou == cur;
}
} // namespace ct

// namespace ct
