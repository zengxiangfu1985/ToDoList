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

signals:
    void taskCompletedToggled(qint64 taskId, bool completed);

private:
    void toggleItemCompleted(QListWidgetItem *item);

    void mousePressEvent(QMouseEvent *event) override;
};

#endif // TOP3LISTWIDGET_H
