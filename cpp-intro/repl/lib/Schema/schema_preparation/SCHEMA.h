#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ct::schema {

struct Hash {
  using is_transparent = void;

  std::size_t operator()(std::string_view from) const noexcept {
    return std::hash<std::string_view>{}(from);
  }
};

struct Equal {
  using is_transparent = void;

  bool operator()(std::string_view a, std::string_view b) const noexcept {
    return a == b;
  }
};

const std::unordered_set<std::string, Hash, Equal> builtin_types = {"int32", "int64", "uint32", "uint64", "string"};

struct Function {
  std::string name_;
  std::vector<std::pair<std::string, std::string>> args_;
  std::unordered_map<std::string, std::string, Hash, Equal> set_args_;
  std::string return_type_;
};

struct Struct {
  std::string name_;
  std::vector<std::pair<std::string, std::string>> fields_;
  std::unordered_map<std::string, std::string, Hash, Equal> set_fields_;

  bool operator==(const Struct& other) const = default;
};

class Schema {
public:
  std::unordered_map<std::string, Function, Hash, Equal> funcs;
  std::unordered_map<std::string, Struct, Hash, Equal> structures;

  bool exist_type(std::string_view pattern) const;

  static bool exist_builtin_type(std::string_view pattern);

  Struct* get_struct(std::string_view from);

  Function* get_func(std::string_view from);

  void ins_func(Function f);

  void ins_struct(Struct s);
};
} // namespace ct::schema
