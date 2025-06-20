/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/04/14 14:11:23
# Desc   :
########################################################################
*/
#pragma once

#include "common.h"

struct Token {
  const char* begin;
  Length len;
  bool operator==(const Token& other) const;
  View str() const;
};  // class Token

struct TokenHash {
  size_t operator()(const Token& token) const;
};  // struct TokenHash
