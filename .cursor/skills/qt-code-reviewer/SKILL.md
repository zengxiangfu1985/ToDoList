---
name: qt-code-reviewer
description: >-
  Reviews C++/Qt code against header contracts in docs/api, scenario EX cases, DB DDL,
  and test-case IDs. Outputs structured Critical/Warning/Info report. Use after Phase
  07 implementation or when the user asks for Qt code review.
---

# Skill: Qt Code Reviewer

> 对照头文件契约、时序图 EX、DB DDL 与测试 ID，系统性审查 C++/Qt 实现，输出结构化审查报告。

## 触发条件

- 用户要求 Code Review 或审查 Qt 代码
- Phase 07 代码生成完成后
- 验收失败需定位规格偏差

## 前置依赖

- `docs/api/*.h` 头文件契约
- 场景时序图（含 EX）
- `docs/database/schema.sql`（如有）
- `src/`、`tests/` 可读

## 执行步骤

### Step 1: 加载参照基准

汇总审查范围：

```markdown
审查范围：S01
- 契约类：CameraManager, MainWindow
- EX 用例：5 个
- DB 表：devices
- 测试 ID：UT-S01-01~08, ST-S01-01~03
```

### Step 2: 头文件契约一致性

| 检查项 | 严重程度 |
|--------|---------|
| public 方法签名与 docs/api/*.h 一致 | Critical |
| signals 名称、参数类型一致 | Critical |
| Q_PROPERTY 与 QML 绑定一致（若用 QML） | Critical |
| Scenario 注释步骤均有实现 | Warning |
| 错误码/枚举与 api-spec.md 一致 | Critical |

### Step 3: 信号槽与线程审查

| 检查项 | 说明 |
|--------|------|
| 跨线程连接 | Worker→UI 必须 QueuedConnection |
| 主线程 UI | 非主线程不得直接操作 QWidget |
| 对象生命周期 | 断开/析构时 disconnect、QPointer |
| 阻塞点 | 主线程无 sync IO、sleep、大循环 |

### Step 4: EX 异常覆盖

逐一对照 EX-N.M：

- 是否有对应分支（错误码、失败信号、对话框）
- 是否静默吞异常（空 catch、仅 qDebug）
- 多步失败是否有回滚（DB transaction、资源释放）

### Step 5: DB / 数据层审查

- 表名、列名与 schema.sql 一致
- QSqlQuery 参数化，无字符串拼接 SQL
- 模型变更发射 dataChanged/rowsInserted
- SQLite 在正确线程使用连接

### Step 6: UI 与原型一致性

- objectName 与 `.ui` 原型一致
- 验收 AC 中的 enable/disable、超时行为
- tr() 包裹用户可见字符串
- .qrc 资源路径正确

### Step 7: 测试对齐

- tests/ 中 UT/ST/GT ID 与 docs/test/ 文档一致
- QSignalSpy 验证关键信号
- [manual] 用例已标注

### Step 8: 输出审查报告

```markdown
# 代码审查报告：S01

## 摘要
| 严重程度 | 数量 |
|---------|------|
| Critical | 1 |
| Warning | 2 |
| Info | 1 |

## Critical
### [C1] connectCamera 签名与契约不符
- **规格**：docs/api/camera_manager.h
- **问题**：缺少 errorMsg 出参
- **修复**：对齐头文件签名
```

**原则**：Critical 修复后再验收；每条发现引用规格来源。

## 输出规范

- 报告输出到对话；可选写入 `docs/verify/code-review-S01.md`
- 不直接修改代码（除非用户明确要求修复）

## 推荐提示词

- `审查 S01 的 Qt 实现是否符合头文件契约`
- `检查信号槽线程安全和 EX 异常覆盖`
