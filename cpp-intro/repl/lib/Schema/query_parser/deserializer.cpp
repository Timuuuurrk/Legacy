#include "deserializer.h"

namespace ct::schema::deserializer {
using namespace types_;

std::unique_ptr<Basic_type>
deserialize_struct(const std::vector<std::byte>& from, std::string_view struct_name, std::size_t& ind, Schema& schema) {
  auto ans = std::make_unique<StructType>();
  ans->name = struct_name;
  auto* strct = schema.get_struct(struct_name);
  for (const auto& cur_field : strct->fields_) {
    ans->seq_fields.emplace_back(cur_field.first, deserialize_primitive(from, cur_field.second, schema, ind));
  }
  return ans;
}

std::unique_ptr<Basic_type> deserialize_primitive(
    const std::vector<std::byte>& from,
    std::string_view type_name,
    Schema& schema,
    std::size_t& ind
) {
  if (type_name == "uint32") {
    auto ans = std::make_unique<Uint32>();
    ans->val = get_value<uint32_t>(from, ind, 4);
    return ans;
  } else if (type_name == "uint64") {
    auto ans = std::make_unique<Uint64>();
    ans->val = get_value<uint64_t>(from, ind, 8);
    return ans;
  } else if (type_name == "int32") {
    auto ans = std::make_unique<Int32>();
    ans->val = get_value<int32_t>(from, ind, 4);
    return ans;
  } else if (type_name == "int64") {
    auto ans = std::make_unique<Int64>();
    ans->val = get_value<int64_t>(from, ind, 8);
    return ans;
  } else if (type_name == "string") {
    auto len = deserialize_primitive(from, "uint32", schema, ind);
    std::string val;
    len->str(val);
    std::size_t length = std::stoull(val);
    auto ans = std::make_unique<String>();
    ans->value = "";
    for (std::size_t i = 0; i < length; ++i) {
      ans->value += static_cast<char>(from[ind++]);
    }
    return ans;
  } else {
    return deserialize_struct(from, type_name, ind, schema);
  }
}
} // namespace ct::schema::deserializer
