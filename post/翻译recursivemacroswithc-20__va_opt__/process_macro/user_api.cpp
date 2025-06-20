/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/04/09 01:21:04
# Desc   :
########################################################################
*/

#include "context.h"
#include "user_api.h"
#include "process_macro.h"

std::shared_ptr<Macro> API::add_macro(const std::string& macro_definition) {
  auto macro = MacroInfo::kMacros.emplace_back(std::make_shared<Macro>(macro_definition));
  MacroInfo::kNameToMacro.emplace(macro->name, macro);
  return macro;
}

std::string API::expand(View input) {
  ExpandInfo expand_info;

  Seq::consumed_type tmp;
  tmp.emplace_back(utils::trim(input));

  Seq input_seq(std::move(tmp), &expand_info);
  Context context {
    .expand_info = &expand_info,
    .input = std::move(input_seq)
  };
  return process_macro::expand(&context);
}
