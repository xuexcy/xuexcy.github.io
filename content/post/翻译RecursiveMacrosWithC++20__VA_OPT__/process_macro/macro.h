/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/04/01 15:18:46
# Desc   :
########################################################################
*/
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "common.h"
#include "utils.h"

/*
  #define OL 123

  name: OL
  type: Macro::Type::ObjectLike
  parameters: {}
  sl: ""
*/
/*
  #define FL(x) ((x) + 1)

  name: FL
  type: Macro::Type::FunctionLike
  parameters: {"x"}
  sl: "((x)+1)"
*/

struct Macro : public utils::Uncopyable {
  std::string macro_definition;
  View name;
  enum class Type { ObjectLike, FunctionLike };  // enum class Type
  Type type;
  std::vector<View> parameters;
  std::vector<std::pair<View, std::optional<Idx>>> sl;

  Macro(const std::string& macro_definition_);
  std::string to_string() const;  // 结果比 macro_definition 少一些无用的空格
  void print() const;

private:
  std::string format_parameters() const;
  std::string format_substitution_list() const;
  std::string format_substitution_list_to_parameter() const;

  bool consume_define(View* input);
  bool consume_macro_name(View* input);
  bool consume_macro_parameters(View* input);
  void consume_substitution_list(View* input);
  std::optional<View> consume_an_valid_identifier(View* input);
};  // class Macro
