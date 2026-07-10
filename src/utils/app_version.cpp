#include "app_version.h"

#include <QLocale>
#include <QDate>
#include <QRegularExpression>
#include <QVector>

namespace {

constexpr const char *kVersion = "1.0.9";
constexpr int kBuildNumber = 2026071009;
constexpr int kVersionParts = 3;

QVector<int> parseVersionParts(const QString &version)
{
    static const QRegularExpression re(QStringLiteral("(\\d+)"));
    QVector<int> parts;
    parts.reserve(kVersionParts);
    auto it = re.globalMatch(version.trimmed());
    while (it.hasNext() && parts.size() < kVersionParts) {
        const QRegularExpressionMatch match = it.next();
        parts.append(match.captured(1).toInt());
    }
    while (parts.size() < kVersionParts)
        parts.append(0);
    return parts;
}

} // namespace

QString AppVersion::versionString()
{
    return QString::fromLatin1(kVersion);
}

int AppVersion::buildNumber()
{
    return kBuildNumber;
}

QString AppVersion::displayString()
{
    return QStringLiteral("%1 (build %2)").arg(versionString()).arg(buildNumber());
}

QString AppVersion::buildDate()
{
    const QString raw = QString::fromLatin1(__DATE__);
    const QDate date = QLocale(QLocale::English).toDate(raw.trimmed(), QStringLiteral("MMM d yyyy"));
    if (date.isValid())
        return date.toString(QStringLiteral("yyyyMMdd"));
    return raw;
}

int AppVersion::compareVersion(const QString &left, const QString &right)
{
    const QVector<int> l = parseVersionParts(left);
    const QVector<int> r = parseVersionParts(right);
    for (int i = 0; i < kVersionParts; ++i) {
        if (l[i] < r[i])
            return -1;
        if (l[i] > r[i])
            return 1;
    }
    return 0;
}

bool AppVersion::isNewerThanCurrent(const QString &version, int build)
{
    const int cmp = compareVersion(version, versionString());
    if (cmp > 0)
        return true;
    if (cmp < 0)
        return false;
    return build > buildNumber();
}

bool AppVersion::isOlderThanCurrent(const QString &version, int build)
{
    const int cmp = compareVersion(version, versionString());
    if (cmp < 0)
        return true;
    if (cmp > 0)
        return false;
    return build < buildNumber();
}
