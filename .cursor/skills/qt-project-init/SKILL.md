---
name: qt-project-init
description: >-
  Initializes a Qt project following the Qt Specification Chain directory layout,
  qt-project.yaml index, and standard docs/src/tests structure. Use when creating
  a new Qt project, initializing spec-chain folders, or when the user mentions
  Qt project init, Phase 0, or qt-project.yaml setup.
---

# Skill: Qt Project Init

> 按 Qt 项目规格链初始化目录结构、`qt-project.yaml` 协作索引与标准文档占位，为 Phase 01 需求分析做准备。

## 触发条件

- 用户要求创建 Qt 项目或初始化规格链结构
- 当前项目缺少 `docs/prd/` 或 `qt-project.yaml`
- 用户提到「初始化 Qt 项目」「接入规格链」

## 核心能力

1. 创建 `docs/` 规格链目录（prd、api、database、test、changes）
2. 创建 `src/`、`tests/`、`resources/` 源码与资源目录
3. 生成 `qt-project.yaml` 项目索引
4. 生成根目录 `AGENTS.md`（AI 协作说明）
5. 可选：生成最小 `CMakeLists.txt` 或 `.pro` 骨架

## 执行步骤

### Step 1: 收集项目信息

向用户确认（信息不足时使用合理默认值）：

- **项目名称**、**一句话描述**
- **UI 技术路线**：Qt Widgets / QML / 混合
- **构建系统**：CMake / qmake
- **Qt 版本**：如 Qt 5.15、Qt 6.x
- **数据存储**：SQLite / QSettings / JSON / 无持久化
- **目标平台**：Windows / Linux / macOS

### Step 2: 创建目录结构

```
project-root/
├── docs/
│   ├── prd/
│   │   ├── 1-product-requirements/
│   │   └── 2-product-design/
│   │       └── prototypes/
│   ├── prd/3-technical-plan/
│   │   ├── 1-architecture/
│   │   └── 2-scenario-implementation/
│   ├── api/
│   ├── database/
│   ├── test/
│   └── changes/
├── src/
│   ├── core/
│   ├── ui/
│   ├── models/
│   └── utils/
├── tests/
│   ├── unit/
│   ├── gui/
│   └── integration/
├── resources/
│   ├── images/
│   └── styles/
├── qt-project.yaml
└── AGENTS.md
```

空目录写入 `.gitkeep`。

### Step 3: 生成 qt-project.yaml

```yaml
project:
  name: "{项目名称}"
  description: "{项目描述}"
  methodology: "Qt Specification Chain"

tech_stack:
  ui: "Qt Widgets"          # 或 QML
  language: "C++17"
  qt_version: "6.x"
  build_system: "CMake"     # 或 qmake
  database: "SQLite"        # 或 none / QSettings-only

scenarios: []               # Phase 01/架构阶段填充

resource_index: []

conventions:
  - "先规格，后代码——9 阶段按序推进"
  - "场景编号 S01/S02 贯穿全链路"
  - "公共 API 以 docs/api/*.h 头文件契约为准"
  - "用户可见字符串使用 tr()，资源放入 .qrc"
```

### Step 4: 生成 AGENTS.md

包含：规格链 9 阶段摘要、`qt-project.yaml` 读取规则、当前 Phase 检测逻辑、关键路径表。

**Phase 检测逻辑**：

| 条件 | 建议 Phase |
|------|-----------|
| 无 `requirements.md` | Phase 01 → qt-prd-writer |
| 无产品设计/原型 | Phase 02 → qt-product-designer |
| 无架构概要 | qt-architecture-designer |
| 无时序图 | Phase 03 → qt-scenario-architect |
| 无 `docs/api/*.h` | Phase 04 → qt-api-designer |
| 无 DDL/数据模型文档 | Phase 05 → qt-db-designer |
| 无测试用例文档 | Phase 06 → qt-test-writer |
| 无业务实现或测试代码 | Phase 07 → qt-code-implementor |
| 有代码但未验收 | Phase 08 → qt-acceptance-verifier |

### Step 5: 输出初始化报告

列出已创建的路径，建议下一步：「帮我写需求文档」（触发 `qt-prd-writer`）。

## 输出规范

- 所有规格文档路径与 `docs/Qt项目规格链.md` 附录一致
- 不覆盖已有 `requirements.md` 或源码，初始化前检查并询问
- `resource_index` 初始为空，各 Phase Skill 产出后追加

## 推荐提示词

- `帮我初始化 Qt 规格链项目`
- `按 Qt 项目规格链创建目录结构，项目名 xxx，使用 Qt Widgets + CMake`
