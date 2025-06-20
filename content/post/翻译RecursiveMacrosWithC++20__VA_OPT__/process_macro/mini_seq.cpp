/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/04/14 14:08:21
# Desc   :
########################################################################
*/

#include "mini_seq.h"

#include <print>

MiniSeq::MiniSeq(View view, ExpandInfo* expand_info) : View(view), expand_info_{expand_info} {}

MiniSeq::consumed_type MiniSeq::consume(size_t len) {
  auto mini_seq = remove_prefix(len);
  mini_seq.clear_replacing_bit_mark();
  return mini_seq;
}

MiniSeq::consumed_type MiniSeq::consume_all() {
  return consume(size());
}

MiniSeq MiniSeq::remove_prefix(size_t len) {
  MiniSeq res(View::substr(0, len), expand_info_);
  for (size_t i = 0; i < len; ++i) {
    const char* ch = &View::operator[](i);
    if (auto it = replacing_bit_mark_.find(ch); it != replacing_bit_mark_.end()) {
      res.replacing_bit_mark_.emplace(ch, std::move(it->second));
      replacing_bit_mark_.erase(it);
    }
  }
  View::remove_prefix(len);
  return res;
}

MiniSeq MiniSeq::remove_all() {
  MiniSeq res("", expand_info_);
  this->swap(res);
  return res;
}

void MiniSeq::add_replacing_bit_mark(const char* ch, View name) {
  replacing_bit_mark_[ch].emplace_back(name);
}

void MiniSeq::clear_replacing_bit_mark() {
  for (auto& [_, macro_names] : replacing_bit_mark_) {
    for (auto& name : macro_names) {
      expand_info_->replacing_bit.erase(name);
    }
  }
}

size_t MiniSeq::consume_ltrim() {
  size_t idx{0};
  while (idx < size() && View::at(idx) == ' ') {
    ++idx;
  }
  remove_prefix(idx).clear_replacing_bit_mark();
  return idx;
}

void MiniSeq::swap(MiniSeq& other) {
  using std::swap;
  swap(expand_info_, other.expand_info_);
  swap(replacing_bit_mark_, other.replacing_bit_mark_);
  View::swap(other);
}
