---
name: qt-merge-executor
description: >-
  Merges change proposal delta files into main Qt Specification Chain documents under
  docs/. Applies ADDED/MODIFIED/REMOVED blocks. Use after qt-change-writer when the
  user asks to merge deltas or execute spec merge.
---

# Skill: Qt Merge Executor

> 将 `docs/changes/<slug>/deltas/` 中的 delta 合并入主规格文档，保持格式一致，合并后标记 SPEC_MERGED。

## 触发条件

- 用户要求合并变更、执行 merge delta
- `docs/changes/<slug>/deltas/` 已就绪且用户已确认提案
- qt-change-writer 完成 delta 产出

## 前置依赖

- `docs/changes/<slug>/proposal.md` 与 `tasks.md` 存在
- deltas 目录含 ADDED/MODIFIED/REMOVED 标记文件

## 核心能力

1. 解析 delta 文件与目标主文档映射
2. 执行 ADDED / MODIFIED / REMOVED 合并
3. 同步更新 `qt-project.yaml` → `resource_index`
4. 输出变更摘要；**不自动运行测试或验收**（人类确认点）

## Delta → 主文档映射

| Delta 路径 | 主文档路径 |
|-----------|-----------|
| `deltas/prd/...` | `docs/prd/...` |
| `deltas/api/...` | `docs/api/...` |
| `deltas/database/...` | `docs/database/...` |
| `deltas/test/...` | `docs/test/...` |

代码（`src/`、`tests/`）**无 delta**，由 qt-code-implementor 直接修改。

## 执行步骤

### Step 1: 读取合并清单

从 `tasks.md` 的 `[delta]` section 列出待合并文件，通读所有 delta 与目标主文档。

### Step 2: 逐个合并

对每个 delta：

1. 读取 ADDED / MODIFIED / REMOVED 块
2. 定位主文档对应章节（标题模糊匹配时注意空格/层级）
3. 执行合并：
   - **ADDED**：在指定位置插入
   - **MODIFIED**：替换同名章节全文
   - **REMOVED**：删除章节并保留删除原因注释（可选）

**合并原则**：

- 只改 delta 指定部分，不重排无关内容
- 找不到章节 → 暂停询问用户
- 合并 `.h` / `.ui` / `.sql` 整文件时，以 delta 中 MODIFIED 全文为准

### Step 3: 更新索引

合并完成后更新 `qt-project.yaml` → `resource_index`（新增/变更文件各一条）。

### Step 4: 标记与摘要

- 创建 `docs/changes/<slug>/SPEC_MERGED` 空文件或写入合并日期
- 输出摘要：

```
合并完成：
- docs/prd/1-product-requirements/requirements.md：新增 1 节，修改 2 节
- docs/api/measurement_engine.h：MODIFIED 全文件
```

### Step 5: 提示后续（不自动执行）

```
✅ 规格已合并。接下来：

1. 按 qt-code-implementor 实现 [code] 任务
2. 运行构建与 ctest
3. 使用 qt-acceptance-verifier 生成验收报告
4. 验收通过后归档提案（proposal.md 状态 → MERGED）
```

**人类确认点**：测试、验收、git commit 仅在用户明确要求时执行。

若用户要求 commit，使用消息如：`docs(<slug>): merge Qt spec deltas`

## 实践经验

- 先读完所有 delta 再动手
- MODIFIED 章节标题必须与主文档一致
- 合并信号/槽变更时，提醒后续全项目 connect 检查
- 合并 schema 时，确认 migration 文件已一并合并

## 推荐提示词

- `合并 add-thermal-compensation 的 delta 到主文档`
- `执行变更合并，更新 requirements 和头文件契约`
