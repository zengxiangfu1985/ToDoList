#include "../../src/core/update/update_apply.h"
#include "../../src/core/update/update_manifest.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QTextStream>

namespace {

void logLine(const QString &message)
{
    QTextStream out(stdout);
    out << message << '\n';
    out.flush();
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

} // namespace

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("ToDoListUpdater"));

    const QStringList args = app.arguments();
    if (!args.contains(QStringLiteral("--apply"))) {
        logLine(QStringLiteral("Usage: ToDoListUpdater.exe --apply"));
        return 1;
    }

    QString error;
    const UpdatePendingInfo pending = UpdateManifest::readPendingFile(&error);
    if (!pending.valid) {
        logLine(error.isEmpty() ? QStringLiteral("Invalid pending file") : error);
        return 2;
    }

    const bool ok = UpdateApplyEngine::applyPackage(
        pending, &error, [](const QString &message) { logLine(message); });

    if (!ok) {
        logLine(error.isEmpty() ? QStringLiteral("Update failed") : error);
        return 3;
    }

    if (!restartApplication(pending)) {
        logLine(QStringLiteral("Update succeeded but failed to restart ToDoList.exe"));
        return 4;
    }

    return 0;
}
