/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/03/27 15:32:18
# Desc   :
########################################################################
*/

#include "process_macro.h"

#include <cassert>
#include <cctype>
#include <format>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "context.h"
#include "macro.h"
#include "utils.h"

namespace process_macro {

std::string expand(Context* context) {
  process_tokens(context);
  std::stringstream ss;
  for (auto& elem : context->output) {
    ss << elem;
  }
  return ss.str();
}

void splice(Seq::consumed_type&& from, Seq::consumed_type* to) {
  to->splice(to->end(), from);
}

void splice(Seq::consumed_type* from, Seq::consumed_type* to) {
  splice(std::move(*from), to);
}

void process_tokens(Context* context) {
  std::println("process_tokens: {}", context->str());
  // 调用本函数的地方
  //  1. 用户调用 user_api::expand: 此时 context 不可能有右空格
  //  2. 一次宏替换得到新的 input: 此时 new_context 不可能有右空格
  //  3. 宏替换前处理宏参数: 根据 scan_arguments 中的逻辑，参数没有消除右空格，我们需要在本函数 return 前将其 rtrim
  context->consume_ltrim();
  while (true) {
    auto [start_idx, len, macro] = get_a_macro(context->input);
    if (std::string::npos == start_idx) {
      auto tmp_output = context->consume(context->input.size());
      utils::rtrim(&tmp_output);
      splice(&tmp_output, &context->output);
      context->reset_replacing_bit_at_end();
      return;
    } else {
      if (start_idx) {
        splice(context->consume(start_idx), &context->output);
      }
      auto token= context->consume_a_token(len);
      process_macro(token, macro, context);
    }
  }
}

std::tuple<StartIdx, Length, MacroPtr> get_a_macro(const Seq& input) {
  auto [start_idx, len, it] = utils::get_an_valid_identifier(input,
      MacroInfo::kNameToMacro, [](auto& elem) {
        return elem.first;
      }
  );
  return { start_idx,len,  it != MacroInfo::kNameToMacro.end() ? it->second : nullptr };
}



void process_macro(Token token, MacroPtr macro, Context* context) {
  std::println("process_macro: {}", context->str());
  std::println("  macro_name: {}", macro->name);
  // 1. If T is the name of a macro for which the replacing bit is true, cpp sets the unavailable
  // bit on token T
  auto name = macro->name;
  auto expand_info = context->expand_info;
  if (auto it = expand_info->replacing_bit.find(name); it != expand_info->replacing_bit.end()) {
    expand_info->unavailable_token.emplace(token);
    // 当 input 和 rest 中没有了这个 token，它的 ub 其实可以被移除，如果 token 指向的 std::string 是 new 出来
    // 的，也可以 delete 掉. 这里可以节省一点内存以及查找 ub 的时间.
    // 只是这里不好标记 token 是否还在使用.
  }
  bool need_expand{false};
  bool ub_is_true = (expand_info->unavailable_token.find(token) != expand_info->unavailable_token.end());
  if (Macro::Type::ObjectLike == macro->type && !ub_is_true) {
    // 2. If T is the name of an object-like macro and T’s unavailable bit is clear, then T is
    // expanded.
    need_expand = true;
  } else if (Macro::Type::FunctionLike == macro->type && !ub_is_true) {
    // 3. If T is the name of a function-like macro, T’s unavailable bit is clear, and T is followed
    // by (, then T is expanded.
    StartIdx i{0};
    bool found_left_paren{false};
    for (auto ch : *context) {  // 找类函数宏的参数起始括号 '('
      if (ch == ' ') {
        ++i;
        continue;
      } else if (ch == '(') {
        found_left_paren = true;
        break;
      } else {
        break;
      }
    }
    if (found_left_paren) {
      need_expand = true;
      context->consume(i + 1);  // 读取空格 + 左括号
      context->consume_ltrim();
    }
  }
  if (need_expand) {
    auto new_input = Seq(expand_macro_once(macro, context), context->expand_info);
    std::println("new_input: {}", new_input.str());
    // Finally, cpp sets the replacing bit to true on the macro named T.
    context->set_replacing_bit(name);

    auto old_input_len = context->input.size();
    auto new_input_len = new_input.size();
    Seq new_rest(context->expand_info);
    new_rest.splice(&context->input);
    new_rest.splice(&context->rest);
    Context new_context {
      .expand_info = context->expand_info,
      .input = std::move(new_input),
      .rest = std::move(new_rest)
    };
    // With the replacing bit true, cpp continues processing input as usual from the tokens it just added to the input list.
    // This may result in more macro expansions, so is sometimes called the rescan phase.
    process_tokens(&new_context);
    // Once cpp has consumed all tokens generated from the substitution list, it clears the replacing bit on the macro named T.
    splice(&new_context.output, &context->output);
    if (new_context.removed_length - new_input_len <= old_input_len) {
      auto consumed_old_input_len = new_context.removed_length - new_input_len;
      old_input_len -= consumed_old_input_len;
      context->input = new_context.rest.remove_prefix(old_input_len);
      context->rest = new_context.rest.remove_all();
    } else {
      auto consumed_old_input_len = old_input_len;
      auto consumed_old_rest_len = new_context.removed_length - new_input_len - consumed_old_input_len;
      context->rest = new_context.rest.remove_all();
    }
      context->removed_length += new_context.removed_length - new_input_len;
  } else {
    // If cpp decides not to macro-expand T, it simply adds T to the current output token list
    context->output.emplace_back(name);
  }
}

Seq::consumed_type expand_macro_once(MacroPtr macro, Context* context) {
  std::println("expand_macro_once: {}", context->str());
  std::vector<Seq::consumed_type> expanded_arguments;
  if (Macro::Type::FunctionLike == macro->type) {
    // When T is a function-like macro
    // cpp scans all of the arguments supplied to T
    auto args = scan_arguments(context);
    if (!args.has_value()) {
      throw std::format(
        "ERROR: can't not scan arguments for T because of syntax error.\n"
        "Macro: {}\nSourceCode: {}",
        macro->to_string(), context->rest.str());
    }
    if (args.value().size() != macro->parameters.size()) {
      auto args_str = [&args]() {
        std::stringstream ss;
        for (auto& [arg, delimiter] : args.value()) {
          ss << arg.str() << delimiter.str();
        }
        return ss.str();
      }();
      // if T is called with an invalid number of arguments, then the program is ill-formed.
      throw std::format(
        "ERROR: T is called with an invalid number of arguments, then the program is ill-formed.\n"
        "Macro: {}\nArguments: ({}\nSourceCode: {}",
        macro->to_string(), args_str, context->rest.str());
    }
    for (auto& [arg, separator]: args.value()) {
      // and performs macro expansion on them
      // but instead of placing output tokens in the main preprocessor output,
      // it builds a replacement token list for each of T’s arguments
      arg.consume_ltrim();
      Context new_context {
        .expand_info = context->expand_info,
        .input = std::move(arg)
      };
      process_tokens(&new_context);
      expanded_arguments.emplace_back(new_context.output);
      separator.consume_all();
    }
  }
  // Cpp takes T’s substitution list
  // if T had arguments, replaces any occurrences of parameter names with the corresponding argument
  // token lists computed in step 1
  Seq::consumed_type replaced_res;
  for (auto& [view, arg_idx] : macro->sl) {
    if (arg_idx.has_value()) {
      auto arg = expanded_arguments[arg_idx.value()];  // copy arg
      splice(&arg, &replaced_res);
    } else {
      replaced_res.emplace_back(view);
    }
  }
  // It also performs stringification and pasting as indicated by # and ## in the substitution list.
  // TODO
  // replaced_res = process_hashtag(replaced_res);

  // It then logically prepends the resulting tokens to the input list.
  return replaced_res;
}

std::optional<std::vector<std::pair<Seq, Seq>>> scan_arguments(Context* context) {
  std::println("scan_arguments: {}", context->str());
  // 当没有逗号时，可能有 0 或 1 个参数, 比如 () 和 (a)
  // 当有逗号时，至少有两个参数, 比如 (a, b) 或 (a,) 或 (b,) 或 (a,,c,d,e)
  std::vector<std::pair<Seq, Seq>> args;
  while (context->size()) {
    auto arg = scan_argument(context);
    if (!arg.has_value()) {
      return std::nullopt;
    }
    bool is_empty_arg = (0 == arg.value().first.size());
    bool is_right_parenthesis = (*arg.value().second.begin() == ')');
    if (!(args.empty() && is_empty_arg && is_right_parenthesis)) {
      args.emplace_back(std::move(arg.value()));
    }

    if (is_right_parenthesis) {
      return args;
    }
  }
  return std::nullopt;
}

std::optional<std::pair<Seq, Seq>> scan_argument(Context* context) {
  // context->ltrim();
  // auto start_idx = rest.size() - ltrim(rest).size();
  // 1. 引号成对出现，比如 'a' , "abc(x+y)"
  // 2. 有左小括号必有右小括号, 但是可以没有左只有右, 比如 "(a + b)" 和 ") + b + c"
  // 3. 逗号在括号内不算外层参数列表的分隔符，可以跳过, 比如 "(a, b, c)",但如果逗号不再括号内，就要终止扫描,比如
  //    "(a, b), c" 的第 2 个逗号就要终止继续扫描，得到参数为 "(a, b)"
  size_t left_paren_count{0};
  char founded_quote{0};
  // size_t end_idx{start_idx};
  size_t len{0};
  Seq argument;
  for (auto ch : *context) {
    if (founded_quote) {
      if (ch == founded_quote) {
        founded_quote = 0;
      } // else continue  // 引号后的字符不需要终止
    } else {
      if (ch == '\'' || ch == '\"'){
        founded_quote = ch;
      } else if (ch == '(') {
        ++left_paren_count;
      } else if (ch == ')') {
        if (left_paren_count > 0) {
          --left_paren_count;
        } else {
          // 终止扫描, 当遇到一个多余的右括号，正好和参数列表开始的左括号匹配, 比如 F(a, b), 那么在
          // scan_arguments 开始时，我们扫描的是 "a, b)", 现在扫描完 b 后遇到一个没有匹配的右括号，
          // 那么参数列表就扫描完了
          return std::optional<std::pair<Seq, Seq>>(
              std::in_place, context->remove_prefix(len), context->remove_prefix(1));
          break;
        }
      } else if (ch == ',') {
        if (left_paren_count == 0) {
          return std::optional<std::pair<Seq, Seq>>(
              std::in_place, context->remove_prefix(len), context->remove_prefix(1));
          break;  // 终止扫描 (不在引号内也不在括号内)
        }
      }
      ++len;
    }
  }
  return std::nullopt;
}

}  // namespace process_macro
