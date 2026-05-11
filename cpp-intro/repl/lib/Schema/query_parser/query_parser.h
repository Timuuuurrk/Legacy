#pragma once
#include "../schema_preparation/SCHEMA.h"
#include "ParsingError.h"
#include "TokenParser.h"
#include "types.h"

#include <ctpg/ctpg.hpp>

#include <cstdint>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace ct::schema::query_parser {
using namespace token_parser;
using namespace types_;

void parse_primitive(const std::string& type, std::unique_ptr<Basic_type>& from);
void parse_struct(const std::string& type, std::unique_ptr<Basic_type>& from);
std::unique_ptr<Basic_type> preparse_struct(auto&& fields_);
std::unique_ptr<Basic_type> parse_number(const std::string& type, const std::string& cur_number_);

class QueryParser {
public:
  explicit QueryParser(Schema schema);

  std::unique_ptr<FuncType> parse(std::string& query);
};

} // namespace ct::schema::query_parser
