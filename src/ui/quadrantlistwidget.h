#ifndef QUADRANTLISTWIDGET_H
#define QUADRANTLISTWIDGET_H

#include "../core/task_types.h"

#include <QListWidget>

class QMouseEvent;

class QuadrantListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit QuadrantListWidget(EisenhowerQuadrant quadrant, QWidget *parent = nullptr);

    EisenhowerQuadrant quadrant() const { return m_quadrant; }
    void setTaskItems(const QVector<TaskItem> &tasks);
    void removeTask(qint64 taskId);

signals:
    void taskQuadrantChanged(qint64 taskId, EisenhowerQuadrant from, EisenhowerQuadrant to);
    void taskCompletedToggled(qint64 taskId, bool completed);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void startDrag(Qt::DropActions supportedActions) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    void toggleItemCompleted(QListWidgetItem *item);

    EisenhowerQuadrant m_quadrant;
};

#endif // QUADRANTLISTWIDGET_H
