#ifndef OPENAI_COMPATIBLE_PROVIDER_H
#define OPENAI_COMPATIBLE_PROVIDER_H

#include "llm_provider.h"

// DeepSeek / Kimi / Custom — OpenAI 兼容 chat/completions
class OpenAiCompatibleProvider : public LlmProvider
{
    Q_OBJECT
public:
    explicit OpenAiCompatibleProvider(QObject *parent = nullptr);

    LlmProviderType providerType() const override;
    QString displayName() const override;
    bool isConfigured() const override;
    LlmResponse complete(const LlmRequest &request) override;
};

#endif // OPENAI_COMPATIBLE_PROVIDER_H
