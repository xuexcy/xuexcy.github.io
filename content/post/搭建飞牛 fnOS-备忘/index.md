---
title: 搭建飞牛 fnOS-备忘
description: 给自己备忘
date: 2025-11-12T21:39:31+08:00
draft: false
categories:
  - 备忘
tags:
  - nas
  - 备忘
  - 飞牛OS
---
## 说明
1. 安装时间: 2025-11-12
2. fnOS 版本: 0.9.35
3. 设备: 2013 款华硕 x550vc 笔记本
    - cpu: intel 酷睿 i5-3230M 2.6G HZ 双核
    - cpu 核显: hd graphics 4000
    - gpu: nvidia geforce gt 720M
    - 硬盘: 三星 850 evo ssd 256G (自己加的)
## 系统安装
### 准备
1. 网线: <font color="#ff0000">需要使用笔记本的有线网口</font>
    - 系统装完最后一步需要分配 ip，此时只能通过网线分配， 不能通过连接 wifi 分配。
    - 官方有在一些帖子中回复， 有计划后续支持安装系统时通过 wifi 分配 ip
2. 16G U 盘用于刻制 fnOS
3. 一台装好 windows 系统的电脑，用于将 fnOS 刻录到 U 盘
    - 通过 U 盘将 fnOS 装到电脑硬盘时，需要将<font color="#ff0000">整个硬盘抹掉</font>（不是分区，是整个物理硬盘）。(我的 ssd 本来是两个分区，一个装了 windows 10，本来打算用另一个分区装 fnOS 的，这样就可以双系统，在有需要的时候还可以切回 windows 使用，但实际上是不行的)
    - mac/linux 系统应该也有可以刻录 iso 文件的工具，飞牛官方的指导文档中给的是 windows 中的刻录软件的下载链接

### 安装方式及安装时的问题
1. 按官网指导方式安装: [如何安装和初始化飞牛 fnOS？](https://help.fnnas.com/articles/fnosV1/start/install-os.md)
2. U 盘安装系统时，按 Esc 进入 boot 界面，选择 U 盘启动即可
3. U 盘安装最后一步，识别到网卡但是显示 “网卡未连接”
    - 关机再来，多装几次，在分配 ip 的地方也多刷新检测几次
    - 检测成功后，如果没有分配 ip，就手动填写 ip/掩码等

## 系统使用
### 基础设置
1. 系统设置 -> 网络设置: 在网口或网卡中将 ip 设置成固定 ip
2. UPS 管理: 启用 UPS 支持
3. 设置笔记本盒盖不休眠
    1. 修改 `/etc/systemd/logind.conf` 文件
		 ```
		 sudo vim /etc/systemd/logind.conf
		 HandleSuspendKey=ignore
		 HandleLidSwitch=ignore
		 HandleLidSwitchExternalPower=ignore
		 ```
	2. 重启系统

### 一些问题
1. 当前 fnOS 版本和官方驱动不支持本电脑的 cpu 核显和 gpu

### 功能使用
1. 家庭相册(TODO)