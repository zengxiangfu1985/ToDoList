## UT-S03-01 — Factory 创建四种 Provider

**预期**：Ollama/DeepSeek/Kimi/Custom 均可 create

## UT-S03-02 — 无 API Key 时 isConfigured false

**输入**：DeepSeek config 空 apiKey  
**预期**：isConfigured() == false
