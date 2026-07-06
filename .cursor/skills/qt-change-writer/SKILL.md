---
name: qt-change-writer
description: >-
  Creates Qt Specification Chain change proposals (proposal.md, tasks.md) analyzing
  impact on requirements, .ui/.qml, headers, schema, and tests. Use for Phase 09
  delta changes, bug fixes, or feature iterations in Qt projects.
---

# Skill: Qt Change Writer

> Phase 09 · 变更提案：每次功能变更或 Bug 修复从 Delta 提案开始，先规格后代码，保持全链路可追溯。

## 触发条件

- 用户描述新功能、需求变更或 Bug 修复
- 用户提到「变更提案」「Phase 09」「迭代」
- 需分析对 `.ui`、信号槽签名、DB Schema 的影响

## 前置依赖

- 项目已有主规格文档（`docs/` 下）
- 变更目录：`docs/changes/<slug>/`（不存在则创建）

## 核心能力

1. 理解变更意图与关联场景
2. 扫描 docs/ 定位影响范围
3. 判断变更类型与最小更新集
4. 生成 proposal.md 与 tasks.md
5. 产出 delta 文件（不直接改主文档）

## Qt 变更风险速查

| 变更类型 | 风险 | 缓解 |
|---------|------|------|
| 信号/槽签名 | connect 静默失败 | 全项目搜索 connect，编译验证 |
| Q_PROPERTY | QML 绑定失效 | 同步 QML 属性名 |
| DB Schema | 旧数据不兼容 | migration SQL + 版本号 |
| .ui objectName | ui->xxx 编译错误 | 同步 C++ 与 .ui |
| 新增第三方库 | 部署依赖增加 | 更新 CMake/.pro，验证链接 |

## 执行步骤

### Step 1: 理解变更意图

确认：变更是什么、原因、关联场景 S0x。

### Step 2: 分析影响范围

扫描：

- `docs/prd/1-product-requirements/`
- `docs/prd/2-product-design/`（含 prototypes）
- `docs/prd/3-technical-plan/`
- `docs/api/`
- `docs/database/`
- `docs/test/`
- `src/`、`tests/`（代码影响，不产出 delta）

### Step 3: 判断变更类型

| 类型 | 最少更新 |
|------|---------|
| 需求级 | 需求→设计→场景→接口→数据→测试→代码 |
| 设计级 | 原型+场景+接口+测试+代码 |
| 接口级 | 头文件+数据+测试+代码+全 connect |
| 数据级 | schema+migration+Model+测试+代码 |
| 代码级 | 代码+测试+重新验收 |

### Step 4: 生成 proposal.md

```markdown
# 变更提案：add-thermal-compensation

## 变更原因
用户反馈高温环境下测量精度下降。

## 变更类型
需求级

## 影响范围
- 需求：新增 S04 — 温度补偿
- UI：mainwindow.ui 新增「温度补偿」开关
- 接口：MeasurementEngine::setTemperature(double)
- 数据：measurements 表新增 temperature 字段
- 测试：UT-S04-01、GT-S04-01
- 代码：src/core/measurementengine.cpp

## 回滚策略
通过 QSettings 开关禁用补偿功能。
```

### Step 5: 生成 tasks.md

```markdown
# 实现任务

## [delta] 规格变更
- [ ] deltas/prd/1-product-requirements/requirements.md — 新增 S04
- [ ] deltas/prd/2-product-design/prototypes/mainwindow.ui — 新增控件
- [ ] deltas/prd/3-technical-plan/2-scenario-implementation/S04-*.md
- [ ] deltas/api/measurement_engine.h
- [ ] deltas/database/migration_v2.sql
- [ ] deltas/test/S04-test-cases.md

## [code] 代码实现
- [ ] src/core/measurementengine.cpp
- [ ] tests/unit/test_measurement_engine.cpp
```

**禁止**在 tasks.md 写验收命令；验收由 qt-acceptance-verifier 独立执行。

### Step 6: 产出 Delta 文件

写入 `docs/changes/<slug>/deltas/`，镜像 `docs/` 结构：

```markdown
## ADDED — S04 温度补偿
[完整新增章节内容]

## MODIFIED — MeasurementEngine 接口
[修改后的完整章节或文件内容]

## REMOVED — ...
[删除说明]
```

**禁止直接修改 `docs/` 主文档**——由 qt-merge-executor 合并。

每完成一个 delta，更新 tasks.md 对应项为 `[x]`。

### Step 7: 引导后续

1. 用户确认 → qt-merge-executor 合并 delta
2. qt-code-implementor 实现代码
3. qt-acceptance-verifier 验收
4. 归档：将提案标记 `MERGED`（在 proposal.md 或 changes 目录 README）

## 输出规范

- `docs/changes/<slug>/proposal.md`
- `docs/changes/<slug>/tasks.md`
- `docs/changes/<slug>/deltas/**`

## 推荐提示词

- `我要给主界面加温度补偿，帮我写变更提案`
- `这个 Bug 只改 connect 逻辑，分析影响范围`
