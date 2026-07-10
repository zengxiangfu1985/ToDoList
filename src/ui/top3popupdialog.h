#ifndef TOP3POPUPDIALOG_H
#define TOP3POPUPDIALOG_H

#include "../core/task_types.h"

#include <QDialog>
#include <QHash>
#include <QVector>
namespace Ui {
class Top3PopupDialog;
}

class Top3PopupDialog : public QDialog
{
    Q_OBJECT
public:
    explicit Top3PopupDialog(QWidget *parent = nullptr);
    ~Top3PopupDialog() override;

    void setRecommendations(const QVector<PriorityRecommendation> &items,
                            const QHash<qint64, bool> &completedById = {});

signals:
    void taskCompletedToggled(qint64 taskId, bool completed);
    void focusRequested(qint64 taskId);

private:
    Ui::Top3PopupDialog *ui;
};

#endif // TOP3POPUPDIALOG_H
