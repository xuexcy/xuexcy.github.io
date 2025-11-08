---
title: 阿姆达尔(Amdahl)定律公式推导
description: Amdahl's law
date: 2025-06-20T13:52:25+08:00
draft: false
categories:
  - 并行
tags:
  - 并行
math: mathjax
---
## Amdahl 定律公式推导

-  Amdahl 定律：串行片段所占总体程序比例  $f_s$ , 那么使用 $N$ 个处理器取得的<font color="#ff0000">整体性能增益</font> $P$ 是: $$ \Large{P = \frac{1}{f_s + \frac{1 - f_s}{N}}}$$
	- 推导:
		1. 假设原来需要的时间为  $T_{old}$，串行程序占比 $f_s$，并行程序占比 $(1 - f_s)$，则：$$\Huge{T_{old} = f_s * T_{old} + (1 - f_s) * T_{old}}$$
		2. 使用 $N$ 个处理器后加速了并行部分后，新的耗时为：$$\Large{T_{new} = f_s * T_{old} + \frac{(1 - f_s) * T_{old}}{N}}$$
		3. 性能增益(耗时的倒数代表性能)为: $$ \Huge{P = \frac{\frac{1}{T_{new}}}{\frac{1}{f_old}}=\frac{T_{old}}{T_{new}} = \frac{1}{f_{s} + \frac{1 - f_{s}}{N}}}$$
## 可伸缩性和 Amdahl 定律
> C++并发编程实战 (第2版) 第 8 章 8.4.2 可伸缩性和 Amdahl 定律
>> 1. 可伸缩性: 可伸缩性的意义是，随着处理器数目增加，将缩短程序执行操作的时间，或者，程序在给定时间内所能处理的数据量会增加。
>> 2. Amdahl 定律:
>>
>> 		a. 若每一项操作都能并行化，则”串行“片段的比例为 0，加速比是 $N$(前文性能增益 $P$ 的值)。再举一例，如果”串行“片段的比例为 1/3，即便无限增加处理器数目，加速比也不可能超过 3。
>>
>>	  b. 若我们可以缩短”串行“片段的长度，或降低线程等待的概率，便能在具备更多处理器的系统上提高性能增益。对应地，如果我们可以向系统提供更多的数据以进行处理，从而令”并行“片段一直忙于工作，也能降低”串行“片段所占的比例，从而提升性能增益 $P$。
>> 3. 线程不一定总会执行实际工作。有时候它们需要等待其他线程，或等待 I/O 操作完成，或等待其他事项。<font color="#ff0000">如果在等待期间，我们为系统指派实际工作，即可”隐藏“等待行为</font>
>

# refs
1. https://en.wikipedia.org/wiki/Amdahl%27s_law
2. https://zh.wikipedia.org/wiki/%E9%98%BF%E5%A7%86%E8%BE%BE%E5%B0%94%E5%AE%9A%E5%BE%8B
3. [# C++并发编程实战 (第2版)](https://book.douban.com/subject/35653912/)
