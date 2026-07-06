#ifndef CYBER_AI_PANEL_H
#define CYBER_AI_PANEL_H

#include <QGroupBox>

class CyberAiPanel : public QGroupBox
{
    Q_OBJECT
public:
    explicit CyberAiPanel(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // CYBER_AI_PANEL_H
