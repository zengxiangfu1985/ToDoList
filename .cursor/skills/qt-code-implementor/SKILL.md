---
name: qt-code-implementor
description: >-
  Generates C++/Qt business code, .ui/.qml, CMake/qmake build files, and QTest code
  aligned with header contracts, DDL, and test-case IDs. Use for Phase 07 code
  generation or implementing scenario S01 in Qt.
---

# Skill: Qt Code Implementor

> Phase 07 · 代码生成：读取完整规格链，生成 C++/Qt 业务代码、测试代码与构建配置，按场景分批闭环交付。

## 触发条件

- 用户要求实现 Qt 代码、生成 S01 或进入 Phase 07
- `docs/test/` 测试用例文档已存在
- 用户说「按规格实现」「帮我写 CameraManager」

## 前置依赖

| 文档 | 路径 | 必需 |
|------|------|------|
| 需求 | `requirements.md` | ✅ |
| 原型 | `prototypes/*.ui\|qml` | ✅ |
| 时序图 | `2-scenario-implementation/` | ✅ |
| 头文件契约 | `docs/api/*.h` | ✅ |
| 数据 | `docs/database/` | 有则必读 |
| 测试规格 | `docs/test/` | ✅ |
| 项目配置 | `qt-project.yaml` | ✅ |

## 核心能力

1. 加载全链路规格，建立实现基准
2. 按场景分批：业务 + 测试 + 构建同批交付
3. 实现与头文件契约、DDL 严格一致
4. 生成 QTest 代码，用例 ID 与文档一致
5. 配置 CMake/qmake 与 `ctest`

## 执行步骤

### Step 1: 加载规格上下文

读取上表全部文档。确认：

- 本批场景（S01…）
- 涉及类、DB 表
- UT/ST/GT 用例 ID 清单
- Widgets/QML、CMake/qmake

### Step 2: 规划分批

**大任务按场景拆分，每批必须闭环。**

批前声明：

```markdown
## 本批范围
- 场景：S01
- 类：CameraManager, MainWindow 连接逻辑
- 表：devices
- 用例：UT-S01-01~08, ST-S01-01~03, GT-S01-01
```

### Step 3: 生成业务代码

**目录**（与规格链一致）：

```
src/core/     # CameraManager.cpp 等
src/ui/       # MainWindow, .ui
src/models/   # ItemModel
tests/unit/
tests/gui/
tests/integration/
```

**Qt 代码规范**：

- 内存：QObject 父子树、QScopedPointer、QSharedPointer
- 线程：耗时操作在 QThread/QtConcurrent，主线程不阻塞
- 信号槽：跨线程 `Qt::QueuedConnection`
- UI 字符串：`tr()` 包裹
- 资源：图标/QSS 入 `.qrc`
- 头文件：与 `docs/api/*.h` 一致；PIMPL 在 `.cpp` 实现

**与契约一致性**：

- 每个 public 方法/信号有 Scenario 注释
- EX 分支有对应错误处理
- DB 列名与 schema.sql 一致

### Step 4: 生成测试代码

```cpp
void TestCameraManager::ut_S01_01_validDeviceConnect()
{
    QSignalSpy spy(&mgr, &CameraManager::cameraStatusChanged);
    QVERIFY(mgr.connectCamera(QStringLiteral("CAM_001")));
    QCOMPARE(spy.count(), 1);
    // 测试函数名或 meta 数据标注 UT-S01-01
}
```

- 每个 UT/ST/GT ID 对应独立测试函数
- GUI 测试：`QTest::mouseClick`、`QTest::qWait`
- 输出：`./test_app -xml -o test-results.xml` 或 CTest

**Reporter**：在测试 main 或 CMake 中配置 XML/JSONL 输出到 `docs/verify/test-results.xml`（或项目约定路径）。

### Step 5: 构建配置

**CMake 示例要点**：

```cmake
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Test Sql)
enable_testing()
add_executable(test_camera_manager tests/unit/test_camera_manager.cpp)
add_test(NAME UT-S01-01 COMMAND test_camera_manager)
```

或更新 `.pro`：`QT += testlib`、`CONFIG += testcase`。

### Step 6: 批后自检

- [ ] 头文件 public API 与 `docs/api/` 一致
- [ ] 批前声明的所有 UT/ST/GT 在 tests/ 中存在
- [ ] `cmake --build` / `qmake && make` 通过
- [ ] `ctest --output-on-failure` 或 `./test_app` 可运行
- [ ] 无硬编码密钥；UI objectName 与原型一致

### Step 7: 引导下一步

- 本批完成 → qt-code-reviewer 或 qt-acceptance-verifier
- 更新 `qt-project.yaml` → `resource_index`（implementation-manifest 可选）

## 推荐提示词

> 请按 Phase 07 执行 S01 实现。每批同时交付业务代码、UT/ST/GT 测试与 CMakeLists.txt。输出前先列出本批覆盖的用例 ID。

## 输出规范

| 产物 | 路径 |
|------|------|
| 业务代码 | `src/` |
| 测试 | `tests/` |
| 构建 | `CMakeLists.txt` 或 `*.pro` |
| 测试结果 | `docs/verify/test-results.xml` |
