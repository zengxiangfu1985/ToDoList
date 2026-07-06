#ifndef AIANALYSISTRACEDIALOG_H
#define AIANALYSISTRACEDIALOG_H

#include "../core/task_types.h"

#include <QDialog>

class AiAnalysisTraceDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AiAnalysisTraceDialog(const PriorityAnalysisResult &result, QWidget *parent = nullptr);
};

#endif // AIANALYSISTRACEDIALOG_H
