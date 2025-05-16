---
title: 翻译RecursiveMacrosWithC++20__VA_OPT__
description: 使用 C++20 __VA_OPT__ 特性实现递归宏
date: 2025-04-18T16:22:10+08:00
draft: false
categories:
  - cpp
tags:
  - cpp
  - 宏
  - 翻译
math: mathjax
---
[TOC]
# 翻译: [Recursive macros with C++20 \_\_VA\_OPT\_\_](https://www.scs.stanford.edu/~dm/blog/va-opt.html)
- **水平有限,欢迎指正,谢谢!**
- 多平台发布内容不一致时，以 github 更新为主
- 个人添加的一些注解可能有些啰嗦,大部分都是在反反复复解释不同的宏是如何展开了,主要是为了记录自己理解原文内容过程中的一些思考,各位阅读时可以跳过.
## 译者前言
- 标题: 使用 C++20 的 \_\_VA\_OPT\_\_ 实现递归宏
- 原文信息:
  - 作者: David Mazières
  - 日期: 2021/06
- 译文信息:
  - 作者: 薛成韵(xuechengyun)
  - 日期: 2025/03/26
  - 邮箱:
    - xuechengyunxue@gmail.com
    - 1433094974@qq.com
  - github 地址: [link](https://github.com/xuexcy/public_attachments/tree/main/翻译RecursiveMacrosWithC++20__VA_OPT__/翻译.md)
- 文中一些名词与解释:
  - preprocessor: 预处理器,文中提到的 C/C++ pre-preprocessor, cpp 都翻译成预处理器
  - variable-argument macro: 可变参数宏, 如 `#define PRINT(...) print(__VA_ARGS__)`
  - macro type: 宏类型
    - object-like macro: 类对象宏,如 `#define OL 123`
    - function-like macro: 类函数宏,如 `#define FL(x) ((x)+1)`
  - macro name: 宏名字,如 `OL`,`FL`
  - macro parameters: 宏参数,如 `x`
  - substitution list: 替换列表,宏名字和参数后的那部分,如 `123`,`((x)+1)`, 另外，替换列表可以为空, 比如 `#define EMPTY`
  ```cpp
  using Str = std::string;
  struct Macro {
    Str name;
    enum class Type { ObjectLike, FunctionLike };
    Type type;
    std::vector<Str> parameters;
    Str sl;  // substitution list
  };  // struct Macro
  ```
  - replacing bit: 替换位,后文简称为 rb. 一个宏对应一个 bit, 当一个宏发生了替换后就会将对应的 bit 置为 true，后续将会处理替换产生的新结果.在读取到替换结果之后的一个字符时，该 bit 会被重置为 false.
    - `FL(x)+FL(y)` : 当 `FL(x)` 替换成 `((x) + 1)` 后, `FL` 的 replacing bit 会被置为 `true`, 将替换结果 `((x) + 1)` 继续处理完后, 会读取 `FL(x)` 后面的加号 `+`，在读取 `+` 前会将 `FL` 的 replacing bit 重置为 `false`
  - unavailable bit: 不可用位(或者叫不可扩展位吧), 后文简称为 ub. 标记某个 token 是否能被扩展,一个 token 对应一个 bit.
  - token: 一种符号,如 `FL`,`123`, `x`,`+`,`1`,`(`, `)`;两个值相同的字符串可能是同一个 token，也可能是不同的 token
    > 参数和替换列表中对应的 token 是同一个 token，可以认为是同一个字符串的引用.
    > 当 token 在参数中不能展开时，替换到替换列表后依然不能展开.
    > ```cpp
    > #define ADD(a, b) a + b
    > #define DOUBLE(c) ADD(c, c)
    > #define ADD_FIVE(d) ADD(d, FIVE)
    > #define FIVE 5
    > ```
    >
    > 假设此时 FIVE 不能被展开
    >
    > **DOUBLE(FIVE)**
    > 1. 展开 `DOUBLE(FIVE)` 得到 `ADD(FIVE, FIVE)`，此时 `ADD` 的两个参数 `a: FIVE` 和 `b: FIVE` 是同一个 token，也就是 `DOUBLE` 的参数 `c: FIVE` 这个 token
    > 2. 展开得到 `ADD(FIVE, FIVE)` 得到 `FIVE + FIVE`, 此时这两个 `FIVE` 分别对应 `ADD` 的参数 `a: FIVE`, `b: FIVE`, 而上一步已知 `a: FIVE` 和 `b: FIVE` 是同一个 token, 于是 `FIVE + FIVE` 中的两个 `FIVE` 也还是同一个 token
    >
    > **ADD_FIVE(FIVE)**
    > 1. 展开 `ADD_FIVE(FIVE)` 得到 `ADD(FIVE, FIVE)`, 此时这两个参数 `a: FIVE` 和 `b: FIVE` 不是同一个 token, 其中 `a: FIVE` 是 `ADD_FIVE` 的参数 `d: FIVE`, `b: FIVE` 是 `ADD_FIVE` 的替换列表中的 `FIVE`
    > 2. 展开 `ADD(FIVE, FIVE)` 得到 `FIVE + FIVE`, 此时这两个 `FIVE` 分别对应 `ADD` 的参数 `a: FIVE`, `b: FIVE`, 而上一步已知 `a: FIVE` 和 `b: FIVE` 不是同一个 token, 于是 `FIVE + FIVE` 中的两个 `FIVE` 就不是同一个 token
    >

- 注意:
  - 文中存在一些圆括号`()`,一些是原文已有的,一些是译者额外加的.译者加的圆括号中,里面的内容可能是原文英文,形如"译文(原文)",也有可能是译文,形如"原文(译文)",比如 "pre-processor(预处理)、类对象宏(*object-like macros*).另外还可能添加了一些注释.
  - 标有"xuechengyun 注"的地方都是个人对原文的一些理解、解释或附加说明,主要是为了更方便的理解原文
- 参考:
  - [知乎: 聊一聊新的宏\_\_VA\_OPT\_\_](https://zhuanlan.zhihu.com/p/683808863)

## Introduction(引言)
如果能在 c++ 中定义可以被优雅打印的枚举岂不是很好? 在 C++20 中,你可以定义一个宏,这个宏可以同时创建一个枚举类型和定义一个将枚举值转换成字符串的函数.样例如下:

```cpp
MAKE_ENUM(MYType, ZERO, ONE, TWO, THREE);

void test(MyType e) {
  std::cout << to_cstring(e) << " = " << e << std::endl;
}

int main() {
  test(ZERO);
  test(ONE);
  test(TWO);
  test(THREE);
}
```
**OUTPUT**:
```
ZERO = 0
ONE = 1
TWO = 2
THREE = 3
```
实现上述功能的关键是 C++20 中的一个新的 pre-processor(预处理器)特性 `__VA_OPT__(x)`.当 variable-argument marco(可变参数宏)的参数个数非 0 时,它可以展开成 `x`,否则什么也不做.该特性正是实现递归宏中基本情况所需的,它可以做一些事情,比如让 `FOR_EACH` 将它的每一个参数都填充到另一个宏中.下面是如何使用 `FOR_EACH` 定义 `MAKE_ENUM`:

> xuechengyun 注:
>
> `FOR_EACH(PRINT, a, b, c)` 可以扩展为
> ```cpp
> PRINT(a)
> PRINT(b)
> PRINT(c)
> ```
> 这样就可以通过宏 `PRINT` 输出所有参数.
>
```cpp
#define ENUM_CASE(name) case name: return #name;

#define MAKE_ENUM(type, ...)                  \
  enum type { __VA_ARGS__ };                  \
  constexpr const char* to_cstring(type _e) { \
    using enum type;                          \
    switch (_e) {                             \
      FOR_EACH(ENUM_CASE, __VA_ARGS__)        \
      default:                                \
        return "unknown";                     \
    }                                         \
  }
```
完整代码见 [make_enum.cc](https://www.scs.stanford.edu/~dm/blog/make_enum.cc).本博客余下部分将会讲解如何通过一种简单且普遍的方法来定义 `FOR_EACH`.虽然参数列表的大小有一个(任意的)限制,但是该限制随着代码长度增大呈指数级增长.加上 5 行额外的代码,接受的参数就可以超过 300 个,如此数量看起来已经很多了.另外相较于旧方法中,为每种可能数量的参数都各自定义一个宏来说,这种方法更容易被接受.

在这个博客中,我将讲解 C/C++ 中的宏是如何运作的;接着展示如何将宏和 C++20 中的 `__VA__OPT__` 结合起来做一些牛逼的事.

## C macro overview (概述 C 语言中的宏)
C 和 C++ 支持两种类型的宏,一种是没有参数的类对象宏(*object-like* macros),一种是有参数的类函数宏(*function-like* macros).下面是例子.

```cpp
#define OL 123         // object-like macro 类对象宏
#define FL(x) ((x)+1)  // function-like macro 类函数宏
```
宏的主体被称为*替换列表*(*substitution list*),即宏名字和可选参数后面部分.如上单个 token `123` 是 `OL` 的替换列表,token 列表 `((x)+1)` 是 `FL` 的替换列表.

宏扩展发生在 C/C++ 预处理器(缩写 cpp, 即 C/C++ preprocess,后文简称"预处理器",以免和 c++ 程序语言混淆)将程序源代码转换为一系列词法标记(lexical tokens)之后.标识符(identifiers)(例如 `FL`),数字(numbers),字符字面量(character literals), 字符串(quoted strings),圆括号(parentheses, `(`, `)` ),还有操作符(例如 `+` ) 都是 token 示例.预处理器通过将很多 tokens 原样复制,有效地将输入 tokens 列表转换为输出 tokens, 并在适当的地方将宏展开.

预处理器旨在保证源代码的预处理一定会终止(termination).个人认为在编程语言中 termination 被看得过重了(被高估了, overrated).我的意思是,C 语言明确的知道一个微不足道的 [cpptorture.c](https://www.scs.stanford.edu/~dm/blog/cpptorture.c) 程序的编译需要超过100年的时间和数艾字节(exabytes, EB, 1EB = 1024PB=2^60B)的内存,但知道这一事实又有什么可欣慰的呢?有些离题了.实际上,人们喜欢编写像 linux `<sys/epoll.h>` 头文件这样的代码：

> xuechengyun 注:
>
> 在 `cpptorture.c` 中的代码用了 64 行形如如下的宏定义来计算 $x * 2^{64}$.
> ```cpp
> #define X1(x) X2(x)+X2(x)`
> ...
> #define X30(x) X31(x)+X32(x)
> ...
> #define X64(x) x+x
> ```
> 提前说明一下,一次宏的处理分为三步(后面我们还会继续提到这三步):
>> 1. 处理参数(对参数中的宏也会进行处理)
>> 2. 将处理后的参数替换(replace)到宏的替换列表(substitution list)中
>> 3. 将替换后的结果作为新的输入重新处理.
>>
>
> 其中前两步和宏的定义相关,第 1 步和宏的参数个数相关,第 2 步和宏的替换列表相关.但是第 3 步是处理宏替换后产生的新结果,它的处理不依赖于当前这个宏的定义.
>
> 假设调用 `X1(3)` 来计算 $3 * 2^{64}$,那么:
>> `X64(3)` 展开需要 [ $(2^1 - 1)$ 次替换 ],得到 `3+3`,也就是使用 [ $(2^1 -1)$ 个加号 ] 将 [ $2^1$ 个 3 ] 相加
>>
>> `X63(3)` 展开需要 [ $1$ 次替换 + `X64(3)`展开两次 ] = [ $1$ 次替换 + $(2^2-2)$ 次替换 ] = [ $(2^2 - 1)$ 次替换 ],得到 `3+3+3+3`,也就是使用 [ $(2^2 -1)$ 个加号 ] 将 [ $(2^2)$ 个 `3` ] 相加
>>
>> `X62(3)` 展开需要 [ $1$ 次替换 + `X63(3)` 展开两次 ] = [ $1$ 次替换 + $(2^3-2)$ 次替换 ] = [ $(2^3 - 1)$ 次替换 ],得到 `3+3+3+3+3+3+3+3`,也就是使用 [ $(2^3 -1)$ 个加号 ] 将 [ $(2^3)$ 个 `3` ] 相加
>>
>> `X61(3)` 展开需要 [ $1$ 次替换 + `X62(3)` 展开两次 ] = [ $1$ 次替换 + $(2^4-2)$ 次替换 ] = [ $(2^4 - 1)$ 次替换 ],得到 `3+3+3+3+3+3+3+3+3+3+3+3+3+3+3+3`,也就是使用 [ $(2^4 -1)$ 个加号 ] 将 [ $(2^4)$ 个 `3` ] 相加
>>
>> ...
>>
>> `X1(3)` 展开需要 [ $1$ 次替换 + `X2(3)` 展开两次 ] = [ $1$ 次替换 + $(2^{64}-2)$ 次替换 ] = [ $(2^{64} - 1)$ 次替换 ],得到 [ $(2^{64} - 1)$ 个加号 ] 将 [ $(2^{64})$ 个 `3` ] 相加
>>
>
> 最终我们调用 `X1(3)` 展开后的代码需要的字节数就是数字 `3` 和加号操作符 `+` 的个数之和个字节,即 $(2^{65}-1)B$ = $(2^5 * 2^{60} - 1)B$ = $32EB - 1B$.也就是文中说的编译需要数艾字节的内存(many exabytes of memory).
>
> 最终 `X1(3)` 展开需要 $2^{64} - 1$ 次替换(也就是 `+` 的个数).假设一次替换需要 1 纳秒($1\ ns=10^{-9}\ s$), 且 $2^{10} \approx 10^3$,那么 $2^{64}-1\ ns\approx 2^4 \times 10^{18}\ ns = 16 \times 10^9\ s=16\times10^{9}\div60\div60\div24\div365\ 年\approx507 年$.也就是文中说的编译需要超过 100 年的时间(over 100 years).
>
> 注意,这里我们说的一次替换的时间包含了预处理器读取 token、参数列表、扩展参数列表、替换 substitution list 所需的时间,比如预处理器读取 `X1`、`(3)`、将 `3` 扩展成 `3`、替换参数扩展结果 `3` 到替换列表中得到 `X2(3) + X2(3)`.然后预处理器又重新扫描了这个结果并进行处理,此时除了对两个 `X2(3)` 进行了“替换”,还多耗读取了两个空格和一个加号,不过在上面的计算中就不细究了.
>

``` cpp
enum EPOLL_EVENTS {
  EPOLLIN = 0x001,
#define EPOLLIN EPOLLIN
  EPOLLIN = 0x002,
#define EPOLLPRI EPOLLPRI
  EPOLLOUT = 0x004,
#define EPOLLOUT EPOLOUTL
/* ... */
};
```
将 `EPOLL_EVENTS` 定义成一个枚举挺好的,因为这样有助于调试,而且更优雅.对于程序来说,能够使用 `#ifdef EPOLLPRI` 来检查特定标志的可用性也很好.所以,利用 C/C++ 预处理器通常不会递归展开宏的事实,`<sys/epoll.h>` 头文件将这两个问题都解决了.如此定义 `enum EPOLL_EVENT` ,token `EPOLLIN` 就会扩展一次成为 `EPOLLIN` 后停止扩展, 所以它实际上等价于支持 `#ifdef` 的 enum.

> xuechengyun 注:
>
> `<sys/epoll.h>` 首先将 `EPOLL_EVENTS` 定义成了枚举类型,这样更方便调试,代码看起来也舒服.另外,为了使用 `#ifdef EPOLLPRI` 来检查这个 flag 是否被设置,在 `enum EPOLL_EVENTS` 中又定义了和枚举值名称相同的宏,如 `#define EPOLLINE EPOLLIN`,这样使用起来更方便.
>
> 这种实现方式的前提是预处理器不会递归展开宏.如果可以递归展开,那么预处理器可以认为 `#define EPOLLIN EPOLLIN` 中的第二个 `EPOLLIN` (即宏的主体、替换列表)依然是一个宏,然后继续展开.那宏展开就会无法终止(termination)

为了防止宏递归展开,预处理器将每个定义的宏关联到一个 bit.该 bit 反映了宏当前是否被它的替换列表(substitution list)替换了,在此我们称之为 *replacing* bit(替换位).预处理器又将输入流(input stream)中的每一个 token 和一个 bit 关联起来,表明这个 token 不能进行宏扩展(macro-expanded),在此我们称之为 *unavailable* bit(不可用位).一开始 replacing 和 unavailable bits 都被清空为 false 了.

> xuechengyun 注:
>
> 每个宏关联一个 bit,叫 replacing bit,后文提到的 rb, macro_rb 即此含义
>
> 每个 token 关联一个 bit,叫 unavailable bit,后文提到的 ub, token_ub 即此含义
>
> ```cpp
> std::unordered_set<Str, Macro> name_to_macro;
> std::unordered_map<Str, bool> macro_name_to_rb;
> std::unordered_map<Token, bool> token_to_ub;
> ```
当预处理器处理每个输入 token T 时,它会设置 T 的 unavailable bit,并按如下流程决定是否对 T 进行宏展开:
1. 如果 `T` 是一个 *replacing* bit 被置为 true 的宏的名字,预处理器就会将其 *unavailable* bit 也置为 true.注意,即使 `T` 在一个不可以被宏展开的上下文中 (因为 `T` 可能是一个类函数宏,但是后面没有 `(` 跟着,所以它没有办法被展开)(xuechengyun 注: 后文中有一个 `#define PARENS ()` 宏就是让 `T` 后面没有 `(`,以阻止 `T` 展开),预处理器依然会设置其 unavailable bit.另外,一旦 unavailable bit 被置为 true,就再也不会被重置为 false.
    > xuechengyun 注:
    >
    > token_ub 被置为 true 后就无法重置为 false
    > ```cpp
    > // T 是一个宏名字 + rb 为 true
    > if (auto it = macro_name_to_rb.find(T.str()); it != macro_name_to_rb.end() && it->second) {
    >   token_to_ub[T] = true;
    > }
    > ```
    >

2. 如果 `T` 是一个类对象宏的名字,并且 `T` 的 unavailable bit 是 false,那么就将 `T` 展开.
    > xuechengyun 注:
    >
    > ```cpp
    > // T 是一个类对象宏名字 + ub 为 false
    > if (auto it = name_to_macro.find(T);
    >     it != name_to_macro.end() &&
    >     Macro::Type::ObjectLike == it->second.type &&
    >     !token_to_ub[T]) {
    >  expand_macro(T)
    > }
    > ```
3. 如果 `T` 是一个类函数宏的名字,且 `T` 的 unavailable bit 是 false,且 `T` 后面跟随了 `(`,那么就将 `T` 展开.注意,如果调用 `T` 使用的参数数量不对,那么程序就有问题(xuechengyun 注: 假设 `#define T(a, b) a + b` 那么 `T()`, `T(1)`, `T(1, 2, 3)` 就是有问题的程序).
    > xuechengyun 注:
    >
    > ```cpp
    > // T 是一个类函数宏名字 + ub 为 false + 名字后面有括号 '('
    > if (auto it = name_to_macro.find(T);
    >     it != name_to_macro.end() &&
    >     Macro::Type::FunctionLike == it->second.type &&
    >     !token_to_ub[T] &&
    >     '(' == next_char_of(T)) {
    >   args = scan_arguments();
    >   assert(args.size() == it->second.parameters.size());
    >   expand_macro(T, args);
    > }
    > ```

如果预处理器决定不对 `T` 进行宏扩展,那么它就直接将 `T` 添加到当前的输出 token 列表.反之,通过如下两个阶段扩展 `T`:

1. 当 `T` 是一个类函数宏时,预处理器扫描所有给 `T` 的参数,并在这些参数上进行宏扩展.它像处理普通的 token 一样扫描参数,但是不将输出的 tokens 放到主处理器中输出中,而是为 `T` 的每个参数构建一个替换 token 列表(replacement token list).同时记录原始的、非宏展开的参数,以便和 `#` 和 `##` 一起使用.
    > xuechengyun 注:
    >
    > ```cpp
    > #define A(x) (x*x)
    > #define B(x, y) x+y
    > // 如下是输入列表
    > B(A(3),B)(4,5)+B(6,7)
    > ```
    > 首先, `B` 是一个类函数宏,扫描到参数 `A(3)`, `B`;然后在参数 `A(3)` 、`B` 分别进行扩展,得到 `(3*3)` 和 `B`;接着为 `B` 的每个参数构建一个 replacement tokens list,也就是`((3*3), B)`
    >

2. 预处理器拿到 `T` 的替换列表后,如果 `T` 有参数,将替换列表中出现的参数名用第 1 步计算出来的对应的参数 token list 替换掉.同时根据替换列表中的 `#` 和 `##` 标识进行字符串化和拼接.然后,在逻辑上将得到的结果 tokens 添加到输入列表中.最后,预处理器将宏 `T` 的 replacing bit 设置为 true.
    > xuechengyun 注:
    >
    > 预处理器拿到 `B` 的替换列表 `x+y`,此时发现 `B` 有两个参数 `(x, y)`,就用第一步计算出的 replacement token list `((3*3), B)` 替换 `B` 的替换列表中的对应部分,即将 `x` 替换成 `(3*3)`,将 `y` 替换成 `B`,最终得到 `(3*3)+B`. 然后,将这个结果添加到输入列表中,即得到 `(3*3)+B(4,5)+B(6,7)`.最后,将 `B` 的 replacing bit 设置为 true.
    >

    当 replacing bit 被设置为 true 后,预处理器继续像往常一样处理输入,这些输入就是刚刚添加到输入列表中的 tokens.这可能导致更多的宏展开,所以有时把它叫做*重新扫描阶段*(*rescan phase*).一旦预处理器处理了由替换列表产生的所有 tokens,它就会将 `T` 的 replacing bit 重置为 false.
    > xuechengyun 注:
    >
    > `B` 的 replacing bit 为设置为 true 后,预处理器继续处理刚刚添加到输入列表中的 tokens,也就是 `(3*3)+B`,当处理到尾部这个 `B` 时,虽然后续还有 `(4,5)+B(6,7)` 让 `B` 可以读取到参数 `(4, 5)`,但是由于 `B` 的 replacing bit 为 true,所以不对这个尾部的 `B` 进行展开.于是对第一个对 `B` 宏替换后的结果重新扫描后并处理后,最终得到了 `(3*3)+B`.
    >
    > 此时,预处理器将 `B` 的替换列表产生的所有 tokens `(3*3)+B` 都处理完了.到此为止,预处理器将和第一个 `B` 相关的 tokens 都处理完了,于是将 replacing bit 设置为 false.另外输出列表为 `(3*3)+B`
    >
    > 接着预处理器开始处理剩下的 tokens `(4,5)+B(6,7)` 并输出 `(4,5)+6+7`.最终输出列表为 `(3*3)+B(4,5)+6+7`
    >

> xuechengyun 注:
>
> ```cpp
> auto args = scan_args_for(T);
> assert(args.size() == T.parameters.size());
> std::vector<std::list<View>> replace_tokens;
> for (size_t i = 0; i < args.size(); ++i) {
>   // 第 1 步: 对实参进行宏处理
>   replace_token.emplace_back(process_tokens(arg));
> }
> // 第 2 步: 将 replace_tokens 放到替换表中对应形参名的地方
> auto new_sl = T.sl;
> replace(T.parameters/*from*/, replace_tokens/*to*/, &new_sl);
> macro_name_to_rb[T] = true;
> process_tokens(new_sl);  // 也就是说在这个 new_sl 中再次遇到 T 的话,是不会再处理展开它的
> macro_name_to_rb[T] = false;
> ```

我们来看一个简单的例子:

`FL(FL(5))    => ((((5)+1))+1)`

在第一阶段,外面这个宏 `FL` 的参数 `FL(5)` 会被展开为 token list `((5)+1)`,从而得到 `FL(((5)+1))`.展开外面 `FL` 这个宏就是将替换列表中的形参 x 替换成这个实参 `((5)+1)`,于是得到 `((((5)+1))+1)`.结果应该还是挺直观的.需要注意的是,由于里面的那个 `FL` 的展开发生在第一阶段,所以 `FL` 的 replacing bit 一直都是 false,且没有任何 token 的 unavailable bit 被设置为 true 过.

现在我们来看一个更有趣的例子:

```cpp
#define ID(arg) arg
ID(ID)(ID)(X)  // => ID(ID)(X)
```

先看 token 序列的第一部分 `ID(ID)`. 我们从第 1 阶段开始,对内部的 `ID` 进行展开,由于这是一个后面没有 `(` 的类函数宏,所以预处理器不会展开它.于是,预处理器将外面这个 `ID` 的替换列表中的 `arg` 替换成 `ID`,然后将这个替换结果放到输入列表(input list)中(xuechengyun 注: 此时整个输入为 `ID` + `(ID)(X)`,其中前者是第一个 `ID` 替换后的结果,后者是预处理器还没有开始处理的剩余部分).接着将宏 `ID` 的 replacing bit 设置为 true 后进入第 2 阶段(重新扫描).在处理当前的第一个 token `ID` 时,预处理器将它的 unavailable bit 设置为 true(因为 `ID` 的 replacing bit 是 true) 且不展开它.最后,预处理器将 `ID` 的 replacing bit 重置为 false,不过,此时已经没有什么需要展开了,因为第三个 `ID` 后面没有 `(`.

实际上规范中存在一个[已知的歧义](https://open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#268),即究竟何时重置替换位.如果一个宏展开后以一个类函数宏结尾,但是该类函数宏的参数需要从展开部分的后面读取 tokens(xuechengyun 注: 比如我们前面读取到 `B(A(3),B))` 并展开后得到了 `(3*3)+B`, 此时展开后的结果就是以一个类函数宏 `B` 结尾,这个宏的参数需要从后面的部分 `(4,5)+B(6,7)` 中读取),那又会发生什么?在实践中,编译器似乎会做一些直观的事,并在完全遵循宏展开后面的第一个 token 前重置 replacing bit.例子如下:
> xuechengyun 注:
>
> 什么叫"在完全遵循宏展开后面的第一个 token"?
> 以 `B(A(3),B)(4,5)+B(6,7)` 为例, 当 `B(A(3),B)` 处理完后紧跟的第一个 token 就是 `(4,5)+B(6,7)` 的第一个字符,也就是 `(`,也就是在处理 `(` 前将 `B` 的 replacing bit 重置,这样 `B(6,7)` 才可以展开.
>
```cpp
#define LPAREN (
#define ID2(arg) arg

ID(ID2)(ID)(X)            // =>  X
ID(ID2 LPAREN)ID)(X)      // =>  X
ID(ID2 LPAREN ID))(X)     // =>  ID(X)
ID(ID2 (ID))(X)           // =>  ID(X)
```

在上面的例子中,不同位置的 `(ID)` 成为 `ID2` 的参数后, 都被移动到了第一个 `ID` 的参数中,并且你可以看到,当第二个 `ID` 一旦被移动到第一个 `ID` 的替换列表中,就会立刻将 `ID` 的 unavailable bit 设置为 true.
> xuechengyun 注:
>
> 当第一个 `ID` 的替换完成后就会将其 rb 置为 true,而 rb 为 true 后就会立刻将替换结果中的 `ID` 的 ub 置为 true.
> 上面说的 `ID` 成为 `ID2` 的参数指的是第 3、4 种情况.
>

> xuechengyun 注:
>
> 当读取到一个 token 是宏名字,并确定要展开后,执行以下 3 步:
>   1. 读取宏的参数并展开,得到 expanded_arguments
>   2. 使用 expanded_arguments 替换宏的替换列表(substitution list),替换后的结果为 new_input. 将 token 的 replacing bit 设置为 true
>   3. 继续处理 new_input, 这些 new_input 又可能产生 new_new_input, 反正等它们都处理完后,在将一开始的那个 token 的 replacing bit 重置为 false
>
> 我们将这 3 步分为两个部分:
>   1. 宏定义相关: 前两步为一个部分,执行过程和宏的定义相关.比如,第 1 步依赖宏的参数个数,如果读取的参数数量和宏定义的参数数量不一致,那么程序就是 ill-formed 的.第 2 步依赖于宏的替换表.
>   2. 宏定义无关: 第 3 步是处理宏展开后的结果,即得到的 new_input,在处理 new_input 的过程当中并不依赖于原来那个宏的定义.只是在遇到原来那个宏时,不再对其进行展开(识别到 replacing bit 为 true).
>
> 简单来说就是参数替换后,如果替换的结果(new_input)中又有这个宏名字,那么在处理 new_input 时(rescan phase)就不会在展开这个宏,比如 `ID(ID)X` -> `ID(X)`, new_input = `ID`,那就不处理它.当这些结果处理完后就会重置 replacing bit,之后再遇到同名字的宏还是需要展开. 比如 `ID(ID)ID(X)` -> `ID` + `ID(X)` -> `ID` + `X` -> `IDX`
>

## Recursive macros(递归宏)
当然,C 预处理器可以通过 `#include` 指令简单的实现地递归.文件可以 include 自己, 并适当地定义/取消定义常量(`#undef/#define` constants),然后通过 `#if` 条件来实现基本情况.虽然通过递归 include-file 的方式并不那么实用,但你确实可以递归地展开宏,至少可以相互递归展开.我第一次见到 [Paul Fultz](https://github.com/pfultz2/Cloak/wiki/C-Preprocessor-tricks,-tips,-and-idioms) 提出的一个小技巧,是通过将 token 一直隐藏到另一个宏执行完重新扫描过程,来避免在要展开的宏上设置 unavailable bit.

> xuechengyun 注:
>
> 如下就是通过 [文件 `#include` 自己] + [`#undef/#define` 常量] + [`#if` 条件] 来实现递归宏.目的是通过宏 `RESULT` 来计算 $N$ 的阶乘.我们来看一下 4 的阶乘怎么算:
> 1. 定义 $N = 4, 即 `#define N 4`,用户使用宏 `RESULT` 需要 `#include "factorial.h"`
> 2. `#include "factorial.h"` 首先得到 `N = 4; RESULT = 1`
> 3. 进入 `#if N > 0`, 得到 `TEMP = N = 4, TEMP_RESULT = RESULT * N = 1 * 4 = 4`
> 4. 然后 `#undef N #define N (TEMP - 1)` 得到 `N = TEMP - 1 = 3`,同理得到 `RESULT=TEMP_RESULT=4`
> 5. 接着 `#include "factorial.h"` 后开始重复第 2 ~ 4 步:
>    递归第一次: `N = 3; RESULT = 4` -> `N = 2; RESULT = 12`
>    递归第二次: `N = 2; RESULT = 12` -> `N = 1; RESULT = 24`
>    递归第三次: `N = 1; RESULT = 24` -> `N = 0; RESULT = 24`
>    递归第四次: `N = 0; RESULT = 24` -> `#if N > 0` 为 false -> 终止程序
> 6. 最终, `RESULT = 24`, 即 4 的阶乘为 24
> ```cpp
> // factorial.h
> #ifndef N
> #define N 5      // 初始值
> #endif
>
> #ifndef RESULT
> #define RESULT 1  // 初始结果
> #endif
>
> #if N > 0
>    #define TEMP N
>    #define TEMP_RESULT (RESULT * N)
>
>     #undef N
>     #define N (TEMP - 1)
>
>     #undef RESULT
>     #define RESULT TEMP_RESULT
>
>     #include "factorial.h"
> #endif
> ```

我们来看一个例子:
```cpp
#define ID(arg) arg
#define PARENS ()  // 注意 () 前的空格,也就是这是一个类对象宏
#define F_AGAIN() F
#define F() f F_AGAIN PARENS()

// F() -> f F_AGAIN PARENS()
//    重新扫描 F 的替换结果
//    f F_AGAIN PARENS() -> f F_AGAIN ()()
F()          // => f F_AGAIN ()()

// ID(F()) -> (...) -> ID(f F_AGAIN ()()) -> f F_AGAIN ()()
// 此时 "ID(F())" 的参数展开和替换列表替换已经完成,也就是我们前面提到的"宏定义相关"部分
//   重新扫描 ID 的替换结果, 对 "F_AGAIN ()" 进行替换
//   f F_AGAIN ()() -> f F()
//     重新扫描 F_AGAIN 的替换结果, 对 "F()" 进行替换
//     f F() -> f f F_AGAIN PARENS()
//       重新扫描 F 的替换结果, 对 "PARENS" 进行替换
//       f f F_AGAIN ()()
ID(F())      // => f f F_AGAIN ()()

// ID(ID(F())) -> (...) -> ID(f f F_AGAIN ()()) -> f f F_AGAIN ()()
// 此时 ID(ID(F())) 的参数展开和替换列表替换已经完成,也就是我们前面提到的"宏定义相关"部分
//   重新扫描 ID 的替换结果, 对 "F_AGAIN ()" 进行替换
//   f f F_AGAIN ()() -> f f F()
//     重新扫描 F_AGAIN 的替换结果,对 "F()" 进行替换
//     f f F() -> f f f F_AGAIN PARENS()
//       重新扫描 F() 的替换结果,对 "PARENS" 进行替换
//       f f f F_AGAIN PARENS() -> f f f F_AGAIN()
ID(ID(F()))  // => f f f F_AGAIN ()()
```

当 `F()` 展开后, 注意 `F_AGAIN` 后面没有 `(` (xuechengyun 注: `F_AGAIN` 后面是 `PARENS`,此时 `PARENS` 还没有展开成 `()`),所以它不会被当成宏进行展开.当然,在下一步 `PARENS` 被展开成了 `()`,但此时预处理器已经将 `F_AGAIN` 这个 token 输出了, 所以决定此时再展开它(指 `F_AGAIN`)已经太晚了.因此, `F()` 的输出(也就是 `f F_AGAIN ()()`)可能包含一个没有展开的宏调用,但是所有 tokens 的 unavailable bit 都是 false.

> xuechengyun 注:
>
> 在某个宏 `T` 发生替换后其 rb 被设置为 true,当重新扫描替换结果又遇到了 `T` 才会将其 ub 设置为 true.
>
> **这里我们的目的是让 `F` 和 `F_AGAIN` 相互递归调用,并避免 ub 被设置为 true**.如果 `#define F() f F_AGAIN ()() `,那么当 `F` 发生替换后重新扫描时,又会对 `F_AGAIN` 进行替换并得到 `F`,在对 `F_AGAIN` 的替换结果重新扫描时就会将 `F` 的 ub 设置为 true.
>
> 为了阻止 ub 为设置为 false,我们只需要在对 `F` 的替换结果重新扫描时阻止 `F_AGAIN` 展开,因为如果 `F_AGAIN` 展开就会再次遇到 `F`,`F` 的 ub 就会被设置为 true.阻止类函数宏展开的方法就是让其后面没有 `(`.另外,为了实现递归调用,我们又需要为 `F_AGAIN` 后面添加一个 `(`.
>
> `PARENS` 这个技巧的原理就是:
>> 1. 当预处理器重新扫描 `F` 的替换结果并遇到 `F_AGAIN` 时,让其后面没有 `(` 以至于无法展开,通过终止递归调用来避免将 `F` 的 ub 设置为 true.
>> 2. 当预处理器扫描到 `F_AGAIN` 后面时,通过其后面的宏 `PARENS` 展开为其添加一个 `(`,如果还存在下一次扫描(比如在外层添加一个 `ID` 就会重新扫描一遍),由 `PARENS` 展开来的 `()` 又能让 `F_AGAIN` 继续递归展开.
>>
>
> 最终, `PARENS` 提供了一个让 `F` 在宏替换后就终止递归调用,使用 `ID` 又能继续递归调用的能力.同时也避免了 `F` 的 ub 被设置为 true.
>

现在来看看当我们调用 `ID(F())` 时发生了什么.首先我们将参数 `F()` 展开成 `f F_AGAIN ()()`.这就完成了对宏 `F` 的处理,所以我们将 `F` 的 replacing bit 重置.接着,将 `ID` 的替换列表中的 `arg` (也叫做单 token `arg`) 替换成 `f F_AGAIN ()()`. 接着,预处理器将 `ID` 的 replacing bit 设置为 true 并重新扫描(rescan) `f F_AGAIN ()()`,如此导至 `F_AGAIN` 和 `F` 先后被展开.当然,再次使用 `PARENS` 这种技巧依然可以阻止第二个 `F_AGAIN` 被展开.

每当我们将 `F()` 传递给标识宏(identity macro, 也就是展开成宏的参数自身) `ID` 时,它就会多展开一次.即使我们不能无限递归,我们还是可以设置一个任意的最大递归次数(xuechengyun 注: 就是不断的在外层添加 `ID`, 添加多少个就可以递归调用多少次).当我们简单地生成一些和我们写的代码行数呈指数级增长的宏调用时(还记得我们的 [trivial cpptoture.c program](https://www.scs.stanford.edu/~dm/blog/cpptorture.c) 吗?),真正的限制是我们能给预处理器提供多少时间和内存,而不是预处理器不是图灵完备的事实.如下 5 行代码重新扫描了 342 次(`EXPAND4` 被调用了 256 次,当然中间的宏也会导致重新扫描)

```cpp
#define EXPAND(arg) EXPAND1(EXPAND1(EXPAND1(EXPAND1(arg))))
#define EXPAND1(arg) EXPAND2(EXPAND2(EXPAND2(EXPAND2(arg))))
#define EXPAND2(arg) EXPAND3(EXPAND3(EXPAND3(EXPAND3(arg))))
#define EXPAND3(arg) EXPAND4(EXPAND4(EXPAND4(EXPAND4(arg))))
#define EXPAND4(arg) arg
```
> xuechengyun 注:
>
>> 1 个 arg 作为初始参数被 scan 1 次
>>
>> 1 个 EXPAND(arg) 替换得到 4 个 EXPAND1 并 re-scan 1 次
>>
>> 4 个 EXPAND1 替换得到 16 个 EXPAND2 并 re-scan 4 次
>>
>> 16 个 EXPAND2 替换得到 64 个 EXPAND3 并 re-scan 16 次
>>
>> 64 个 EXPAND3 替换得到 256 个 EXPAND4 并 re-scan 64 次
>>
>> 256 个 EXPAND4 替换后 re-scan 256 次
>>
> 所以 5 行代码扫描了 1 + 1 + 4 + 16 + 64 + 256 = 342 次; 另外, 因为有 256 个 EXPAND4, 所以 EXPAND4 被调用了 256 次.
>
> 不过,256 这个数字并不重要.重要的是代码总共被扫描了多少次,即 342.
>
> 前面提到的标识宏(identity macro) `#define ID(arg) arg` 的作用就是让 arg 作为参数先展开一次(即 `ID(arg)`),然后得到结果,结果又被 re-scan 一次. 如果需要这个结果再被展开一次,那就将结果再次包含到标识宏中(即 `ID(ID(arg))`).
>
> 如此,如果遇到同名且不能展开的宏, 即 replacing bit 被置为 1 的宏,就用 `ID` 再包一层,此时其 replacing bit 就会被重置,展开结果也会作为外一层的 `ID` 的参数被重新扫描.为了重新扫描多次,我们可以多包几层 `ID` 宏, 比如包 $N = 7$ 层,那就是 `#define EXPAND(arg) ID(ID(ID(ID(ID(ID(ID(arg)))))))`, 这样我们就可以扫描 $N+1=8$ 次.
>
> 文中 `EXPAND` 的实现就是将在外层添加 `ID` 宏(也就是 `EXPAND4` 宏)的效率呈指数级增长.也就是说你不嫌麻烦,完全可以将 $N$ 设置成 341,然后像下面这样做:
> ```cpp
> // 其中有 341 个 `ID`
> #define EXPAND(arg) ID(ID(ID(ID(   ....(arg)....       ))))
> // 或者像 cpptorture.c 那样
> #define EXPAND1(arg) EXPAND2(arg)
> #define EXPAND2(arg) EXPAND3(arg)
> ...
> #define EXPAND340(arg) EXPAND341(arg)
> #define EXPAND341(arg) arg
> ```
>

## Variable-argument macros(可变参数宏)
C++11 添加了可变参数宏.当使用 `#define` 定义宏,如果最后一个宏参数是 `...` 而不是一个标识符(identifier)时,它可以接受任意数量的参数,替换列表中特殊的 token `__VA_ARGS__` 会被扩展成所有的这些参数,参数间用都好分隔(commas).一个典型的例子如下:

`#define LOG(...) printf(__VA_ARGS__)`

不幸的是,在很多场景中都存在一个小小的恼人的事,那就是当 `...` 表示 0 个参数时,很难编写一个能够正确生成 C 代码的宏.例如,假设你想要使用一个宏在括号里打印信息.你可能会尝试做如下的事:

```cpp
#define LOG(fmt, ...) printf("[" fmt "]", __VA_ARGS__)
LOG("level %d", lvl);  // => printf("[" "level %d" "]", lvl);
```

这里我们利用了 C 语言连接相邻字符串常量的事实.由于 `LOG` 的第一个参数 (对应于参数 `fmt`)预计是一个字符串常量,因此我们可以为 `printf` 构造一个新的格式化参数,其中,这个字符串被括起来了.不幸的是,如果在格式化字符串后面没有参数,那就无法起作用:
```cpp
LOG("hello")          // => printf("[" "hello" "]", );
```

`printf("hello",)` 中多余的逗号在 C 和 C++ 语言中是一种语法错误.C++20 通过添加一个新的特殊标识符 `__VA_OPT__` 来解决这个问题.这个序列 `__VA_OPT__(x)` 只能用在可变参数宏的替换列表中,如果 `__VA_ARGS__` 非空 `__VA_OPT__(x)` 就会展开成 `x`,否则展开后就什么都没有了.这让我们可以通过在参数列表为空时抑制逗号来修复宏 `LOG` 的问题.

```cpp
#define LOG(fmt, ...) printf("[" fmt "]" __VA_OPT__(,) __VA_ARGS__)
LOG("hello");          // => printf("[" "hello" "]");
```

自然,一个意想不到的好处就是,区分空参和非空参列表正是我们要实现递归的基本情况所需要的机制.

## The `FOR_EACH` macro(`FOR_EACH` 宏)
现在我们有了实现 `FOR_EACH` 宏所需的所有部分:

```cpp
#define PARENS ()

#define EXPAND(...) EXPAND4(EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__))))
#define EXPAND4(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
#define EXPAND3(...) EXPAND2(EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__))))
#define EXPAND2(...) EXPAND1(EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__))))
#define EXPAND1(...) __VA_ARGS__

#define FOR_EACH(macro, ...)                                      \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER(macro, __VA_ARGS__)))
#define FOR_EACH_HELPER(macro, a1, ...)                           \
  macro(a1)                                                       \
  __VA_OPT__(FOR_EACH_AGAIN PARENS (macro, __VA_ARGS__))
#define FOR_EACH_AGAIN() FOR_EACH_HELPER

FOR_EACH(F, a, b, c, 1, 2, 3)    // => F(a) F(b) F(c) F(1) F(2) F(3)
```
> xuechengyun 注:
>
> 这里的 `FOR_EACH` 的第一个参数 `macro` 是一个参数数量为 1 的类函数宏，文章最后我会给出一种让 `macro` 可以是接受任意参数数量的实现方法
>

注意,我们已经调整了 `EXPAND` 的写法,这样就可以通过简单地使用 `__VA_ARGS__` 而不是一个叫做 `arg` 的来处理输出逗号的宏.
> xuechengyun 注：
>
> 也就是使用了 `...` 和 `__VA_ARGS__` 将 `EXPAND` 实现成了可变参数宏
>
> 如果不使用可变参数 `...`,而是使用 `arg`,那么如果实参展开后有逗号的话,就会出现参数数量不符的情况,比如:
>
> ```cpp
> #define REVERSE(a, b) b, a, REVERSE_AGAIN PARENS
> #define REVERSE_AGAIN() REVERSE
> // 这里我们使用 arg 而不是 ...
> #define EXPAND(arg) EXPAND1(EXPAND1(arg))
> #define EXPAND1(arg) arg
> // 我们预期得到 b, a, d, c, f, e
> EXPAND(REVERSE(a, b)(c, d)(e, f))
> // 1. 处理 EXPAND 的参数,得到参数展开的结果为 "b, a, REVERSE_AGAIN ()(c, d)(e, f)"
> // 2. 将参数展开结果替换到 EXPAND 的替换列表,得到 "EXPAND1(EXPAND1(b, a, REVERSE_AGAIN()(c, d)(e, f)))"
> // 3. 处理第一个 EXPAND1 的参数,即扩展 "EXPAND1(b, a, REVERSE_AGAIN()(c, d)(e, f))",此时读取第二个 EXPAND1 的参数时,就会发现参数数量不对,输入中有三个参数 "b", "a", "REVERSE_AGAIN()(c, d)(e, f)",但实际上 EXPAND1 只接受一个参数.
> // 于是预处理器就会报错,这也就是为什么我们要使用 ... 而不是 arg 的原因
> ```
>

大部分工作发生在 `FOR_EACH_HELPER(macro, a1, ...)` 中,它将参数 `a1` 应用到 `macro` 中,如果剩余的参数不是空的,就接着使用 `__VA_OPT__` 来进行递归.和前一节一样,它也使用了 `PARENS` 这个技巧来实现递归.唯一美中不足的是我们必须要一直重新扫描宏,这就是为什么 `FOR_EACH` 宏要将 `FOR_EACH_HELPER` 嵌在 `EXPAND` 中的原因.另外,`FOR_EACH` 也使用了 `__VA_OPTS__` 来处理空参数的情况.

> xuechengyun 注:
>
> `FOR_EACH` 需要将 `...` 代表的每一个参数都依次放到 macro 的参数列表中，所以需要一直扫描以便获取下一个参数 `a`，而可以一直扫描的前提就是将需要扫描的部分放到 `EXPAND` 中, 也就是 `FOR_EACH_HELPER` 部分.
>
> 美中不足的是,在我们可能只需要扫描三五次就可以结束时,`EXPAND` 依然会强制扫描 342 次.比如 EXPAND(3),我们知道其实只需要扫描 1 次就已经得到了最终结果 `3`,但是它还是会先替换为 `EXPAND4(EXPAND4(EXPAND4(EXPAND4(3))))`,接着继续重新扫描,最终扫描 342 次得到 `3`.
>

我会在生产环境的代码中使用这个东西吗?我在思考这个问题.在我首个 10 年 C++ 编程生涯中,我曾经认为成为一名优秀的 C++ 程序员就是要展示你有多聪明.现在作为一名睿智的资深教员,我知道成为一名优秀的 C++ 程序员就是要表现出克制.你需要知道 *如何* 以及 *何时* 展示聪明才智.所以,让我们从一些可选方法开始进行成本-效益分析:

1. 你可以手动维护单独的枚举声明和 `pretty-print/scanner` 函数,但这样可能存在不同步的风险.
2. 你可以用另一个程序生成一个 C++ 代码,但是这样会使构建过程变得复杂,并且不会让代码更具可读性.C++ 不是一个生成文本的好语言.如果你使用 perl、python 或 bash,代码不一定对其他 C++ 程序员更透明.
3. 我正在做的是: 我写了一个和 `MAKE_ENUM` 等价的宏,这个宏将 `__VA_ARGS__` 字符串化,并将它传递给一个大约 25 行的函数,这个函数将这些字符串解析到一个 `std::vector<std::string>`.接着我使用一个包含了 `std::maps` 的 `EnumTable` 类型来获取枚举值和字符串.`EnumTab` 的构造函数接受一个 `std::vector<std::string`> 和包含常量的 `std::initializer_list`.所以基本上我的宏最终在为每个枚举类型生成了一个这样的函数:

    ```cpp
    static inline const EnumTab&
    getEnumTab(const Enum*)
    {
      static const EnumTab tab(EnumTab::parse_va_args(#__VA_ARS__), {__VA_ARGS__});
      return tab;
    }
    ```
    这个代码并未发布,我至今都不想公开展示它的原因之一是,枚举解析(这些枚举需要被读取并写入一个人类可读的文件中)的代码过于粗糙.

所以我认为 `FOR_EACH` 这个方法实际上是完胜选项 2 和 3.最克制的选项(在 C++ 中你应该总是考虑这个选项, 克制、克制、再克制) 是第 1 个.

使用 `FOR_EACH` 的复杂度是多少?如果你不知道预处理器如何工作,那么理解 `FOR_EACH` 是如何工作的绝对很棘手.不幸的是,很难弄清楚预处理器是如何工作的.直到我已经理解了预处理器如何工作,我才能够理解 C++ 的 [language specification](https://timsong-cpp.github.io/cppwp/n4861/cpp.rescan#3)(语言规范) 中宏替换的部分. [https://en.cppreference.com](https://en.cppreference.com) 没有涉及到必要的细节.另一方面,我现在有了这篇博客发表,我可以在我的代码中引用它,所以写这篇文章实际上是决定我是否要使用这个技巧的一部分.

`FOR_EACH` 也远非我所见过的最粗暴的宏用法.它甚至都没有使用 token 粘贴符(`##`) 来合成无法被文本搜索的新 tokens.虽然这种实现方式很难理解,但至少代码很短.更重要的是,`FOR_EACH` 的接口非常直观.对于一个多行的 C 语言宏,我认为 `MAKE_ENUM` 相当可读.并且一旦你在一个地方使用 `FOR_EACH`,那么你可以潜在的将复杂性分摊到其他宏的使用上.

不管你如何权衡,有一点可以肯定: `__VA_OPT__` 的引入使得 `FOR_EACH` 明显比老版本的 C++ 中 [brittle](https://stackoverflow.com/questions/36197213/for-each-macro-with-two-or-more-params-in-call-macro)(脆弱的) 和 [disgusting](https://stackoverflow.com/questions/14732803/preprocessor-variadic-for-each-macro-compatible-with-msvc10)(恶心的) 的方法更牛逼 plus.


# [译者附加说明](https://github.com/xuexcy/public_attachments/tree/main/翻译RecursiveMacrosWithC++20__VA_OPT__/README.md)

