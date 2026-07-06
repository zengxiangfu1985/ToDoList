# 验收报告 — ToDoList MVP + S04

**日期**：2026-07-04  
**Gate 结论**：**CONDITIONAL PASS**

> 自动化 UT 全部通过；S01/S02/S03 部分 AC 与 S04 托盘/快捷键为 GUI 手工项，需人工确认后升为 PASS。

## 摘要

| 类别 | 定义数 | 通过 | 失败 | 跳过/待人工 |
|------|--------|------|------|-------------|
| AC（自动化可测） | 8 | 8 | 0 | 0 |
| AC（GUI/集成） | 7 | — | — | 7 [manual] |
| UT | 7 | 7 | 0 | 0 |
| GT | 3 | — | — | 3 [manual] |

**测试源文件**：`docs/verify/test-results.xml`（Qt 5.15.2 QTest）

## UT 结果

| 用例 ID | 函数名 | 结果 | 耗时 |
|---------|--------|------|------|
| UT-S01-01 | ut_S01_01_addTask | ✅ pass | 16ms |
| UT-S01-02 | ut_S01_02_emptyTitle | ✅ pass | 11ms |
| UT-S02-01 | ut_S02_01_ruleScorePerformance | ✅ pass | <1ms |
| UT-S02-02 | ut_S02_02_ruleTop3Fallback | ✅ pass | <1ms |
| UT-S03-01 | ut_S03_01_factoryCreatesAll | ✅ pass | <1ms |
| UT-S03-02 | ut_S03_02_deepseekRequiresApiKey | ✅ pass | <1ms |
| UT-S03-03 | ut_S03_03_settingsRoundTrip | ✅ pass | <1ms |

## AC 追溯矩阵

| AC ID | 对应用例 | 结果 | 说明 |
|-------|---------|------|------|
| AC-S01-1 | UT-S01-01 | ✅ | SQLite 写入验证 |
| AC-S01-EX1 | UT-S01-02 | ✅ | 空标题拒绝 |
| AC-S01-2 | — | [manual] | 象限编辑 UI 待 GT |
| AC-S01-3 | — | [manual] | 重启加载待 GT |
| AC-S02-1 | UT-S02-01 | ✅ | 50 任务 <200ms |
| AC-S02-EX1 | UT-S02-02 | ✅ | 规则层 Top3 降级 |
| AC-S02-2 | — | [manual] | 需 Ollama 联调 |
| AC-S02-3 | UT-S02-02 | ✅ | 理由 ≥10 字 |
| AC-S03-1 | UT-S03-01 | ✅ | 四种 Provider |
| AC-S03-2 | UT-S03-03 | ✅ | QSettings 往返 |
| AC-S03-3 | UT-S03-02 | ✅ | 无 Key 时 isConfigured=false |
| AC-S03-EX1 | — | [manual] | Ollama 错误文案需联调 |
| AC-S04-1 | GT-S04-01 | [manual] | 托盘最小化 |
| AC-S04-2 | GT-S04-02 | [manual] | Win+Shift+T |
| AC-S04-3 | GT-S04-03 | [manual] | Win+Shift+P |

## Qt 专项

| 检查项 | 结果 |
|--------|------|
| 编译（Win MinGW Qt 5.15.2） | ✅ |
| LLM 异步（QtConcurrent） | ✅ 主线程不阻塞 |
| 跨线程信号 | ✅ QueuedConnection |
| 内存泄漏 | ⚠️ 未跑 valgrind/CRT（Debug 构建） |
| 高 DPI | ⚠️ [manual] |

## 缺口与建议

1. **GT-S04-01~03**：在 Windows 上手工验证托盘与全局快捷键（注意与其他应用快捷键冲突）。
2. **AC-S02-2**：启动 Ollama + Qwen 后点击「AI 分析优先级」确认 5 秒内 Top 3。
3. **连接名警告**：测试中出现 `duplicate connection name 'todolist_conn'`，建议 TaskRepository 使用唯一连接名或单例。

## 复现命令

```powershell
cd d:\Qt\QtProjects\ToDoList\build-tests
qmake ..\tests\tests.pro
mingw32-make
.\debug\tst_core.exe -xml -o ..\docs\verify\test-results.xml
```

```powershell
cd d:\Qt\QtProjects\ToDoList\build
.\debug\ToDoList.exe
```

## Gate 判定说明

- **CONDITIONAL PASS**：核心数据层、规则优先级、LLM Provider 抽象自动化测试全部通过；S04 与部分 UI/联调 AC 标记 [manual]，完成人工清单后可升级为 **PASS**。
