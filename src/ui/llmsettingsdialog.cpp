#include "llmsettingsdialog.h"
#include "ui_llmsettingsdialog.h"

#include "../core/ai/llm_provider.h"
#include "../core/ai/llm_service.h"
#include "../utils/app_theme.h"

#include <QFormLayout>
#include <QMessageBox>
#include <QSet>

LlmSettingsDialog::LlmSettingsDialog(LlmService *service, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LlmSettingsDialog)
    , m_service(service)
    , m_config(service ? service->config() : LlmProviderFactory::loadFromSettings())
{
    ui->setupUi(this);
    AppTheme::styleDialog(this);
    setWindowTitle(tr("LLM 设置"));

    const ThemeMetrics &m = AppTheme::metrics();
    setMinimumWidth(qMax(520, m.fontBase * 34));
    ui->formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    ui->formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->formLayout->setHorizontalSpacing(12);
    ui->formLayout->setVerticalSpacing(10);

    ui->comboProvider->addItem(tr("Ollama (本地 Qwen)"), static_cast<int>(LlmProviderType::Ollama));
    ui->comboProvider->addItem(tr("DeepSeek"), static_cast<int>(LlmProviderType::DeepSeek));
    ui->comboProvider->addItem(tr("Kimi (Moonshot)"), static_cast<int>(LlmProviderType::Kimi));
    ui->comboProvider->addItem(tr("Custom OpenAI"), static_cast<int>(LlmProviderType::CustomOpenAI));

    m_loadingUi = true;
    loadProviderProfile(m_config.provider, m_config.model);
    m_loadingUi = false;

    connect(ui->comboProvider, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LlmSettingsDialog::onProviderChanged);
    connect(ui->comboSavedModels, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LlmSettingsDialog::onSavedModelChanged);
    connect(ui->btnTestConnection, &QPushButton::clicked, this, &LlmSettingsDialog::onTestConnection);
    connect(ui->btnSave, &QPushButton::clicked, this, &LlmSettingsDialog::onSave);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

LlmSettingsDialog::~LlmSettingsDialog()
{
    delete ui;
}

QString LlmSettingsDialog::profileCacheKey(LlmProviderType type, const QString &model)
{
    return QStringLiteral("%1|%2").arg(static_cast<int>(type)).arg(model.trimmed());
}

LlmProviderType LlmSettingsDialog::currentProviderType() const
{
    return static_cast<LlmProviderType>(ui->comboProvider->currentData().toInt());
}

LlmConfig LlmSettingsDialog::config() const
{
    return m_config;
}

void LlmSettingsDialog::loadConfigToUi(const LlmConfig &config)
{
    m_config = config;
    for (int i = 0; i < ui->comboProvider->count(); ++i) {
        if (ui->comboProvider->itemData(i).toInt() == static_cast<int>(config.provider)) {
            ui->comboProvider->setCurrentIndex(i);
            break;
        }
    }
    ui->editBaseUrl->setText(config.baseUrl);
    ui->editApiKey->setText(config.apiKey);
    ui->editModel->setText(config.model);
}

LlmConfig LlmSettingsDialog::configFromUi() const
{
    LlmConfig cfg;
    cfg.provider = currentProviderType();
    cfg.baseUrl = ui->editBaseUrl->text().trimmed();
    cfg.apiKey = ui->editApiKey->text();
    cfg.model = ui->editModel->text().trimmed();
    cfg.timeoutMs = m_config.timeoutMs;
    if (cfg.provider == LlmProviderType::Ollama) {
        if (cfg.timeoutMs < 180000)
            cfg.timeoutMs = 180000;
    } else if (cfg.timeoutMs <= 0) {
        cfg.timeoutMs = 30000;
    }
    return cfg;
}

void LlmSettingsDialog::stashCurrentProfileToCache()
{
    const LlmConfig cfg = configFromUi();
    if (cfg.model.trimmed().isEmpty())
        return;
    m_profileCache.insert(profileCacheKey(cfg.provider, cfg.model), cfg);
}

void LlmSettingsDialog::refreshSavedModelsCombo(LlmProviderType type, const QString &selectModel)
{
    QSet<QString> models;
    for (const QString &model : LlmProviderFactory::savedModels(type))
        models.insert(model);

    for (auto it = m_profileCache.constBegin(); it != m_profileCache.constEnd(); ++it) {
        const LlmConfig &cfg = it.value();
        if (cfg.provider == type && !cfg.model.trimmed().isEmpty())
            models.insert(cfg.model.trimmed());
    }

    QStringList sorted = models.values();
    sorted.sort();

    ui->comboSavedModels->blockSignals(true);
    ui->comboSavedModels->clear();
    for (const QString &model : sorted)
        ui->comboSavedModels->addItem(model, model);

    const QString target = selectModel.trimmed();
    int index = target.isEmpty() ? -1 : ui->comboSavedModels->findData(target);
    if (index < 0 && !sorted.isEmpty())
        index = ui->comboSavedModels->findData(sorted.first());
    ui->comboSavedModels->setCurrentIndex(index);
    ui->comboSavedModels->setEnabled(!sorted.isEmpty());
    ui->comboSavedModels->blockSignals(false);
}

void LlmSettingsDialog::loadProviderProfile(LlmProviderType type, const QString &model)
{
    LlmConfig cfg;
    const QString modelName = model.trimmed();

    if (!modelName.isEmpty()) {
        const QString key = profileCacheKey(type, modelName);
        cfg = m_profileCache.contains(key)
            ? m_profileCache.value(key)
            : LlmProviderFactory::loadProfile(type, modelName);
    } else {
        cfg = LlmProviderFactory::loadProfile(type);
        const QString key = profileCacheKey(type, cfg.model);
        if (m_profileCache.contains(key))
            cfg = m_profileCache.value(key);
    }

    loadConfigToUi(cfg);
    refreshSavedModelsCombo(type, cfg.model);
}

void LlmSettingsDialog::onProviderChanged(int index)
{
    Q_UNUSED(index);
    if (m_loadingUi)
        return;

    stashCurrentProfileToCache();

    m_loadingUi = true;
    loadProviderProfile(currentProviderType());
    m_loadingUi = false;
}

void LlmSettingsDialog::onSavedModelChanged(int index)
{
    if (m_loadingUi || index < 0)
        return;

    stashCurrentProfileToCache();

    const QString model = ui->comboSavedModels->itemData(index).toString();
    const auto type = currentProviderType();
    const QString key = profileCacheKey(type, model);

    LlmConfig cfg = m_profileCache.contains(key)
        ? m_profileCache.value(key)
        : LlmProviderFactory::loadProfile(type, model);

    m_loadingUi = true;
    loadConfigToUi(cfg);
    m_loadingUi = false;
}

void LlmSettingsDialog::onTestConnection()
{
    const LlmConfig cfg = configFromUi();
    auto provider = LlmProviderFactory::create(cfg);
    if (!provider) {
        QMessageBox::warning(this, tr("测试"), tr("无法创建 Provider"));
        return;
    }
    if (!provider->isConfigured()) {
        QMessageBox::warning(this, tr("测试"), tr("请先填写必要配置（云端需 API Key）"));
        return;
    }
    ui->btnTestConnection->setEnabled(false);
    const LlmResponse resp = provider->testConnection();
    ui->btnTestConnection->setEnabled(true);
    if (resp.success) {
        m_profileCache.insert(profileCacheKey(cfg.provider, cfg.model), cfg);
        refreshSavedModelsCombo(cfg.provider, cfg.model);
        QMessageBox::information(this, tr("测试"), tr("连接成功"));
    } else {
        QMessageBox::warning(this, tr("测试"), resp.errorMessage);
    }
}

void LlmSettingsDialog::onSave()
{
    stashCurrentProfileToCache();
    m_config = configFromUi();

    for (auto it = m_profileCache.constBegin(); it != m_profileCache.constEnd(); ++it)
        LlmProviderFactory::saveProfile(it.value());

    LlmProviderFactory::saveToSettings(m_config);
    if (m_service)
        m_service->setConfig(m_config);
    accept();
}
