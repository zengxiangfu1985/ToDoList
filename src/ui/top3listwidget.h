#ifndef TOP3LISTWIDGET_H
#define TOP3LISTWIDGET_H

#include "../core/task_types.h"

#include <QHash>
#include <QListWidget>
#include <QVector>

class Top3ListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit Top3ListWidget(QWidget *parent = nullptr);

    void setRecommendations(const QVector<PriorityRecommendation> &recs,
                            const QHash<qint64, bool> &completedById = {});
    void syncFromTasks(const QVector<TaskItem> &tasks);
    void removeTask(qint64 taskId);

    void setFocusedTaskId(qint64 taskId);
    qint64 focusedTaskId() const { return m_focusedTaskId; }

    qint64 taskIdAtRow(int row) const;

signals:
    void taskCompletedToggled(qint64 taskId, bool completed);
    void focusRequested(qint64 taskId);

private:
    int preferredRowHeight() const;
    void applyRowHeights();
    void toggleItemCompleted(QListWidgetItem *item);
    void showItemContextMenu(const QPoint &pos);

    void mousePressEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

    qint64 m_focusedTaskId = 0;
};

#endif // TOP3LISTWIDGET_H
