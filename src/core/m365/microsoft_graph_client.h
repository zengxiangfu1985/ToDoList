#ifndef MICROSOFT_GRAPH_CLIENT_H
#define MICROSOFT_GRAPH_CLIENT_H

#include "../task_types.h"

#include <QObject>
#include <QVector>

class M365AuthService;

class MicrosoftGraphClient : public QObject
{
    Q_OBJECT
public:
    explicit MicrosoftGraphClient(M365AuthService *auth, QObject *parent = nullptr);

    QVector<GraphMailItem> fetchFlaggedMessages(QString *errorMsg = nullptr);

private:
    bool ensureToken(QString *errorMsg);

    M365AuthService *m_auth;
};

#endif // MICROSOFT_GRAPH_CLIENT_H
