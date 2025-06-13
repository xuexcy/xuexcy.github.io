---
title: 小问题集合
description: 小问题集合
date: 2025-06-12T18:32:28+08:00
draft: false
categories:
  - tips
tags:
  - tips
---
# 使用 https 从 github clone 仓库
1. 在 https://github.com/settings/tokens 生成一个新 token
2. 执行 git clone https://github.com/${user_name}/${repository_name}.git 并输入密码时，将密码输入成刚刚生成的 token
# vscode + clangd 插件 + cmake 配置代码提示
1. 在 `CMakeListst.txt` 中添加 `set(CMAKE_EXPORT_COMPILE_COMMANDS ON)`
2. clangd 插件的
	1. Arguments 中添加 `--compile-commands-dir=${workspaceFolder}/build`
	2. Fallback Flags 中添加 `--std=c++23`
3. 参考: https://github.com/xuexcy/cpp_cmake_project_template