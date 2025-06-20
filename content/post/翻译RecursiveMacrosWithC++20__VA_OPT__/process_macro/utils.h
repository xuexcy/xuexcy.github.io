/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/04/01 15:11:57
# Desc   :
########################################################################
*/
#pragma once

#include <cassert>
#include <list>
#include <numeric>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "common.h"

namespace utils {

// ({1, 2, 3, 4}, ",")  ->  "1,2,3,4"
template <typename C, typename F>
std::string join_string(const C& c, const std::string& delimiter, F to_string = std::to_string) {
  return std::accumulate(
      c.begin(), c.end(), std::string(), [&](const std::string& a, const auto& b) {
        return a + (a.empty() ? "" : delimiter) + to_string(b);
      });
}

View ltrim(View input);
View rtrim(View input);
View trim(View input);
void ltrim(std::list<View>* input);
void rtrim(std::list<View>* input);
bool consume(View* input, size_t len);

// Identifiers such as FL, numbers, character literals, quoted strings, parentheses,
// and operators such as + are all examples of tokens.
template <typename Iterator>
std::optional<std::tuple<StartIdx, Iterator, std::string>> get_an_valid_identifier(
    Iterator begin, Iterator end) {
  bool found_identifier{false};
  bool found_valid_identifier{false};
  char quote{0};
  Idx cur_idx{0};
  Idx start_idx{std::string::npos};
  std::stringstream ss;
  for (; begin != end; ++begin, ++cur_idx) {
    auto ch = *begin;
    if (quote) {          // 如果在引号内则不用管,直接跳过
      if (quote == ch) {  // 如果又遇到了引号,将引号匹配重置
        quote = 0;
      }
      continue;
    }

    if (!found_identifier) {
      if (std::isalpha(ch) || ch == '_') {  // 开始一个合法标识符
        found_identifier = true;
        found_valid_identifier = true;
        start_idx = cur_idx;
        ss << ch;
      } else if (std::isdigit(ch)) {  // 开始一个不合法标识符或数字
        found_identifier = true;
      } else if (ch == '\'' || ch == '\"') {  // 开始一个引号
        quote = ch;
      }
    } else if (!(std::isalnum(ch) || ch == '_')) {  // 终止一个标识符
      if (found_valid_identifier) {  // 终止一个合法标识符
        break;
      }
      if (ch == '\'' || ch == '\"') {
        quote = ch;
      }
      found_identifier = false;
    } else {
      ss << ch;
    }
  }

  if (begin == end && !found_valid_identifier) {
    return std::nullopt;
  } else {
    return std::make_tuple(start_idx, begin, ss.str());
  }
}

template <
    typename T, typename Container,
    typename Getter = typename Container::value_type (*)(const typename Container::value_type&)>
std::tuple<StartIdx, Length, typename Container::const_iterator> get_an_valid_identifier(
    const T& input, const Container& c,
    Getter getter = [](const typename Container::value_type& value_type) ->
    typename Container::value_type { return value_type; }) {
  auto begin = input.begin();
  auto end = input.end();
  Idx start_idx{0};
  while (begin != end) {
    auto result = get_an_valid_identifier(begin, end);
    if (!result.has_value()) {
      return {std::string::npos, 0, c.end()};
    }
    auto& [tmp_start_idx, right_it, identifier] = result.value();
    start_idx += tmp_start_idx;
    for (auto it = c.begin(); it != c.end(); ++it) {
      if (identifier == getter(*it)) {
        return {start_idx, identifier.size(), it};
      }
    }
    start_idx += identifier.size();
    begin = right_it;
  }
  return {std::string::npos, 0, c.end()};
}

class Uncopyable {
protected:
  Uncopyable() = default;  // 没有这个，继承类将不能定义无参构造函数
  ~Uncopyable() = default;

private:
  Uncopyable(const Uncopyable&);
  Uncopyable& operator=(const Uncopyable&);
};  // class Uncopyable

}  // namespace utils
