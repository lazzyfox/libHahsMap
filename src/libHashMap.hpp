/*
Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Andrey Fokin lazzyfox@gmail.com.
Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*! \mainpage Project LibHashMap library Index Page
 *
 * \section intro_sec Introduction
 *
 *  Single file library for
 *  Hash Map data structure for compile time implementation. 
 *  Main purpose is using as a constexpr  data. 
 *  Data access time is about O(logN) 
 *  Released and tested for gcc 13 (c++ 23)
 *  Product is under MIT license.
 *
 * \section description LibHashMap library
 *
 *   Could be created and used in compile time
 *   There is no modifications interface (add, delete or update features).
 *   Is getter and data existing check only.
 *   HashMap could be created by initializer list only. 
 *
 * \section install Compilation/installation
 *  Could be used a c++ header file or installed as internal library:
 *    cmake .. -DCMAKE_INSTALL_PREFIX:PATH=/your/installation/path
 *    cmake --build . --config Release --target install -- -j $(nproc)
 *
 *
 */

#include <array>
#include <functional>
#include <cstdint>
#include <algorithm>
#include <concepts>
#include <type_traits>

// uncomment to disable assert()
// #define NDEBUG

/// @brief Version 0.0.1
/// HashMap -  it is analog for std::unordered_map but for compile time using.
/// Hahs functions for different data types and sizes
/// General namespace for class
namespace LibHashMap {
  
  /// @brief  HashMap tolls namespace  
  namespace Tools {

    /// @brief struct Node to store data
    template<typename KeyType, typename ValueType, std::unsigned_integral HashType, HashType table_size> struct Node {
      KeyType key;  ///  Original key value
      ValueType val;  ///  Value
      HashType hash;  ///  Hash, calculated for key
      ///  Collisions chain - pointers to nodes with the same hash but different key
      std::array<Node*, table_size> collision_chain{};
      ///  Collisions chain elements (pointers) number
      HashType collisions_number {0};
      
      /// @brief Default constructor
      Node() = default;
      
      /// @brief Constructors to create node by existing data and counted hash
      /// @param key Key value
      /// @param val Value
      /// @param hash Hash (if calculated)
      constexpr Node (KeyType& key, ValueType& val, HashType& hash) : key{key}, val{val}, hash{hash} {}
      constexpr Node (KeyType&& key, ValueType&& val, HashType&& hash) : key{std::move(key)}, val{std::move(val)}, hash{std::move(hash)} {}
      
      /// @brief Copy constructor
      /// @param node  Existing node
      constexpr Node (Node& node) {
        key = node.key;
        val = node.val;
        hash = node.hash;

        if (collisions_number) {
          std::ranges::copy_n(node.collision_chain.begin(), node.collisions_number, collision_chain.begin());
          collisions_number = node.collisions_number;
        }
      }
      
      /// @brief Const copy constructor
      /// @param node Existing node struct
      constexpr Node (const Node& node) {
        key = node.key;
        val = node.val;
        hash = node.hash;

        if (collisions_number) {
          std::ranges::copy_n(node.collision_chain.begin(), node.collisions_number, collision_chain.begin());
          collisions_number = node.collisions_number;
        }
      }
      
      /// @brief Move constructor
      /// @param node Existing node struct
      constexpr Node (Node&& node) {
        key = std::move(node.key);
        val = std::move(node.val);
        hash = std::move(node.hash);

        if (collisions_number) {
          for (decltype (node.collisions_number) count{0}; count < node.collisions_number; ++count) {
            collision_chain[count] = std::move(node.collision_chain[count]);
          }
          collisions_number = std::move(node.collisions_number);
        }
      }
      
      /// @brief Copy assignment
      /// @param node Existing node struct
      /// @return Node struct
      constexpr Node& operator = (Node& node) noexcept {
        key = node.key;
        val = node.val;
        hash = node.hash;

        if (collisions_number) {
          std::ranges::copy_n(node.collision_chain.begin(), node.collisions_number, collision_chain.begin());
          collisions_number = node.collisions_number;
        }
        return this;
      }
      
      /// @brief Copy assignment for const node struct
      /// @param node Existing node structure
      /// @return struct Node
      constexpr Node& operator = (const Node& node) noexcept {
        key = node.key;
        val = node.val;
        hash = node.hash;

        if (collisions_number) {
          std::ranges::copy_n(node.collision_chain.begin(), node.collisions_number, collision_chain.begin());
          collisions_number = node.collisions_number;
        }
        return *this;
      }

      /// @brief Move operator
      /// @param node Existing node structure
      /// @return Node structure
      Node& operator = (Node&& node) noexcept {
        key = std::move(node.key);
        val = std::move(node.val);
        hash = std::move(node.hash);

        if (collisions_number) {
          for (decltype (node.collisions_number) count{0}; count < node.collisions_number; ++count) {
            collision_chain[count] = std::move(node.collision_chain[count]);
          }
          collisions_number = std::move(node.collisions_number);
        }
        return *this;
      }
      
      /// @brief Adding node with different key but with hash already exists
      /// @param node Node struct pointer
      /// @return True if node added to array and collision chan, else false
      constexpr bool addCollision(const Node* const node) noexcept {
        bool ret {false};
        if (collisions_number < table_size) {
          collision_chain[collisions_number] = node;
          ++node;
          ret = true;
        }
        return ret;
      }
    };
    
