#include "task_types.h"

#include <QtMath>

bool M365Config::isConfigured() const
{
    return !clientId.trimmed().isEmpty() && !accessToken.trimmed().isEmpty();
}

void ScoringWeights::normalize()
{
    const double sum = urgency + importance + business + effort;
    if (sum <= 0.0)
        return;
    urgency /= sum;
    importance /= sum;
    business /= sum;
    effort /= sum;
}
