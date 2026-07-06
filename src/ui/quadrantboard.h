#ifndef QUADRANTBOARD_H
#define QUADRANTBOARD_H

#include "../core/task_types.h"

#include <QGroupBox>
#include <QWidget>

class QuadrantListWidget;

class QuadrantBoard : public QWidget
{
    Q_OBJECT
public:
    explicit QuadrantBoard(QWidget *parent = nullptr);

    void bindQuadrantContainers(QGroupBox *q1, QGroupBox *q2, QGroupBox *q3, QGroupBox *q4);
    void setTasks(const QVector<TaskItem> &tasks);

signals:
    void taskQuadrantChanged(qint64 taskId, EisenhowerQuadrant from, EisenhowerQuadrant to);
    void taskCompletedToggled(qint64 taskId, bool completed);

private:
    QuadrantListWidget *m_q1;
    QuadrantListWidget *m_q2;
    QuadrantListWidget *m_q3;
    QuadrantListWidget *m_q4;
};

#endif // QUADRANTBOARD_H