    /// @brief  Hahs function object for counting hash for struct Node key
    template<typename KeyType, std::unsigned_integral HashType = size_t> class HashFunction {
      private :
        std::hash<KeyType> std_hash_counter{};
      public :
        /// @brief Counting hash with defined type HahsType for KeyType value type
        /// @param val KeyType (&, &&) value
        /// @return HashType (&) value
        [[nodiscard]] constexpr auto countHash(auto&& val) noexcept {
          auto hash {std_hash_counter(std::forward<decltype(val)>(val))};
          if constexpr (std::is_same<HashType, size_t>::value) {
            return hash;
          } else {
            return static_cast<HashType>(hash);
          }
        }
        /// @brief Operator to count hash with defined type HahsType for KeyType value type
        /// @param val KeyType (&, &&) value
        /// @return HashType (&) value
        constexpr auto operator()(auto&& val) noexcept {
          auto hash {std_hash_counter(std::forward<decltype(val)>(val))};
          if constexpr (std::is_same<HashType, size_t>::value) {
            return hash;
          } else {
            return static_cast<HashType>(hash);
          }
        }
    };
  }  

  /// @brief Class HashMap  Version 0.0.1
  /// HashMap - Interface for data storing
  template<typename Key, typename Value, std::unsigned_integral Size = size_t, Size dim_size = 0>
  class HashMap : Tools::HashFunction<Key, Size> {
    public :

      /// @brief Constructor to create HashMap class by initializer list
      /// @param lst initializer list
      constexpr explicit HashMap (const std::initializer_list<std::pair<Key, Value>>& lst)
       : Tools::HashFunction<Key, Size>() {
        Size tmp_hash_sz{0}, stor_sz{0};
        std::array<Size, dim_size> tmp_hash_tbl;
        std::array<Key, dim_size> tmp_key_tbl;

        //  Creating new hash for key if key value is not duplicated
        auto countHash = [this, &tmp_hash_tbl, &tmp_hash_sz, &tmp_key_tbl, &stor_sz](auto val) {
          auto hash {Tools::HashFunction<Key, Size>::countHash(std::forward<Key>(val.first))};

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
              
              auto collision_node {*(std::find_if(data_stor.begin(), data_stor.begin() + (pivot_number - 1), [&val](auto&& array_val) {
                bool ret {false};
                if constexpr (std::is_same<Value, std::string>::value || std::is_same<Value, std::string_view>::value) {
                  if (!array_val.val.compare(val.second)) {
                    ret = true;
                  }
                } else if constexpr (std::is_same<Value, const char*>::value) {
                  std::string array_str{array_val.val};
                  std::string val_str{val.second};
                  if (!array_str.compare(val_str)) {
                    ret = true;
                  }
                } else {  //  Integral type
                  if (array_val.val == val.second) {
                    ret = true;
                  }
              }
              return ret;
              }))};

              (collision_node.collision_chain)[collision_node.collisions_number] = &data_stor[pivot_number];
              ++(collision_node.collisions_number);
              --pivot_number;
            } else {  //  Duplicated value
              static_assert(true, "Duplicated hash value");
            }
          }
        };
        assert((void("Number of variables in initialising list should be equal dimension type size"), lst.size() == dim_size));
        static_assert(std::is_same<Size, decltype(dim_size)>::value, "Requested type should be equal dimension type size");
        
