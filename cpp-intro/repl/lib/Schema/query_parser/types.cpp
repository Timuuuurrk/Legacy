#include "types.h"

namespace ct::schema::types_ {

void StructType::str(std::string& ans) const {
  ans = name;
  ans += '{';
  for (std::size_t ind = 0; ind < seq_fields.size(); ++ind) {
    ans += seq_fields[ind].first;
    ans += '=';
    std::string cur_field;
    seq_fields[ind].second->str(cur_field);
    ans += cur_field;
    if (ind != seq_fields.size() - 1) {
      ans += ", ";
    }
  }
  ans += '}';
}

void StructType::serialize(std::vector<std::byte>& dest) const {
  for (auto& cur_field : seq_fields) {
    std::vector<std::byte> cur_val;
    cur_field.second->serialize(cur_val);
    dest.insert(dest.end(), cur_val.begin(), cur_val.end());
  }
}

Int32::Int32(int32_t from)
    : val(from) {}

void Int32::str(std::string& ans) const {
  ans = std::to_string(val);
}

void Int32::serialize(std::vector<std::byte>& dest) const {
  serialize_number<int32_t>(val, dest, 4);
}

Int64::Int64(int64_t from)
    : val(from) {}

void Int64::str(std::string& ans) const {
  ans = std::to_string(val);
}

void Int64::serialize(std::vector<std::byte>& dest) const {
  serialize_number<int64_t>(val, dest, 8);
}

Uint32::Uint32(uint32_t from)
    : val(from) {}

void Uint32::str(std::string& ans) const {
  ans = std::to_string(val);
}

void Uint32::serialize(std::vector<std::byte>& dest) const {
  serialize_number<uint32_t>(val, dest, 4);
}

Uint64::Uint64(uint64_t from)
    : val(from) {}

void Uint64::str(std::string& ans) const {
  ans = std::to_string(val);
}

void Uint64::serialize(std::vector<std::byte>& dest) const {
  serialize_number<uint64_t>(val, dest, 8);
}

String::String(const std::string& from)
    : value(from) {}

void String::str(std::string& ans) const {
  ans = '"';
  ans += value;
  ans += '"';
}

void String::serialize(std::vector<std::byte>& dest) const {
  Uint32 buf;
  buf.val = value.size();
  std::vector<std::byte> len;
  buf.serialize(len);
  dest.insert(dest.end(), len.begin(), len.end());
  for (std::uint32_t i = 0; i < value.size(); ++i) {
    dest.push_back(static_cast<std::byte>(value[i]));
  }
}

void FuncType::str(std::string& ans) const {
  ans = name;
}

void FuncType::serialize(std::vector<std::byte>& dest) const {
  std::uint32_t val = XXH32(name.c_str(), name.size(), 0);
  Uint32 buf;
  buf.val = val;
  std::vector<std::byte> serialized_name;
  buf.serialize(serialized_name);
  dest.insert(dest.end(), serialized_name.begin(), serialized_name.end());
  for (auto& cur_arg : args) {
    serialized_name.clear();
    cur_arg.second->serialize(serialized_name);
    dest.insert(dest.end(), serialized_name.begin(), serialized_name.end());
  }
}

} // namespace ct::schema::types_
