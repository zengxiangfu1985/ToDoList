# ToDoList 匿名使用统计 — Cloudflare Workers

接收 ToDoList 客户端 POST 的匿名事件（启动、心跳、退出、升级成功）。

## 1. 安装 Wrangler（推荐本地安装，不必 `-g`）

全局 `npm install -g wrangler` 在国内往往很慢，且容易卡住。**建议在项目目录本地安装**：

```bash
cd scripts/telemetry-worker

# 可选：临时使用国内镜像（仅当前终端有效）
npm config set registry https://registry.npmmirror.com

npm install
npm run login
```

之后所有命令用 `npx wrangler ...` 或 `npm run deploy`，无需全局安装。

若仍想全局安装，可改用 `pnpm add -g wrangler` 或 `corepack enable && pnpm add -g wrangler`，通常比 npm 快。

## 2. 创建 KV 与 D1

```bash
cd scripts/telemetry-worker

npx wrangler kv namespace create TELEMETRY_KV
npx wrangler kv namespace create TELEMETRY_KV --preview

npx wrangler d1 create todolist-telemetry
npx wrangler d1 execute todolist-telemetry --remote --file=./schema.sql
npx wrangler d1 execute todolist-telemetry --local --file=./schema.sql
```

将输出的 `id` / `database_id` 填入 `wrangler.toml`。

## 3. 设置管理 Token

```bash
npx wrangler secret put ADMIN_TOKEN
```

## 4. 部署

```bash
npm run deploy
# 或: npx wrangler deploy
```

部署成功后得到地址，例如：

`https://todolist-telemetry.your-name.workers.dev`

## 5. 配置 ToDoList 客户端

编辑仓库根目录 `dist/usage-endpoint.json`（或便携版目录下的 `usage-endpoint.json`）：

```json
{
  "enabled": true,
  "endpoint": "https://todolist-telemetry.your-name.workers.dev/v1/report",
  "heartbeat_hours": 24
}
```

用户可在 **设置 → 隐私** 中关闭「发送匿名使用统计」（默认开启）。

## 6. 测试

```bash
curl -X POST "https://todolist-telemetry.your-name.workers.dev/v1/report" \
  -H "Content-Type: application/json" \
  -d "{\"event\":\"app_start\",\"install_id\":\"test-001\",\"version\":\"1.0.7\",\"build\":2026070722,\"os\":\"Windows 11\",\"locale\":\"zh_CN\"}"
```

应返回：`{"ok":true}`

## 7. 查看统计

```text
https://todolist-telemetry.your-name.workers.dev/v1/stats?token=你的ADMIN_TOKEN
```

返回示例：

```json
{
  "installs": 12,
  "active_7d": 8,
  "versions": [{ "version": "1.0.7", "users": 5 }]
}
```

## 上报字段说明

| 字段 | 说明 |
|------|------|
| install_id | 匿名 UUID，不含个人信息 |
| event | app_start / heartbeat / app_exit / upgrade_success |
| version / build | 程序版本 |
| os / locale | 系统与语言 |
| from_version | 仅 upgrade_success 时有值 |

不上传任务内容、API Key 或数据库。
