# S03 — LLM Provider 配置与切换

```mermaid
sequenceDiagram
  actor User
  participant Dialog as LlmSettingsDialog
  participant Settings as QSettings
  participant Factory as LlmProviderFactory
  participant Service as LlmService

  User->>Dialog: 选择 DeepSeek/Kimi/Ollama
  User->>Dialog: 填写 apiKey/model/baseUrl
  User->>Dialog: 点击测试连接
  Dialog->>Factory: create(config)
  Factory->>Service: test provider
  Service-->>Dialog: success/error
  User->>Dialog: 保存
  Dialog->>Settings: 写入 ai/*
  Dialog->>Service: setConfig(config)
  Service-->>MainWindow: providerChanged
```

## Provider 预设

| 类型 | 默认 Base URL | 默认 Model |
|------|---------------|------------|
| Ollama | http://127.0.0.1:11434 | qwen2.5:3b |
| DeepSeek | https://api.deepseek.com/v1 | deepseek-chat |
| Kimi | https://api.moonshot.cn/v1 | moonshot-v1-8k |
