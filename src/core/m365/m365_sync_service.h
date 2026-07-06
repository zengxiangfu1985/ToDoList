#ifndef M365_SYNC_SERVICE_H
#define M365_SYNC_SERVICE_H

#include "../task_types.h"

#include <QObject>

class TaskRepository;
class MicrosoftGraphClient;

class M365SyncService : public QObject
{
    Q_OBJECT
public:
    explicit M365SyncService(MicrosoftGraphClient *graph, QObject *parent = nullptr);

public slots:
    void syncFlaggedMail(TaskRepository *repo);

signals:
    void syncFinished(const M365SyncResult &result);

private:
    MicrosoftGraphClient *m_graph;
};

#endif // M365_SYNC_SERVICE_H
