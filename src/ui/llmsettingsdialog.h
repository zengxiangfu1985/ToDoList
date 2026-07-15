#ifndef LLMSETTINGSDIALOG_H
#define LLMSETTINGSDIALOG_H

#include "../core/task_types.h"

#include <QDialog>
#include <QHash>

class LlmService;

namespace Ui {
class LlmSettingsDialog;
}

class LlmSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LlmSettingsDialog(LlmService *service, QWidget *parent = nullptr);
    ~LlmSettingsDialog() override;

    LlmConfig config() const;

private slots:
    void onProviderChanged(int index);
    void onSavedModelChanged(int index);
    void onTestConnection();
    void onSave();

private:
    static QString profileCacheKey(LlmProviderType type, const QString &model);
    LlmProviderType currentProviderType() const;
    void loadConfigToUi(const LlmConfig &config);
    LlmConfig configFromUi() const;
    void stashCurrentProfileToCache(LlmProviderType provider);
    void refreshSavedModelsCombo(LlmProviderType type, const QString &selectModel = QString());
    void loadProviderProfile(LlmProviderType type, const QString &model = QString());

    Ui::LlmSettingsDialog *ui;
    LlmService *m_service;
    LlmConfig m_config;
    QHash<QString, LlmConfig> m_profileCache;
    LlmProviderType m_uiProviderType = LlmProviderType::Ollama;
    bool m_loadingUi = false;
};

#endif // LLMSETTINGSDIALOG_H
