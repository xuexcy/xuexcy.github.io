/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/04/01 15:29:23
# Desc   :
########################################################################
*/

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
#include <string>

#define OL 123
#define FL(x, y) ((x) + (y))

// #define ID(arg) arg
// #define ID2(arg) ID
// #define LPAREN (
// #define ID3(x) xx
// #define O ID(ID2)(ID)(X)
// #define P ID(ID2 LPAREN)ID)(X)
// #define Q ID(ID2 LPAREN I)D)(X)
// #define R ID(ID2 LPAREN ID))(X)
//
// #define S ID(ID ID2 LPAREN)ID)(X)
// #define Y ID(ID ID2 LPAREN I)D)(X)
// #define Z ID(ID ID2 LPAREN ID))(X)
// replace
#define F()
// 辅助宏用于检测参数数量
#define CHECK(...) CHECK_(__VA_ARGS__, 1, 0)
#define CHECK_(x, y, ...) y

// 检测宏是否为类函数宏
#define IS_FUNCTION(m) CHECK(TRIGGER_PARENS m)

// 触发括号展开的辅助宏
#define TRIGGER_PARENS(...) ,


// 示例宏定义
#define OBJECT_MACRO 100
#define FUNCTION_MACRO(x) x

//#define _GET_F_TYPE(F) _IS_FUNCTION_LIKE(_FUNCTION_LIKE_##F)
//#define _FUNCTION_LIKE_(_)  // 类函数宏会展开为 _FUNCTION_LIKE_()
//#define _IS_FUNCTION_LIKE(...) _IS_FUNCTION_LIKE_(__VA_ARGS__)
//#define _IS_FUNCTION_LIKE_() 0
//#define _IS_FUNCTION_LIKE_(...) 1

int main() {
  //std::println("{}", TOSTRING(FL( 1 2 , 3 )));
  std::println("{}", IS_FUNCTION(F));
  std::println("{}", IS_FUNCTION(OBJECT_MACRO));
  return 0;
}
