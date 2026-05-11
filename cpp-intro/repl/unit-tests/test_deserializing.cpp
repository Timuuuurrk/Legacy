#include "Schema/query_parser/deserializer.h"
#include "Schema/query_parser/types.h"
#include "Schema/schema_preparation/SCHEMA.h"
#include "Schema/schema_preparation/SyntaxError.h"
#include "Schema/schema_preparation/scheme_parser.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <limits>
using namespace ct::schema::types_;
using namespace ct::schema;
using namespace ct::schema::schema_parser;

static std::string read_file(const std::string& file_path) {
  std::filesystem::path from = std::filesystem::path(__FILE__).parent_path();
  std::filesystem::path path = from / "testing_schemas" / file_path;
  std::ifstream fin(path, std::ios::binary);
  if (!fin) {
    std::string s = "Error: cannot open file ";
    s += path;
    throw SyntaxError(s);
  }
  std::ostringstream sstream;
  sstream << fin.rdbuf();
  std::string ans = sstream.str();
  return ans;
}

TEST(DeserializingTest, SimpleInteger) {
  std::string buf = read_file("Person");
  const char* from = buf.c_str();
  Schema my_schema = SchemaParser::parse(from);
  Uint32 buf2(100);
  std::vector<std::byte> checker = {std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x64}};
  std::size_t ind = 0;
  auto ans = deserializer::deserialize_primitive(checker, "uint32", my_schema, ind);
  ASSERT_NE(dynamic_cast<Uint32*>(ans.get()), nullptr);
  EXPECT_EQ(buf2.val, dynamic_cast<Uint32*>(ans.get())->val);
}

TEST(DeserializingTest, SimpleString) {
  std::string buf = read_file("Person");
  const char* from = buf.c_str();
  Schema my_schema = SchemaParser::parse(from);
  String buf2("aboba");
  std::vector<std::byte> checker = {
      std::byte{0x00},
      std::byte{0x00},
      std::byte{0x00},
      std::byte{0x05},
      static_cast<std::byte>('a'),
      static_cast<std::byte>('b'),
      static_cast<std::byte>('o'),
      static_cast<std::byte>('b'),
      static_cast<std::byte>('a')
  };
  std::size_t ind = 0;
  auto ans = deserializer::deserialize_primitive(checker, "string", my_schema, ind);
  ASSERT_NE(dynamic_cast<String*>(ans.get()), nullptr);
  EXPECT_EQ(dynamic_cast<String*>(ans.get())->value, buf2.value);
}

TEST(DeserializingTestLimits, MaxInt32) {
  std::string buf = read_file("Person");
  const char* from = buf.c_str();
  Schema my_schema = SchemaParser::parse(from);
  Int32 buf2(std::numeric_limits<int32_t>::max());
  std::vector<std::byte> checker = {std::byte{0x7f}, std::byte{0xff}, std::byte{0xff}, std::byte{0xff}};
  std::size_t ind = 0;
  auto ans = deserializer::deserialize_primitive(checker, "int32", my_schema, ind);
  ASSERT_NE(dynamic_cast<Int32*>(ans.get()), nullptr);
  EXPECT_EQ(buf2.val, dynamic_cast<Int32*>(ans.get())->val);
}

TEST(DeserializingTestLimits, MaxInt64) {
  std::string buf = read_file("Person");
  const char* from = buf.c_str();
  Schema my_schema = SchemaParser::parse(from);
  Int64 buf2(std::numeric_limits<int64_t>::max());
  std::vector<std::byte> checker = {
      std::byte{0x7f},
      std::byte{0xff},
      std::byte{0xff},
      std::byte{0xff},
      std::byte{0xff},
      std::byte{0xff},
      std::byte{0xff},
      std::byte{0xff}
  };
  std::size_t ind = 0;
  auto ans = deserializer::deserialize_primitive(checker, "int64", my_schema, ind);
  ASSERT_NE(dynamic_cast<Int64*>(ans.get()), nullptr);
  EXPECT_EQ(buf2.val, dynamic_cast<Int64*>(ans.get())->val);
}

