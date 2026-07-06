#ifndef SSL_SUPPORT_H
#define SSL_SUPPORT_H

#include <QString>

class SslSupport
{
public:
    static bool initialize();
    static bool isAvailable();
    static QString unavailableMessage();

private:
    static bool tryLoadFromDirectory(const QString &dir);
    static QStringList candidateDirectories();

    static bool s_initialized;
    static bool s_available;
};

#endif // SSL_SUPPORT_H
