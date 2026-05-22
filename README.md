# AmazonChess

基于 Qt/C++ 实现的亚马逊棋（The Game of the Amazons）桌面应用。

## 项目介绍

本项目为 pku《计算概论》2025秋课程大作业，使用 C++17 结合 Qt5/6 实现了亚马逊棋对弈平台，支持人机与人人对战。AI 支持基础启发式算法和 Alpha-Beta 剪枝，界面风格美观，功能完善。

## 功能特点

- 支持亚马逊棋基本玩法：移动棋子+射箭封锁格子
- 人机对弈、双人对弈模式
- AI 可选择不同算法深度和评估函数，具有一定自我博弈能力
- 棋局保存与读取
- 撤销与重开
- 友好的 Qt 图形界面与简洁交互
- 棋盘界面美观，支持风格自定义

## 主要技术栈

- C++17
- Qt5/Qt6 Widgets
- CMake 构建

## 目录结构

```txt
AmazonChess/
├── AmazonChess/         # 源码目录
│   ├── main.cpp         # 程序入口
│   ├── mainwindow.*     # 主窗口与界面
│   ├── chessboard.*     # 棋盘逻辑与AI
│   ├── gamedialog.*     # 结束弹窗等
│   ├── ai_thread.*      # AI 线程
│   ├── style.qss        # 界面样式
│   ├── test.*           # 单元测试
├── Desktop/             # 部分环境兼容代码/入口（可选）
├── README.md            # 项目说明
```

## 编译与运行

1. 安装 Qt5/6、CMake
2. 在项目根目录下运行

```sh
mkdir build
cd build
cmake ..
make
./AmazonChess
```

如需 Windows 可用 Qt Creator 直接打开项目文件。

## 游戏规则简介

亚马逊棋是 10x10 棋盘，两方各 4 枚“亚马逊”棋子。轮到一方时必须先走一枚棋子，再从新位置射出一支“箭”封锁一格，轮到对方。无法落子/射箭一方判负。规则详情见 [Wikipedia](https://en.wikipedia.org/wiki/Game_of_the_Amazons)。

## TODO

- 优化 AI 算法与性能
- 联机对战/观战功能
- AI 多策略选择

## License

仅供课程作业交流学习使用。

---

如有问题欢迎 Issue 反馈和 PR 贡献。
