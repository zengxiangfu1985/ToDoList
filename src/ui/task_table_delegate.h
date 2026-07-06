#ifndef TASK_TABLE_DELEGATE_H
#define TASK_TABLE_DELEGATE_H

#include "../models/task_table_model.h"

#include <QStyledItemDelegate>

class TaskTableDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TaskTableDelegate(TaskTableModel *model, QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

private:
    static QColor quadrantGlowColor(EisenhowerQuadrant q);
    void paintQuadrantBadge(QPainter *p, const QStyleOptionViewItem &option, const QString &text,
                            EisenhowerQuadrant q) const;
    void paintTitle(QPainter *p, QStyleOptionViewItem opt, const QString &title, bool completed) const;

    TaskTableModel *m_model = nullptr;
};

#endif // TASK_TABLE_DELEGATE_H
