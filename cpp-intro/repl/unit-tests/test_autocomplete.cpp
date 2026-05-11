#include "Schema/autocompletion/AutoCompleteParser.h"
#include "Schema/autocompletion/AutoCompletionError.h"
#include "Schema/query_parser/ParsingError.h"
#include "Schema/query_parser/query_parser.h"
#include "Schema/query_parser/types.h"
#include "Schema/schema_preparation/SCHEMA.h"
#include "Schema/schema_preparation/scheme_parser.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
using namespace ct::schema;
using namespace query_parser;
using namespace types_;
using namespace schema_parser;

static std::string read_file(const std::string& file_path) {
  std::filesystem::path from = std::filesystem::path(__FILE__).parent_path();
  std::filesystem::path path = from / "testing_schemas" / file_path;
  std::ifstream fin(path, std::ios::binary);
  if (!fin) {
    std::string s = "Error: cannot open file ";
    throw SyntaxError(s);
  }
  std::ostringstream sstream;
  sstream << fin.rdbuf();
  std::string ans = sstream.str();
  return ans;
}

class AutoCompleteAccumulator : public ::testing::Test {
  void SetUp() override {
    std::string buf = read_file("Car");
    const char* c = buf.c_str();
    my_schema = SchemaParser::parse(c);
  }

  void TearDown() override {}

protected:
  Schema my_schema;
};

TEST_F(AutoCompleteAccumulator, ParseSimple) {
  std::string cur_query = "getCa";
  std::string cur = ct::schema::query_parser::autocomplete_parser::autocompletion(cur_query, my_schema);
  EXPECT_EQ(cur_query + cur, "getCarId(id=");
}

TEST_F(AutoCompleteAccumulator, ParseQuote) {
  std::vector<std::string> queries = {"getCarsQuantity(n", "getCarsQuantity(name", "getCarsQuantity(name="};
  std::vector<std::string> checker = {
      "getCarsQuantity(name=\"",
      "getCarsQuantity(name=\"",
      "getCarsQuantity(name=\"",
  };

  for (size_t i = 0; i < queries.size(); ++i) {
    std::string cur = ct::schema::query_parser::autocomplete_parser::autocompletion(queries[i], my_schema);
    ASSERT_EQ(queries[i] + cur, checker[i]);
  }
}

TEST_F(AutoCompleteAccumulator, ParseSpecialCases) {
  std::vector<std::string> queries = {
      "getCarsQuantity(name=\"Dodge\", from=Ca",
      "getCarsQuantity(name=\"Dodge\", from=Car"
  };
  std::vector<std::string> checker = {
      "getCarsQuantity(name=\"Dodge\", from=Car{id=",
      "getCarsQuantity(name=\"Dodge\", from=Car{id="
  };

  for (size_t i = 0; i < queries.size(); ++i) {
    std::string cur = ct::schema::query_parser::autocomplete_parser::autocompletion(queries[i], my_schema);
    ASSERT_EQ(queries[i] + cur, checker[i]);
  }
}

TEST_F(AutoCompleteAccumulator, ParsePrimitiveCases) {
  std::vector<std::string> queries = {
      "getCa",
      "getCarsQuantity",
      "getCarsQuantity(n",
      "getCarsQuantity(name",
      "getCarsQuantity(name=",
      "getCarsQuantity(name=\"Dodge"
  };
  std::vector<std::string> checker = {
      "getCarId(id=",
      "getCarsQuantity(from={id=",
      "getCarsQuantity(name=\"",
      "getCarsQuantity(name=\"",
      "getCarsQuantity(name=\"",
      "getCarsQuantity(name=\"Dodge"
  };

  for (size_t i = 0; i < queries.size(); ++i) {
    std::string cur = ct::schema::query_parser::autocomplete_parser::autocompletion(queries[i], my_schema);
    ASSERT_EQ(queries[i] + cur, checker[i]);
  }
}

