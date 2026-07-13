#ifndef HABIT_REMINDER_POPUP_H
#define HABIT_REMINDER_POPUP_H

#include "../core/task_types.h"

#include <QDialog>

class QLabel;
class QPushButton;

class HabitReminderPopup : public QDialog
{
    Q_OBJECT
public:
    explicit HabitReminderPopup(QWidget *parent = nullptr);

    void showReminder(const HabitReminder &habit);

signals:
    void acknowledged(qint64 habitId);
    void snoozed(qint64 habitId);
    void disabledForToday(qint64 habitId);

protected:
    void showEvent(QShowEvent *event) override;

private:
    void positionBottomRight();

    QLabel *m_titleLabel = nullptr;
    QLabel *m_messageLabel = nullptr;
    qint64 m_habitId = 0;
};

#endif // HABIT_REMINDER_POPUP_H
