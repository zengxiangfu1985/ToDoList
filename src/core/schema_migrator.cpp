#include "schema_migrator.h"

#include "../utils/app_logger.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>

namespace {

bool backupDatabase(QSqlDatabase db, QString *errorMsg)
{
    const QString dbPath = db.databaseName();
    if (dbPath.isEmpty() || !QFileInfo::exists(dbPath))
        return true;

    const QFileInfo info(dbPath);
    const QString backupDir = info.dir().filePath(QStringLiteral("backup/db-")
                                                    + QDateTime::currentDateTime().toString(
                                                        QStringLiteral("yyyyMMdd-HHmmss")));
    QDir().mkpath(backupDir);
    const QString backupPath = QDir(backupDir).filePath(info.fileName());
    if (QFile::exists(backupPath))
        QFile::remove(backupPath);
    if (!QFile::copy(dbPath, backupPath)) {
        if (errorMsg)
            *errorMsg = QStringLiteral("无法备份数据库: %1").arg(backupPath);
        return false;
    }
    AppLogger::info("DB", QStringLiteral("升级前已备份数据库: %1").arg(backupPath));
    return true;
}

bool hasColumn(QSqlDatabase db, const QString &table, const QString &column)
{
    QSqlQuery info(db);
    if (!info.exec(QStringLiteral("PRAGMA table_info(%1)").arg(table)))
        return false;
    while (info.next()) {
        if (info.value(1).toString() == column)
            return true;
    }
    return false;
}

} // namespace

bool SchemaMigrator::ensureMetaTable(QSqlDatabase db, QString *errorMsg)
{
    QSqlQuery q(db);
    if (!q.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS schema_meta ("
            "key TEXT PRIMARY KEY,"
            "value TEXT NOT NULL"
            ")"))) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        return false;
    }
    return true;
}

int SchemaMigrator::currentVersion(QSqlDatabase db)
{
    QSqlQuery q(db);
    if (!q.exec(QStringLiteral("SELECT value FROM schema_meta WHERE key='schema_version' LIMIT 1")))
        return 0;
    if (!q.next())
        return 0;
    return q.value(0).toString().toInt();
}

bool SchemaMigrator::setVersion(QSqlDatabase db, int version, QString *errorMsg)
{
    QSqlQuery q(db);
    if (!q.exec(QStringLiteral("INSERT OR REPLACE INTO schema_meta(key, value) VALUES('schema_version', '%1')")
                   .arg(version))) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        return false;
    }
    return true;
}

bool SchemaMigrator::migrateFrom0To1(QSqlDatabase db, QString *errorMsg)
{
    QSqlQuery q(db);

    if (!hasColumn(db, QStringLiteral("tasks"), QStringLiteral("external_source"))) {
        if (!q.exec(QStringLiteral("ALTER TABLE tasks ADD COLUMN external_source TEXT")) && errorMsg)
            *errorMsg = q.lastError().text();
    }
    if (!hasColumn(db, QStringLiteral("tasks"), QStringLiteral("external_id"))) {
        if (!q.exec(QStringLiteral("ALTER TABLE tasks ADD COLUMN external_id TEXT")) && errorMsg)
            *errorMsg = q.lastError().text();
    }
    if (!hasColumn(db, QStringLiteral("tasks"), QStringLiteral("completed_at"))) {
        if (!q.exec(QStringLiteral("ALTER TABLE tasks ADD COLUMN completed_at TEXT")) && errorMsg)
            *errorMsg = q.lastError().text();
    }
    if (!hasColumn(db, QStringLiteral("tasks"), QStringLiteral("archived"))) {
        if (!q.exec(QStringLiteral("ALTER TABLE tasks ADD COLUMN archived INTEGER NOT NULL DEFAULT 0"))
            && errorMsg)
            *errorMsg = q.lastError().text();
    }
    if (!hasColumn(db, QStringLiteral("tasks"), QStringLiteral("archived_at"))) {
        if (!q.exec(QStringLiteral("ALTER TABLE tasks ADD COLUMN archived_at TEXT")) && errorMsg)
            *errorMsg = q.lastError().text();
    }

    if (!q.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS daily_evaluations ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "eval_date TEXT NOT NULL UNIQUE,"
            "tasks_completed INTEGER NOT NULL DEFAULT 0,"
            "tasks_due INTEGER NOT NULL DEFAULT 0,"
            "tasks_pending INTEGER NOT NULL DEFAULT 0,"
            "summary TEXT,"
            "ai_feedback TEXT,"
            "used_llm INTEGER NOT NULL DEFAULT 0,"
            "created_at TEXT NOT NULL"
            ")"))) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        return false;
    }

    if (!hasColumn(db, QStringLiteral("daily_evaluations"), QStringLiteral("task_plan_review"))) {
        if (!q.exec(QStringLiteral("ALTER TABLE daily_evaluations ADD COLUMN task_plan_review TEXT"))
            && errorMsg)
            *errorMsg = q.lastError().text();
    }
    if (!hasColumn(db, QStringLiteral("daily_evaluations"), QStringLiteral("llm_provider"))) {
        if (!q.exec(QStringLiteral(
                "ALTER TABLE daily_evaluations ADD COLUMN llm_provider INTEGER NOT NULL DEFAULT 0"))
            && errorMsg)
            *errorMsg = q.lastError().text();
    }
    if (!hasColumn(db, QStringLiteral("daily_evaluations"), QStringLiteral("llm_model"))) {
        if (!q.exec(QStringLiteral("ALTER TABLE daily_evaluations ADD COLUMN llm_model TEXT"))
            && errorMsg)
            *errorMsg = q.lastError().text();
    }

    if (!q.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS weekly_reports ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "week_start TEXT NOT NULL,"
            "week_end TEXT NOT NULL,"
            "markdown TEXT,"
            "used_llm INTEGER NOT NULL DEFAULT 0,"
            "llm_provider INTEGER NOT NULL DEFAULT 0,"
            "llm_model TEXT,"
            "selected_task_ids TEXT,"
            "created_at TEXT NOT NULL,"
            "UNIQUE(week_start, week_end)"
            ")"))) {
        if (errorMsg)
            *errorMsg = q.lastError().text();
        return false;
    }

    return true;
}

bool SchemaMigrator::migrateStep(QSqlDatabase db, int fromVersion, QString *errorMsg)
{
    switch (fromVersion) {
    case 0:
        return migrateFrom0To1(db, errorMsg);
    default:
        if (errorMsg)
            *errorMsg = QStringLiteral("未知 schema 版本: %1").arg(fromVersion);
        return false;
    }
}

bool SchemaMigrator::migrate(QSqlDatabase db, int targetVersion, QString *errorMsg)
{
    if (!ensureMetaTable(db, errorMsg))
        return false;

    int version = currentVersion(db);
    if (version >= targetVersion)
        return true;

    if (!backupDatabase(db, errorMsg))
        return false;

    while (version < targetVersion) {
        AppLogger::info("DB", QStringLiteral("数据库迁移: %1 -> %2").arg(version).arg(version + 1));
        if (!migrateStep(db, version, errorMsg))
            return false;
        version += 1;
        if (!setVersion(db, version, errorMsg))
            return false;
    }
    return true;
}
