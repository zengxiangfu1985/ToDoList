#ifndef SINGLE_INSTANCE_H
#define SINGLE_INSTANCE_H

#include <QObject>

class QLocalServer;

class SingleInstanceGuard : public QObject
{
    Q_OBJECT
public:
    explicit SingleInstanceGuard(QObject *parent = nullptr);
    ~SingleInstanceGuard() override;

    bool isPrimaryInstance();

signals:
    void raiseRequested();

private:
    void handleNewConnection();

    QLocalServer *m_server = nullptr;
};

#endif // SINGLE_INSTANCE_H
