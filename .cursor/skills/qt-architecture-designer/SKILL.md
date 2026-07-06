---
name: qt-architecture-designer
description: >-
  Establishes Qt project technical architecture before scenario modeling: Widgets vs
  QML, threading model, build system, module boundaries, and non-functional constraints.
  Use between Phase 02 and 03, or when the user mentions Qt architecture or tech stack.
---

# Skill: Qt Architecture Designer

> Phase 2→3 桥梁：在场景建模前建立 Qt 项目技术全局视图——模块划分、线程模型、构建与部署约束。

## 触发条件

- Phase 02 产品设计完成，即将进入场景建模
- 用户要求 Qt 技术选型、架构设计或模块划分
- 需要确定 Widgets/QML、CMake/qmake、SQLite 等

## 核心能力

1. 读取需求与设计文档，理解场景复杂度
2. 绘制 Qt 模块架构图（Mermaid）
3. 定义线程与信号槽边界（主线程 vs QThread）
4. 确定构建系统、Qt 版本、第三方依赖
5. 更新 `qt-project.yaml` 的 `tech_stack`

## 执行步骤

### Step 1: 理解产品全貌

读取：

- `requirements.md`：场景、性能/离线约束
- `design-spec.md` + 原型：UI 复杂度、窗口数量
- `qt-project.yaml`：已有 tech_stack

提取：实时性、硬件集成、数据量、目标平台。

### Step 2: 确定模块架构

典型 Qt 桌面应用分层：

```
src/
├── ui/          # 窗口、.ui 绑定、QSS
├── core/        # 业务逻辑 QObject（CameraManager 等）
├── models/      # QAbstractItemModel 子类
└── utils/       # 工具、算法
```

用 Mermaid 绘制模块与依赖（UI → Core → Models → DB/IO）。

**Mermaid 语法安全**：节点标签用 `ID["显示名"]`，含特殊字符时加双引号。

### Step 3: 线程与事件循环策略

为每类耗时操作定义策略：

| 操作类型 | 推荐方案 |
|---------|---------|
| 网络/磁盘 IO | `QThread` + worker QObject，信号回主线程 |
| CPU 密集算法 | `QtConcurrent` 或专用 QThread |
| 相机/采集 | 独立采集线程，帧通过 `frameReady` 信号传递 |
| SQLite 写入 | 独立线程或 `QSqlDatabase` 连接 per-thread |

标注：**哪些类必须在主线程创建/调用**。

### Step 4: 技术选型表

```markdown
| 维度 | 选型 | 理由 |
|------|------|------|
| UI | Qt Widgets 5.15 | 团队熟悉，Designer 工作流 |
| 构建 | CMake 3.16+ | 跨平台 CI 友好 |
| 数据库 | SQLite + QSqlDatabase | 本地零配置 |
| 配置 | QSettings (INI) | 用户偏好、窗口状态 |
| 国际化 | Qt Linguist (.ts) | 多语言现场设备 |
```

### Step 5: 非功能性约束

- 界面冻结阈值（如 >100ms 视为需优化）
- 内存与泄漏检测策略（MSVC / valgrind / Qt 调试）
- 跨平台编译矩阵
- 日志：`qDebug` / 文件日志 / 分类 `QLoggingCategory`

### Step 6: 预写场景清单

向用户确认 `qt-project.yaml` → `scenarios` 与 requirements 一致（供 qt-scenario-architect 使用）。

## 输出规范

- **路径**：`docs/prd/3-technical-plan/1-architecture/architecture-overview.md`（全局唯一，后续在此文件迭代）
- 更新 `qt-project.yaml`：`tech_stack`、`scenarios`、`resource_index`

**skip_phases 提示**（写入 architecture 文档）：

- 无 SQLite → 可跳过 qt-db-designer 的 DDL 部分，仅 QSettings
- 纯算法库无 UI → 跳过 qt-product-designer 的 UI 原型

## 推荐提示词

- `帮我设计 Qt 项目技术架构`
- `确定 Widgets + SQLite 的模块划分和线程模型`