TEST(DeserializingTestLimits, MaxUint32) {
  std::string buf = read_file("Person");
  const char* from = buf.c_str();
  Schema my_schema = SchemaParser::parse(from);
  Uint32 buf2(std::numeric_limits<uint32_t>::max());
  std::vector<std::byte> checker = {std::byte{0xff}, std::byte{0xff}, std::byte{0xff}, std::byte{0xff}};
  std::size_t ind = 0;
  auto ans = deserializer::deserialize_primitive(checker, "uint32", my_schema, ind);
  ASSERT_NE(dynamic_cast<Uint32*>(ans.get()), nullptr);
  EXPECT_EQ(buf2.val, dynamic_cast<Uint32*>(ans.get())->val);
}

TEST(DeserializingTestLimits, MaxUint64) {
  std::string buf = read_file("Person");
  const char* from = buf.c_str();
  Schema my_schema = SchemaParser::parse(from);
  Uint64 buf2(std::numeric_limits<uint64_t>::max());
  std::vector<std::byte> checker = {
      std::byte{0xff},
      std::byte{0xff},
      std::byte{0xff},
      std::byte{0xff},
      std::byte{0xff},
      std::byte{0xff},
      std::byte{0xff},
      std::byte{0xff}
  };
  std::size_t ind = 0;
  auto ans = deserializer::deserialize_primitive(checker, "uint64", my_schema, ind);
  ASSERT_NE(dynamic_cast<Uint64*>(ans.get()), nullptr);
  EXPECT_EQ(buf2.val, dynamic_cast<Uint64*>(ans.get())->val);
}

TEST(DeserializingTestLimits, MinInt32) {
  std::string buf = read_file("Person");
  const char* from = buf.c_str();
  Schema my_schema = SchemaParser::parse(from);
  Int32 buf2(std::numeric_limits<int32_t>::min());
  std::vector<std::byte> checker = {std::byte{0x80}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}};
  std::size_t ind = 0;
  auto ans = deserializer::deserialize_primitive(checker, "int32", my_schema, ind);
  ASSERT_NE(dynamic_cast<Int32*>(ans.get()), nullptr);
  EXPECT_EQ(buf2.val, dynamic_cast<Int32*>(ans.get())->val);
}

TEST(DeserializingTestLimits, MinInt64) {
  std::string buf = read_file("Person");
  const char* from = buf.c_str();
  Schema my_schema = SchemaParser::parse(from);
  Int64 buf2(std::numeric_limits<int64_t>::min());
  std::vector<std::byte> checker = {
      std::byte{0x80},
      std::byte{0x00},
      std::byte{0x00},
      std::byte{0x00},
      std::byte{0x00},
      std::byte{0x00},
      std::byte{0x00},
      std::byte{0x00}
  };
  std::size_t ind = 0;
  auto ans = deserializer::deserialize_primitive(checker, "int64", my_schema, ind);
  ASSERT_NE(dynamic_cast<Int64*>(ans.get()), nullptr);
  EXPECT_EQ(buf2.val, dynamic_cast<Int64*>(ans.get())->val);
}

TEST(DeserializingTestLimits, MinUint32) {
  std::string buf = read_file("Person");
  const char* from = buf.c_str();
  Schema my_schema = SchemaParser::parse(from);
  Uint32 buf2(std::numeric_limits<uint32_t>::min());
  std::vector<std::byte> checker = {std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}};
  std::size_t ind = 0;
  auto ans = deserializer::deserialize_primitive(checker, "uint32", my_schema, ind);
  ASSERT_NE(dynamic_cast<Uint32*>(ans.get()), nullptr);
  EXPECT_EQ(buf2.val, dynamic_cast<Uint32*>(ans.get())->val);
}

TEST(DeserializingTestLimits, MinUint64) {
  std::string buf = read_file("Person");
  const char* from = buf.c_str();
  Schema my_schema = SchemaParser::parse(from);
  Uint64 buf2(std::numeric_limits<uint64_t>::min());
  std::vector<std::byte> checker = {
      std::byte{0x00},
      std::byte{0x00},
      std::byte{0x00},
      std::byte{0x00},
      std::byte{0x00},
      std::byte{0x00},
      std::byte{0x00},
      std::byte{0x00}
  };
  std::size_t ind = 0;
  auto ans = deserializer::deserialize_primitive(checker, "uint64", my_schema, ind);
  ASSERT_NE(dynamic_cast<Uint64*>(ans.get()), nullptr);
  EXPECT_EQ(buf2.val, dynamic_cast<Uint64*>(ans.get())->val);
}
