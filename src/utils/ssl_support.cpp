#include "ssl_support.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QLibrary>
#include <QSslSocket>

bool SslSupport::s_initialized = false;
bool SslSupport::s_available = false;

QStringList SslSupport::candidateDirectories()
{
    QStringList dirs;
    const QString appDir = QCoreApplication::applicationDirPath();

    dirs << appDir;
    dirs << appDir + QStringLiteral("/ssl");
    dirs << appDir + QStringLiteral("/openssl");

    const QString bundled = QDir(appDir).absoluteFilePath(QStringLiteral("../third_party/openssl/win64"));
    dirs << QDir(bundled).absolutePath();

    const QString envBin = qEnvironmentVariable("OPENSSL_BIN");
    if (!envBin.isEmpty())
        dirs << envBin;

    dirs << QStringLiteral("C:/Program Files/OpenSSL-Win64/bin");
    dirs << QStringLiteral("C:/OpenSSL-Win64/bin");
    dirs << QStringLiteral("D:/Qt/Tools/OpenSSL/Win_x64/bin");

    dirs.removeDuplicates();
    return dirs;
}

bool SslSupport::tryLoadFromDirectory(const QString &dir)
{
    if (dir.isEmpty() || !QDir(dir).exists())
        return false;

    QCoreApplication::addLibraryPath(dir);

    const QString cryptoPath = QDir(dir).absoluteFilePath(QStringLiteral("libcrypto-1_1-x64.dll"));
    const QString sslPath = QDir(dir).absoluteFilePath(QStringLiteral("libssl-1_1-x64.dll"));

    if (!QFileInfo::exists(cryptoPath) || !QFileInfo::exists(sslPath))
        return false;

    QLibrary cryptoLib(cryptoPath);
    QLibrary sslLib(sslPath);
    if (!cryptoLib.load())
        return false;
    if (!sslLib.load())
        return false;

    return QSslSocket::supportsSsl();
}

bool SslSupport::initialize()
{
    if (s_initialized)
        return s_available;

    s_initialized = true;

    if (QSslSocket::supportsSsl()) {
        s_available = true;
        return true;
    }

    for (const QString &dir : candidateDirectories()) {
        if (tryLoadFromDirectory(dir)) {
            s_available = true;
            return true;
        }
    }

    s_available = QSslSocket::supportsSsl();
    return s_available;
}

bool SslSupport::isAvailable()
{
    if (!s_initialized)
        initialize();
    return s_available;
}

QString SslSupport::unavailableMessage()
{
    return QCoreApplication::translate(
        "SslSupport",
        "HTTPS/TLS 不可用：未找到 OpenSSL 运行库 (libssl-1_1-x64.dll / libcrypto-1_1-x64.dll)。\n\n"
        "请任选其一：\n"
        "1. 运行 scripts/setup_openssl.ps1 自动部署 OpenSSL\n"
        "2. 将上述 DLL 复制到程序目录 (%1)\n"
        "3. 在 Qt 维护工具中安装 OpenSSL 1.1 组件，并设置 OPENSSL_BIN 环境变量")
        .arg(QCoreApplication::applicationDirPath());
}
