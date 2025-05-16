---
title: 使用表达式模板做向量计算
description: 使用表达式模板做向量计算
date: 2025-05-16T22:31:29+08:00
draft: false
categories:
  - cpp
tags:
  - cpp
  - 表达式模板
---

**文中链接跳转失效时，请移步至[github 使用表达式模板做向量计算](https://github.com/xuexcy/learning_more_cpp_idioms/blob/main/src/expression_template_deps/README.md)查阅**

# 使用表达式模板做向量计算
# 1. 问题: 向量计算
```cpp
static constexpr size_t N{1000};
using StdArray = std::array<double, N>;
StdArray a;
StdArray b;
// 计算 1.2 * a + a * b 并将结果存入 StdArray c
```
---
# 2. 方案分析
## 2.1 表达式计算<a name="#jump表达式计算"></a>
### 2.1.1 代码实现
[./test_array/calculation_deps.h](./test_array/calculation_deps.h) `calculate_by_expression`
```cpp
StdArray c;
for (auto i = 0; i < a.size(); ++i) {
    c[i] = 1.2 * a[i] + a[i] * b[i];
}
```
### 2.1.2 执行步骤
1. `double result_1 = 1.2 * a[i]`:
    - 读取 a[i]: 读取 N 个 double
    - 计算 1.2 * a[i]: 计算 N 个乘法
2. `double result_ 2= a[i] * b[i]`:
    - 读取 a[i]、b[i]: 读取 2N 个 double（ 在前面计算 1.2 * a[i] 时已经读取了 a[i]，这个值应该已经在缓存中了，不过这里我们还是当成一次读取）
    - 计算 a[i] * b[i]: 计算 N 个乘法
3. `double result_3 = result_1 + result_2`:
    - 计算 result_1 + result_2: 计算 N 个加法
4. `c[i] = result_3;`:
    - 读取 c[i]: 读取 N 个 double
    - 将 result_3 写入 c[i]: 写入 N 个 double
### 2.1.3. 开销
1. 在每轮循环中创建 result_1、result_2、result_3，使用后立刻销毁(开销可忽略不计)
2. 读取 4000 = 3N = N + 2N + N 个 double
3. 写入 1000 = N 个 double 写入
4. 计算: 乘法 2000 = 2N; 加法 1000 = N

## 2.2 操作符重载
### 2.2.1 代码实现
[./array/operator.h](./array/operator.h) `namespace array_operator_overload`
```cpp
namespace array_operator_overload {
StdArray operator+(const StdArray& a, const StdArray& b) {
  Array result;
  for (auto i = 0; i < a.size(); ++i) {
    result[i] = a[i] + b[i];
  }
  return result;
}
StdArray operator*(const StdArray& a, const StdArray& b) {
  Array result;
  for (auto i = 0; i < a.size(); ++i) {
    result[i] = a[i] * b[i];
  }
  return result;
}
} // namespace array_operator_overload
```
### 2.2.2 使用方法
``` cpp
#include "array/operator.h"
using namespace array_operator_overload;
StdArray c = 1.2 * a + a * b;
```
### 2.2.3 执行步骤
1. `StdArray result_1 = 1.2 * a;`: 即 `operator*<double, StdArray>(1.2, a)`
    - 创建临时变量 StdArray result_1: 创建 N 个 double
    - 读取 a: 读取 N 个 double
    - 计算 1.2 * a[i]: 计算 N 个乘法
   将计算结果写入 result_1: 写入 N 个 double
2. `StdArray result_2 = a * b;`: 即 `operator*<StdArray, StdArray>(a, b)`
    - 创建临时变量 StdArray result_2: 创建 N 个 double
    - 读取 a、b: 读取 2 * N 个 double
    - 计算 a[i] * b[i]: 计算 N 个乘法
    - 计算结果写入 result_2: 写入 N 个 double
3. `StdArray result_3 = result1 + result_2;`: 即 `operator+<StdArray, StdArray>(result_1, result_2)`
    - 创建临时变量 StdArray result_3: 创建 N 个 double
    - 读取 result_1、result_2: 读取 2 * N 个 double
    - 计算 result_1[i] + result_2[i]: 计算 N 个加法
    - 计算结果写入 result_3: 写入 N 个 double
4. `StdArray c = {return result_3;};`:
    - 如果编译器支持 copy elision, 应该没有开销
### 2.2.4 开销
1. 先后创建 3 个临时变量 StdArray result_1, result_2, result_3: 创建 3 * N 个 double
2. 读取 5000 = 5N = N + 2 * N + 2 * N 个 double
3. 写入 3000 = 3N 个 double
4. 计算: 乘法 2000 = 2N; 加法 1000 = N

## 2.3 表达式模板(expression template; ET)
### 2.3.1 代码实现
[./array/expression_template/expression.h](./array/expression_template/expression.h) `namespace array_expression_template`
```cpp
// array/expression_template/expression.h
namespace array_expression_template {
struct Expression {};  // Expression

/**
item_type: 数据类型，本文中就是 double
*/

template <class T>
struct Variable : public Expression {
  const item_type& operator[](size_t) const { return t_; }
  const T t_;
};  // Variable, 存储常量 T

template <ArrayLike T>
struct Array : public Expression {
  const item_type& operator[](size_t idx) const { return t_[idx]; }
  const T& t_;
};  // Array, 存储向量引用(可以是 std::array/std::vector 或者自定义的符合 concept ArrayLike 的自定义 类型)

template <IsExpr LExpr, IsExpr RExpr>
struct BinaryExpression : public Expression {
  const LExpr l_expr_;
  const RExpr l_expr_;
};  // BinaryExpression, 存储二元操作符的左右两个表达式

template <IsExpr LExpr, IsExpr RExpr>
struct Add : public BinaryExpression<LExpr, RExpr> {
  item_type operator[](size_t idx) const {
    return l_expr_[idx] + r_expr_[idx];
  }
};  // Add

template <IsExpr LExpr, IsExpr RExpr>
struct Multiply : public BinaryExpression<LExpr, RExpr> {
  item_type operator[](size_t idx) const {
    return l_expr_[idx] * r_expr_[idx];
  }
};  // Multiply

// array/expression_template/operator.h
template <IsExpr LExr, IsExpr RExpr>
auto operator+(const LExpr l_expr, const RExpr r_expr) {
    return Add(l_expr, r_expr);
}
template <IsExpr LExr, IsExpr RExpr>
auto operator*(const LExpr l_expr, const RExpr r_expr) {
    return Multiply(l_expr, r_expr);
}
// more operator overload for Variable and Array
// ...

}  // namespace array_expression_template
```
### 2.3.2 使用方法
```cpp
#include "array/expression_template/expression.h"
using namespace array_expression_template;
Array arr_a(a), arr_b(b);
auto expr = 1.2 * arr_a + arr_a * arr_b;
StdArray c;
collect_result(expr, &c);
```
### 2.3.3 执行步骤
1. `auto expr_1 = 1.2 * arr_a;`: 即 `operator*<double,  Array>(1.2, arr_a)`
    - 创建临时变量 expr_1: 具体类型 `Multiply<Variable<double>, Array<StdArray>>`
2. `auto expr_2 = arr_a * arr_b;`: 即 `operator*<Array,  Array>(arr_a, arr_b)`
    - 创建临时变量 expr_2: 具体类型 `Multiply<Array<StdArray>, Array<StdArray>>`
3. `auto expr = expr_1 + expr_2;`: 即 `operator+<decltype(expr_1), decltype(expr_2)>(expr_1, expr_2)`
    - 创建变量 expr: 具体类型
        ```cpp
        Add<
            Multiply<Variable<double>, Array<StdArray>>,
            Multiply<Array<StdArray>, Array<StdArray>>
        >
        ```
5. `collect_result(expr, &c)`: 像<a href="#jump表达式计算">表达式计算</a>一样进行计算`1.2 * a[i] + a[i] * b[i]` 并将结果赋值给 c[i]
### 2.3.4 开销
1. 我们创建了一些表达式对象
    - 两个 Array: 即 `Array arr_a(a), arr_b(b);`
    - `auto expr`: 根据前面描述的 expr 的具体类型，一共创建了 1 个 Add，2 个 Multiply，3 个 Array，1 个 Variable，共 7 个表达式对象（Instance of `class Expression`）
2. 在 `collect_result` 中，我们通过 `operator[]` 从表达式中取值，此时 `operator[]` 中的 `l_expr_` 和 `r_expr_` 继续调用对应表达式的 `operator[]`，直到从 `Variable` 和 `Array` 返回具体的数值，此时表达式对象一共调用了 7 次重载的 `operator[]`
---
# 3. 方案对比
## 3.1 对比分析
表达式 `1.2 * a + a * b` 中一共有 1 个常量、3 个 StdArray 和 3 个操作符。
> 现定义如下变量
> x : 表达式中的常量个数
> y : 表达式中 StdArray 的个数
> z : 表达式中的操作符个数
> 其中 x + y = z + 1
1. 表达式方案：
    - y: 读取 2 次 a，1 次 b
    - z: 计算 2 次乘法，1 次加法
    - 1: 写入 1 次结果到 StdArray c
2. 操作符重载方案：
    - y: 读取 2 次 a，1 次 b
    - z: 计算 2 次乘法，1 次加法
    - z: 调用 3 次重载函数
    - z - 1: 生成 2 个最终会被调回的临时对象（最后一个临时变量会通过 copy elision 赋值给我们想要的结果 `StdArray c`）
    - z - 1: 读取 2 次临时对象: 除了最后一次重载函数调用，其他的每次重载函数调用的结果都会写入一个临时对象，而这个临时对象一定会再下一个函数调用中读取
    - z: 写入 3 次结果到 StdArray
3. ET 方案：
    - y: 读取 2 次 a，1 次 b
    - z: 计算 2 次乘法，1 次加法
    - x: 生成 1 个 Variable 对象
    - y: 生成 3 个 Array 对象
    - z: 生成 3 个 BinaryExpression 对象
    - z: 调用 3 次 BinaryExpression 的 `operator[]` 重载函数
    - y: 调用 3 次 Array 的 `operator[]` 重载函数
    - x: 调用 1 次 Variable 的 `operator[]` 重载函数
    - 写入 1 次结果到 StdArray c

|  |  表达式方案 | 操作符重载方案 | ET 方案 |
| -- | -- | -- | -- |
| 读取已有的 StdArray 次数 | y | y | y |
| 数值计算次数 | z | z | z |
| 写入 StdArray 次数 | 1 | z | 1 |
| 生成最终可以被销毁的对象个数 | 0 | z - 1 | x + y + z |
| 写入最在可被销毁的对象的次数 | 0 | z - 1 | 0 |
| 重载函数调用次数 | 0 | z | x + y + z |
## 3.2 对比测试
测试使用的表达式更复杂(不是`1.2 * a + a * b`)
1. 相关代码与结果
    - test: [test_array/calculation_test.cc](./test_array/calculation_test.cc)
    - benchmark:
        - 代码 [test_array/calculation_benchmark.cc](./test_array/calculation_benchmark.cc)
        - [机器信息](../../attachment/expression_template_array/output/expression_template_array_benchmark.extra_info)
        - 结果 [expression_template_array_benchmark.txt](../../attachment/expression_template_array/output/expression_template_array_benchmark_g++.txt)
    - profile:
        - 代码 [test_array/calculation_profile.cc](./test_array/calculation_profile.cc)
        - 结果 [expression_template_array_profile_g++.svg](../../attachment/expression_template_array/output/expression_template_array_profile_g++.svg)
2. 结果分析
    - 表达式方案最优(看 benchmark 和 profile 结果)，因为直接从 StdArray 取值并计算表达式，没有函数其他函数调用、较大的临时变量生成等问题，另外，可能还有编译期优化计算。该方案中调用 `std::array operator[]` 耗时占比也比较大
    - 操作符重载和 ET 方案都有较多的函数调用(看 profile 结果)
    - ET 方案主要耗时在调用函数 `operator[]`(看 profile 结果)，生成表达式耗时并不怎么耗时(看 benchmark 结果)
---
# 4. ET 方案说明
## 4.1 方案说明
1. lazy evaluation(惰性计算、延时计算): 一开始只是生成了表达式的实例(Add<Multiply, Multiply> expr)，并没有将结果正真的计算出来，等到 `collect_result` 时才开始遍历计算
## 4.2 在实现过程中用到的优化手段
1. 对 `size()` 和 `operator[]` 使用 inline 关键字
2. 在 `class BinaryExpression` 中存储 size，而不是每次在调用 `size()` 时根据 l_expr_ 和 r_expr_ 去判断(像其构造函数中的 assert 那样)
3. 使用 `class Add` 继承 `class BinaryExpression`，而不是将 `AddOperator` 当成 BinaryExpression 的一个模板参数，因为这样会多一次 `operator()` 函数调用
    ```cpp
    // bad
    template <IsExpr LExpr, IsExpr RExpr, class Op>
      requires SameItem<LExpr, RExpr>
    class BinaryExpression : public Expression {
    public:
      item_type operator[](size_t idx) inline {
        return Op(l_expr_[idx], r_expr_[idx]);
      }
    }
    template <class T>
    struct AddOperator {
      static T operator()(const T& lhs, const T& rhs) {
        return lhs + rhs;
      }
    };
    template <class LExpr, class RExpr>
    using Add = Binary<LExpr, RExpr, AddOperator<LExpr::item_type>::operator>;
    ```
---
# 5. 参考
- https://zhuanlan.zhihu.com/p/701819779
- https://zhuanlan.zhihu.com/p/416276856
- https://www.cnblogs.com/chengxuyuancc/p/3238469.html
- https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Expression-template
