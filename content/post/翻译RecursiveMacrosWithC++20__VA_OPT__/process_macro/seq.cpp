/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/04/14 14:15:58
# Desc   :
########################################################################
*/

#include "seq.h"

#include <cassert>
#include <print>
#include <sstream>

Seq::Seq(ExpandInfo* expand_info) : expand_info_(expand_info) {}

Seq::Seq(data_type&& data, ExpandInfo* expand_info)
    : data_(std::move(data)), expand_info_(expand_info) {
  for (auto& elem : data_) {
    assert(elem.expand_info_ == expand_info);
    size_ += elem.size();
  }
}

Seq::Seq(consumed_type&& data, ExpandInfo* expand_info) : expand_info_(expand_info) {
  for (auto& elem : data) {
    data_.emplace_back(std::move(elem), expand_info_);
    size_ += elem.size();
  }
}

Seq::Seq(Seq&& other)
    : data_(std::move(other.data_)), size_(other.size_), expand_info_(other.expand_info_) {
}

Seq& Seq::operator=(Seq&& other) {
  data_ = std::move(other.data_);
  size_ = other.size_;
  expand_info_ = other.expand_info_;
  return *this;
}

Token Seq::consume_a_token(size_t len) {
  // MiniSeq 中的 View 指向的 std::string 只有两种可能
  //  1. 用户的输入 input
  //  2. 宏定义中 substitution list
  // 所以不用担心 View 指向的 std::string 的生命周期问题
  //
  // 当 token 属于多个不同的 MiniSeq 时，说明 token 并不存在于已有的 std::string 中,
  // 此种情况下我们需要 new 一个 std::string 来保证 token 指向的 std::string 不被销毁
  auto prefix = consume(len);
  if (prefix.size() == 1) {
    return {prefix.begin()->begin(), len};
  } else {
    std::string res;
    for (auto& mini_seq : prefix) {
      res += std::string(mini_seq);
    }
    auto token = new std::string(res);
    expand_info_->temp_tokens.insert(token);
    return { &(*token->begin()), token->size() };
  }
}

Seq::consumed_type Seq::consume(size_t len) {
  Seq seq = remove_prefix(len);
  seq.clear_replacing_bit_mark();
  consumed_type res;
  for (auto& mini_seq : seq.data_) {
    res.emplace_back(mini_seq);
  }
  return res;
}

Seq::consumed_type Seq::consume_all() {
  return consume(size());
}

Seq Seq::remove_prefix(size_t len) {
  size_ -= len;
  data_type prefix;
  while (len) {
    auto& mini_seq = data_.front();
    if (len >= mini_seq.size()) {
      len -= mini_seq.size();
      prefix.emplace_back(std::move(mini_seq));
      data_.pop_front();
    } else {
      prefix.emplace_back(mini_seq.remove_prefix(len));
      break;
    }
  }
  return {std::move(prefix), expand_info_};
}

Seq Seq::remove_all() {
  Seq res(std::move(*this));
  return res;
}

void Seq::clear_replacing_bit_mark() {
  for (auto& mini_seq : data_) {
    mini_seq.clear_replacing_bit_mark();
  }
}

size_t Seq::consume_ltrim() {
  size_t space_len{0};
  while (!data_.empty()) {
    auto& mini_seq = data_.front();
    space_len += mini_seq.consume_ltrim();
    if (mini_seq.empty()) {
      data_.pop_front();
    } else {
      break;
    }
  }
  size_ -= space_len;
  return space_len;
}

size_t Seq::size() const {
  return size_;
}

//void Seq::swap(consumed_type& other) {
//  std::swap(data_, other);
//  for (auto& elem : data_) {
//    other.emplace_back(elem);
//  }
//  data_.clear();
//}
//
// void Seq::swap(Seq& other) {
//   using std::swap;
//   swap(data_, other.data_);
//   swap(size_, other.size_);
//   swap(expand_info_, other.expand_info_);
// }

void Seq::append(View view) {
  data_.emplace_back(view, expand_info_);
  size_ += view.size();
}
void Seq::splice(data_type* next_data) {
  for (auto& elem : *next_data) {
    size_ += elem.size();
  }
  data_.splice(data_.end(), *next_data);
}

void Seq::splice(Seq* next_data) {
  data_.splice(data_.end(), next_data->data_);
  size_ += next_data->size_;
  next_data->size_ = 0;
}

std::string Seq::str() const {
  std::stringstream ss;
  for (auto& elem : data_) {
    ss << elem;
  }
  return ss.str();
}

void Seq::add_replacing_bit_mark(View macro_name) {
  auto& front = data_.front();
  front.add_replacing_bit_mark(front.begin(), macro_name);
}

Seq::iterator Seq::begin() const {
  return iterator(data_.begin(), 0);
}

Seq::iterator Seq::end() const {
  return iterator(data_.end(), 0);
}

Seq::iterator::iterator(data_iterator it, Idx idx) : it_(it), idx_(idx) {}

Seq::iterator Seq::iterator::operator++() {
  ++idx_;
  if (idx_ >= it_->size()) {  // MiniSeq 可能是空的，比如 "#define EMPTY" 发生替换后，就得到了空的结果
    ++it_;
    idx_ = 0;
  }
  return *this;
}

bool Seq::iterator::operator==(const iterator& other) const {
  return it_ == other.it_ && idx_ == other.idx_;
}

bool Seq::iterator::operator!=(const iterator& other) const {
  return !operator==(other);
}

const Seq::iterator::value_type& Seq::iterator::operator*() const {
  return it_->at(idx_);
}
