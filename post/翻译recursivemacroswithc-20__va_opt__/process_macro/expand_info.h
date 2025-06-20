/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/04/14 14:12:56
# Desc   :
########################################################################
*/
#pragma once

#include <string>

#include "common.h"
#include "token.h"

struct ExpandInfo {
  std::unordered_set<View> replacing_bit;
  std::unordered_set<Token, TokenHash> unavailable_token;
  std::unordered_set<std::string*> temp_tokens;
  ~ExpandInfo() {
    for (auto ptr : temp_tokens) {
      delete ptr;
    }
  }
};  // class Expand