        std::ranges::for_each(lst, countHash);
        std::ranges::sort(data_stor, [](auto& lhs, auto& rhs){return lhs.hash < rhs.hash;});
      }
      
      /// @brief Move constructor to create HashMap class by initializer list
      /// @param lst initializer list
      constexpr explicit HashMap (std::initializer_list<std::pair<Key, Value>>&& lst) 
        : Tools::HashFunction<Key, Size>() {
        Size tmp_hash_sz{0}, stor_sz{0};
        std::array<Size, dim_size> tmp_hash_tbl;
        std::array<Key, dim_size> tmp_key_tbl;

        //  Creating new hash for key if key value is not duplicated
        auto countHash = [this, &tmp_hash_tbl, &tmp_hash_sz, &tmp_key_tbl, &stor_sz](auto val) {
          auto hash {Tools::HashFunction<Key, Size>::countHash(std::forward<Key>(val.first))};

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
              
              auto collision_node {*(std::find_if(data_stor.begin(), data_stor.begin() + (pivot_number - 1), [&val](auto&& array_val) {
                bool ret {false};
                if constexpr (std::is_same<Value, std::string>::value || std::is_same<Value, std::string_view>::value) {
                  if (!array_val.val.compare(val.second)) {
                    ret = true;
                  }
                } else if constexpr (std::is_same<Value, const char*>::value) {
                  std::string array_str{array_val.val};
                  std::string val_str{val.second};
                  if (!array_str.compare(val_str)) {
                    ret = true;
                  }
                } else {  //  Integral type
                  if (array_val.val == val.second) {
                    ret = true;
                  }
              }
              return ret;
              }))};

              (collision_node.collision_chain)[collision_node.collisions_number] = &data_stor[pivot_number];
              ++(collision_node.collisions_number);
              --pivot_number;
            } else {  //  Duplicated value
              static_assert(true, "Duplicated hash value");
            }
          }
        };
        assert((void("Number of variables in initialising list should be equal dimension type size"), lst.size() == dim_size));
        static_assert(std::is_same<Size, decltype(dim_size)>::value, "Requested type should be equal dimension type size");
        
        std::ranges::for_each(lst, countHash);
        std::ranges::sort(data_stor, [](auto& lhs, auto& rhs){return lhs.hash < rhs.hash;});
      }

      HashMap(HashMap&) = delete;
      HashMap(const HashMap&) = delete;
      HashMap(HashMap&&) = delete;
      HashMap& operator = (HashMap&) = delete;
      HashMap& operator = (const HashMap&) = delete;
      HashMap& operator = (HashMap&&) = delete;
      
      /// @brief Get element by key
      /// @param key KeyType (&, &&) value
      /// @return Value (&) value
      constexpr auto get(auto&& key) noexcept {
        const Value* val{nullptr};
        Size pos{(Size)floor(pivot_number / 2)};
        const Tools::Node<Key, Value, Size, dim_size>* array_val;
        const auto key_hash {Tools::HashFunction<Key, Size>::countHash(std::forward<Key>(key))};

        do {
          array_val =&data_stor[pos];
          auto res {array_val->hash <=> key_hash};

          if (res == 0) {
            if (!array_val->collisions_number) {
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
        } while (pos >= 0 && pos < pivot_number); 
        
        return val;
      }
      
      ///  Check if element exists in map
      /// @brief Check if element exists in map
      /// @param key KeyType (&, &&) value
      /// @return true if exists, else false
      constexpr bool exists(auto&& key) noexcept {
        bool ret {false};
        Size pos{(Size)floor(pivot_number / 2)};
        const Tools::Node<Key, Value, Size, dim_size>* array_val;
        const auto key_hash {Tools::HashFunction<Key, Size>::countHash(std::forward<Key>(key))};
        
        do {
          array_val = &data_stor[pos];
          auto res {array_val->hash <=> key_hash};

          if (res == 0) {
            ret = true;
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
        } while (pos >= 0 && pos < pivot_number); 
        return ret;
      }
      
      private :
        Size pivot_number {dim_size - 1};
        std::array<Tools::Node<Key, Value, Size, dim_size>, dim_size> data_stor{};
  };
}
