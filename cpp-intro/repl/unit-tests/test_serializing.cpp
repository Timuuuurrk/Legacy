#include "Schema/query_parser/types.h"

#include <gtest/gtest.h>

#include <limits>
using namespace ct::schema::types_;

TEST(SerializingTest, SimpleInteger) {
  Uint32 buf(100);
  std::vector<std::byte> ans;
  buf.serialize(ans);
  std::vector<std::byte> checker = {std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x64}};
  EXPECT_EQ(checker, ans);
}

TEST(SerializingTest, SimpleString) {
  String buf("aboba");
  std::vector<std::byte> ans;
  buf.serialize(ans);
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
  EXPECT_EQ(ans, checker);
}

TEST(SerializingTest, SimpleFunction) {
  Uint32 id(42);
  String name("John");

  StructType buf2;
  buf2.seq_fields.emplace_back("id", std::make_unique<Uint32>(id));
  buf2.seq_fields.emplace_back("name", std::make_unique<String>(name));

  FuncType buf;
  buf.name = "getId";
  buf.args.emplace_back("person", std::make_unique<StructType>(std::move(buf2)));
  std::vector<std::byte> ans;
  buf.serialize(ans);

  std::vector<std::byte> checker = {
      std::byte{0x69},
      std::byte{0x5d},
      std::byte{0xff},
      std::byte{0xb3},
      std::byte{0x00},
      std::byte{0x00},
      std::byte{0x00},
      std::byte{0x2a},
      std::byte{0x00},
      std::byte{0x00},
      std::byte{0x00},
      std::byte{0x04},
      std::byte{0x4a},
      std::byte{0x6f},
      std::byte{0x68},
      std::byte{0x6e}
  };

  EXPECT_EQ(ans, checker);
}

TEST(SerializingTestLimits, MaxInt32) {
  Int32 buf(std::numeric_limits<int32_t>::max());
  std::vector<std::byte> ans;
  buf.serialize(ans);
  std::vector<std::byte> checker = {std::byte{0x7f}, std::byte{0xff}, std::byte{0xff}, std::byte{0xff}};
  EXPECT_EQ(checker, ans);
}

TEST(SerializingTestLimits, MaxInt64) {
  Int64 buf(std::numeric_limits<int64_t>::max());
  std::vector<std::byte> ans;
  buf.serialize(ans);
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
  EXPECT_EQ(checker, ans);
}

TEST(SerializingTestLimits, MaxUint32) {
  Uint32 buf(std::numeric_limits<uint32_t>::max());
  std::vector<std::byte> ans;
  buf.serialize(ans);
  std::vector<std::byte> checker = {std::byte{0xff}, std::byte{0xff}, std::byte{0xff}, std::byte{0xff}};
  EXPECT_EQ(checker, ans);
}

TEST(SerializingTestLimits, MaxUint64) {
  Uint64 buf(std::numeric_limits<uint64_t>::max());
  std::vector<std::byte> ans;
  buf.serialize(ans);
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
  EXPECT_EQ(checker, ans);
}

TEST(SerializingTestLimits, MinInt32) {
  Int32 buf(std::numeric_limits<int32_t>::min());
  std::vector<std::byte> ans;
  buf.serialize(ans);
  std::vector<std::byte> checker = {std::byte{0x80}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}};
  EXPECT_EQ(checker, ans);
}

TEST(SerializingTestLimits, MinInt64) {
  Int64 buf(std::numeric_limits<int64_t>::min());
  std::vector<std::byte> ans;
  buf.serialize(ans);
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
  EXPECT_EQ(checker, ans);
}

TEST(SerializingTestLimits, MinUint32) {
  Uint32 buf(std::numeric_limits<uint32_t>::min());
  std::vector<std::byte> ans;
  buf.serialize(ans);
  std::vector<std::byte> checker = {std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}};
  EXPECT_EQ(checker, ans);
}

TEST(SerializingTestLimits, MinUint64) {
  Uint64 buf(std::numeric_limits<uint64_t>::min());
  std::vector<std::byte> ans;
  buf.serialize(ans);
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
  EXPECT_EQ(checker, ans);
}
