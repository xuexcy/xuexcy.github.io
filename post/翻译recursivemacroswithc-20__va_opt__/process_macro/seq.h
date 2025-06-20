/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/04/14 14:15:43
# Desc   :
########################################################################
*/
#pragma once

#include <list>

#include "mini_seq.h"

class Context;

class Seq {
public:
  friend class Context;
  template <typename T>
  using container_type = std::list<T>;
  using element_type = MiniSeq;

  using data_type = container_type<element_type>;
  using data_iterator = data_type::const_iterator;
  using value_type = element_type::value_type;
  using consumed_type = container_type<element_type::consumed_type>;

private:
  data_type data_;
  size_t size_{0};
  ExpandInfo* expand_info_{nullptr};

public:
  Seq() = default;
  Seq(ExpandInfo* expand_info);
  Seq(data_type&& data, ExpandInfo* expand_info);
  Seq(consumed_type&& data, ExpandInfo* expand_info);
  Seq(const Seq& other) = delete;
  Seq(Seq&& other);
  Seq& operator=(Seq&& other);

  Token consume_a_token(size_t len);
  consumed_type consume(size_t len);
  consumed_type consume_all();
  Seq remove_prefix(size_t len);
  Seq remove_all();
  void clear_replacing_bit_mark();
  size_t consume_ltrim();
  size_t size() const;

  void swap(consumed_type& other);
  void swap(Seq& other);
  void append(View view);
  void splice(data_type* next_data);
  void splice(Seq* next_data);

  std::string str() const;
private:
  void add_replacing_bit_mark(View macro_name);

public:
  class iterator {
  public:
    using value_type = Seq::value_type;
    iterator(data_iterator it, Idx idx);
    iterator operator++();
    bool operator==(const iterator& other) const;
    bool operator!=(const iterator& other) const;
    const value_type& operator*() const;

  private:
    data_iterator it_;
    Idx idx_{0};
  };  // class iterator
  iterator begin() const;
  iterator end() const;
  int* a;
};  // class Seq
