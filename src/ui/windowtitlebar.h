#ifndef WINDOWTITLEBAR_H
#define WINDOWTITLEBAR_H

#include <QIcon>
#include <QWidget>

class QLabel;
class QPushButton;

class WindowTitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit WindowTitleBar(QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setTitleBarIcon(const QIcon &icon);
    void updateMaximizeButton(bool maximized);

signals:
    void minimizeRequested();
    void maximizeRequested();
    void closeRequested();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    QLabel *m_iconLabel = nullptr;
    QLabel *m_titleLabel;
    QPushButton *m_minBtn;
    QPushButton *m_maxBtn;
    QPushButton *m_closeBtn;
    bool m_dragging = false;
    QPoint m_dragOffset;
};

#endif // WINDOWTITLEBAR_H
