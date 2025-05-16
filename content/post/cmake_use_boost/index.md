---
title: cmake导入boost使用
description: 通过cmake下载boost release压缩包并在c++项目中使用
date: 2024-10-29T21:17:03+08:00
draft: false
categories:
  - cpp
tags:
  - cpp
  - cmake
  - boost
  - thirdparty
---

# [cmake_use_boost](https://github.com/xuexcy/public_attachments/tree/main/cmake_use_boost)

```shell
.
├── CMakeLists.txt
├── build_and_run.sh
├── main.cc
├── output.txt
└── thirdparty
    └── boost
```
# CMakeLists.txt
```cmake title:CMakeLists.txt
cmake_minimum_required(VERSION 3.28)
project(cmake_use_boost)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置三方库存放目录为 项目根目录/thirdparty
set(THIRDPARTY_DIR ${CMAKE_SOURCE_DIR}/thirdparty)
message("thirdparty_dir: " ${THIRDPARTY_DIR})

include(FetchContent)

# import boost
# 设置需要使用的 boost 库
set(BOOST_INCLUDE_LIBRARIES bimap)
set(BOOST_ENABLE_CMAKE ON)
FetchContent_Declare(
    boost
    # boost release 下载地址
    URL https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-cmake.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP ON
    # 下载后解压到本地的目录
    SOURCE_DIR ${THIRDPARTY_DIR}/boost
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(boost)

add_executable(main main.cc)
# 链接 boost 库
target_link_libraries(main boost_bimap)
```

# main.cc
```cpp title:main.cc
#include "boost/bimap.hpp"
int main() {
    using bm_type = boost::bimap<int, std::string>
    using bm_value_type = bm_type::value_type;
    bm_type bm;
    bm.insert(bm_value_type(1, "one"));
    return 0;
}
```
