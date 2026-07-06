#include "quadrantlistwidget.h"

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

static const int kCheckboxSize = 22;
static const int kCheckboxRightPad = 8;

static QRect checkboxRect(const QRect &itemRect)
{
    return QRect(itemRect.right() - kCheckboxRightPad - kCheckboxSize,
                 itemRect.center().y() - kCheckboxSize / 2,
                 kCheckboxSize, kCheckboxSize);
}

static void paintTaskCheckbox(QPainter *p, const QRect &rect, bool checked, bool hover)
{
    p->save();
    p->setRenderHint(QPainter::Antialiasing);

    QColor bg(0x0c, 0x14, 0x24);
    QColor borderTop(0x18, 0x20, 0x30);
    QColor borderBottom(0x06, 0x0a, 0x12);
    if (checked) {
        bg = hover ? QColor(0x20, 0x58, 0x88) : QColor(0x1a, 0x50, 0x80);
        borderTop = hover ? QColor(0x4a, 0x80, 0xa8) : QColor(0x3a, 0x70, 0x98);
        borderBottom = QColor(0x12, 0x30, 0x50);
    } else if (hover) {
        bg = QColor(0x10, 0x18, 0x28);
        borderTop = QColor(0x28, 0x40, 0x60);
    }

    p->setPen(Qt::NoPen);
    p->setBrush(bg);
    p->drawRoundedRect(rect, 6, 6);

    p->setPen(QPen(borderTop, 1));
    p->drawLine(rect.topLeft() + QPoint(1, 0), rect.topRight() + QPoint(-1, 0));
    p->setPen(QPen(borderBottom, 1));
    p->drawLine(rect.bottomLeft() + QPoint(1, 0), rect.bottomRight() + QPoint(-1, 0));
    p->setPen(QPen(QColor(0x0a, 0x10, 0x1c), 1));
    p->setBrush(Qt::NoBrush);
    p->drawRoundedRect(rect.adjusted(0, 0, -1, -1), 6, 6);

    if (checked) {
        QPen tickPen(Qt::white, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p->setPen(tickPen);
        const QPoint c = rect.center();
        QPainterPath tick;
        tick.moveTo(c.x() - 5, c.y());
        tick.lineTo(c.x() - 1, c.y() + 4);
        tick.lineTo(c.x() + 6, c.y() - 5);
        p->drawPath(tick);
    }

    p->restore();
}

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

        const QRect checkR = checkboxRect(opt.rect);
        QRect textRect = opt.rect.adjusted(8, 0, -(kCheckboxSize + kCheckboxRightPad + 4), 0);
        const QString title = index.data(Qt::DisplayRole).toString();
        const QString elided = opt.fontMetrics.elidedText(title, Qt::ElideRight, textRect.width());
        painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, elided);

        paintTaskCheckbox(painter, checkR, completed, hover);

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
        const QRect checkR = checkboxRect(visualItemRect(item));
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
