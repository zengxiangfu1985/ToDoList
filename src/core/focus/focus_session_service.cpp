#include "focus_session_service.h"

#include "../../utils/app_logger.h"

FocusSessionService::FocusSessionService(QObject *parent)
    : QObject(parent)
{
    m_timer.setInterval(1000);
    connect(&m_timer, &QTimer::timeout, this, [this]() {
        if (m_state != FocusSessionState::Running || m_session.remainingSec <= 0)
            return;

        --m_session.remainingSec;
        emitTick();

        if (m_session.remainingSec <= 0) {
            m_timer.stop();
            enterAwaitingResult();
        }
    });
}

void FocusSessionService::start(qint64 taskId, const QString &taskTitle, int durationSec, qint64 dbSessionId)
{
    m_session.taskId = taskId;
    m_session.taskTitle = taskTitle;
    m_session.durationSec = durationSec;
    m_session.remainingSec = durationSec;
    m_session.pomodoroIndex = 1;
    m_session.dbSessionId = dbSessionId;

    setState(FocusSessionState::Running);
    m_timer.start();
    emitTick();

    AppLogger::info("FOCUS",
                    QStringLiteral("开始 Focus 25 taskId=%1 duration=%2s").arg(taskId).arg(durationSec));
}

void FocusSessionService::pause()
{
    if (m_state != FocusSessionState::Running)
        return;
    m_timer.stop();
    setState(FocusSessionState::Paused);
}

void FocusSessionService::resume()
{
    if (m_state != FocusSessionState::Paused)
        return;
    setState(FocusSessionState::Running);
    m_timer.start();
}

void FocusSessionService::extendMinutes(int minutes)
{
    if (m_state != FocusSessionState::Running && m_state != FocusSessionState::Paused)
        return;
    m_session.remainingSec += minutes * 60;
    emitTick();
}

void FocusSessionService::abandonMidSession()
{
    if (m_state == FocusSessionState::Idle)
        return;
    m_timer.stop();
    AppLogger::info("FOCUS", QStringLiteral("中途放弃 Focus taskId=%1").arg(m_session.taskId));
    reset();
}

void FocusSessionService::enterAwaitingResult()
{
    if (m_state == FocusSessionState::Idle)
        return;
    setState(FocusSessionState::AwaitingResult);
    emit awaitingResult();
    AppLogger::info("FOCUS", QStringLiteral("Focus 倒计时结束 taskId=%1").arg(m_session.taskId));
}

void FocusSessionService::restartRound(int durationSec)
{
    m_session.durationSec = durationSec;
    m_session.remainingSec = durationSec;
    ++m_session.pomodoroIndex;
    setState(FocusSessionState::Running);
    m_timer.start();
    emitTick();
    AppLogger::info("FOCUS",
                    QStringLiteral("再来一轮 Focus taskId=%1 round=%2")
                        .arg(m_session.taskId)
                        .arg(m_session.pomodoroIndex));
}

void FocusSessionService::setDbSessionId(qint64 sessionId)
{
    m_session.dbSessionId = sessionId;
}

void FocusSessionService::reset()
{
    m_timer.stop();
    m_session = {};
    setState(FocusSessionState::Idle);
    emit sessionEnded();
}

void FocusSessionService::setState(FocusSessionState state)
{
    if (m_state == state)
        return;
    m_state = state;
    emit stateChanged(m_state);
}

void FocusSessionService::emitTick()
{
    emit tick(m_session.remainingSec);
}
