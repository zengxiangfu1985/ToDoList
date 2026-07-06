#ifndef LOCK_SCREEN_WIDGET_H
#define LOCK_SCREEN_WIDGET_H

#include <QWidget>

class QLineEdit;
class QLabel;

class LockScreenWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LockScreenWidget(QWidget *parent = nullptr);

    void refreshHint();
    void setLoginMode(bool startupLogin);

signals:
    void unlockRequested(const QString &password);
    void unlockSucceeded();
    void exitRequested();

public slots:
    void showUnlockError(const QString &message);
    void onUnlockAccepted();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    void attemptUnlock();
    void repositionPanel();

    QWidget *m_panel = nullptr;
    QLabel *m_titleLabel = nullptr;
    QLabel *m_hintLabel = nullptr;
    QLabel *m_errorLabel = nullptr;
    QLineEdit *m_passwordEdit = nullptr;
    bool m_startupLogin = false;
};

#endif // LOCK_SCREEN_WIDGET_H
