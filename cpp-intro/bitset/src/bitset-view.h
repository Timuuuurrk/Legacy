#pragma once

#include "bitset-iterator.h"
#include "bitset.h"

#include <cstddef>
#include <functional>
#include <sstream>
#include <string>

namespace ct {
class BitSet;

template <typename T>
class BitsetView;

template <typename T>
class BitsetView {
  BitsetIterator<T> start;
  BitsetIterator<T> finish;

public:
  friend class BitsetView<const T>;
  friend class BitSet;
  using Value = bool;
  using Reference = BitsetReference<T>;
  using ConstReference = BitsetReference<const T>;
  using Iterator = BitsetIterator<T>;
  using ConstIterator = BitsetIterator<const T>;
  using View = BitsetView;
  using ConstView = BitsetView<const T>;

  template <typename U>
  friend void swap(BitsetView& our, BitsetView& other);

  BitsetView() = default;

  ~BitsetView() = default;

  operator BitsetView<const T>() const {
    return BitsetView<const T>{start, finish};
  }

  BitsetView& operator=(const BitsetView& other) = default;

  explicit BitsetView(BitSet& other)
      : start(other.begin())
      , finish(other.end()) {}

  template <typename U = T, typename = std::enable_if_t<std::is_const_v<U>>>
  explicit BitsetView(const BitSet& other)
      : start(other.begin())
      , finish(other.end()) {}

  BitsetView(const BitsetIterator<T>& begin_, const BitsetIterator<T>& end_)
      : start(begin_)
      , finish(end_) {}

  BitsetReference<T> operator[](const std::size_t ind) const {
    return start[ind];
  }

  BitsetIterator<T> begin() const {
    return start;
  }

  BitsetIterator<T> end() const {
    return finish;
  }

private:
  BitsetView(T* cur, const std::size_t offset, const std::size_t cnt) {
    start = BitsetIterator(cur, offset);
    if (cnt == NPOS) {
      finish = BitsetIterator(cur, offset + sizeof(cur));
    } else {
      finish = BitsetIterator(cur, offset + cnt);
    }
  }

  template <typename Func>
  std::size_t
  viewOperationsStatic(const std::size_t init, const Func& update, const std::size_t& step = sz_type) const {
    std::size_t left = size();
    Iterator it = begin();
    MyWord cur;
    std::size_t result = init;
    while (left >= step) {
      cur = it.readBits(step);
      update(cur, sz_type, result);
      if (step == 1 && init != result) {
        // short circuit
        return result;
      }
      it += step;
      left -= step;
    }
    if (left > 0) {
      cur = it.readBits(left);
      update(cur, left, result);
    }
    return result;
  }

  void viewOperationsDynamic(MyWord checker) const {
    std::size_t left = size();
    Iterator it = begin();
    MyWord cur;
    while (left >= sz_type) {
      cur = it.readBits(sz_type);
      // set, reset, flip
      if (checker == 1) {
        // flip
        cur = ~cur;
        it.writeBits(cur, sz_type);
      } else {
        it.writeBits(checker, sz_type);
      }
      it += sz_type;
      left -= sz_type;
    }
    if (left > 0) {
      cur = it.readBits(left);
      if (checker == 1) {
        cur = ~cur & ((1ULL << left) - 1);
        it.writeBits(cur, left);
      } else {
        checker &= (1ULL << left) - 1;
        it.writeBits(checker, left);
      }
    }
  }

  template <typename Func>
  const BitsetView& applyOper(const Func& oper, const BitsetView<const T>& other) const {
    if (other.begin() == other.end()) {
      return *this;
    }
    Iterator it1 = begin();
    ConstIterator it2 = other.begin();
    MyWord cur1, cur2;
    const std::size_t reminder = size() % sz_type;
    if (reminder > 0) {
      cur1 = it1.readBits(reminder);
      cur2 = it2.readBits(reminder);
      cur1 = oper(cur1, cur2);
      it1.writeBits(cur1, reminder);
      it1 += reminder;
      it2 += reminder;
    }
    const std::size_t blocks = size() / sz_type;
    for (std::size_t i = 0; i < blocks; ++i) {
      cur1 = it1.readBits(sz_type);
      cur2 = it2.readBits(sz_type);
      cur1 = oper(cur1, cur2);
      it1.writeBits(cur1, sz_type);
      it1 += sz_type;
      it2 += sz_type;
    }

    return *this;
  }

  static std::stringstream output(const BitsetView& our) {
    std::stringstream out;
    Iterator it = our.begin();
    const auto end = our.end();
    while (it + sz_type <= end) {
      const MyWord cur = it.readBits(sz_type);
      for (std::size_t i = 0; i < sz_type; ++i) {
        out << ((cur >> i) & 1 ? "1" : "0");
      }
      it += sz_type;
    }
    if (it != end) {
      const std::size_t left = end - it;
      const MyWord cur = it.readBits(left);
      for (std::size_t i = 0; i < left; ++i) {
        out << ((cur >> i) & 1 ? "1" : "0");
      }
    }
    return out;
  }

public:
  bool all() const {
    auto update = [](const MyWord& cur, const std::size_t, std::size_t& result) {
      result &= cur;
    };
    return static_cast<bool>(viewOperationsStatic(1, update, 1));
  }

