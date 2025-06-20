/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/03/26 00:32:20
# Desc   : ref: https://www.scs.stanford.edu/~dm/blog/va-opt.html
########################################################################
*/

#include <print>

#define STRINGIFY(...) #__VA_ARGS__
#define TOSTRING(...) STRINGIFY(__VA_ARGS__)

#define PARENS ()

#define EXPAND(...) EXPAND4(EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__))))
#define EXPAND4(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
#define EXPAND3(...) EXPAND2(EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__))))
#define EXPAND2(...) EXPAND1(EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__))))
#define EXPAND1(...) __VA_ARGS__

/*
假设 x 有括号, 形如 (a, b, c)
  HAS_PAREN(x) -> CHECK_HAS_PAREN(HAS_PAREN_PROBE (a, b, c)) -> CHECK_HAS_PAREN() -> NOT_0 -> 1
假设 x 没有有括号, 形如 abc
  HAS_PAREN(x) -> CHECK_HAS_PAREN(HAS_PAREN_PROBE abc)) -> NOT_01 -> 1
*/
#define HAS_PAREN(x) CHECK_HAS_PAREN(HAS_PAREN_PROBE x)
#define HAS_PAREN_PROBE(...)
#define NOT_0 1
#define NOT_01 0
#define CHECK_HAS_PAREN(...) NOT_0##__VA_OPT__(1)

#define IIF(c) IIF_IMPL(c)
#define IIF_IMPL(c) IIF_ ## c
#define IIF_0(t, f) f
#define IIF_1(t, f) t

// 当 arg 有括号时调用 macro arg
// 当 arg 没有括号时,给它添加一个括号,即调用 macro(arg)
#define CALL_MACRO(macro, arg) IIF(HAS_PAREN(arg))(macro arg, macro(arg))

// (a,b,c) : 三个参数 {a, b, c}
// ARGS(a,b,c): 三个参数 {a, b, c}
// ARGS((a,b,c)): 一个参数 {(a,b,c)}
#define ARGS(...) (__VA_ARGS__)

#define FOR_EACH(macro, ...)                                      \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER(macro, __VA_ARGS__)))
#define FOR_EACH_HELPER(macro, a1, ...)                           \
  CALL_MACRO(macro, a1)                                                       \
  __VA_OPT__(FOR_EACH_AGAIN PARENS (macro, __VA_ARGS__))
#define FOR_EACH_AGAIN() FOR_EACH_HELPER

#define PRINT(a, b) std::println("{}+{}={}", a, b, a + b);
#define PRINT_STRING(value) std::println("value: {}", TOSTRING(value));

#define EXPAND_ADD(expand_macro, ...) __VA_OPT__(expand_macro(EXPAND_ADD_HELPER(__VA_ARGS__)))
#define EXPAND_ADD_HELPER(x, ...) x __VA_OPT__(+ EXPAND_ADD_AGAIN PARENS(__VA_ARGS__))
#define EXPAND_ADD_AGAIN() EXPAND_ADD_HELPER
#define PRINT_ADD(...) std::println("add expression: {}", TOSTRING(EXPAND_ADD(EXPAND, __VA_ARGS__)));
#define PRINT_ADD_2(...) std::println("add expression: {}", TOSTRING(EXPAND_ADD(A_EXPAND, __VA_ARGS__)));

#define PRINT_HELLO() std::println("hello");
#define PRINT_HELLO_2 std::println("hello2");

void run_for_each() {
  std::println("FOR_EACH PRINT");
  FOR_EACH(PRINT, (1, 2), (3, 4), ARGS(5, 6));
  std::println();

  std::println("FOR_EACH PRINT_STRING first time");
  FOR_EACH(PRINT_STRING, PRINT_STRING, 1, (2), ARGS(3), ARGS((4)), "5", ("6"), ARGS("7"), ARGS(("8")));
  std::println();

  std::println("FOR_EACH PRINT_STRING second time");
  FOR_EACH(PRINT_STRING, PRINT_STRING, a, a + b, (a), (a + b), ARGS(a), ARGS(a + b), ARGS((a)), ARGS((a + b)), ARGS(((a + b), c)));
  std::println();

  std::println("FOR_EACH PRINT_ADD");
  FOR_EACH(PRINT_ADD, , (), a, a + b, (a, b), ARGS(a, b), ARGS((a, b)), ARGS((a, b), c, (d, e)));
  std::println();

  std::println("FOR_EACH PRINT_ADD_2");
  FOR_EACH(PRINT_ADD_2, , (), a, a + b, (a, b), ARGS(a, b), ARGS((a, b), c, (d, e)));
  std::println();

  std::println("FOR_EACH PRINT_HELLO");
  FOR_EACH(PRINT_HELLO, , (), , ());
  std::println();

  std::println("FOR_EACH PRINT_HELLO_2");
  // FOR_EACH(PRINT_HELLO_2, , , , );
  std::println("FOR_EACH can not handle object-like macro PRINT_HELLO_2");
  std::println();
}
int main() {
  run_for_each();
  return 0;
}

