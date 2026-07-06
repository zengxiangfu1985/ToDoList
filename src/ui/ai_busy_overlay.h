#ifndef AI_BUSY_OVERLAY_H
#define AI_BUSY_OVERLAY_H

#include <QWidget>

class QLabel;
class QPropertyAnimation;
class QGraphicsOpacityEffect;
class QTimer;

class SpinnerWidget : public QWidget
{
public:
    explicit SpinnerWidget(QWidget *parent = nullptr);

    void start();
    void stop();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_angle = 0;
    QTimer *m_timer = nullptr;
};

class AiBusyOverlay : public QWidget
{
public:
    explicit AiBusyOverlay(QWidget *parent = nullptr);

    void showAnimated();
    void hideAnimated();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void repositionPanel();
    void onFadeOutFinished();

    QWidget *m_panel = nullptr;
    QLabel *m_label = nullptr;
    SpinnerWidget *m_spinner = nullptr;
    QGraphicsOpacityEffect *m_opacity = nullptr;
    QPropertyAnimation *m_fadeIn = nullptr;
    QPropertyAnimation *m_fadeOut = nullptr;
    bool m_hiding = false;
};

#endif // AI_BUSY_OVERLAY_H
