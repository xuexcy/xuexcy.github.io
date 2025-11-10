---
title: 备忘录
description: 开发，系统，软件等问题
date: 2025-06-12T18:32:28+08:00
draft: false
categories:
  - tips
tags:
  - MacOs
  - git
  - vscode
  - clangd
---
## 使用 https 从 github clone 仓库
1. 在 https://github.com/settings/tokens 生成一个新 token
2. 执行 git clone https://github.com/${user_name}/${repository_name}.git 并输入密码时，将密码输入成刚刚生成的 token
## vscode + clangd 插件 + cmake 配置代码提示
1. 在 `CMakeListst.txt` 中添加 `set(CMAKE_EXPORT_COMPILE_COMMANDS ON)`
2. clangd 插件的
	1. Arguments 中添加 `--compile-commands-dir=${workspaceFolder}/build`
	2. Fallback Flags 中添加 `--std=c++23`
3. 参考: https://github.com/xuexcy/cpp_cmake_project_template
## mac finder 默认以列表展示文件
```shell
defaults write com.apple.finder FXPreferredViewStyle -string "Nlsv"
sudo find / -name ".DS_Store"  -exec rm {} \;
killall Finder
```
## mac 禁用更新&取消更新提示小红点
1. 禁止更新: 打开文件 `sudo vim /etc/hosts`  添加如下内容
```
## Mac Software Update (sysin)
127.0.0.1 swdist.apple.com
127.0.0.1 swscan.apple.com
127.0.0.1 swcdn.apple.com  #optional, download url
127.0.0.1 gdmf.apple.com
127.0.0.1 mesu.apple.com
127.0.0.1 xp.apple.com
```
2. 取消更新提示小红点, 终端执行如下命令
```
defaults delete com.apple.systempreferences AttentionPrefBundleIDs
defaults delete com.apple.systempreferences DidShowPrefBundleIDs
killall Dock
```
3. 如果 xcode command line 需要更新，需要将上面添加的 host 内容注释掉
refs: https://sysin.org/blog/disable-macos-sonoma-update/

## mac intel 芯片安装 windows 11
https://www.bilibili.com/video/BV17m4y1L7ZU/?vd_source=fd19bce771432fd95d40a4fd97c8e733

## mac hhkb classic 键盘配置
1. 键盘背面打开 dip 2 切换到 mac 系统
2. 使用 Karabiner-Elements  改键 <br>
		a.  [下载软件](https://karabiner-elements.pqrs.org/) <br>
		b. [导入配置](https://github.com/xuexcy/personal_conf/tree/master/karabiner)
3.  mac 系统配置快捷键: 系统设置 -> 键盘快捷键 -> 启动台与程序坞 <br>
		a. 打开/关闭隐藏程序坞: command + L <br>
		b. 显示启动台: command + K 
##  键盘 [cmk87说明书](https://manuals.plus/zh-CN/fl-esports/cmk87-three-mode-87-keys-rgb-wireless-mechanical-keyboard-manual#axzz8WcZgApDl)
## 手表 [佳明265说明书](https://support.garmin.com/zh-CN/?productID=886785&tab=videos)
