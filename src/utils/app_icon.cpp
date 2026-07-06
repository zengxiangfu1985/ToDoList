#include "app_icon.h"

#include "app_logger.h"

#include <QApplication>
#include <QColor>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QPixmap>
#include <QWidget>

#ifdef Q_OS_WIN
#include <QtWin>
#include <windows.h>
#endif

namespace {

constexpr int kIconSizes[] = {16, 20, 24, 32, 40, 48, 64, 96, 128, 256};

QIcon iconFromMasterPixmap(const QPixmap &master)
{
    QIcon icon;
    if (master.isNull())
        return icon;

    for (int size : kIconSizes) {
        icon.addPixmap(master.scaled(size, size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation),
                       QIcon::Normal, QIcon::Off);
        icon.addPixmap(master.scaled(size, size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation),
                       QIcon::Normal, QIcon::On);
    }
    return icon;
}

QIcon fallbackIcon()
{
    QPixmap pm(256, 256);
    pm.fill(QColor(0x25, 0x63, 0xeb));
    return iconFromMasterPixmap(pm);
}

QPixmap loadMasterPixmap(const QString &path, QString *loadedFrom)
{
    if (path.isEmpty() || !QFile::exists(path))
        return {};

    QPixmap pm(path);
    if (pm.isNull())
        return {};

    if (loadedFrom)
        *loadedFrom = path;
    return pm;
}

} // namespace

QIcon loadAppIcon()
{
    QString loadedFrom;
    const QString appDir = QCoreApplication::applicationDirPath();

    const QStringList pngCandidates = {
        QDir(appDir).filePath(QStringLiteral("app.png")),
        QDir(appDir).filePath(QStringLiteral("icons/app.png")),
        QStringLiteral(":/icons/app.png"),
    };

    for (const QString &path : pngCandidates) {
        const QPixmap master = loadMasterPixmap(path, &loadedFrom);
        if (!master.isNull()) {
            AppLogger::info("APP",
                            QStringLiteral("加载应用图标: %1 (%2x%3)")
                                .arg(loadedFrom)
                                .arg(master.width())
                                .arg(master.height()));
            return iconFromMasterPixmap(master);
        }
    }

    const QStringList icoCandidates = {
        QDir(appDir).filePath(QStringLiteral("app.ico")),
        QDir(appDir).filePath(QStringLiteral("icons/app.ico")),
        QStringLiteral(":/icons/app.ico"),
    };

    for (const QString &path : icoCandidates) {
        if (!QFile::exists(path))
            continue;
        const QIcon ico(path);
        if (!ico.isNull()) {
            AppLogger::info("APP", QStringLiteral("加载应用图标: %1").arg(path));
            return ico;
        }
    }

    AppLogger::warn("APP", QStringLiteral("未找到应用图标文件，使用占位图标"));
    return fallbackIcon();
}

void applyWindowIcon(QWidget *widget, const QIcon &icon)
{
    const QIcon effective = icon.isNull() ? loadAppIcon() : icon;
    if (effective.isNull() || !widget)
        return;

    QApplication::setWindowIcon(effective);
    widget->setWindowIcon(effective);

#ifdef Q_OS_WIN
    widget->ensurePolished();
    const WId wid = widget->winId();
    if (wid) {
        const HWND hwnd = reinterpret_cast<HWND>(wid);
        const auto setNativeIcon = [&](int iconType, int size) {
            const HICON hIcon = QtWin::toHICON(effective.pixmap(size, size));
            if (hIcon) {
                SendMessageW(hwnd, WM_SETICON, static_cast<WPARAM>(iconType),
                             reinterpret_cast<LPARAM>(hIcon));
            }
        };
        setNativeIcon(ICON_BIG, 32);
        setNativeIcon(ICON_SMALL, 16);
    }
#endif
}
