/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/04/09 01:13:17
# Desc   :
########################################################################
*/
#pragma once

#include <list>
#include <memory>
#include <optional>
#include <print>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "common.h"
#include "macro.h"
#include "mini_seq.h"
#include "seq.h"
#include "utils.h"

using MacroPtr = std::shared_ptr<Macro>;
class MacroInfo {
public:
  static std::vector<MacroPtr> kMacros;
  static std::unordered_map<View, MacroPtr> kNameToMacro;
};  // class MacroInfo


struct Context {
  ExpandInfo* expand_info;
  Seq input;
  Seq rest;
  std::vector<View> replacing_bit_at_end;
  Seq::consumed_type output;
  size_t removed_length{0};

  Token consume_a_token(size_t len);
  Seq::consumed_type consume(size_t len);
  Seq remove_prefix(size_t len);
  size_t consume_ltrim();
  void set_replacing_bit(View macro_name);
  void reset_replacing_bit_at_end();
  size_t size() const;
  std::string str() const;

  class iterator {
  public:
    using DataType = decltype(input);
    using DataIterator = DataType::iterator;
    using value_type = DataType::value_type;
    iterator(DataIterator it, DataIterator input_end, DataIterator rest_begin);
    iterator operator++();
    bool operator==(const iterator& other) const;
    bool operator!=(const iterator& other) const;
    const value_type& operator*() const;
  private:
    DataIterator it_;
    DataIterator input_end_;
    DataIterator rest_begin_;
  };  // class iterator
  iterator begin() const;
  iterator end() const;
};  // struct Context
