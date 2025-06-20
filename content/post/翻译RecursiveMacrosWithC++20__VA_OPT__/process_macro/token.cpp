/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/04/14 14:12:04
# Desc   :
########################################################################
*/

#include "token.h"

bool Token::operator==(const Token& other) const {
  return begin == other.begin && len == other.len;
}

View Token::str() const {
  return View(begin, len);
}

size_t TokenHash::operator()(const Token& token) const {
  return std::hash<const void*>()(token.begin) ^ (std::hash<Length>()(token.len) << 1);
}

