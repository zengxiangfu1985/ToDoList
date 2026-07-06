#ifndef LLM_JSON_UTILS_H
#define LLM_JSON_UTILS_H

#include <QByteArray>
#include <QJsonArray>
#include <QString>

namespace LlmJsonUtils {

QByteArray extractPayload(const QString &content);
QJsonArray parseArray(const QByteArray &jsonBytes, QString *errorMessage = nullptr);

} // namespace LlmJsonUtils

#endif // LLM_JSON_UTILS_H
