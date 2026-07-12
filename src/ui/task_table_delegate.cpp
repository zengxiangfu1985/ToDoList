#include "task_table_delegate.h"

#include "task_checkbox_utils.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionButton>

TaskTableDelegate::TaskTableDelegate(TaskTableModel *model, QObject *parent)
    : QStyledItemDelegate(parent)
    , m_model(model)
{
}

QRect TaskTableDelegate::centeredCheckboxRect(const QRect &cellRect)
{
    QRect checkR(0, 0, TaskCheckbox::kSize, TaskCheckbox::kSize);
    checkR.moveCenter(cellRect.center());
    return checkR;
}

QColor TaskTableDelegate::quadrantGlowColor(EisenhowerQuadrant q)
{
    switch (q) {
    case EisenhowerQuadrant::Q1_UrgentImportant: return QColor(0x5b, 0x9b, 0xd5);
    case EisenhowerQuadrant::Q2_NotUrgentImportant: return QColor(0x7a, 0x8f, 0xd4);
    case EisenhowerQuadrant::Q3_UrgentNotImportant: return QColor(0xb8, 0x9b, 0x5e);
    case EisenhowerQuadrant::Q4_NotUrgentNotImportant: return QColor(0x6d, 0x98, 0x73);
    default: return QColor(0x8a, 0x9b, 0xb8);
    }
}

void TaskTableDelegate::paintQuadrantBadge(QPainter *p, const QStyleOptionViewItem &option,
                                           const QString &text, EisenhowerQuadrant q) const
{
    if (!isQuadrantAssigned(q)) {
        p->save();
        if (option.state & QStyle::State_Selected)
            p->fillRect(option.rect, option.palette.highlight());
        p->setPen(QColor(0x8a, 0x9b, 0xb8));
        p->drawText(option.rect, Qt::AlignCenter, text);
        p->restore();
        return;
    }

    const QColor accent = quadrantGlowColor(q);
    QRect badge = option.rect;
    badge.setWidth(44);
    badge.setHeight(26);
    badge.moveCenter(option.rect.center());

    p->setPen(QPen(accent, 1));
    p->setBrush(QColor(0x0c, 0x14, 0x24));
    p->drawRoundedRect(badge, 6, 6);

    p->setPen(accent);
    QFont f = option.font;
    f.setBold(true);
    p->setFont(f);
    p->drawText(badge, Qt::AlignCenter, text);
}

void TaskTableDelegate::paintTitle(QPainter *p, QStyleOptionViewItem opt, const QString &title,
                                   bool completed) const
{
    if (opt.state & QStyle::State_Selected) {
        p->fillRect(opt.rect, opt.palette.highlight());
        opt.palette.setColor(QPalette::Text, opt.palette.highlightedText().color());
    } else if (opt.state & QStyle::State_MouseOver) {
        p->fillRect(opt.rect, QColor(0x14, 0x22, 0x3c, 120));
    }

    p->setPen(opt.palette.text().color());
    QFont f = opt.font;
    if (completed) {
        f.setStrikeOut(true);
        p->setPen(QColor(0x8f, 0xa8, 0x88));
    }
    p->setFont(f);

    QRect textRect = opt.rect.adjusted(12, 0, -8, 0);
    const QString elided = opt.fontMetrics.elidedText(title, Qt::ElideRight, textRect.width());
    p->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, elided);
}

void TaskTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    if (!m_model) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    const TaskItem task = m_model->taskAt(index.row());

    if (index.column() == TaskTableModel::ColQuadrant) {
        painter->save();
        if (opt.state & QStyle::State_Selected)
            painter->fillRect(opt.rect, opt.palette.highlight());
        paintQuadrantBadge(painter, opt, index.data(Qt::DisplayRole).toString(), task.quadrant);
        painter->restore();
        return;
    }

    if (index.column() == TaskTableModel::ColTitle) {
        painter->save();
        paintTitle(painter, opt, task.title, task.completed);
        painter->restore();
        return;
    }

    if (index.column() == TaskTableModel::ColDue || index.column() == TaskTableModel::ColScore
        || index.column() == TaskTableModel::ColIndex
        || index.column() == TaskTableModel::ColCompleted) {
        painter->save();
        if (opt.state & QStyle::State_Selected)
            painter->fillRect(opt.rect, opt.palette.highlight());
        if (index.column() == TaskTableModel::ColCompleted) {
            if (m_model->readOnly()) {
                const QString text = index.data(Qt::DisplayRole).toString();
                if (!text.isEmpty()) {
                    painter->setPen(task.completed ? QColor(0x6d, 0x98, 0x73) : QColor(0x9a, 0xab, 0xc8));
                    painter->drawText(opt.rect, Qt::AlignCenter, text);
                    painter->restore();
                    return;
                }
            } else {
                const bool completed = index.data(Qt::CheckStateRole).toInt() == Qt::Checked;
                const bool hover = opt.state & QStyle::State_MouseOver;
                TaskCheckbox::paint(painter, centeredCheckboxRect(opt.rect), completed, hover);
                painter->restore();
                return;
            }
        }
        painter->setPen(QColor(0x9a, 0xab, 0xc8));
        painter->drawText(opt.rect, Qt::AlignCenter, index.data(Qt::DisplayRole).toString());
        painter->restore();
        return;
    }

    QStyledItemDelegate::paint(painter, option, index);
}

bool TaskTableDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                      const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (!m_model || m_model->readOnly() || index.column() != TaskTableModel::ColCompleted)
        return QStyledItemDelegate::editorEvent(event, model, option, index);

    if (event->type() == QEvent::MouseButtonRelease) {
        const auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() != Qt::LeftButton)
            return false;

        if (!centeredCheckboxRect(option.rect).contains(mouseEvent->pos()))
            return false;

        const Qt::CheckState newState =
            index.data(Qt::CheckStateRole).toInt() == Qt::Checked ? Qt::Unchecked : Qt::Checked;
        return model->setData(index, newState, Qt::CheckStateRole);
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
