#ifndef OLLAMA_LLM_PROVIDER_H
#define OLLAMA_LLM_PROVIDER_H

#include "llm_provider.h"

// 本地 Qwen 等模型，经 Ollama HTTP API
class OllamaLlmProvider : public LlmProvider
{
    Q_OBJECT
public:
    explicit OllamaLlmProvider(QObject *parent = nullptr);

    LlmProviderType providerType() const override;
    QString displayName() const override;
    bool isConfigured() const override;
    LlmResponse complete(const LlmRequest &request) override;
};

#endif // OLLAMA_LLM_PROVIDER_H
