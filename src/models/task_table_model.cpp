#include "task_table_model.h"

TaskTableModel::TaskTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void TaskTableModel::setTasks(const QVector<TaskItem> &tasks)
{
    beginResetModel();
    m_tasks = tasks;
    m_deleteSelected.clear();
    endResetModel();
}

TaskItem TaskTableModel::taskAt(int row) const
{
    if (row < 0 || row >= m_tasks.size())
        return {};
    return m_tasks.at(row);
}

int TaskTableModel::rowForTaskId(qint64 id) const
{
    for (int i = 0; i < m_tasks.size(); ++i) {
        if (m_tasks.at(i).id == id)
            return i;
    }
    return -1;
}

void TaskTableModel::setDeleteMode(bool enabled)
{
    if (m_deleteMode == enabled)
        return;
    m_deleteMode = enabled;
    m_deleteSelected.clear();
    emit deleteModeChanged(enabled);
    emit headerDataChanged(Qt::Horizontal, ColSelect, ColSelect);
    if (rowCount() > 0)
        emit dataChanged(index(0, ColSelect), index(rowCount() - 1, ColCompleted), {Qt::CheckStateRole});
}

bool TaskTableModel::deleteMode() const
{
    return m_deleteMode;
}

QVector<qint64> TaskTableModel::selectedTaskIdsForDelete() const
{
    QVector<qint64> ids;
    ids.reserve(m_deleteSelected.size());
    for (qint64 id : m_deleteSelected)
        ids.append(id);
    return ids;
}

void TaskTableModel::clearDeleteSelection()
{
    m_deleteSelected.clear();
    if (rowCount() > 0)
        emit dataChanged(index(0, ColSelect), index(rowCount() - 1, ColSelect), {Qt::CheckStateRole});
}

QString TaskTableModel::quadrantText(EisenhowerQuadrant q)
{
    if (!isQuadrantAssigned(q))
        return QObject::tr("待AI");
    return QStringLiteral("Q%1").arg(static_cast<int>(q));
}

int TaskTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_tasks.size();
}

int TaskTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return ColCount;
}

QVariant TaskTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_tasks.size())
        return {};

    const TaskItem &t = m_tasks.at(index.row());

    if (role == Qt::TextAlignmentRole)
        return static_cast<int>(Qt::AlignCenter);

    if (role == Qt::CheckStateRole) {
        if (index.column() == ColCompleted)
            return t.completed ? Qt::Checked : Qt::Unchecked;
        if (index.column() == ColSelect && m_deleteMode)
            return m_deleteSelected.contains(t.id) ? Qt::Checked : Qt::Unchecked;
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case ColSelect: return {};
        case ColIndex: return index.row() + 1;
        case ColTitle: return t.title;
        case ColDue:
            return t.dueAt.isValid()
                       ? t.dueAt.toLocalTime().toString(QStringLiteral("yyyy-MM-dd HH:mm"))
                       : QStringLiteral("-");
        case ColQuadrant: return quadrantText(t.quadrant);
        case ColScore: return QString::number(t.ruleScore, 'f', 1);
        case ColCompleted: return {};
        default: return {};
        }
    }

    if (role == Qt::UserRole)
        return QVariant::fromValue(static_cast<qlonglong>(t.id));

    if (role == Qt::ToolTipRole) {
        if (index.column() == ColTitle)
            return t.title;
        if (index.column() == ColCompleted && t.completed && t.completedAt.isValid())
            return QObject::tr("完成于 %1").arg(t.completedAt.toLocalTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")));
    }

    return {};
}

bool TaskTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_tasks.size())
        return false;

    const qint64 taskId = m_tasks.at(index.row()).id;

    if (index.column() == ColCompleted && role == Qt::CheckStateRole) {
        const bool completed = value.toInt() == Qt::Checked;
        if (m_tasks[index.row()].completed == completed)
            return true;
        m_tasks[index.row()].completed = completed;
        emit dataChanged(index, index, {Qt::CheckStateRole});
        emit taskCompletedToggled(taskId, completed);
        return true;
    }

    if (index.column() == ColSelect && role == Qt::CheckStateRole && m_deleteMode) {
        const bool selected = value.toInt() == Qt::Checked;
        if (selected)
            m_deleteSelected.insert(taskId);
        else
            m_deleteSelected.remove(taskId);
        emit dataChanged(index, index, {Qt::CheckStateRole});
        return true;
    }

    return false;
}

Qt::ItemFlags TaskTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags f = QAbstractTableModel::flags(index);
    if (index.column() == ColCompleted)
        f |= Qt::ItemIsUserCheckable;
    if (index.column() == ColSelect && m_deleteMode)
        f |= Qt::ItemIsUserCheckable;
    return f;
}

QVariant TaskTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
        return QAbstractTableModel::headerData(section, orientation, role);

    if (role == Qt::TextAlignmentRole)
        return static_cast<int>(Qt::AlignCenter);

    if (role == Qt::DisplayRole) {
        switch (section) {
        case ColSelect: return m_deleteMode ? tr("选择") : QString();
        case ColIndex: return tr("序号");
        case ColTitle: return tr("标题");
        case ColDue: return tr("截止");
        case ColQuadrant: return tr("象限");
        case ColScore: return tr("分数");
        case ColCompleted: return tr("完成");
        default: return {};
        }
    }
    return {};
}
