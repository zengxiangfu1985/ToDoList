---
name: qt-db-designer
description: >-
  Derives Qt data models from C++ header contracts: SQLite schema.sql, QAbstractItemModel
  specs, QSettings keys, and migration DDL. Use for Phase 05 data design, SQLite,
  or QSettings schema in Qt projects.
---

# Skill: Qt DB Designer

> Phase 05 · 数据设计：从接口契约推导实体，生成 SQLite DDL、Model/View 数据模型说明、QSettings/JSON 配置 Schema。

## 触发条件

- 用户要求设计数据库、数据模型或配置 Schema
- 用户提到「Phase 05」「数据设计」「schema.sql」
- 已有 `docs/api/` 头文件契约

## 前置依赖

- `docs/api/` 含头文件契约（qt-api-designer 产出）
- `qt-project.yaml` → `tech_stack.database` 已填写

## 核心能力

1. 从头文件方法参数/返回值识别持久化实体
2. 生成 SQLite `schema.sql` 与增量 migration
3. 设计 `QAbstractTableModel` / `QAbstractItemModel` 类规格
4. 定义 QSettings 键名规范与 JSON 配置结构
5. 标注数据库线程策略

## 执行步骤

### Step 1: 确认存储方案

| 方案 | 适用 | Qt API |
|------|------|--------|
| SQLite | 结构化本地数据 | QSqlDatabase, QSqlQuery |
| QSettings | 用户偏好、窗口状态 | QSettings |
| JSON | 设备参数、标定文件 | QJsonDocument |
| QDataStream | 大二进制块 | 文件序列化 |

从 `tech_stack.database` 与架构文档确认主存储；可组合使用。

### Step 2: 提取数据实体

扫描头文件中的：

- 持久化结构体（`CameraInfo`、`MeasurementRecord`）
- CRUD 类方法（save/load/list/delete）
- 信号携带需落库的数据

输出实体清单：实体名 / 来源类 / 核心字段。

### Step 3: 设计 SQLite DDL

```sql
-- 来源：MeasurementEngine + S02 Step 4
CREATE TABLE measurements (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  device_id TEXT NOT NULL,
  captured_at TEXT NOT NULL,  -- ISO 8601
  image_path TEXT NOT NULL,
  FOREIGN KEY (device_id) REFERENCES devices(id)
);

CREATE INDEX idx_measurements_device ON measurements(device_id);
```

**SQLite + Qt 约定**：

- 时间存 ISO 8601 TEXT 或 INTEGER Unix ms
- 外键：`PRAGMA foreign_keys = ON` 在连接后执行
- UUID 主键：TEXT，应用层生成
- 增量变更：`docs/database/migration_v1_to_v2.sql`

### Step 4: 设计 Model/View 类

在 `docs/database/data-model.md` 描述：

```markdown
## DeviceListModel : QAbstractTableModel

- **列**：名称、状态、IP
- **数据来源**：CameraManager::connectedCameras()
- **变更通知**：连接 rowsInserted/dataChanged 与 Manager 信号
- **线程**：仅主线程访问模型
```

必须说明：`dataChanged()`、`rowsInserted()` 等信号何时发射。

### Step 5: QSettings / JSON Schema

```markdown
## QSettings 键规范（Organization/AppName）

| 键 | 类型 | 默认 | 说明 |
|----|------|------|------|
| ui/mainWindowGeometry | QByteArray | — | 窗口几何 |
| capture/defaultFps | int | 25 | 默认帧率 |
```

键名统一前缀，禁止散落在代码中的 magic string。

### Step 6: 数据库线程策略

在 data-model.md 明确：

- SQLite 连接名 per-thread
- 异步查询结果通过信号 `queryFinished` 回主线程
- 禁止在非所属线程使用 QSqlQuery

## 输出规范

| 产物 | 路径 |
|------|------|
| DDL | `docs/database/schema.sql` |
| 迁移 | `docs/database/migration_*.sql` |
| 模型说明 | `docs/database/data-model.md` |

## 收尾步骤（强制）

更新 `qt-project.yaml` → `resource_index`。

## 推荐提示词

- `基于头文件契约设计 SQLite 表结构`
- `帮我写 QSettings 键规范和 DeviceListModel 说明`
