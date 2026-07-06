#include "app_logger.h"
#include "../core/task_archive.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QTextStream>
#include <QtGlobal>

static QMutex s_mutex;
static QString s_logFilePath;
static bool s_installedQtHandler = false;
static QtMessageHandler s_previousHandler = nullptr;

static void writeLogUnlocked(const char *level, const QString &category, const QString &message)
{
    if (s_logFilePath.isEmpty())
        return;

    QFile file(s_logFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;

    QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#endif
    out << QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"))
        << " [" << level << "] [" << category << "] " << message << "\n";
}

static void writeLog(const char *level, const QString &category, const QString &message)
{
    QMutexLocker lock(&s_mutex);
    writeLogUnlocked(level, category, message);
}

static void qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    const char *level = "DEBUG";
    switch (type) {
    case QtInfoMsg: level = "INFO"; break;
    case QtWarningMsg: level = "WARN"; break;
    case QtCriticalMsg: level = "ERROR"; break;
    case QtFatalMsg: level = "FATAL"; break;
    default: break;
    }

    QString category = QStringLiteral("Qt");
    if (context.category && context.category[0] != '\0')
        category = QString::fromUtf8(context.category);

    writeLog(level, category, msg);

    if (s_previousHandler)
        s_previousHandler(type, context, msg);
}

void AppLogger::initialize()
{
    {
        QMutexLocker lock(&s_mutex);
        const QString logDir = TaskArchive::defaultDataDirectory() + QStringLiteral("/logs");
        QDir().mkpath(logDir);
        s_logFilePath = logDir + QStringLiteral("/todolist-")
                        + QDate::currentDate().toString(QStringLiteral("yyyy-MM-dd"))
                        + QStringLiteral(".log");

        if (!s_installedQtHandler) {
            s_previousHandler = qInstallMessageHandler(qtMessageHandler);
            s_installedQtHandler = true;
        }
    }

    writeLog("INFO", "APP",
             QStringLiteral("=== ToDoList 启动 === 可执行文件: %1")
                 .arg(QCoreApplication::applicationFilePath()));
    writeLog("INFO", "APP",
             QStringLiteral("数据目录: %1 | 日志: %2")
                 .arg(TaskArchive::defaultDataDirectory(), s_logFilePath));
}

void AppLogger::shutdown()
{
    writeLog("INFO", "APP", QStringLiteral("=== ToDoList 退出 ==="));
}

QString AppLogger::logDirectory()
{
    return TaskArchive::defaultDataDirectory() + QStringLiteral("/logs");
}

QString AppLogger::currentLogFilePath()
{
    QMutexLocker lock(&s_mutex);
    return s_logFilePath;
}

void AppLogger::debug(const QString &category, const QString &message)
{
    writeLog("DEBUG", category, message);
}

void AppLogger::info(const QString &category, const QString &message)
{
    writeLog("INFO", category, message);
}

void AppLogger::warn(const QString &category, const QString &message)
{
    writeLog("WARN", category, message);
}

void AppLogger::error(const QString &category, const QString &message)
{
    writeLog("ERROR", category, message);
}
