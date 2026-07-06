#include "../../src/core/update/update_apply.h"
#include "../../src/core/update/update_manifest.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QTextStream>

namespace {

void appendLogFile(const QString &logPath, const QString &message)
{
    if (logPath.isEmpty())
        return;
    QDir().mkpath(QFileInfo(logPath).absolutePath());
    QFile file(logPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;
    const QString line = QStringLiteral("[%1] %2\n")
                             .arg(QDateTime::currentDateTime().toString(
                                      QStringLiteral("yyyy-MM-dd hh:mm:ss.zzz")),
                                  message);
    file.write(line.toUtf8());
}

void logLine(const QString &logPath, const QString &message)
{
    QTextStream out(stdout);
    out << message << '\n';
    out.flush();
    appendLogFile(logPath, message);
}

bool restartApplication(const UpdatePendingInfo &pending)
{
    if (!pending.restart)
        return true;

    const QString exePath = QDir(pending.installDir).filePath(QStringLiteral("ToDoList.exe"));
    if (!QFile::exists(exePath))
        return false;
    return QProcess::startDetached(exePath, {}, pending.installDir);
}

QString updaterLogPath(const UpdatePendingInfo &pending)
{
    return QDir(pending.installDir)
        .filePath(QStringLiteral("data/logs/updater-")
                  + QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd"))
                  + QStringLiteral(".log"));
}

} // namespace

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("ToDoListUpdater"));

    const QStringList args = app.arguments();
    if (!args.contains(QStringLiteral("--apply"))) {
        logLine(QString(), QStringLiteral("Usage: ToDoListUpdater.exe --apply"));
        return 1;
    }

    QString error;
    const UpdatePendingInfo pending = UpdateManifest::readPendingFile(&error);
    if (!pending.valid) {
        logLine(QString(), error.isEmpty() ? QStringLiteral("Invalid pending file") : error);
        return 2;
    }

    const QString logPath = updaterLogPath(pending);
    logLine(logPath, QStringLiteral("=== ToDoListUpdater 开始 ==="));
    logLine(logPath, QStringLiteral("install_dir: %1").arg(pending.installDir));
    logLine(logPath, QStringLiteral("zip_path: %1").arg(pending.zipPath));
    logLine(logPath, QStringLiteral("target: %1 (build %2)")
                        .arg(pending.targetVersion)
                        .arg(pending.targetBuild));

    const bool ok = UpdateApplyEngine::applyPackage(
        pending, &error, [logPath](const QString &message) { logLine(logPath, message); });

    if (!ok) {
        logLine(logPath, error.isEmpty() ? QStringLiteral("Update failed") : error);
        return 3;
    }

    if (!restartApplication(pending)) {
        logLine(logPath, QStringLiteral("Update succeeded but failed to restart ToDoList.exe"));
        return 4;
    }

    logLine(logPath, QStringLiteral("=== ToDoListUpdater 完成 ==="));
    return 0;
}