TEST_F(AutoCompleteAccumulator, ParseStructure) {
  std::vector<std::string> queries = {
      R"(getCarsQuantity(name="Dodge")",
      R"(getCarsQuantity(name="Dodge", from=)",
      R"(getCarsQuantity(name="Dodge", from={id=42, p)",
      R"(getCarsQuantity(name="Dodge", from={id=42, price=100})"
  };
  std::vector<std::string> checker = {
      R"(getCarsQuantity(name="Dodge", from={id=)",
      R"(getCarsQuantity(name="Dodge", from={id=)",
      R"(getCarsQuantity(name="Dodge", from={id=42, price=)",
      R"(getCarsQuantity(name="Dodge", from={id=42, price=100}))"
  };

  for (size_t i = 0; i < queries.size(); ++i) {
    std::string cur = ct::schema::query_parser::autocomplete_parser::autocompletion(queries[i], my_schema);
    ASSERT_EQ(queries[i] + cur, checker[i]);
  }
}

TEST_F(AutoCompleteAccumulator, ParseNested) {
  std::vector<std::string> queries = {
      R"(getCarPass(name="Tesla")",
      R"(getCarPass(name="Tesla", region=102)",
      R"(getCarPass(name="Tesla", reg)",
      R"(getCarPass(name="Tesla", region=102, from)",
      R"(getCarPass(name="Tesla", region=102, from={Model="Model X",)",
      R"(getCarPass(name="Tesla", region=102, from={Model="Model X",car=C)",
      R"(getCarPass(name="Tesla", region=102, from={Model="Model X",car={id=0, price=100})"
  };
  std::vector<std::string> checker = {
      R"(getCarPass(name="Tesla", from={Model=")",
      R"(getCarPass(name="Tesla", region=102)",
      R"(getCarPass(name="Tesla", region=)",
      R"(getCarPass(name="Tesla", region=102, from={Model=")",
      R"(getCarPass(name="Tesla", region=102, from={Model="Model X",car={id=)",
      R"(getCarPass(name="Tesla", region=102, from={Model="Model X",car=Car{id=)",
      R"(getCarPass(name="Tesla", region=102, from={Model="Model X",car={id=0, price=100}}))"
  };

  for (size_t i = 0; i < queries.size(); ++i) {
    std::string cur = ct::schema::query_parser::autocomplete_parser::autocompletion(queries[i], my_schema);
    ASSERT_EQ(queries[i] + cur, checker[i]);
  }
}

TEST_F(AutoCompleteAccumulator, ParseRecursiveFields) {
  std::string buf = read_file("Numbers");
  const char* c = buf.c_str();
  my_schema = SchemaParser::parse(c);
  std::vector<std::string> queries = {
      "getNumber(n",
      "getNumber(num=N4{d=N",
      "getNumber(num=N4{d=N3{c={b=N1{a={val=100}",
      "getNumber(num=N4{d=N3{c={b=N1{a={val=100"
  };
  std::vector<std::string> checker = {
      "getNumber(num={d={c={b={a={val=",
      "getNumber(num=N4{d=N3{c={b={a={val=",
      "getNumber(num=N4{d=N3{c={b=N1{a={val=100}}}}}, kek={b={a={val=",
      "getNumber(num=N4{d=N3{c={b=N1{a={val=100"
  };

  for (size_t i = 0; i < queries.size(); ++i) {
    std::string cur = ct::schema::query_parser::autocomplete_parser::autocompletion(queries[i], my_schema);
    ASSERT_EQ(queries[i] + cur, checker[i]);
  }
}

TEST_F(AutoCompleteAccumulator, ParseErrorAutoComplete) {
  std::vector<std::string> queries = {
      "geTCa",
      "getCarsQuantity(r",
      "getCarsQuantity(name==",
      "getCarsQuantity(name=\"Dodge\", {}",
      "getCarsQuantity(name=\"Dodge\", fromm",
  };

  for (size_t i = 0; i < queries.size(); ++i) {
    EXPECT_ANY_THROW(ct::schema::query_parser::autocomplete_parser::autocompletion(queries[i], my_schema));
  }
}
