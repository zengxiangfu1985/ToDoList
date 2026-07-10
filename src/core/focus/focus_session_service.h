#ifndef FOCUS_SESSION_SERVICE_H
#define FOCUS_SESSION_SERVICE_H

#include "../task_types.h"

#include <QObject>
#include <QTimer>

enum class FocusSessionState {
    Idle,
    Running,
    Paused,
    AwaitingResult
};

struct ActiveFocusSession {
    qint64 taskId = 0;
    QString taskTitle;
    int durationSec = 25 * 60;
    int remainingSec = 0;
    int pomodoroIndex = 1;
    qint64 dbSessionId = 0;
};

class FocusSessionService : public QObject
{
    Q_OBJECT
public:
    explicit FocusSessionService(QObject *parent = nullptr);

    FocusSessionState state() const { return m_state; }
    ActiveFocusSession current() const { return m_session; }
    bool isActive() const { return m_state != FocusSessionState::Idle; }

    void start(qint64 taskId, const QString &taskTitle, int durationSec, qint64 dbSessionId);
    void pause();
    void resume();
    void extendMinutes(int minutes);
    void abandonMidSession();
    void enterAwaitingResult();
    void restartRound(int durationSec);
    void setDbSessionId(qint64 sessionId);
    void reset();

signals:
    void tick(int remainingSec);
    void stateChanged(FocusSessionState state);
    void awaitingResult();
    void sessionEnded();

private:
    void setState(FocusSessionState state);
    void emitTick();

    QTimer m_timer;
    ActiveFocusSession m_session;
    FocusSessionState m_state = FocusSessionState::Idle;
};

#endif // FOCUS_SESSION_SERVICE_H
