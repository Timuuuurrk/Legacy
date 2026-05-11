#include "TokenParser.h"

namespace ct::schema::token_parser {
TokenParser::TokenParser(std::string from_)
    : from(std::move(from_)) {}

bool TokenParser::eof() const {
  return curPos == from.size();
}

char TokenParser::peek() const {
  return from[curPos];
}

char TokenParser::next() {
  if (curPos == from.size()) {
    throw std::runtime_error("Expected: token index out of bounds!\n");
  }
  return from[curPos++];
}

void TokenParser::expect(char expected) {
  if (peek() != expected) {
    throw ParsingError("Expected: " + std::string(1, expected) + "\n");
  }
  ++curPos;
  skip_whitespaces();
}

void TokenParser::expect(char expected, bool& fl) {
  fl = curPos < from.size() && from[curPos] == expected;
  if (fl) {
    curPos++;
    skip_whitespaces();
  }
}

bool TokenParser::is_whitespace(char cur) {
  return cur == ' ' || cur == '\t' || cur == '\n';
}

void TokenParser::skip_whitespaces() {
  while (curPos < from.size() && is_whitespace(from[curPos])) {
    ++curPos;
  }
}

bool TokenParser::find(const std::string& term) {
  for (char cur : term) {
    if (curPos < from.size() && from[curPos] == cur) {
      return true;
    }
  }
  if (curPos == from.size()) {
    std::string possibles;
    for (char e : term) {
      possibles += e;
      possibles += "or";
    }
    possibles.erase(possibles.size() - 2);
    throw ParsingError("Error: expected: " + possibles + "\n");
  }
  return false;
}

std::string TokenParser::take_while(const std::string& term, bool fl) {
  std::string ans;
  skip_whitespaces();
  bool fl2 = true;
  while (!eof() && !find(term)) {
    if (fl2) {
      ans += from[curPos];
    }
    curPos++;
    if (!fl && curPos < from.size() && is_whitespace(from[curPos])) {
      fl2 = false;
    }
  }
  return ans;
}

bool TokenParser::match(const char& need) {
  if (!eof() && peek() == need) {
    ++curPos;
    return true;
  }
  return false;
}
} // namespace ct::schema::token_parser
