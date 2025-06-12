---
title: 使用 https 从 github clone 仓库
description: 使用 https 从 github clone 仓库
date: 2025-06-12T18:32:28+08:00
draft: false
categories:
  - github
tags:
  - github
  - git
  - https
---
1. 在 https://github.com/settings/tokens 生成一个新 token
2. 执行 git clone https://github.com/${user_name}/${repository_name}.git 并输入密码时，将密码输入成刚刚生成的 token