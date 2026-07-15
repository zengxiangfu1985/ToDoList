#ifndef LLM_PROVIDER_H
#define LLM_PROVIDER_H

#include "../task_types.h"

#include <QObject>
#include <QStringList>
#include <memory>

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

class LlmProviderFactory
{
public:
    static LlmProviderPtr create(const LlmConfig &config, QObject *parent = nullptr);
    static LlmConfig defaultConfig(LlmProviderType type);
    static QStringList suggestedModels(LlmProviderType type);
    static QString sanitizeModelKey(const QString &model);
    static LlmConfig loadProfile(LlmProviderType type, const QString &model = QString());
    static void saveProfile(const LlmConfig &config);
    static QStringList savedModels(LlmProviderType type);
    static LlmConfig loadFromSettings();
    static void saveToSettings(const LlmConfig &config);
};

#endif // LLM_PROVIDER_H
