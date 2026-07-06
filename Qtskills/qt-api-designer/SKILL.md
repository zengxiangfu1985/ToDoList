---
name: qt-api-designer
description: >-
  Designs C++ header file contracts (QObject classes, signals, slots, Q_PROPERTY)
  traced to scenario sequence diagram steps. Use for Phase 04 interface design,
  docs/api headers, or Qt public API specification—not OpenAPI/REST.
---

# Skill: Qt API Designer

> Phase 04 · 接口设计：从场景时序图生成 **C++ 头文件契约**，每个公共方法/信号追溯到 Scenario Step，避免 AI 临场发明接口。

## 触发条件

- 用户要求设计 Qt 类接口、头文件契约或 API 规格
- 用户提到「Phase 04」「接口设计」「docs/api」
- 场景时序图已完成，需定义 C++ 公共 API

## 前置依赖

- `docs/prd/3-technical-plan/2-scenario-implementation/` 含场景时序图
- `architecture-overview.md` 含模块划分

若时序图为空，提示先完成 qt-scenario-architect。

## 核心能力

1. 从时序图提取跨对象边界的调用与信号
2. 设计 QObject 类：公共方法、signals、slots、Q_PROPERTY
3. 定义参数结构体、枚举、错误码
4. 标注线程安全约定与主线程限制
5. 产出 `api-spec.md` 说明文档

## 执行步骤

### Step 1: 读取场景上下文

遍历时序图，收集：

- 每个 Step 的调用方/被调用方
- 信号参数与方向
- EX 异常对应的错误返回方式

### Step 2: 提取接口清单

输出表格供用户确认：

| # | 类名 | 方法/信号 | 来源场景 | Step |
|---|------|----------|---------|------|
| 1 | CameraManager | connectCamera | S01 | Step 2 |
| 2 | CameraManager | cameraStatusChanged | S01 | Step 4 |

**原则**：时序图中没有的跨边界调用，不应新增公共 API。

### Step 3: 设计统一约定

**错误处理**：

```cpp
enum class CameraError {
    None,
    DeviceNotFound,
    AlreadyConnected,
    Timeout
};
// 同步方法：bool + QString *errorMsg 或 std::optional + Error
```

**线程约定**（在 api-spec.md 中列表）：

- `CameraManager` 主线程创建；`connectCamera()` 主线程调用
- `frameReady` 信号可能在 Worker 线程发射，槽需 `Qt::QueuedConnection`

**命名**：

- 类名 PascalCase，文件与类名一致
- 信号：过去式或状态名（`cameraStatusChanged`）
- 槽：`on_对象_信号` 或动词（`connectCamera`）

### Step 4: 生成头文件

每个核心类一个 `.h`，注释标注场景步骤：

```cpp
// Scenario: S01 Step 2
class CameraManager : public QObject {
    Q_OBJECT
public:
    explicit CameraManager(QObject *parent = nullptr);

    bool connectCamera(const QString &deviceId, QString *errorMsg = nullptr);
    void disconnectAll();
    QList<CameraInfo> connectedCameras() const;

signals:
    // Scenario: S01 Step 4
    void cameraStatusChanged(const QString &deviceId, CameraStatus status);
    // Scenario: S02 Step 4
    void frameReady(const QString &deviceId, const QImage &frame);

private:
    QScopedPointer<CameraManagerPrivate> d;  // PIMPL
};
```

**Qt 注意事项**：

- 优先 Qt 类型（QString、QList、QImage）以保证信号槽元类型
- 标注 `Q_INVOKABLE`（QML 可调用时）
- MOC 限制：含 Q_OBJECT 的类注意模板与多重继承

### Step 5: 追溯完整性检查

- 正向：每个时序 Step 的跨对象调用在头文件中有对应 API
- 反向：每个 public 方法/信号有 Scenario 注释
- 异常：每个 EX 有错误码或失败信号覆盖

## 输出规范

| 产物 | 路径 |
|------|------|
| 头文件契约 | `docs/api/*.h` |
| 接口说明 | `docs/api/api-spec.md` |

头文件为**设计契约**；Phase 07 在 `src/` 实现对应 `.cpp`。

## 收尾步骤（强制）

更新 `qt-project.yaml` → `resource_index`（每个 .h + api-spec.md）。

## 推荐提示词

- `基于 S01 时序图设计 CameraManager 头文件`
- `帮我生成 docs/api 下的 C++ 接口契约`
