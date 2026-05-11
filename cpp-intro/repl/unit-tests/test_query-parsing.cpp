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

class QueryAccumulator : public ::testing::Test {
  void SetUp() override {
    schemas_["Car"] = read_file("Car");
    schemas_["Person"] = read_file("Person");
    schemas_["Numbers"] = read_file("Numbers");
  }

  void TearDown() override {}

protected:
  std::unordered_map<std::string, std::string> schemas_;

  Schema my_schema;
};

class RandomTestsAccumulator
    : public QueryAccumulator
    , public ::testing::WithParamInterface<std::size_t> {};

TEST_F(QueryAccumulator, ParseFunction) {
  std::string buf = schemas_["Car"];
  const char* from = buf.c_str();
  my_schema = SchemaParser::parse(from);
  QueryParser car_parser(std::move(my_schema));
  std::string cur_query = "getCarId(id=42)";
  auto parsed_query = car_parser.parse(cur_query);
  ASSERT_EQ(parsed_query->args.size(), 1);
  EXPECT_EQ(parsed_query->name, "getCarId");
  EXPECT_EQ(dynamic_cast<Uint32*>(parsed_query->args[0].second.get())->val, 42);
}

TEST_F(QueryAccumulator, ParseStructureInQuery) {
  std::string buf = schemas_["Car"];
  const char* from = buf.c_str();
  my_schema = SchemaParser::parse(from);
  QueryParser car_parser(std::move(my_schema));
  std::string cur_query = "getCarPrice(car={id=100, price=1000000})";

  auto checker_query = [&car_parser](std::string cur_quer) {
    auto parsed_query = car_parser.parse(cur_quer);
    ASSERT_EQ(parsed_query->args.size(), 1);
    auto* ans = dynamic_cast<StructType*>(parsed_query->args[0].second.get());
    EXPECT_EQ(parsed_query->name, "getCarPrice");
    EXPECT_EQ(ans->name, "Car");
    ASSERT_EQ(ans->seq_fields.size(), 2);
    EXPECT_EQ(ans->seq_fields[0].first, "id");
    EXPECT_EQ(ans->seq_fields[1].first, "price");
    EXPECT_EQ(dynamic_cast<Uint32*>(ans->seq_fields[0].second.get())->val, 100);
    EXPECT_EQ(dynamic_cast<Uint64*>(ans->seq_fields[1].second.get())->val, 1000000);
  };
  checker_query(cur_query);
  checker_query("getCarPrice(car=Car{id=100, price=1000000})");
}

TEST_F(QueryAccumulator, ParseNested) {
  std::string buf = schemas_["Person"];
  const char* from = buf.c_str();
  my_schema = SchemaParser::parse(from);
  QueryParser person_parser(std::move(my_schema));
  auto checker_query = [&person_parser](std::string cur_quer) {
    auto parsed_query = person_parser.parse(cur_quer);
    ASSERT_EQ(parsed_query->args.size(), 1);
    auto* ans = dynamic_cast<StructType*>(parsed_query->args[0].second.get());
    EXPECT_EQ(parsed_query->name, "getName");
    EXPECT_EQ(ans->name, "Person");
    EXPECT_EQ(ans->seq_fields.size(), 2);
    EXPECT_EQ(ans->seq_fields[0].first, "name");
    ASSERT_EQ(ans->seq_fields[1].first, "email");
    auto* nm = dynamic_cast<StructType*>(ans->seq_fields[0].second.get());

    EXPECT_EQ(nm->name, "N");
    EXPECT_EQ(dynamic_cast<String*>(nm->seq_fields[0].second.get())->value, "Egor");

    EXPECT_EQ(dynamic_cast<String*>(ans->seq_fields[1].second.get())->value, "kotik@gmail.com");
  };
  std::string name = "\"Egor\"";
  std::string email = "\"kotik@gmail.com\"";
  checker_query("getName(p={name={n=" + name + "}, email=" + email + "})");
  checker_query("getName(p=Person{name={n=" + name + "}, email=" + email + "})");
  checker_query("getName(p={name=N{n=" + name + "}, email=" + email + "})");
  checker_query("getName(p=Person{name=N{n=" + name + "}, email=" + email + "})");
}

static void checker_random_missing_argument(QueryParser& car_parser, std::vector<std::string> cur_query, size_t ind) {
  std::string query;
  for (std::size_t i = 0; i < cur_query.size(); ++i) {
    if (i != ind) {
      query += cur_query[i];
    }
  }
  auto parsed_query = car_parser.parse(query);
}

const std::vector<std::string> tokens =
    {"getCarPrice", "(", "car", "=", "{", "id", "=", "0", ",", "price", "=", "1000", "}", ")"};
const std::vector<std::string> transkript = {
    "getCarPrice",
    "open_function_bracket",
    "car",
    "equal_sign_1",
    "open_structure_bracket",
    "id",
    "equal_sign_2",
    "field_value_1",
    "comma",
    "price",
    "equal_sign_3",
    "field_value_2",
    "close_structure_bracket",
    "close_function_bracket"
};

TEST_P(RandomTestsAccumulator, MissingTokens) {
  std::string buf = schemas_["Car"];
  const char* from = buf.c_str();
  my_schema = SchemaParser::parse(from);
  QueryParser car_parser(std::move(my_schema));
  const size_t ind = GetParam();
  EXPECT_THROW(checker_random_missing_argument(car_parser, tokens, ind), ParsingError);
}

INSTANTIATE_TEST_SUITE_P(
    MissingTokensTesting,
    RandomTestsAccumulator,
    ::testing::Range(size_t(0), size_t(14)),
    [](const ::testing::TestParamInfo<std::size_t>& from) { return "Missing_token_" + transkript[from.param]; }
);
