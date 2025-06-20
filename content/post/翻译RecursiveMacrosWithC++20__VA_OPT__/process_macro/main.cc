/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/04/01 14:12:31
# Desc   :
########################################################################
*/
#include <print>
#include <sstream>

#include "user_api.h"

std::string g_macros = R"(
#define OL 123
#define FL(x) ((x) + 1)
#define ID(arg) arg
#define LPAREN (
#define ID2(arg) arg
#define PARENS ()
#define F_AGAIN() F
#define F() f F_AGAIN PARENS()
#define ADD(x, y) x + y + xx + y + x + yy
#define EMPTY
#define A(x) (x*x)
#define B(x, y) x+y
)";

void load_macros() {
  std::istringstream iss(g_macros);
  std::string line;
  while (std::getline(iss, line)) {
    if (!line.empty()) {
      API::add_macro(line)->print();
    }
  }
}
void print_expand(const std::string& input, const std::string& excepted) {
  auto output = API::expand(input);
  std::println("input: \"{}\", output: \"{}\"", input, output);
  assert(output == excepted);
  std::println();
}

void run() {
  std::vector<std::pair<std::string, std::string>> vec {
    {"OL123", "OL123"},
    {"OL 123", "123 123"},
    {"FL(5)", "((5) + 1)"},
    {"FL(FL(5))", "((((5) + 1)) + 1)"},
    {"ID(ID)(ID)(X)", "ID(ID)(X)"},
    {"ID(ID2)(ID)(X)", "X"},
    {"ID(ID2 LPAREN)ID)(X)", "X"},
    {"ID(ID2 LPAREN ID))(X)", "ID(X)"},
    {"ID(ID2 (ID))(X)", "ID(X)"},
    {"F()","f F_AGAIN ()()"},
    {"ID(F())", "f f F_AGAIN ()()"},
    {"ID(ID(F()))", "f f f F_AGAIN ()()"},
    {"EMPTY ", ""},
    {" EMPTY", ""},
    {" EMPTY ", ""},
    {" EMPTY() ", "()"},
    {" EMPTY() )", "() )"},
    {"ID(EMPTY)", ""},
    {"ID( EMPTY() )", "()"},
    {"B(A(3), B)(4,5)+B(6,7)", "(3*3)+B(4,5)+6+7"},
  };
  for (auto& [input, excepted] : vec) {
    print_expand(input, excepted);
  }
}
void only_run() {
  // print_expand("ID(ID2)(ID)(X)", "X");
  // print_expand("OL123", "OL123");
  // print_expand("OL 123", "123 123");
  // print_expand("FL(5)", "((5) + 1)");
  // print_expand("FL(FL(5))", "((((5) + 1)) + 1)");
  // print_expand("ID(ID)(ID)(X)", "ID(ID)(X)");
  // print_expand("ID(ID2)(ID)(X)", "X");
  // print_expand("F()","f F_AGAIN ()()");
  print_expand("ID(F())", "f f F_AGAIN ()()");
  print_expand("ID(ID(F()))", "f f f F_AGAIN ()()");
}
int main() {
  load_macros();
  //only_run();
   run();
  return 0;
}
