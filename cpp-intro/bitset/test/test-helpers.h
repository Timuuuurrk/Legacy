#pragma once

#include "bitset.h"

#include <catch2/matchers/catch_matchers.hpp>

#include <vector>

namespace ct::test {

std::vector<bool> string_to_bools(std::string_view str);

class BitSetEqualsString : public Catch::Matchers::MatcherBase<BitSet> {
public:
  explicit BitSetEqualsString(std::string_view expected);

  bool match(const BitSet& actual) const final;

  std::string describe() const final;

private:
  std::string_view _expected;
};

} // namespace ct::test
