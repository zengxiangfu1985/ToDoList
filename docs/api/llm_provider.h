#ifndef LLM_PROVIDER_H
#define LLM_PROVIDER_H

#include "task_types.h"

#include <QObject>
#include <memory>

// Scenario: S03 — 可插拔 LLM 接口
class LlmProvider : public QObject
{
    Q_OBJECT
public:
    explicit LlmProvider(QObject *parent = nullptr);
    ~LlmProvider() override;

    virtual LlmProviderType providerType() const = 0;
    virtual QString displayName() const = 0;
    virtual bool isConfigured() const;
    virtual LlmResponse complete(const LlmRequest &request) = 0;
    virtual LlmResponse testConnection();

    LlmConfig config() const;
    void setConfig(const LlmConfig &config);

protected:
    LlmConfig m_config;
};

using LlmProviderPtr = std::unique_ptr<LlmProvider>;

// Scenario: S03 Step 2
class LlmProviderFactory
{
public:
    static LlmProviderPtr create(const LlmConfig &config, QObject *parent = nullptr);
    static LlmConfig defaultConfig(LlmProviderType type);
    static LlmConfig loadFromSettings();
    static void saveToSettings(const LlmConfig &config);
};

#endif // LLM_PROVIDER_H
