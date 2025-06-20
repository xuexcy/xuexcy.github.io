/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/04/01 14:09:57
# Desc   :
########################################################################
*/
#pragma once

#include <compare>
#include <cstdint>
#include <format>
#include <memory>
#include <numeric>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "context.h"
#include "macro.h"
#include "utils.h"

namespace process_macro {

std::string expand(Context* context);

void process_tokens(Context *context);

std::tuple<StartIdx, Length, MacroPtr> get_a_macro(const Seq& input);

void process_macro(Token token, MacroPtr macro, Context* context);

Seq::consumed_type expand_macro_once(MacroPtr macro, Context* context);

std::optional<std::vector<std::pair<Seq, Seq>>> scan_arguments(Context* context);

// <参数，参数后面的 ',' 或 ')'>
std::optional<std::pair<Seq, Seq>> scan_argument(Context* context);

}  // namespace process_macro
