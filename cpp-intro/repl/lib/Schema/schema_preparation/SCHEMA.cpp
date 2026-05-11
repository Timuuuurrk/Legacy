#include "SCHEMA.h"

namespace ct::schema {

bool Schema::exist_type(std::string_view pattern) const {
  return structures.find(pattern) != structures.end() || builtin_types.find(pattern) != builtin_types.end();
}

bool Schema::exist_builtin_type(std::string_view pattern) {
  return builtin_types.find(std::string(pattern)) != builtin_types.end();
}

Struct* Schema::get_struct(std::string_view from) {
  auto it = structures.find(from);
  return it == structures.end() ? nullptr : &it->second;
}

Function* Schema::get_func(std::string_view from) {
  auto it = funcs.find(from);
  return it == funcs.end() ? nullptr : &it->second;
}

void Schema::ins_func(Function f) {
  funcs[f.name_] = std::move(f);
}

void Schema::ins_struct(Struct s) {
  structures[s.name_] = std::move(s);
}
} // namespace ct::schema
