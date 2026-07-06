# Qt Skills — Qt 项目规格链 Skill 集

本目录包含适配 **Qt 项目规格链**（见 `docs/Qt项目规格链.md`）的 Cursor Agent Skills，参照 `skills/` 中互联网项目 OpenLogos Skills 的结构与 workflow 编写，路径与产物格式已替换为 Qt/C++ 约定。

## 规格链与 Skill 映射

| Phase | 阶段 | Skill | 主要产物 |
|-------|------|-------|---------|
| 01 | 需求分析 | `qt-prd-writer` | `docs/prd/1-product-requirements/requirements.md` |
| 02 | 产品设计 | `qt-product-designer` | 功能规格 + `.ui`/`.qml` 原型 |
| — | 技术架构（Phase 2→3 桥梁） | `qt-architecture-designer` | `docs/prd/3-technical-plan/1-architecture/` |
| 03 | 场景建模 | `qt-scenario-architect` | 信号槽时序图 + 异常用例 |
| 04 | 接口设计 | `qt-api-designer` | C++ 头文件契约（`docs/api/*.h`） |
| 05 | 数据设计 | `qt-db-designer` | SQLite DDL / 数据模型 / QSettings Schema |
| 06 | 测试设计 | `qt-test-writer` | UT/ST/GT 测试用例文档 |
| 07 | 代码生成 | `qt-code-implementor` | `src/`、`tests/`、构建配置 |
| 08 | 验收验证 | `qt-acceptance-verifier` | 验收报告（Gate PASS/FAIL） |
| 09 | 变更提案 | `qt-change-writer` | `docs/changes/<slug>/proposal.md` |

## 支撑 Skill

| Skill | 用途 |
|-------|------|
| `qt-project-init` | 初始化 Qt 规格链目录结构与 `qt-project.yaml` |
| `qt-code-reviewer` | 对照头文件契约、时序图 EX、DDL 审查 C++/Qt 代码 |
| `qt-merge-executor` | 将变更 delta 合并入主规格文档 |

## 与互联网 Skills 的差异

- **无 OpenAPI / HTTP 编排**：接口契约为 C++ 头文件；跳过 `test-orchestrator`、部署编排类 Skill
- **路径**：`docs/` 替代 `logos/resources/`；源码在 `src/`、`tests/`
- **测试 ID**：`UT-` / `ST-` / `GT-`（GUI 测试）；运行器为 Qt Test + `ctest`
- **UI 原型**：Qt Designer `.ui` 或 QML，非 HTML
- **索引文件**：`qt-project.yaml`（非 `logos-project.yaml`）

## 推荐使用顺序

```
qt-project-init → qt-prd-writer → qt-product-designer → qt-architecture-designer
→ qt-scenario-architect → qt-api-designer → qt-db-designer → qt-test-writer
→ qt-code-implementor → qt-code-reviewer → qt-acceptance-verifier
```

迭代变更：`qt-change-writer` → `qt-merge-executor` → 重新走受影响 Phase。
