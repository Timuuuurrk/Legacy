#pragma once
#include "SCHEMA.h"
#include "SyntaxError.h"

#include <ctpg/ctpg.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace ct::schema::schema_parser {
struct Accumulate {
  Schema schema_;
  std::vector<std::string> types_;
  std::vector<std::string> locations_;

  void ins_elem(std::string type, std::string location);

  void check_types_correctness();
};

using name_n_type = std::pair<std::string, std::string>;
using name_n_type_list = std::vector<name_n_type>;

class SchemaParser {
public:
  static Schema parse(const char*& src);
};

} // namespace ct::schema::schema_parser
