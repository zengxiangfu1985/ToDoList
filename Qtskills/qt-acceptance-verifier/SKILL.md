---
name: qt-acceptance-verifier
description: >-
  Runs Qt tests (ctest/QTest), cross-matches results with requirements and test-case
  IDs, and produces a traceable acceptance report with PASS/FAIL gate. Use for Phase
  08 acceptance verification or test-results.xml review.
---

# Skill: Qt Acceptance Verifier

> Phase 08 · 验收验证：读取测试输出，与需求 AC、测试用例文档交叉匹配，生成可追溯验收报告与 Gate 结论。

## 触发条件

- 用户要求验收、验证测试或通过 Phase 08
- 测试代码已运行，存在 `test-results.xml` 或 CTest 输出
- 用户提到「验收报告」「Gate PASS/FAIL」

## 前置依赖

- `docs/test/*-test-cases.md` 含 UT/ST/GT ID
- `requirements.md` 含 AC-Sxx-n
- 测试可编译运行（Phase 07 完成）

## 核心能力

1. 编译并运行 Qt Test / CTest
2. 解析 `test-results.xml` 或 JSONL
3. 映射：AC → 用例 ID → 运行结果
4. 统计覆盖率与失败项
5. 输出 Gate 结论（PASS / FAIL）及 Qt 特有验收项

## 执行步骤

### Step 1: 运行测试

```bash
mkdir build && cd build
cmake .. && cmake --build .
ctest --output-on-failure

# 或直接 Qt Test XML 输出
./tests/unit/test_app -xml -o ../docs/verify/test-results.xml
```

Windows MSVC 环境使用对应生成器（如 `-G "NMake Makefiles"` 或 Qt Creator 构建目录）。

### Step 2: 收集测试结果

读取：

- `docs/verify/test-results.xml`（Qt Test `-xml` / `-xunitxml`）
- 或 CTest 控制台输出
- 测试代码中的用例 ID（与文档对照）

**ID 必须一致**：`UT-S01-01` 在文档、代码、结果文件中完全相同。

### Step 3: 交叉匹配

构建三层追溯：

```
AC-S01-1 → ST-S01-01, GT-S01-01 → pass/fail
AC-S01-2 → GT-S01-02 → pass/fail
```

统计：

- 需求/AC 总数、已覆盖数
- UT/ST/GT 通过率
- 未覆盖 AC、失败用例列表

### Step 4: Qt 特有验收项

| 检查项 | 标准 |
|--------|------|
| 内存泄漏 | Debug 构建 + CRT/valgrind 无泄漏报告 |
| UI 响应 | 主线程阻塞操作 <100ms（或需求指定值） |
| 跨平台 | 目标平台矩阵编译通过（若适用） |
| 高 DPI | 125%/150% 缩放下布局正常（GT 或 [manual]） |
| 信号槽 | 跨线程连接无 direct 调用 UI |

标记 [manual] 用例：单独列出，不纳入自动化 Gate。

### Step 5: 生成验收报告

写入 `docs/verify/acceptance-report.md`：

```markdown
# 验收报告

**日期**：YYYY-MM-DD
**Gate 结论**：PASS / FAIL

## 摘要
| 类别 | 定义数 | 通过 | 失败 | 跳过 |
|------|--------|------|------|------|
| AC | 12 | 12 | 0 | 0 |
| UT | 24 | 23 | 1 | 0 |
| ST | 8 | 8 | 0 | 0 |
| GT | 4 | 4 | 0 | 0 |

## 未通过项
- UT-S02-05：`connectCamera` 超时处理未实现

## AC 追溯
| AC ID | 用例 | 结果 |
|-------|------|------|
| AC-S01-1 | ST-S01-01 | ✅ pass |

## Qt 专项
- 内存泄漏：✅
- UI 冻结：⚠️ GT-S03-02 超过 100ms
```

### Step 6: Gate 判定

- **PASS**：每条 AC 有对应通过测试（[manual] 需人工确认并记录）
- **FAIL**：列出缺口 → 回到规格（qt-change-writer）或代码（qt-code-implementor）

## 输出规范

- 报告：`docs/verify/acceptance-report.md`
- 原始结果：`docs/verify/test-results.xml`
- 更新 `qt-project.yaml` → `resource_index`

## 推荐提示词

- `运行测试并生成验收报告`
- `对照 AC-S01 检查测试是否全部通过`
