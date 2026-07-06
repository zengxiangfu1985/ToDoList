#ifndef APP_LOGGER_H
#define APP_LOGGER_H

#include <QString>

class AppLogger
{
public:
    static void initialize();
    static void shutdown();

    static QString logDirectory();
    static QString currentLogFilePath();

    static void debug(const QString &category, const QString &message);
    static void info(const QString &category, const QString &message);
    static void warn(const QString &category, const QString &message);
    static void error(const QString &category, const QString &message);
};

#endif // APP_LOGGER_H
