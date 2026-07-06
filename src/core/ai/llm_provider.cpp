#include "llm_provider.h"
#include "ollama_llm_provider.h"
#include "openai_compatible_provider.h"
#include "../../utils/portable_settings.h"

#include <QSettings>

namespace {

QSettings appSettings()
{
    return PortableSettings::open();
}

QString profilePrefix(LlmProviderType type, const QString &model)
{
    return QStringLiteral("ai/profiles/%1/%2/")
        .arg(static_cast<int>(type))
        .arg(LlmProviderFactory::sanitizeModelKey(model));
}

QString providerModelsKey(LlmProviderType type)
{
    return QStringLiteral("ai/providers/%1/models").arg(static_cast<int>(type));
}

QString providerLastModelKey(LlmProviderType type)
{
    return QStringLiteral("ai/providers/%1/lastModel").arg(static_cast<int>(type));
}

void migrateLegacySettings(QSettings &s)
{
    if (s.contains(QStringLiteral("ai/profilesMigrated")))
        return;

    const bool hasLegacy = s.contains(QStringLiteral("ai/baseUrl")) || s.contains(QStringLiteral("ai/model"));
    if (!hasLegacy) {
        s.setValue(QStringLiteral("ai/profilesMigrated"), true);
        return;
    }

    LlmConfig legacy;
    legacy.provider = static_cast<LlmProviderType>(s.value(QStringLiteral("ai/provider"), 0).toInt());
    legacy = LlmProviderFactory::defaultConfig(legacy.provider);
    legacy.baseUrl = s.value(QStringLiteral("ai/baseUrl"), legacy.baseUrl).toString();
    legacy.apiKey = s.value(QStringLiteral("ai/apiKey")).toString();
    legacy.model = s.value(QStringLiteral("ai/model"), legacy.model).toString();
    legacy.timeoutMs = s.value(QStringLiteral("ai/timeoutMs"), 30000).toInt();

    if (!legacy.model.trimmed().isEmpty())
        LlmProviderFactory::saveProfile(legacy);

    s.setValue(QStringLiteral("ai/activeProvider"), static_cast<int>(legacy.provider));
    s.setValue(QStringLiteral("ai/profilesMigrated"), true);
}

} // namespace

LlmProvider::LlmProvider(QObject *parent)
    : QObject(parent)
{
}

LlmProvider::~LlmProvider() = default;

bool LlmProvider::isConfigured() const
{
    return !m_config.model.trimmed().isEmpty() && !m_config.baseUrl.trimmed().isEmpty();
}

LlmResponse LlmProvider::testConnection()
{
    LlmRequest req;
    req.systemPrompt = QStringLiteral("You are a helpful assistant.");
    req.userPrompt = QStringLiteral("Reply with OK only.");
    req.maxTokens = 64;
    return complete(req);
}

LlmConfig LlmProvider::config() const
{
    return m_config;
}

void LlmProvider::setConfig(const LlmConfig &config)
{
    m_config = config;
}

LlmProviderPtr LlmProviderFactory::create(const LlmConfig &config, QObject *parent)
{
    LlmProviderPtr provider;
    switch (config.provider) {
    case LlmProviderType::Ollama:
        provider.reset(new OllamaLlmProvider(parent));
        break;
    case LlmProviderType::DeepSeek:
    case LlmProviderType::Kimi:
    case LlmProviderType::CustomOpenAI:
        provider.reset(new OpenAiCompatibleProvider(parent));
        break;
    }
    if (provider)
        provider->setConfig(config);
    return provider;
}

LlmConfig LlmProviderFactory::defaultConfig(LlmProviderType type)
{
    LlmConfig cfg;
    cfg.provider = type;
    switch (type) {
    case LlmProviderType::Ollama:
        cfg.baseUrl = QStringLiteral("http://127.0.0.1:11434");
        cfg.model = QStringLiteral("qwen2.5:3b");
        cfg.timeoutMs = 180000;
        break;
    case LlmProviderType::DeepSeek:
        cfg.baseUrl = QStringLiteral("https://api.deepseek.com/v1");
        cfg.model = QStringLiteral("deepseek-chat");
        break;
    case LlmProviderType::Kimi:
        cfg.baseUrl = QStringLiteral("https://api.moonshot.cn/v1");
        cfg.model = QStringLiteral("moonshot-v1-8k");
        break;
    case LlmProviderType::CustomOpenAI:
        cfg.baseUrl = QStringLiteral("https://api.openai.com/v1");
        cfg.model = QStringLiteral("gpt-4o-mini");
        break;
    }
    return cfg;
}

