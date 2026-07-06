---
name: qt-product-designer
description: >-
  Converts Qt requirements into design specs and previewable UI prototypes (.ui or
  .qml). Covers window layout, signals, modal relationships, and interaction-level
  acceptance. Use for Phase 02 product design, Qt Designer, or QML prototypes.
---

# Skill: Qt Product Designer

> Phase 02 · 产品设计：读取 `requirements.md`，产出功能规格与可预览的 Qt UI 原型（Widgets `.ui` 或 QML），作为后续场景与实现的视觉契约。

## 触发条件

- 用户要求 Qt 界面设计、功能规格或 UI 原型
- 用户提到「Phase 02」「产品设计」
- 已有 `requirements.md`，需开始设计解决方案

## 前置依赖

- `docs/prd/1-product-requirements/requirements.md` 存在且含场景清单

## 核心能力

1. 确认 UI 技术路线（Qt Widgets / QML / 混合）
2. 设计窗口结构与导航（QMainWindow、QDialog、StackedWidget 等）
3. 逐场景细化交互规格与控件级验收条件
4. 生成 `.ui` 或 `.qml` 原型文件
5. 标注关键信号（如 `clicked()`、`currentIndexChanged()`）

## 执行步骤

### Step 1: 读取需求并确认技术路线

从 `requirements.md` 和 `qt-project.yaml` 的 `tech_stack.ui` 提取：

- 场景清单 S01、S02…
- Widgets 还是 QML
- 窗口间模态/非模态关系

**场景粒度检查**：若 Phase 01 为 CRUD 级碎片场景，建议回到 qt-prd-writer 重组后再设计。

### Step 2: 设计信息架构

桌面 Qt 应用重点：

- 主窗口 / 子窗口 / 对话框层级
- 菜单栏、工具栏、状态栏、Dock 布局
- 快捷键与 `QAction` 映射
- Model/View 数据展示区域（QTableView、QListView 等）

输出窗口结构树或表格。

### Step 3: 逐场景细化交互规格

每个场景包含：

- **涉及窗口/页面**
- **交互流程**（ numbered steps）
- **控件清单**（objectName 命名规范，便于 `ui->` 或 findChild）
- **状态变化**（enabled/disabled、进度、错误提示）
- **交互级验收条件**（在 Phase 01 AC 基础上细化到控件）

示例：

```markdown
### S01 — 设备连接 — 交互规格

**涉及窗口**：MainWindow、DeviceListPanel

**交互流程**：
1. 用户点击工具栏「扫描设备」→ 触发 `scanDevices()` 槽
2. QListView 显示枚举结果；无设备时按钮置灰
3. 双击列表项 → 发起连接，状态栏显示进度

**关键信号**：
- `QPushButton::clicked` → `on_scanButton_clicked`
- `CameraManager::deviceListUpdated`
```

### Step 4: 生成 UI 原型

**Qt Widgets**：

- 产出 `docs/prd/2-product-design/prototypes/*.ui`
- 设置有意义的 `objectName`
- 在注释或 design-spec 中标注 objectName ↔ 业务含义

**QML**：

- 产出 `docs/prd/2-product-design/prototypes/*.qml`
- 标注 `id`、`signal`、属性绑定

原型须能在 Qt Designer 或 QML Live Preview 中打开预览。

### Step 5: 输出设计文档

- `docs/prd/2-product-design/design-spec.md`：按场景组织的交互规格汇总
- 原型文件与场景编号对应（如 `S01-device-connect.ui`）

## 输出规范

| 产物 | 路径 |
|------|------|
| 功能规格 | `docs/prd/2-product-design/design-spec.md` |
| UI 原型 | `docs/prd/2-product-design/prototypes/` |
| 场景编号 | 与 Phase 01 一致；拆分子场景用 S01.1 |

**Qt 特有注意事项**：

- 明确 Widgets vs QML，不在同一文件混用两种范式
- 标注线程敏感 UI（耗时操作不得阻塞主线程）
- 高 DPI：`Qt::AA_EnableHighDpiScaling` 或 Qt6 默认策略

## 收尾步骤（强制）

更新 `qt-project.yaml` → `resource_index`（design-spec + 各原型文件各一条）。

## 推荐提示词

- `基于需求文档做 Qt 产品设计`
- `帮我设计 S01 的界面原型，使用 Qt Widgets`
