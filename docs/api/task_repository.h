#ifndef TASK_REPOSITORY_H
#define TASK_REPOSITORY_H

#include "task_types.h"

#include <QObject>
#include <QVector>

// Scenario: S01 Step 3-4
class TaskRepository : public QObject
{
    Q_OBJECT
public:
    explicit TaskRepository(QObject *parent = nullptr);
    ~TaskRepository() override;

    bool open(const QString &dbPath, QString *errorMsg = nullptr);
    QVector<TaskItem> allTasks() const;
    bool addTask(TaskItem *task, QString *errorMsg = nullptr);
    bool updateTask(const TaskItem &task, QString *errorMsg = nullptr);
    bool deleteTask(qint64 id, QString *errorMsg = nullptr);

signals:
    void tasksChanged();

private:
    bool ensureSchema(QString *errorMsg);
    class Private;
    Private *d;
};

#endif // TASK_REPOSITORY_H
