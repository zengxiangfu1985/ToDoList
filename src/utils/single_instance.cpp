#include "single_instance.h"

#include <QLocalServer>
#include <QLocalSocket>

namespace {

QString serverName()
{
    return QStringLiteral("ToDoList.SingleInstance.v1");
}

} // namespace

SingleInstanceGuard::SingleInstanceGuard(QObject *parent)
    : QObject(parent)
{
}

SingleInstanceGuard::~SingleInstanceGuard()
{
    if (m_server) {
        m_server->close();
        QLocalServer::removeServer(serverName());
    }
}

bool SingleInstanceGuard::isPrimaryInstance()
{
    QLocalSocket probe;
    probe.connectToServer(serverName());
    if (probe.waitForConnected(300)) {
        probe.write("raise");
        probe.flush();
        probe.waitForBytesWritten(300);
        probe.disconnectFromServer();
        return false;
    }

    QLocalServer::removeServer(serverName());

    m_server = new QLocalServer(this);
    connect(m_server, &QLocalServer::newConnection, this, &SingleInstanceGuard::handleNewConnection);

    if (!m_server->listen(serverName())) {
        QLocalSocket retry;
        retry.connectToServer(serverName());
        if (retry.waitForConnected(300)) {
            retry.write("raise");
            retry.flush();
            retry.waitForBytesWritten(300);
            retry.disconnectFromServer();
            return false;
        }
        return false;
    }

    return true;
}

void SingleInstanceGuard::handleNewConnection()
{
    if (!m_server)
        return;

    QLocalSocket *socket = m_server->nextPendingConnection();
    if (!socket)
        return;

    connect(socket, &QLocalSocket::readyRead, this, [this, socket]() {
        if (socket->readAll() == QByteArray("raise"))
            emit raiseRequested();
        socket->deleteLater();
    });
}
