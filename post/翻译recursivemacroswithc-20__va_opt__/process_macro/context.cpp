/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/04/09 01:13:25
# Desc   :
########################################################################
*/

#include "context.h"

std::vector<MacroPtr> MacroInfo::kMacros;
std::unordered_map<View, MacroPtr> MacroInfo::kNameToMacro;

Token Context::consume_a_token(size_t len) {
  removed_length += len;
  return input.consume_a_token(len);
}

Seq::consumed_type Context::consume(size_t len) {
  auto seq = remove_prefix(len);
  return seq.consume(len);
}

Seq Context::remove_prefix(size_t len) {
  removed_length += len;
  auto len1 = std::min(input.size(), len);
  Seq res = input.remove_prefix(len1);
  len -= len1;
  if (len) {
    Seq res2 = rest.remove_prefix(len);
    res.splice(&res2);
  }
  return res;
}

size_t Context::consume_ltrim() {
  auto len = input.consume_ltrim();
  if (input.size() == 0) {
    len += rest.consume_ltrim();
  }
  removed_length += len;
  return len;
}

void Context::set_replacing_bit(View macro_name) {
  expand_info->replacing_bit.insert(macro_name);
  if (input.size()) {
    input.add_replacing_bit_mark(macro_name);
  } else if (rest.size()) {
    rest.add_replacing_bit_mark(macro_name);
  } else {
    // 当 rest 也为空时，说明已经将 context 中的所有字符[读取]完了，那么将这些字符[处理]完时,
    // 就需要将 replacing_bit_at_end 中的 macro_name 的 rb 重置,也就是调用 reset_replacing_bit_at_end()
    replacing_bit_at_end.emplace_back(macro_name);
  }
}

void Context::reset_replacing_bit_at_end() {
  if (size()) { return; }
  for (auto& macro_name : replacing_bit_at_end) {
    expand_info->replacing_bit.erase(macro_name);
  }
}

size_t Context::size() const {
  return input.size() + rest.size();
}

std::string Context::str() const {
  return std::format("output: {}, input: \"{}\" , rest: \"{}\"", output, input.str(), rest.str());
}

using CIter = Context::iterator;
CIter Context::begin() const {
  auto& tmp = (input.size() == 0 ? rest : input);
  return iterator(tmp.begin(), input.end(), rest.begin());
}

CIter Context::end() const {
  return iterator(rest.end(), input.end(), rest.begin());
}

CIter::iterator(
    CIter::DataIterator it, CIter::DataIterator input_end, CIter::DataIterator rest_begin)
    : it_(it), input_end_(input_end), rest_begin_(rest_begin) {}

CIter CIter::operator++() {
  ++it_;
  if (it_ == input_end_) {
    it_ = rest_begin_;
  }
  return *this;
}

bool CIter::operator==(const iterator& other) const {
  return it_ == other.it_;
}
bool CIter::operator!=(const iterator& other) const {
  return it_ != other.it_;
}
const CIter::value_type& CIter::operator*() const {
  return *it_;
}
