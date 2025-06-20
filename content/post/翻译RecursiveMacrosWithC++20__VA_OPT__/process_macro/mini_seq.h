/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/04/14 14:06:33
# Desc   :
########################################################################
*/
#pragma once

#include "common.h"
#include "expand_info.h"
#include <unordered_map>
#include <vector>

class Seq;

class MiniSeq : public View {
public:
  friend class Seq;
  using value_type = View::value_type;
  using consumed_type = View;

private:
  ExpandInfo* expand_info_{nullptr};
  std::unordered_map<const char*, std::vector<View>> replacing_bit_mark_;

public:
  MiniSeq(View view, ExpandInfo* expand_info);
  consumed_type consume(size_t len);
  consumed_type consume_all();
  MiniSeq remove_prefix(size_t len);
  MiniSeq remove_all();
  void add_replacing_bit_mark(const char* ch, View name);
  void clear_replacing_bit_mark();
  size_t consume_ltrim();
  void swap(MiniSeq& other);
};  // class MiniSeq
