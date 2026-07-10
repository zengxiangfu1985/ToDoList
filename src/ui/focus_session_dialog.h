#ifndef FOCUS_SESSION_DIALOG_H
#define FOCUS_SESSION_DIALOG_H

#include "../core/focus/focus_session_service.h"

#include <QDialog>

namespace Ui {
class FocusSessionDialog;
}

class FocusSessionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FocusSessionDialog(QWidget *parent = nullptr);
    ~FocusSessionDialog() override;

    void bindSession(FocusSessionService *service);
    void refreshUi();

signals:
    void pauseRequested();
    void resumeRequested();
    void extendRequested();
    void abandonRequested();
    void completeRequested();
    void anotherRoundRequested();
    void skipRequested();

protected:
    void showEvent(QShowEvent *event) override;

private:
    void showRunningPanel(bool running);
    void updateCountdownLabel(int remainingSec);

    Ui::FocusSessionDialog *ui;
    FocusSessionService *m_service = nullptr;
};

#endif // FOCUS_SESSION_DIALOG_H
