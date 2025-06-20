/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/04/09 01:19:14
# Desc   :
########################################################################
*/
#pragma once

#include <memory>

#include "macro.h"

class API {
public:
  static std::shared_ptr<Macro> add_macro(const std::string& macro_definition);
  static std::string expand(View input);
};  // class API
