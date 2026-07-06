#ifndef SCHEMA_MIGRATOR_H
#define SCHEMA_MIGRATOR_H

#include <QSqlDatabase>
#include <QString>

class SchemaMigrator
{
public:
    static constexpr int kCurrentSchemaVersion = 1;

    static bool migrate(QSqlDatabase db, int targetVersion, QString *errorMsg = nullptr);

private:
    static bool ensureMetaTable(QSqlDatabase db, QString *errorMsg);
    static int currentVersion(QSqlDatabase db);
    static bool setVersion(QSqlDatabase db, int version, QString *errorMsg);
    static bool migrateStep(QSqlDatabase db, int fromVersion, QString *errorMsg);
    static bool migrateFrom0To1(QSqlDatabase db, QString *errorMsg);
};

#endif // SCHEMA_MIGRATOR_H
