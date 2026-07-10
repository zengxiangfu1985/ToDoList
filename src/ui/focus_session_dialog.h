#ifndef FOCUS_SESSION_DIALOG_H
#define FOCUS_SESSION_DIALOG_H

#include "../core/focus/focus_session_service.h"

#include <QDialog>

class QEvent;
class QTimer;

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
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void showRunningPanel(bool running);
    void updateCountdownLabel(int remainingSec);
    void updateCompactLabel(const QString &taskTitle);
    void setExpanded(bool expanded);
    void positionBottomRight();
    void scheduleCollapse();
    void cancelCollapse();
    bool shouldStayExpanded() const;

    Ui::FocusSessionDialog *ui;
    FocusSessionService *m_service = nullptr;
    QTimer *m_collapseTimer = nullptr;
    bool m_expanded = false;
    bool m_hovered = false;
    bool m_pinned = false;
    bool m_forceExpanded = false;
    QString m_taskTitle;
};

#endif // FOCUS_SESSION_DIALOG_H