QString LlmProviderFactory::sanitizeModelKey(const QString &model)
{
    QString key = model.trimmed();
    key.replace(QLatin1Char(':'), QLatin1Char('_'));
    key.replace(QLatin1Char('/'), QLatin1Char('_'));
    key.replace(QLatin1Char('\\'), QLatin1Char('_'));
    return key;
}

LlmConfig LlmProviderFactory::loadProfile(LlmProviderType type, const QString &model)
{
    QSettings s = appSettings();
    migrateLegacySettings(s);

    QString modelName = model.trimmed();
    if (modelName.isEmpty())
        modelName = s.value(providerLastModelKey(type)).toString().trimmed();

    LlmConfig cfg = defaultConfig(type);
    if (modelName.isEmpty())
        return cfg;

    const QString prefix = profilePrefix(type, modelName);
    if (!s.contains(prefix + QStringLiteral("model")))
        return cfg;

    cfg.provider = type;
    cfg.model = s.value(prefix + QStringLiteral("model"), modelName).toString();
    cfg.baseUrl = s.value(prefix + QStringLiteral("baseUrl"), cfg.baseUrl).toString();
    cfg.apiKey = s.value(prefix + QStringLiteral("apiKey")).toString();
    cfg.timeoutMs = s.value(prefix + QStringLiteral("timeoutMs"), cfg.timeoutMs).toInt();
    if (type == LlmProviderType::Ollama && cfg.timeoutMs < 180000)
        cfg.timeoutMs = 180000;
    return cfg;
}

void LlmProviderFactory::saveProfile(const LlmConfig &config)
{
    const QString modelName = config.model.trimmed();
    if (modelName.isEmpty())
        return;

    LlmConfig toSave = config;
    if (toSave.provider == LlmProviderType::Ollama && toSave.timeoutMs < 180000)
        toSave.timeoutMs = 180000;

    QSettings s = appSettings();
    const QString prefix = profilePrefix(toSave.provider, modelName);
    s.setValue(prefix + QStringLiteral("model"), modelName);
    s.setValue(prefix + QStringLiteral("baseUrl"), toSave.baseUrl.trimmed());
    s.setValue(prefix + QStringLiteral("apiKey"), toSave.apiKey);
    s.setValue(prefix + QStringLiteral("timeoutMs"), toSave.timeoutMs);

    const QString modelsKey = providerModelsKey(toSave.provider);
    QStringList models = s.value(modelsKey).toStringList();
    if (!models.contains(modelName)) {
        models.append(modelName);
        s.setValue(modelsKey, models);
    }
    s.setValue(providerLastModelKey(toSave.provider), modelName);
}

QStringList LlmProviderFactory::savedModels(LlmProviderType type)
{
    QSettings s = appSettings();
    migrateLegacySettings(s);
    return s.value(providerModelsKey(type)).toStringList();
}

LlmConfig LlmProviderFactory::loadFromSettings()
{
    QSettings s = appSettings();
    migrateLegacySettings(s);

    const auto type = static_cast<LlmProviderType>(
        s.value(QStringLiteral("ai/activeProvider"),
                s.value(QStringLiteral("ai/provider"), 0)).toInt());
    LlmConfig cfg = loadProfile(type);
    if (cfg.provider == LlmProviderType::Ollama && cfg.timeoutMs < 180000)
        cfg.timeoutMs = 180000;
    return cfg;
}

void LlmProviderFactory::saveToSettings(const LlmConfig &config)
{
    saveProfile(config);

    QSettings s = appSettings();
    s.setValue(QStringLiteral("ai/activeProvider"), static_cast<int>(config.provider));

    // 兼容旧版单配置键
    s.setValue(QStringLiteral("ai/provider"), static_cast<int>(config.provider));
    s.setValue(QStringLiteral("ai/baseUrl"), config.baseUrl);
    s.setValue(QStringLiteral("ai/apiKey"), config.apiKey);
    s.setValue(QStringLiteral("ai/model"), config.model);
    s.setValue(QStringLiteral("ai/timeoutMs"), config.timeoutMs);
}
