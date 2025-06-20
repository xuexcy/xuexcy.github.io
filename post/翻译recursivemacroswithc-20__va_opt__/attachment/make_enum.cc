/* compile with:
   c++ -std=c++20 -Wall -Werror make_enum.cc -o make_enum
 */

#include <iostream>

#define PARENS ()

// Rescan macro tokens 256 times
#define EXPAND(arg) EXPAND1(EXPAND1(EXPAND1(EXPAND1(arg))))
#define EXPAND1(arg) EXPAND2(EXPAND2(EXPAND2(EXPAND2(arg))))
#define EXPAND2(arg) EXPAND3(EXPAND3(EXPAND3(EXPAND3(arg))))
#define EXPAND3(arg) EXPAND4(EXPAND4(EXPAND4(EXPAND4(arg))))
#define EXPAND4(arg) arg

#define FOR_EACH(macro, ...)                                    \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER(macro, __VA_ARGS__)))
#define FOR_EACH_HELPER(macro, a1, ...)                         \
  macro(a1)                                                     \
  __VA_OPT__(FOR_EACH_AGAIN PARENS (macro, __VA_ARGS__))
#define FOR_EACH_AGAIN() FOR_EACH_HELPER

#define ENUM_CASE(name) case name: return #name;

#define MAKE_ENUM(type, ...)                    \
enum type {                                     \
  __VA_ARGS__                                   \
};                                              \
constexpr const char *                          \
to_cstring(type _e)                             \
{                                               \
  using enum type;                              \
  switch (_e) {                                 \
  FOR_EACH(ENUM_CASE, __VA_ARGS__)              \
  default:                                      \
    return "unknown";                           \
  }                                             \
}

MAKE_ENUM(MyType, ZERO, ONE, TWO, THREE);

void
test(MyType e)
{
  std::cout << to_cstring(e) << " = " << e << std::endl;
}

int
main()
{
  test(ZERO);
  test(ONE);
  test(TWO);
  test(THREE);
}

/*
  Local Variables:
  c-macro-preprocessor: "c++ -x c++ -std=c++20 -E -"
  End:
*/
