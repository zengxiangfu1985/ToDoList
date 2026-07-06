#ifndef TASK_TABLE_MODEL_H
#define TASK_TABLE_MODEL_H

#include "../core/task_types.h"

#include <QAbstractTableModel>
#include <QSet>
#include <QVector>

class TaskTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column {
        ColSelect = 0,
        ColIndex,
        ColTitle,
        ColDue,
        ColQuadrant,
        ColScore,
        ColCompleted,
        ColCount
    };

    explicit TaskTableModel(QObject *parent = nullptr);

    void setTasks(const QVector<TaskItem> &tasks);
    TaskItem taskAt(int row) const;
    int rowForTaskId(qint64 id) const;

    void setDeleteMode(bool enabled);
    bool deleteMode() const;
    QVector<qint64> selectedTaskIdsForDelete() const;
    void clearDeleteSelection();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

signals:
    void taskCompletedToggled(qint64 taskId, bool completed);
    void deleteModeChanged(bool enabled);

private:
    static QString quadrantText(EisenhowerQuadrant q);

    QVector<TaskItem> m_tasks;
    bool m_deleteMode = false;
    QSet<qint64> m_deleteSelected;
};

#endif // TASK_TABLE_MODEL_H
