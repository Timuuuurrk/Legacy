#pragma once
#include "Schema/schema_preparation/SCHEMA.h"
#include "types.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ct::schema::deserializer {
using namespace types_;

std::unique_ptr<Basic_type>
deserialize_struct(const std::vector<std::byte>& from, std::string_view struct_name, std::size_t& ind, Schema& schema);

std::unique_ptr<Basic_type>
deserialize_primitive(const std::vector<std::byte>& from, std::string_view type_name, Schema& schema, std::size_t& ind);

template <typename Type>
Type get_value(const std::vector<std::byte>& from, std::size_t& ind, std::size_t shift) {
  using U = std::make_unsigned_t<Type>;
  U ans = 0;
  for (std::size_t i = 0; i < shift; ++i) {
    ans += (to_integer<uint64_t>(from[ind++])) << ((shift - 1 - i) * 8);
  }
  return static_cast<Type>(ans);
}

} // namespace ct::schema::deserializer
