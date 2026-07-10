#include "quadrantlistwidget.h"

#include "task_checkbox_utils.h"

#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFont>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QSignalBlocker>
#include <QSizePolicy>
#include <QStyledItemDelegate>

static const char kTaskMime[] = "application/x-todolist-task";
static const int kCompletedRole = Qt::UserRole + 1;

class QuadrantItemDelegate : public QStyledItemDelegate
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

QuadrantListWidget::QuadrantListWidget(EisenhowerQuadrant quadrant, QWidget *parent)
    : QListWidget(parent)
    , m_quadrant(quadrant)
{
    setAcceptDrops(true);
    setDragEnabled(true);
    setDefaultDropAction(Qt::MoveAction);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setObjectName(QStringLiteral("quadrantList"));
    setItemDelegate(new QuadrantItemDelegate(this));
}

void QuadrantListWidget::setTaskItems(const QVector<TaskItem> &tasks)
{
    QSignalBlocker blocker(this);
    clear();
    for (const TaskItem &t : tasks) {
        if (!isQuadrantAssigned(t.quadrant) || t.quadrant != m_quadrant)
            continue;
        auto *item = new QListWidgetItem(t.title);
        item->setData(Qt::UserRole, static_cast<qlonglong>(t.id));
        item->setData(kCompletedRole, t.completed);
        addItem(item);
    }
}

void QuadrantListWidget::removeTask(qint64 taskId)
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

void QuadrantListWidget::toggleItemCompleted(QListWidgetItem *item)
{
    if (!item)
        return;

    const bool completed = !item->data(kCompletedRole).toBool();
    const qint64 taskId = item->data(Qt::UserRole).toLongLong();
    item->setData(kCompletedRole, completed);
    emit taskCompletedToggled(taskId, completed);
    // 勿在 emit 之后访问 item：tasksChanged 会同步 clear() 列表项。
}

void QuadrantListWidget::mousePressEvent(QMouseEvent *event)
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

void QuadrantListWidget::startDrag(Qt::DropActions supportedActions)
{
    QListWidgetItem *item = currentItem();
    if (!item)
        return;

    auto *mime = new QMimeData;
    const qlonglong id = item->data(Qt::UserRole).toLongLong();
    mime->setData(kTaskMime, QByteArray::number(id));
    mime->setText(item->text());

    auto *drag = new QDrag(this);
    drag->setMimeData(mime);
    drag->exec(supportedActions, Qt::MoveAction);
}

void QuadrantListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(kTaskMime))
        event->acceptProposedAction();
}

void QuadrantListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(kTaskMime))
        event->acceptProposedAction();
}

void QuadrantListWidget::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasFormat(kTaskMime))
        return;

    const qlonglong taskId = event->mimeData()->data(kTaskMime).toLongLong();
    auto *sourceList = qobject_cast<QuadrantListWidget *>(event->source());
    const EisenhowerQuadrant from = sourceList ? sourceList->quadrant() : m_quadrant;

    if (from != m_quadrant)
        emit taskQuadrantChanged(taskId, from, m_quadrant);

    event->acceptProposedAction();
}
