[TOC]
翻译部分见:[index.md](./index.md),下文是对译文内容的一些补充.

# 使 `FOR_EACH` 的第一个参数 `macro` 的参数列表支持不同长度
原文中的 `FOR_EACH` 的第一个参数 `macro` 只能是接受一个参数的类函数宏,比如 `FOR_EACH(F, a, b, c, 1, 2, 3)`, 如果我们想要表达多个参数或变长参数的宏,就可能需要写成 `FOR_EACH(add, (1, 2), (3, 4))`, 解决办法如下
1. 根据输入的 tokens 是否有括号来判断是单一参数还是参数列表,比如 `1 2` 表示单一参数,`(1, 2)` 表示参数列表,参数分别为 `1`、`2`.
```cpp
#define HAS_PAREN(x) CHECK_HAS_PAREN(HAS_PAREN_PROBE x)
#define HAS_PAREN_PROBE(...)
#define NOT_0 1
#define NOT_01 0
#define CHECK_HAS_PAREN(...) NOT_0##__VA_OPT__(1)
// HAS_PAREN(1 2) -> CHECK_HAS_PAREN(HAS_PAREN_PROBE 1 2)
//    NOT_0##1 -> NOT_01 -> 0

// HAS_PAREN((1, 2)) -> CHECK_HAS_PAREN(HAS_PAREN_PROBE (1, 2)) -> CHECK_HAS_PAREN() ->
//    NOT_0## -> NOT_0 -> 1
```

2. 判断 tokens 是否有括号后,根据判断结果决定如何调用宏,如果没有括号,那就将 tokens 当成参数,即 `macro(tokens)`,如果有括号,那就将 tokens 当成参数列表,即 `macro tokens`
```cpp
#define IIF(c) IIF_IMPL(c)
#define IIF_IMPL(c) IIF_ ## c
#define IIF_0(t, f) f
#define IIF_1(t, f) t
// 当 arg 有括号时调用 macro arg
// 当 arg 没有括号时,给它添加一个括号,即调用 macro(arg)
#define CALL_MACRO(macro, arg) IIF(HAS_PAREN(arg))(macro arg, macro(arg))
```
4. 修改 `FOR_EACH_HELPER` 中对 `macro` 的调用方式
```cpp
#define FOR_EACH_HELPER(macro, a1, ...)                           \
  macro(a1)
  __VA_OPT__(FOR_EACH_AGAIN PARENS (macro, __VA_ARGS__))
                  |
                  |
                  V
#define FOR_EACH_HELPER(macro, a1, ...)                           \
  CALL_MACRO(macro, a1)                                                       \
  __VA_OPT__(FOR_EACH_AGAIN PARENS (macro, __VA_ARGS__))
```
5. 如此,参数 `macro` 就可以是任意的宏,使用样例见 [variable_argument_for_each_main.cc](./variable_argument_for_each_main.cc), 程序运行结果见[variable_argument_for_each_main.stdout](./variable_argument_for_each_main.stdout)
  - **不过, 对于 `#define PRINT_HELLO_2 std::println("hello2");` 这种类对象宏,当前的 `FOR_EACH` 依然无法处理**,当然，通过 `FOR_EACH` 多次调用没有参数的宏好像也没有什么意义
6. 另外，可以使用 `#define ARGS(...) (__VA_ARGS_)` 来区分 `(a, b, c)` 到底是一个参数 `(a, b, c)` 还是三个参数 `a`, `b`, `c`.
  - `FOR_EACH(PRINT, ARGS((a, b, c)))`: 一个参数 `(a, b, c)`
  - `FOR_EACH(PRINT, (a, b, c))`: 三个参数 `a`, `b`, `c`
  - `FOR_EACH(PRINT, ARGS(a, b, c))`: 三个参数 `a`, `b`, `c`

# 编译 `cpptorture.c` 时系统内存使用情况
- 电脑配置: macmini M4 芯片 + 16G 内存
- 内存使用情况见 [compile_cpptorture_memory_use](./attachment/compile_cpptorture_memory_use)
1. 使用 vscode 打开本目录,并在窗口打开 [cpptorture.c](attachment/cpptorture.c), vscode 插件 cland 开始扩展其中的宏,随后反复崩溃、重启.重启多次后不再重启.
2. 通过 `g++ cpptorture.c` 编译该程序, cc1plus 进程逐渐将内存耗尽,随后开始使用交换内存,消耗的交换内存也开始逐渐增多.
3. 数分钟后依然未完成编译.终止编译后内存回复使用.

原文中描述预处理器处理 `cpptorture.c` 需要数百年和数 `EB` 的内存.本次测试看到的现象基本符合预期.

# 根据原文 [c-macro-overview](https://www.scs.stanford.edu/~dm/blog/va-opt.html#c-macro-overview) 中的描述,实现一个简易的宏处理代码
代码见 [./process_macro](./process_macro/)
代码中的一些已知问题:
1. 没有实现对 `##` 和 `#` 的处理;没有实现 `__VA_ARGS__` 和 `__VA_OPT__`
2. 程序在处理宏调用出错时会直接抛出异常(比如参数个数不对),而不是返回一个错误码(传递错误码比较麻烦,懒得实现了)
