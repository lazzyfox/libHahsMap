#include <gtest/gtest.h>
#include "gmock/gmock.h"

#include "../src/libHashMap.hpp"

#include <typeinfo>
#include <iostream>

using namespace LibHashMap;
using namespace LibHashMap::Tools;

TEST(HashTest, SizeT) {
  HashFunction<int, uint8_t> hash{};
  auto a {hash(1)};
  auto b {hash(2)};
  auto c {hash(3)};
  EXPECT_EQ (a, 1);
  EXPECT_EQ (b, 2);
  EXPECT_EQ (c, 3);
}

TEST(Ctr, Create_NoSort) {
  HashMap<int, char, uint8_t, 3> hash{{1,'a'}, {2,'b'}, {3,'c'}};
  ASSERT_TRUE(hash.get(1));
  ASSERT_TRUE(hash.get(2));
  ASSERT_TRUE(hash.get(3));
  auto a {*hash.get(1)};
  auto b {*hash.get(2)};
  auto c {*hash.get(3)};
  EXPECT_EQ (a, 'a');
  EXPECT_EQ (b, 'b');
  EXPECT_EQ (c, 'c');
}

TEST(Ctr, Create_Sort) {
  HashMap<int, char, uint8_t, 3> hash{{2,'b'}, {3,'c'}, {1,'a'}};
  ASSERT_TRUE(hash.get(1));
  ASSERT_TRUE(hash.get(2));
  ASSERT_TRUE(hash.get(3));
  auto a {*hash.get(1)};
  auto b {*hash.get(2)};
  auto c {*hash.get(3)};
  EXPECT_EQ (a, 'a');
  EXPECT_EQ (b, 'b');
  EXPECT_EQ (c, 'c');
}

TEST(Ctr, CreateFalse) {
  HashMap<int, char, uint8_t, 3> hash{{2,'b'}, {3,'c'}, {1,'a'}};
  auto a {hash.get(-1)};
  auto b {hash.get(0)};
  auto c {hash.get(4)};
  EXPECT_EQ (a, nullptr);
  EXPECT_EQ (b, nullptr);
  EXPECT_EQ (c, nullptr);
}

TEST(Ctr, Create_Sort_String_Size_t) {
  using namespace std::literals;
  HashMap<std::string, char, size_t, 3> hash{{"Two"s,'b'}, {"Three"s,'c'}, {"One"s,'a'}};
  ASSERT_TRUE(hash.get("One"s));
  ASSERT_TRUE(hash.get("Two"s));
  ASSERT_TRUE(hash.get("Three"s));
  auto a {*hash.get("One"s)};
  auto b {*hash.get("Two"s)};
  auto c {*hash.get("Three"s)};
  EXPECT_EQ (a, 'a');
  EXPECT_EQ (b, 'b');
  EXPECT_EQ (c, 'c');
}

TEST(Ctr, Create_Sort_String_View_Size_t) {
  using namespace std::literals;
  HashMap<std::string_view, char, size_t, 3> hash{{"Two"sv,'b'}, {"Three"sv,'c'}, {"One"sv,'a'}};
  ASSERT_TRUE(hash.get("One"sv));
  ASSERT_TRUE(hash.get("Two"sv));
  ASSERT_TRUE(hash.get("Three"sv));
  auto a {*hash.get("One"sv)};
  auto b {*hash.get("Two"sv)};
  auto c {*hash.get("Three"sv)};
  EXPECT_EQ (a, 'a');
  EXPECT_EQ (b, 'b');
  EXPECT_EQ (c, 'c');
}

TEST(Ctr, Create_Sort_String_Uint8_t) {
  using namespace std::literals;
  HashMap<std::string, char, uint8_t, 3> hash{{"Two"s,'b'}, {"Three"s,'c'}, {"One"s,'a'}};
  ASSERT_TRUE(hash.get("One"s));
  ASSERT_TRUE(hash.get("Two"s));
  ASSERT_TRUE(hash.get("Three"s));
  auto a {*hash.get("One"s)};
  auto b {*hash.get("Two"s)};
  auto c {*hash.get("Three"s)};
  EXPECT_EQ (a, 'a');
  EXPECT_EQ (b, 'b');
  EXPECT_EQ (c, 'c');
}

TEST(Ctr, Create_Sort_String_View_Uint8_t) {
  using namespace std::literals;
  HashMap<std::string_view, char, uint8_t, 3> hash{{"Two"sv,'b'}, {"Three"sv,'c'}, {"One"sv,'a'}};
  ASSERT_TRUE(hash.get("One"sv));
  ASSERT_TRUE(hash.get("Two"sv));
  ASSERT_TRUE(hash.get("Three"sv));
  auto a {*hash.get("One"sv)};
  auto b {*hash.get("Two"sv)};
  auto c {*hash.get("Three"sv)};
  EXPECT_EQ (a, 'a');
  EXPECT_EQ (b, 'b');
  EXPECT_EQ (c, 'c');
}


TEST(Collision, String) {
  using namespace std::literals;
  HashMap<std::string, char, size_t, 3> hash{{"xqzrbn"s,'b'}, {"test",'c'}, {"krumld"s,'a'}};
  ASSERT_TRUE(hash.get("krumld"s));
  ASSERT_TRUE(hash.get("xqzrbn"s));
  ASSERT_TRUE(hash.get("test"));
  auto a {*hash.get("krumld"s)};
  auto b {*hash.get("xqzrbn"s)};
  auto c {*hash.get("test")};
  EXPECT_EQ (a, 'a');
  EXPECT_EQ (b, 'b');
  EXPECT_EQ (c, 'c');
}


TEST(EXISTS, Exists) {
  HashMap<int, char, uint8_t, 3> hash{{2,'b'}, {3,'c'}, {1,'a'}};
  ASSERT_TRUE(hash.get(1));
  ASSERT_TRUE(hash.get(2));
  ASSERT_TRUE(hash.get(3));
  auto a {hash.exists(1)};
  auto b {hash.exists(2)};
  auto c {hash.exists(3)};
  EXPECT_TRUE (a);
  EXPECT_TRUE (b);
  EXPECT_TRUE (c);
}

TEST(EXISTS, NoExists) {
  HashMap<int, char, uint8_t, 3> hash{{2,'b'}, {3,'c'}, {1,'a'}};
  auto a {hash.exists(-1)};
  auto b {hash.exists(0)};
  auto c {hash.exists(5)};
  EXPECT_FALSE (a);
  EXPECT_FALSE (b);
  EXPECT_FALSE (c);
}



GTEST_API_ int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


