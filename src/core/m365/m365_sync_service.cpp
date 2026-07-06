#include "m365_sync_service.h"
#include "microsoft_graph_client.h"
#include "../task_repository.h"

#include <QtConcurrent>

M365SyncService::M365SyncService(MicrosoftGraphClient *graph, QObject *parent)
    : QObject(parent)
    , m_graph(graph)
{
    qRegisterMetaType<M365SyncResult>("M365SyncResult");
}

void M365SyncService::syncFlaggedMail(TaskRepository *repo)
{
    if (!repo || !m_graph) {
        M365SyncResult r;
        r.errorMessage = QStringLiteral("同步服务未就绪");
        emit syncFinished(r);
        return;
    }

    QtConcurrent::run([this, repo]() {
        M365SyncResult result;
        QString err;
        const QVector<GraphMailItem> mails = m_graph->fetchFlaggedMessages(&err);
        if (mails.isEmpty() && !err.isEmpty()) {
            result.errorMessage = err;
            QMetaObject::invokeMethod(this, [this, result]() { emit syncFinished(result); }, Qt::QueuedConnection);
            return;
        }

        for (const GraphMailItem &mail : mails) {
            if (repo->findByExternal(QStringLiteral("outlook"), mail.id).id > 0) {
                result.skipped++;
                continue;
            }
            TaskItem task;
            task.title = mail.subject;
            task.dueAt = mail.receivedAt.addDays(1);
            task.quadrant = EisenhowerQuadrant::Q2_NotUrgentImportant;
            task.externalSource = QStringLiteral("outlook");
            task.externalId = mail.id;
            if (repo->addTask(&task, &err))
                result.imported++;
        }

        result.success = true;
        QMetaObject::invokeMethod(this, [this, result]() { emit syncFinished(result); }, Qt::QueuedConnection);
    });
}
