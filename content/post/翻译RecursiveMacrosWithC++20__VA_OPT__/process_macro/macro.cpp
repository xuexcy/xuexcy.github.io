/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/04/01 15:26:43
# Desc   :
########################################################################
*/
#include "macro.h"

#include <format>
#include <optional>
#include <print>

#include "common.h"
#include "utils.h"

Macro::Macro(const std::string& macro_definition_) : macro_definition(macro_definition_) {
  auto throw_exception = [&](auto& err_msg) {
    throw std::format("Not a macro: {}\nERROR_MSG: {}", macro_definition, err_msg);
  };

  View input = macro_definition;
  input = utils::trim(input);

  // 读取 #define
  if (!consume_define(&input)) {
    throw_exception("\"#define\" not found");
  };
  // 读取宏名字
  if (!consume_macro_name(&input)) {
    throw_exception("macro name not found");
  }

  // 读取宏的类型,注意，'(' 括号必须紧跟宏名字，所以这里不要先 ltrim
  type = input.starts_with('(') ? Type::FunctionLike : Type::ObjectLike;
  if (type == Type::ObjectLike) {
    input = utils::ltrim(input);
    consume_substitution_list(&input);
    return;
  }

  input = input.substr(1);  // 去掉 '('

  // 读取宏的参数
  if (!consume_macro_parameters(&input)) {
    throw_exception("macro parameters not valid");
  }
  input = utils::ltrim(input);
  consume_substitution_list(&input);
}

View copy_view(const std::string& source, const std::string& dest, View view) {
  return View(dest.data() + (view.data() - source.data()), view.size());
}

bool Macro::consume_define(View* input) {
  static const std::string kDefine{"#define "};  // 注意，后面有一个空格

  *input = utils::ltrim(*input);
  if (input->size() < kDefine.size()) {
    return false;
  }
  if (input->substr(0, kDefine.size()) != kDefine) {
    return false;
  }
  *input = input->substr(kDefine.size());
  return true;
}

bool Macro::consume_macro_name(View* input) {
  if (auto res = consume_an_valid_identifier(input); res.has_value()) {
    this->name = res.value();
    return true;
  } else {
    return false;
  }
}

std::optional<View> Macro::consume_an_valid_identifier(View* input) {
  *input = utils::ltrim(*input);
  auto result = utils::get_an_valid_identifier(input->begin(), input->end());
  if (!result.has_value()) {
    return std::nullopt;
  }
  auto& [start_idx, end_it, str] = result.value();
  if (start_idx != 0) {
    return std::nullopt;
  }
  auto res = input->substr(0, str.size());
  input->remove_prefix(str.size());
  return res;
}

bool Macro::consume_macro_parameters(View* input) {
  // case 1: ")"
  // case 2: "a)"
  // case 3: "a, b)"
  bool need_comma{false};
  while (!input->empty()) {
    *input = utils::ltrim(*input);
    if (input->starts_with(')')) {
      *input = input->substr(1);
      return true;
    } else if (need_comma) {
      if (input->starts_with(',')) {
        *input = input->substr(1);
      } else {
        return false;
      }
    }

    auto parameter = consume_an_valid_identifier(input);
    if (parameter.has_value()) {
      parameters.emplace_back(parameter.value());
      need_comma = true;
      continue;
    } else {
      return false;
    }
  }
  return false;
}

void Macro::consume_substitution_list(View* input) {
  if (input->empty()) {  // 替换列表可能为空，比如 #define EMPTY
    sl.emplace_back(*input, std::nullopt);
    return;
  }
  while (!input->empty()) {
    auto [start_idx, len, it] = utils::get_an_valid_identifier(*input, parameters);
    if (start_idx == std::string::npos) {
      sl.emplace_back(*input, std::nullopt);
      return;
    } else {
      if (0 != start_idx) {
        sl.emplace_back(input->substr(0, start_idx), std::nullopt);
        *input = input->substr(start_idx);
      }
      sl.emplace_back(input->substr(0, len), it - parameters.begin());
      *input = input->substr(len);
    }
  }
}

std::string Macro::to_string() const {
  return std::format("#define {}{} {}", name, format_parameters(), format_substitution_list());
}

void Macro::print() const {
  std::println("definition: \"{}\"", macro_definition);
  std::println("name: \"{}\"", name);
  std::println("type: {}", type == Type::ObjectLike ? "ObjectLike" : "FunctionLike");
  std::println("parameters: {}", format_parameters());
  std::println("substitution list: {}", format_substitution_list());
  std::println(
      "substitution_list_to_parameter:{{\n{}\n}}", format_substitution_list_to_parameter());
  std::println();
}

std::string Macro::format_parameters() const {
  if (Type::ObjectLike == type) {
    return "";
  } else {
    return std::format(
        "({})", utils::join_string(parameters, ", ", [](auto& elem) { return std::string(elem); }));
  }
}

std::string Macro::format_substitution_list() const {
  return std::format(
      "\"{}\"", utils::join_string(sl, "", [](auto& elem) { return std::string(elem.first); }));
}

std::string Macro::format_substitution_list_to_parameter() const {
  return std::format(
      "{}", utils::join_string(sl, ",\n", [this](auto& elem) {
        return std::format(
            "    \"{}\"{}", elem.first,
            !elem.second.has_value()
                ? ""
                : std::format(
                      " : ({}, {})", parameters.at(elem.second.value()), elem.second.value()));
      }));
}
