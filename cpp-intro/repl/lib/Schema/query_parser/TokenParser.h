#pragma once
#include "ParsingError.h"

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace ct::schema::token_parser {
class TokenParser {
  std::size_t curPos = 0;
  std::string from;

public:
  explicit TokenParser(std::string from_);

  bool eof() const;

  char peek() const;

  char next();

  void expect(char expected);

  void expect(char expected, bool& fl);

  static bool is_whitespace(char cur);

  void skip_whitespaces();

  bool find(const std::string& term);

  std::string take_while(const std::string& term, bool fl = true);

  bool match(const char& need);
};
} // namespace ct::schema::token_parser
