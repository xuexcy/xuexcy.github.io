/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/04/01 15:17:17
# Desc   :
########################################################################
*/

#include "utils.h"
#include <list>
#include <ranges>

namespace utils {

View ltrim(View input) {
  auto start = input.find_first_not_of(' ');
  return start == View::npos ? input.substr(input.size()) : input.substr(start);
}

View rtrim(View input) {
  auto end = input.find_last_not_of(' ');
  return end == View::npos ? input.substr(0, 0) : input.substr(0, end + 1);
//  int32_t idx = input.size() - 1;
//  while (idx >=0 && input[idx] == ' ') { --idx; }
//  return idx >= 0 ? input.substr(0, idx + 1) : "";
}

View trim(View input) {
  return rtrim(ltrim(input));
}

void ltrim(std::list<View>* input) {
  for (auto& elem : *input) {
    elem = ltrim(elem);
    if (!elem.empty()) {
      break;
    }
  }
  while (!input->empty() && input->front().empty()) {
    input->pop_front();
  }
}

void rtrim(std::list<View>* input) {
  for (auto& elem : *input | std::views::reverse) {
    elem = rtrim(elem);
    if (!elem.empty()) {
      break;
    }
  }
  while (!input->empty() && input->back().empty()) {
    input->pop_back();
  }
}

bool consume(View* input, size_t len) {
  if (len > input->size()) {
    return false;
  }
  *input = input->substr(len);
  return true;
}

}  // namespace utils
