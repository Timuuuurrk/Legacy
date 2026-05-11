#pragma once
#include <cstddef>
#include <cstdint>
#include <limits>

namespace my_constraints {
using MyWord = uint64_t;
constexpr MyWord mask_max = ~0ULL;
constexpr uint64_t sz_type = std::numeric_limits<MyWord>::digits;
constexpr std::size_t NPOS = -1;

constexpr MyWord getSegment(const uint8_t left, const uint8_t right) {
  //[l;r]
  const MyWord msk = right == sz_type - 1 ? ~0ULL : (1ULL << (right + 1u)) - 1;
  const MyWord lmsk = (1ULL << left) - 1;
  return msk & (~lmsk);
}
} // namespace my_constraints
