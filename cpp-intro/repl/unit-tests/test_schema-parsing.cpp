#include "Schema/schema_preparation/SCHEMA.h"
#include "Schema/schema_preparation/SyntaxError.h"
#include "Schema/schema_preparation/scheme_parser.h"

#include <gtest/gtest.h>
using namespace ct::schema;

TEST(SchemaParsingTest, Point) {
  const char* point = R"(
    fn getX -> int64 {
        string p;
    }
    fn getPoint -> string {
        int64 x;
    }
  )";
  Schema schema = schema_parser::SchemaParser::parse(point);
  ASSERT_EQ(schema.funcs.size(), 2);
  EXPECT_NE(schema.get_func("getX"), nullptr);
  EXPECT_NE(schema.get_func("getPoint"), nullptr);
  EXPECT_EQ(schema.get_func("getx"), nullptr);
}

TEST(SchemaParsingTest, Structure) {
  const char* car = R"(struct Car {
      uint32 id;
      uint64 price;
    }

    fn getCarId -> uint32 {
      Car car;
    }

    fn getCarPrice -> uint64 {
      Car car;
    })";
  Schema schema = schema_parser::SchemaParser::parse(car);
  ASSERT_EQ(schema.funcs.size(), 2);
  ASSERT_EQ(schema.structures.size(), 1);

  ASSERT_NE(schema.funcs.find("getCarId"), schema.funcs.end());
  ASSERT_NE(schema.funcs.find("getCarPrice"), schema.funcs.end());

  auto p1 = std::make_pair("id", "uint32");
  auto p2 = std::make_pair("price", "uint64");
  std::unordered_map<std::string, std::string, Hash, Equal> set_fields;
  set_fields[p1.first] = p1.second;
  set_fields[p2.first] = p2.second;
  const Struct c{"Car", {p1, p2}, std::move(set_fields)};
  EXPECT_EQ(schema.funcs["getCarId"].args_[0].second, "Car");
  EXPECT_EQ(schema.structures["Car"], c);
}

TEST(SchemaParsingTest, NestStructure) {
  const char* nest = R"(struct X{
        int32 p;
      }
      struct Y{
        X val;
      }
      fn getP -> int32 {
        Y arg;
      }
    )";
  Schema schema = schema_parser::SchemaParser::parse(nest);
  ASSERT_EQ(schema.structures["Y"].fields_.size(), 1);
  EXPECT_EQ(schema.structures["Y"].fields_[0].second, "X");
}

TEST(SchemaParsingTest, UnknownTypeFunction) {
  const char* type = R"(
      fn getX -> int {
        kek v;
      }
    )";
  EXPECT_THROW(schema_parser::SchemaParser::parse(type), SyntaxError);
}

TEST(SchemaParsingTest, UnknownTypeStructure) {
  const char* type = R"(
      struct X{
         int v;
      }
    )";
  EXPECT_THROW(schema_parser::SchemaParser::parse(type), SyntaxError);
}

TEST(SchemaParsingTest, RecursiveStructure) {
  const char* type = R"(
      struct X{
         X v;
      }
    )";
  EXPECT_THROW(schema_parser::SchemaParser::parse(type), SyntaxError);
}
