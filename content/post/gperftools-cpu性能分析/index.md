---
title: gperftools-cpu性能分析
description: 通过 gperf 对程序进行性能分析
date: 2025-04-18T16:38:34+08:00
draft: false
categories:
  - cpp
tags:
  - cpp
  - cmake
  - gperf
  - 性能分析
---

# 软件安装
- 在 mac os 上安装 `brew install gperftools`
# 在 CMakeLists.txt 中链接 profiler
- 假设程序为：[calculation_profile.cc](https://github.com/xuexcy/learning_more_cpp_idioms/blob/main/src/expression_template_deps/test_array/CMakeLists.txt)
```cmake
set(LIB_PROFILER_DIR "/opt/homebrew/Cellar/gperftools/2.16/lib")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${LIB_PROFILER_DIR} -lprofiler -ltcmalloc")
add_executable(expression_template_array_profile calculation_profile.cc)
```
# 执行 profiler
```bash
bin_name=expression_template_array_profile
prof_file=$bin_name.prof
text_file=$bin_name.txt
svg_file=$bin_name.svg

# 生成 prof 文件
env CPUPROFILE=$prof_file $bin_name

# 输出文本和 svg 图片
pprof $bin_path $prof_file --text > $text_file
pprof $bin_path $prof_file --svg > $svg_file
```
# TODO
火焰图
# 其他
- 遇到问题 `otool-classic: can't open file: /usr/lib/system/libXXXXX`，好像可以不用管，不影响结果。
	- refs: [# otool-classic unable to find /usr/lib libraries on MacOS](https://github.com/google/pprof/issues/726)
	- https://forums.developer.apple.com/forums/thread/722360