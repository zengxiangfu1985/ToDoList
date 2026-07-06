#ifndef CYBER_QUADRANT_BOX_H
#define CYBER_QUADRANT_BOX_H

#include "../core/task_types.h"

#include <QGroupBox>

class CyberQuadrantBox : public QGroupBox
{
    Q_OBJECT
public:
    explicit CyberQuadrantBox(QWidget *parent = nullptr);

    void setQuadrant(EisenhowerQuadrant quadrant);
    EisenhowerQuadrant quadrant() const { return m_quadrant; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    struct AccentColors {
        QColor title;
        QColor leftBar;
        QColor cornerTopRight;
        QColor cornerBottomLeft;
    };

    AccentColors accentColors() const;

    EisenhowerQuadrant m_quadrant = EisenhowerQuadrant::Q1_UrgentImportant;
};

#endif // CYBER_QUADRANT_BOX_H