  bool any() const {
    auto update = [](const MyWord& cur, const std::size_t, std::size_t& result) {
      result |= static_cast<std::size_t>(cur != 0);
    };
    return static_cast<bool>(viewOperationsStatic(0, update, 1));
  }

  std::size_t count() const {
    auto update = [](const MyWord& cur, const std::size_t left, std::size_t& result) {
      if (left > 0) {
        result += std::popcount(cur);
      }
    };
    return viewOperationsStatic(0, update);
  }

  std::size_t count(const std::function<std::size_t(const MyWord&)>& custom_counter) const {
    auto update = [custom_counter](const MyWord& cur, const std::size_t, std::size_t& result) {
      result += custom_counter(cur);
    };
    return viewOperationsStatic(0, update);
  }

  /*
   I made an overload of the count method in case the user wants his own bit counter,
   if the counter were a class field as a lambda function,
   then the class would not be trivially copyable,
   so the decision was made to overload the method
  */

  std::size_t size() const {
    return finish - start;
  }

  bool empty() const {
    return size() == 0;
  }

  BitsetView subview(const std::size_t offset = 0, const std::size_t cnt = NPOS) const {
    if (offset > size()) {
      return {finish, finish};
    }
    BitsetIterator start_ = start + offset;
    BitsetIterator end_ = (cnt > size() || offset + cnt > size()) ? finish : start + offset + cnt;
    return {start_, end_};
  }

  const BitsetView& reset() const {
    viewOperationsDynamic(0);
    return *this;
  }

  const BitsetView& set() const {
    viewOperationsDynamic(mask_max);
    return *this;
  }

  const BitsetView& flip() const {
    viewOperationsDynamic(1);
    return *this;
  }

  const BitsetView& operator^=(const BitsetView<const T>& other) const {
    return applyOper(std::bit_xor<T>(), other);
  }

  const BitsetView& operator&=(const BitsetView<const T>& other) const {
    return applyOper(std::bit_and<T>(), other);
  }

  const BitsetView& operator|=(const BitsetView<const T>& other) const {
    return applyOper(std::bit_or<T>(), other);
  }

  friend BitSet operator~(const BitsetView& our) {
    return ~BitSet(our);
  }

  friend BitSet operator&(const BitsetView& our, const BitsetView& other) {
    return BitSet(our) & BitSet(other);
  }

  friend BitSet operator|(const BitsetView& our, const BitsetView& other) {
    return BitSet(our) | BitSet(other);
  }

  friend BitSet operator^(const BitsetView& our, const BitsetView& other) {
    return BitSet(our) ^ BitSet(other);
  }

  friend std::string to_string(const BitsetView& our) {
    const std::stringstream ss = output(our);
    return ss.str();
  }

  friend std::ostream& operator<<(std::ostream& out, const BitsetView& our) {
    const std::stringstream ss = output(our);
    out << ss.rdbuf();
    return out;
  }

  friend BitSet operator>>(const BitsetView& our, std::size_t count) {
    BitSet ans(our.subview(0, our.size() - std::min(count, our.size())));
    return ans;
  }

  friend BitSet operator<<(const BitsetView& our, const std::size_t count) {
    BitSet ans(our);
    ans <<= count;
    return ans;
  }

  friend bool operator==(const BitsetView& our, const BitsetView& other) {
    if (our.size() != other.size()) {
      return false;
    }
    BitsetIterator<T> cur = our.begin(), cur2 = other.begin();
    std::size_t left = our.size();
    while (left >= sz_type) {
      if (cur.readBits(sz_type) != cur2.readBits(sz_type)) {
        return false;
      }
      cur += sz_type;
      cur2 += sz_type;
      left -= sz_type;
    }
    if (left > 0) {
      if (cur.readBits(left) != cur2.readBits(left)) {
        return false;
      }
    }
    return true;
  }

  friend bool operator==(const BitsetView& our, const BitSet& other) {
    const BitsetView<const T> cur(other);
    return our == cur;
  }

  friend bool operator!=(const BitsetView& our, const BitSet& other) {
    return !(our == other);
  }

  friend bool operator!=(const BitsetView& our, const BitsetView& other) {
    return !(our == other);
  }

  void swap(BitsetView& other) {
    using std::swap;
    std::swap(this->start, other.start);
    std::swap(this->finish, other.finish);
  }

  friend void swap(BitsetView& our, BitsetView& other) {
    our.swap(other);
  }
};

} // namespace ct
