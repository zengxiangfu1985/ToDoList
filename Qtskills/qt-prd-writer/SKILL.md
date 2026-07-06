---
name: qt-prd-writer
description: >-
  Writes scene-driven Qt product requirements with user personas, pain points,
  scenario IDs (S01/S02), and observable acceptance criteria (AC-Sxx-n).
  Use for Phase 01 requirements, PRD, or when the user mentions 需求分析 or
  requirements.md.
---

# Skill: Qt PRD Writer

> Phase 01 · 需求分析：生成场景驱动的 `requirements.md`，场景编号贯穿后续原型、时序图、头文件契约与 Qt Test 用例。

## 触发条件

- 用户要求写需求文档、PRD 或梳理 Qt 桌面/嵌入式产品需求
- 用户提到「Phase 01」「需求分析」
- 项目处于规格链第一阶段

## 核心能力

1. 梳理产品定位与具体用户画像（现场工程师、操作员等）
2. 提炼痛点因果链（P01、P02…）
3. 定义完整用户旅程场景（S01、S02…），非 CRUD 清单
4. 为每个场景编写**可观察**验收标准（AC-Sxx-n，含量化指标）
5. 识别 Qt 相关约束（离线、多线程、高 DPI、无网络等）

## 执行步骤

### Step 1: 理解产品定位

确认：一句话定位、目标用户（具体到角色与使用环境）、成功指标。

**Qt 项目追问要点**：

- 是否需要离线运行？
- 是否有实时性要求（帧率、响应时间）？
- 是否涉及硬件（相机、串口、PLC）？
- 部署形态：安装包 / 绿色版 / 嵌入式？

### Step 2: 提炼用户痛点

每条痛点：`因为 [原因] → 导致 [痛点] → 造成 [后果]`，编号 P01、P02…

### Step 3: 识别和定义场景

场景 = **完整用户旅程**，主路径至少 3 个可感知步骤。

**粒度自检**（必须通过）：

1. **单操作测试**：仅一次按钮点击或单次 API 调用 → 粒度过细
2. **CRUD 测试**：创建/查询/更新/删除拆成 4 场景 → 按业务目标合并
3. **业务价值测试**：结果不能只是「数据写入数据库」
4. **步骤数测试**：主路径 ≥ 3 步

输出场景清单表：编号 / 名称 / 触发条件 / 关联痛点 / 优先级。

### Step 4: 编写验收标准

Qt 项目使用 **AC-Sxx-n** 格式（与规格链示例一致），强调可观察、可量化：

```markdown
### S01 — 设备连接与初始化

#### 验收标准
- AC-S01-1：软件启动后 5 秒内完成相机枚举。
- AC-S01-2：双击相机列表项后，3 秒内显示实时预览，帧率不低于 25fps。
```

也可辅以 GIVEN/WHEN/THEN，但每条必须能映射到后续 UT/ST/GT。

**避免模糊词**：「快速」「友好」→ 改为毫秒数、帧率、超时秒数。

### Step 5: 约束与「不做」清单

- 技术约束：Qt 版本、Widgets/QML、平台、第三方 SDK
- 资源约束：团队、交付窗口
- 不做清单：本阶段明确排除的功能

### Step 6: 组装并写入文档

## 输出规范

- **路径**：`docs/prd/1-product-requirements/requirements.md`
- 每个 P0/P1 场景：≥1 正常 + ≥1 异常验收标准
- 场景编号全局唯一，废弃不复用

## 收尾步骤（强制）

追加 `qt-project.yaml` → `resource_index`：

```yaml
resource_index:
  - path: docs/prd/1-product-requirements/requirements.md
    desc: 产品需求真源。涉及场景、验收标准、用户画像时必读。
```

同步写入 `scenarios` 字段（供后续 Skill 使用）：

```yaml
scenarios:
  - id: S01
    name: 设备连接与初始化
```

## 推荐提示词

- `帮我写 Qt 项目需求文档`
- `我要做一个 xxx 桌面软件，帮我梳理场景和验收标准`
