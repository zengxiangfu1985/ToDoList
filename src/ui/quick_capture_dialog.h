#ifndef QUICK_CAPTURE_DIALOG_H
#define QUICK_CAPTURE_DIALOG_H

#include "../core/task_types.h"

#include <QDialog>

class TaskRepository;
class QShowEvent;

namespace Ui {
class QuickCaptureDialog;
}

class QuickCaptureDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QuickCaptureDialog(TaskRepository *repo, QWidget *parent = nullptr);
    ~QuickCaptureDialog() override;

    void setLlmConfig(const LlmConfig &config);
    void setAutoAnalyzeDefault(bool enabled);

    int savedCount() const;
    bool autoAnalyzeRequested() const;
    bool usedLlm() const;

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onSave();

private:
    void setBusy(bool busy, const QString &status = QString());
    void showSuccessAndClose(const QString &message);

    Ui::QuickCaptureDialog *ui;
    TaskRepository *m_repo = nullptr;
    LlmConfig m_llmConfig;
    int m_savedCount = 0;
    bool m_usedLlm = false;
    bool m_autoAnalyzeRequested = true;
};

#endif // QUICK_CAPTURE_DIALOG_H
