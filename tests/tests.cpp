#include <gtest/gtest.h>
#include "gmock/gmock.h"

#include "../src/libHashMap.hpp"

#include <typeinfo>
#include <iostream>

using namespace LibHashMap;
using namespace LibHashMap::Tools;

template<typename KeyType, std::unsigned_integral HashType = size_t> class TestHashFunction {
   private :
    std::hash<KeyType> std_hash_counter{};
   public :
    [[nodiscard]] constexpr auto countHash(auto&& val) noexcept {
      const std::string t1{"test1"}, t2 {"test2"}, t3 {"test3"}, t4 {"test4"}, t5 {"test5"};
      if (!val.compare(t1)) return static_cast<HashType>(111);;
      if (!val.compare(t2)) return static_cast<HashType>(111);;
      if (!val.compare(t3)) return static_cast<HashType>(111);;
      if (!val.compare(t4)) return static_cast<HashType>(222);;
      if (!val.compare(t5)) return static_cast<HashType>(222);;
       
      auto hash {std_hash_counter(std::forward<decltype(val)>(val))};
      if constexpr (std::is_same<HashType, size_t>::value) {
        return hash;
      } else {
        return static_cast<HashType>(hash);
      }
    }
};

template<typename Key, typename Value, std::unsigned_integral Size = size_t, Size dim_size = 0>
  class TestHashMap : TestHashFunction<Key, Size> {
    public :
      
      constexpr explicit TestHashMap (std::initializer_list<std::pair<Key, Value>>&& lst) 
        : TestHashFunction<Key, Size>() {
        Size tmp_hash_sz{0}, stor_sz{0};
        std::array<Size, dim_size> tmp_hash_tbl;
        std::array<Key, dim_size> tmp_key_tbl;

        //  Creating new hash for key if key value is not duplicated
        auto countHash = [this, &tmp_hash_tbl, &tmp_hash_sz, &tmp_key_tbl, &stor_sz](auto val) {
          auto hash {TestHashFunction<Key, Size>::countHash(std::forward<Key>(val.first))};

          if (auto it_collision_node {std::ranges::find(tmp_hash_tbl, hash)}; it_collision_node == tmp_hash_tbl.end()) { //  Just add new data node (no value duplication or cash collision case)
            tmp_hash_tbl[tmp_hash_sz] = hash;
            tmp_key_tbl[tmp_hash_sz] = val.first;
            Tools::Node<Key, Value, Size, dim_size> node{std::forward<Key>(val.first), std::forward<Value>(val.second), std::forward<Size>(hash)};
            data_stor[stor_sz] = std::move(node);
            ++tmp_hash_sz;
            ++stor_sz;
          } else {
            if (std::ranges::find(tmp_key_tbl, val.first) == tmp_key_tbl.end()) {  //  Hash collision case
              Tools::Node<Key, Value, Size, dim_size> node{std::forward<Key>(val.first), std::forward<Value>(val.second), std::forward<Size>(hash)};
              data_stor[pivot_number] = std::move(node);
              auto collision_node {std::ranges::find_if(data_stor.begin(), data_stor.begin() + (dim_size - (dim_size - pivot_number)), [&hash] (const auto& stor_node){return stor_node.hash == hash;})};
              collision_node->collision_chain[collision_node->collisions_number++] = &data_stor[pivot_number];
              --pivot_number;
            } else {  //  Duplicated value
              static_assert(true, "Duplicated hash value");
            }
          }
        };
        assert((void("Number of variables in initialising list should be equal dimension type size"), lst.size() == dim_size));
        static_assert(std::is_same<Size, decltype(dim_size)>::value, "Requested type should be equal dimension type size");
        
        std::ranges::for_each(lst, countHash);
        std::ranges::sort(data_stor.begin(), data_stor.begin() + (dim_size - (dim_size - pivot_number - 1)), [](auto& lhs, auto& rhs){return lhs.hash < rhs.hash;});
      }

      constexpr auto get(auto&& key) noexcept {
        const Value* val{nullptr};
        Size pos{(Size)floor(pivot_number / 2)};
        const Tools::Node<Key, Value, Size, dim_size>* array_val;
        const auto key_hash {TestHashFunction<Key, Size>::countHash(std::forward<Key>(key))};

        do {
          array_val =&data_stor[pos];
          auto res {array_val->hash <=> key_hash};

          if (res == 0) {
            if (array_val->collisions_number) {
              if (array_val->key == key) {  //  There are collisions but first value is in main collision node
                val = &(array_val->val);
              } else {
                for (auto count : array_val->collision_chain) {  //  Looking value in collision chain
                  if (key == count->key) {
                    val = &count->val;
                    break;
                  }
                }
              }
            } else {  //  There are no collisions at all
              val = &array_val->val;  
            }
            break;
          }
          if (res > 0) {
            if (pos == 0) {
              break;
            }
            if (pos == 1) {
              --pos;
            } else {
              pos -= pos/2; 
            }
          }
          if (res < 0) {
            if (pos == 1) {
              ++pos;
            } else {
              pos += pos/2; 
            }
          }
        } while (pos >= 0 && pos <= pivot_number); 
        
        return val;
      }
      
      private :
        Size pivot_number {dim_size - 1};
        std::array<Tools::Node<Key, Value, Size, dim_size>, dim_size> data_stor{};
  };


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
   TestHashMap<std::string, char, uint8_t, 8> hash{{"xqzrbn"s,'b'}, {"test",'c'}, {"krumld"s,'a'}, {"test1"s, 'd'}, {"test2"s, 'e'}, {"test3"s, 'f'}, {"test4"s, 'g'}, {"test5"s, 'h'}};
   ASSERT_TRUE(hash.get("krumld"s));
   ASSERT_TRUE(hash.get("xqzrbn"s));
   ASSERT_TRUE(hash.get("test"s));
   ASSERT_TRUE(hash.get("test1"s));
   ASSERT_TRUE(hash.get("test2"s));
   ASSERT_TRUE(hash.get("test3"s));
   ASSERT_TRUE(hash.get("test4"s));
   ASSERT_TRUE(hash.get("test5"s));
   auto a {*hash.get("krumld"s)};
   auto b {*hash.get("xqzrbn"s)};
   auto c {*hash.get("test"s)};
   auto d {*hash.get("test1"s)};
   auto e {*hash.get("test2"s)};
   auto f {*hash.get("test3"s)};
   auto g {*hash.get("test4"s)};
   auto h {*hash.get("test5"s)};
   EXPECT_EQ (a, 'a');
   EXPECT_EQ (b, 'b');
   EXPECT_EQ (c, 'c');
   EXPECT_EQ (d, 'd');
   EXPECT_EQ (e, 'e');
   EXPECT_EQ (f, 'f');
   EXPECT_EQ (g, 'g');
   EXPECT_EQ (h, 'h');
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


