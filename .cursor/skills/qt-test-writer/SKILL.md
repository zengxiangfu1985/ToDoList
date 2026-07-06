---
name: qt-test-writer
description: >-
  Designs Qt Test cases (UT/ST/GT) from scenario diagrams and header contracts before
  code generation. Uses QTestLib, QSignalSpy, and GUI test IDs. Use for Phase 06
  test design or docs/test test-case documents.
---

# Skill: Qt Test Writer

> Phase 06 · 测试设计：在代码之前定义「正确」——为每个场景生成 UT/ST/GT 用例，ID 贯穿测试代码与验收报告。

## 触发条件

- 用户要求设计测试用例或 Qt Test 方案
- 用户提到「Phase 06」「测试设计」「测试先行」
- 场景时序图与头文件契约已完成

## 前置依赖

- 场景时序图（**必需**）
- `docs/api/*.h`（**必需**）
- `docs/database/`（有则读取）
- `requirements.md`（验收标准 AC-Sxx-n）

## 核心能力

1. 从头文件/API 约束提取 UT 用例
2. 从 DB 约束提取 UT 用例
3. 从时序图 Step 序列提取 ST（集成）用例
4. 从 EX 异常提取 ST 用例
5. 设计 GT（GUI）用例：QTest::mouseClick、控件状态
6. 建立 AC-Sxx-n → 用例 ID 追溯表

## Qt 测试类型

| 类型 | ID 前缀 | 工具 | 用途 |
|------|---------|------|------|
| 单元测试 | UT- | QTestLib | 类、算法、工具函数 |
| 场景/集成 | ST- | QTest + 多模块 | 时序主路径、EX 分支 |
| GUI 测试 | GT- | QTest GUI | 按钮、列表、对话框 |
| 基准 | BM- | QBENCHMARK | 图像/算法性能 |

## 执行步骤

### Step 1: 加载上下文

确认：Step 数、EX 数、涉及类、DB 表、AC 条目。

### Step 2: 设计单元测试（UT）

来源：

- **头文件**：返回值、错误码、边界参数
- **DB**：UNIQUE、NOT NULL、FK、CHECK
- **业务规则**：状态机、权限

格式示例：

```markdown
## UT-S01-01 — 使用有效设备 ID 连接相机

**类型**：Unit
**来源**：S01 Step 2
**输入**：`CameraManager::connectCamera("CAM_001")`
**预期**：返回 `true`，QSignalSpy 捕获 `cameraStatusChanged("CAM_001", Connected)`
**边界**：重复连接返回 `false`，errorMsg 为「设备已连接」
```

### Step 3: 设计场景测试（ST）

- **主路径**：Step 1→N 完整调用链，验证最终 DB/信号状态
- **异常路径**：每个 EX 至少 1 个 ST

### Step 4: 设计 GUI 测试（GT）

```markdown
## GT-S01-01 — 点击连接后列表更新

**类型**：GUI Test
**来源**：S01 Step 3 / AC-S01-2
**输入**：模拟点击「连接相机」，选择设备
**预期**：5 秒内 QListView 出现新条目，状态列「已连接」
**边界**：无设备时按钮 setEnabled(false)
```

**[manual] 规则**：需人工视觉判断、特定硬件、多显示器 DPI 感知 → ID 后缀 `[manual]`，不计入自动化覆盖率。

### Step 5: 覆盖度校验

- [ ] 每条 AC-Sxx-n 至少 1 个 UT/ST/GT
- [ ] 每个 EX 至少 1 个 ST
- [ ] 头文件每个 public 错误分支有 UT 或 ST
- [ ] DB 每个 UNIQUE/CHECK 有 UT

### Step 6: 验收追溯表

| AC ID | 验收条件 | 覆盖用例 |
|-------|---------|---------|
| AC-S01-1 | 5 秒内完成枚举 | ST-S01-01 |
| AC-S01-2 | 3 秒内预览 ≥25fps | GT-S01-01, BM-S01-01 |

## 输出规范

- **路径**：`docs/test/S01-test-cases.md`（每场景一文件）
- **ID 全局唯一**：`UT-S01-01`、`ST-S01-01`、`GT-S01-01`
- ID 是设计文档 ↔ `tests/` 代码 ↔ `test-results.xml` 的**绑定合约**

## 收尾步骤（强制）

更新 `qt-project.yaml` → `resource_index`。

## 推荐提示词

- `帮我设计 S01 的 Qt Test 用例`
- `基于时序图和头文件写 UT/ST/GT 测试规格`
