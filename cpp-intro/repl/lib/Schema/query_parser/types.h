#pragma once
#include <xxhash.h>

#include <algorithm>
#include <bit>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace ct::schema::types_ {
struct Basic_type {
  virtual void serialize(std::vector<std::byte>& dest) const = 0;
  virtual void str(std::string& ans) const = 0;

  template <typename U>
  static std::byte get_byte(std::uint32_t i, U val) {
    return static_cast<std::byte>((val >> (8 * i)) & 0xFF);
  }

  Basic_type& operator=(const Basic_type&) = default;
  virtual ~Basic_type() = default;
};

struct StructType : Basic_type {
  std::string name;
  std::vector<std::pair<std::string, std::unique_ptr<Basic_type>>> seq_fields;

  void str(std::string& ans) const override;

  void serialize(std::vector<std::byte>& dest) const override;
};

template <typename Type>
void serialize_number(Type val, std::vector<std::byte>& dest, std::size_t shift) {
  if constexpr (std::endian::native == std::endian::big) {
    for (std::size_t i = 0; i < shift; i++) {
      dest.push_back(Basic_type::get_byte<Type>(i, val));
    }
  } else {
    for (int32_t i = shift - 1; i > -1; --i) {
      dest.push_back(Basic_type::get_byte<Type>(i, val));
    }
  }
}

struct Int32 : Basic_type {
  int32_t val;
  std::string val_str;
  Int32() = default;

  Int32(int32_t from);

  void str(std::string& ans) const override;
  void serialize(std::vector<std::byte>& dest) const override;
};

struct Int64 : Basic_type {
  int64_t val;
  std::string val_str;
  Int64() = default;

  Int64(int64_t from);

  void str(std::string& ans) const override;

  void serialize(std::vector<std::byte>& dest) const override;
};

struct Uint32 : Basic_type {
  uint32_t val;
  std::string val_str;
  Uint32() = default;

  Uint32(uint32_t from);

  void str(std::string& ans) const override;

  void serialize(std::vector<std::byte>& dest) const override;
};

struct Uint64 : Basic_type {
  uint64_t val;
  std::string val_str;
  Uint64() = default;

  Uint64(uint64_t from);

  void str(std::string& ans) const override;

  void serialize(std::vector<std::byte>& dest) const override;
};

struct String : Basic_type {
  std::string value;

  String() = default;

  String(const std::string& from);

  void str(std::string& ans) const override;

  void serialize(std::vector<std::byte>& dest) const override;
};

struct FuncType : Basic_type {
  std::string name;
  std::vector<std::pair<std::string, std::unique_ptr<Basic_type>>> args;
  std::unordered_map<std::string, std::unique_ptr<Basic_type>> set_args_;

  void str(std::string& ans) const override;

  void serialize(std::vector<std::byte>& dest) const override;
};

} // namespace ct::schema::types_
