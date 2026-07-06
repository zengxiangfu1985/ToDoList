#include "llm_json_utils.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>

namespace LlmJsonUtils {

QByteArray extractPayload(const QString &content)
{
    QString text = content.trimmed();
    if (text.isEmpty())
        return {};

    static const QRegularExpression fenceRe(
        QStringLiteral("```(?:json)?\\s*([\\s\\S]*?)```"),
        QRegularExpression::CaseInsensitiveOption);
    const QRegularExpressionMatch fenceMatch = fenceRe.match(text);
    if (fenceMatch.hasMatch())
        text = fenceMatch.captured(1).trimmed();

    QJsonParseError err;
    QJsonDocument::fromJson(text.toUtf8(), &err);
    if (err.error == QJsonParseError::NoError)
        return text.toUtf8();

    int start = -1;
    for (int i = 0; i < text.size(); ++i) {
        const QChar ch = text.at(i);
        if (ch == QLatin1Char('[') || ch == QLatin1Char('{')) {
            start = i;
            break;
        }
    }
    if (start < 0)
        return text.toUtf8();

    const QChar open = text.at(start);
    const QChar close = (open == QLatin1Char('[')) ? QLatin1Char(']') : QLatin1Char('}');
    int depth = 0;
    int end = -1;
    for (int i = start; i < text.size(); ++i) {
        const QChar ch = text.at(i);
        if (ch == open)
            ++depth;
        else if (ch == close) {
            --depth;
            if (depth == 0) {
                end = i;
                break;
            }
        }
    }
    if (end >= start)
        return text.mid(start, end - start + 1).toUtf8();
    return text.toUtf8();
}

QJsonArray parseArray(const QByteArray &jsonBytes, QString *errorMessage)
{
    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(jsonBytes, &err);
    if (err.error != QJsonParseError::NoError) {
        if (errorMessage)
            *errorMessage = err.errorString();
        return {};
    }
    if (doc.isArray())
        return doc.array();
    if (doc.isObject()) {
        const QJsonArray tasks = doc.object().value(QStringLiteral("tasks")).toArray();
        if (!tasks.isEmpty())
            return tasks;
        const QJsonArray results = doc.object().value(QStringLiteral("results")).toArray();
        if (!results.isEmpty())
            return results;
    }
    if (errorMessage)
        *errorMessage = QStringLiteral("JSON 不是数组或 tasks/results 字段");
    return {};
}

} // namespace LlmJsonUtils
