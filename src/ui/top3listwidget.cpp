#include "top3listwidget.h"

#include "../core/ai/ai_prompts.h"

#include "task_checkbox_utils.h"

#include <QMouseEvent>
#include <QPainter>
#include <QSet>
#include <QSignalBlocker>
#include <QStyledItemDelegate>

namespace {

constexpr int kCompletedRole = Qt::UserRole + 2;
constexpr int kReasonRole = Qt::UserRole + 1;

class Top3ItemDelegate : public QStyledItemDelegate
{
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        const bool completed = index.data(kCompletedRole).toBool();
        const bool hover = opt.state & QStyle::State_MouseOver;

        painter->save();

        if (opt.state & QStyle::State_Selected)
            painter->fillRect(opt.rect, opt.palette.highlight());
        else if (hover)
            painter->fillRect(opt.rect, QColor(0x14, 0x22, 0x3c, 120));

        QFont font = opt.font;
        if (completed)
            font.setStrikeOut(true);
        painter->setFont(font);
        painter->setPen(completed ? QColor(0x8f, 0xa8, 0x88) : opt.palette.text().color());

        const QRect checkR = TaskCheckbox::rectForItem(opt.rect);
        QRect textRect = opt.rect.adjusted(8, 0, -(TaskCheckbox::kSize + TaskCheckbox::kRightPad + 4), 0);
        const QString title = index.data(Qt::DisplayRole).toString();
        const QString elided = opt.fontMetrics.elidedText(title, Qt::ElideRight, textRect.width());
        painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, elided);

        TaskCheckbox::paint(painter, checkR, completed, hover);

        painter->restore();
    }
};

} // namespace

Top3ListWidget::Top3ListWidget(QWidget *parent)
    : QListWidget(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setObjectName(QStringLiteral("top3List"));
    setItemDelegate(new Top3ItemDelegate(this));
}

void Top3ListWidget::setRecommendations(const QVector<PriorityRecommendation> &recs,
                                        const QHash<qint64, bool> &completedById)
{
    QSignalBlocker blocker(this);
    clear();

    for (const PriorityRecommendation &rec : recs) {
        const QString reason = AiPrompts::sanitizeTop3Reason(rec.reason);
        auto *item = new QListWidgetItem(
            QObject::tr("#%1 %2 (%3)").arg(rec.rank).arg(rec.title).arg(rec.score, 0, 'f', 1));
        item->setData(Qt::UserRole, rec.taskId);
        item->setData(kReasonRole, reason);
        item->setData(kCompletedRole, completedById.value(rec.taskId, false));
        item->setToolTip(reason.isEmpty()
                             ? QObject::tr("点击或双击查看推荐理由")
                             : reason);
        addItem(item);
    }
}

void Top3ListWidget::syncFromTasks(const QVector<TaskItem> &tasks)
{
    QHash<qint64, bool> completedById;
    QSet<qint64> activeIds;
    for (const TaskItem &task : tasks) {
        completedById.insert(task.id, task.completed);
        activeIds.insert(task.id);
    }

    QSignalBlocker blocker(this);
    for (int i = count() - 1; i >= 0; --i) {
        QListWidgetItem *item = this->item(i);
        if (!item)
            continue;
        const qint64 taskId = item->data(Qt::UserRole).toLongLong();
        if (!activeIds.contains(taskId)) {
            delete takeItem(i);
            continue;
        }
        item->setData(kCompletedRole, completedById.value(taskId));
    }
    viewport()->update();
}

void Top3ListWidget::removeTask(qint64 taskId)
{
    if (taskId <= 0)
        return;

    QSignalBlocker blocker(this);
    for (int i = count() - 1; i >= 0; --i) {
        QListWidgetItem *item = this->item(i);
        if (!item)
            continue;
        if (item->data(Qt::UserRole).toLongLong() == taskId) {
            delete takeItem(i);
            viewport()->update();
            return;
        }
    }
}

void Top3ListWidget::toggleItemCompleted(QListWidgetItem *item)
{
    if (!item)
        return;

    const bool completed = !item->data(kCompletedRole).toBool();
    const qint64 taskId = item->data(Qt::UserRole).toLongLong();
    if (taskId <= 0)
        return;

    item->setData(kCompletedRole, completed);
    viewport()->update();
    emit taskCompletedToggled(taskId, completed);
}

void Top3ListWidget::mousePressEvent(QMouseEvent *event)
{
    QListWidgetItem *item = itemAt(event->pos());
    if (item && event->button() == Qt::LeftButton) {
        const QRect checkR = TaskCheckbox::rectForItem(visualItemRect(item));
        if (checkR.contains(event->pos())) {
            toggleItemCompleted(item);
            event->accept();
            return;
        }
    }
    QListWidget::mousePressEvent(event);
}
