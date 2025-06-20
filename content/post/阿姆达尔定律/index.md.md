---
title: 阿姆达尔定律
description: Amdahl's law
date: 2025-06-20T13:52:25+08:00
draft: false
categories:
  - 并行
tags:
  - 并行
---
## 8.4.2 可伸缩性和 Amdahl 定律
-  Amdahl 定律：串行片段所占总体程序比例  $f_s$ , 那么使用 $N$ 个处理器取得的<font color="#ff0000">整体性能增益</font> $P$ 是: $$ \Large{P = \frac{1}{f_s + \frac{1 - f_s}{N}}}$$
	- 推导:
		1. 假设原来需要的时间为  $T_{old}$，串行程序占比 $f_s$，并行程序占比 $(1 - f_s)$，则：$$\Huge{T_{old} = f_s * T_{old} + (1 - f_s) * T_{old}}$$
		2. 使用 $N$ 个处理器后加速了并行部分后，新的耗时为：$$\Large{T_{new} = f_s * T_{old} + \frac{(1 - f_s) * T_{old}}{N}}$$
		3. 性能增益(耗时的倒数代表性能)为: $$ \Huge{P = \frac{\frac{1}{T_{new}}}{\frac{1}{f_old}}=\frac{T_{old}}{T_{new}} = \frac{1}{f_{s} + \frac{1 - f_{s}}{N}}}$$
- 可伸缩性: 增加处理器数量，将缩短程序执行时间或增加单位时间内数据处理量
- 线程在等待期间，为系统指派别的实际工作，可以<font color="#ff0000">"掩藏"等待行为</font>

# refs
1. https://en.wikipedia.org/wiki/Amdahl%27s_law
2. https://zh.wikipedia.org/wiki/%E9%98%BF%E5%A7%86%E8%BE%BE%E5%B0%94%E5%AE%9A%E5%BE%8B