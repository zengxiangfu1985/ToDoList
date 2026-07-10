#include "quadrantboard.h"
#include "quadrantlistwidget.h"
#include "cyber_quadrant_box.h"

#include "../utils/app_theme.h"

#include <QGroupBox>
#include <QSizePolicy>
#include <QVBoxLayout>

static void applyQuadrantElevation(CyberQuadrantBox *box)
{
    AppTheme::applyElevation(box, 14, 2, 40);
}

QuadrantBoard::QuadrantBoard(QWidget *parent)
    : QWidget(parent)
    , m_q1(new QuadrantListWidget(EisenhowerQuadrant::Q1_UrgentImportant, this))
    , m_q2(new QuadrantListWidget(EisenhowerQuadrant::Q2_NotUrgentImportant, this))
    , m_q3(new QuadrantListWidget(EisenhowerQuadrant::Q3_UrgentNotImportant, this))
    , m_q4(new QuadrantListWidget(EisenhowerQuadrant::Q4_NotUrgentNotImportant, this))
{
    auto connectList = [this](QuadrantListWidget *list) {
        connect(list, &QuadrantListWidget::taskQuadrantChanged, this, &QuadrantBoard::taskQuadrantChanged);
        connect(list, &QuadrantListWidget::taskCompletedToggled, this, &QuadrantBoard::taskCompletedToggled);
    };
    connectList(m_q1);
    connectList(m_q2);
    connectList(m_q3);
    connectList(m_q4);
}

void QuadrantBoard::bindQuadrantContainers(QGroupBox *q1, QGroupBox *q2, QGroupBox *q3, QGroupBox *q4)
{
    const auto embed = [](QGroupBox *box, QuadrantListWidget *list) {
        auto *layout = new QVBoxLayout(box);
        layout->setContentsMargins(8, 6, 6, 6);
        layout->setSpacing(0);
        list->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        layout->addWidget(list, 1);
    };
    embed(q1, m_q1);
    embed(q2, m_q2);
    embed(q3, m_q3);
    embed(q4, m_q4);

    if (auto *box = qobject_cast<CyberQuadrantBox *>(q1)) {
        box->setQuadrant(EisenhowerQuadrant::Q1_UrgentImportant);
        applyQuadrantElevation(box);
    }
    if (auto *box = qobject_cast<CyberQuadrantBox *>(q2)) {
        box->setQuadrant(EisenhowerQuadrant::Q2_NotUrgentImportant);
        applyQuadrantElevation(box);
    }
    if (auto *box = qobject_cast<CyberQuadrantBox *>(q3)) {
        box->setQuadrant(EisenhowerQuadrant::Q3_UrgentNotImportant);
        applyQuadrantElevation(box);
    }
    if (auto *box = qobject_cast<CyberQuadrantBox *>(q4)) {
        box->setQuadrant(EisenhowerQuadrant::Q4_NotUrgentNotImportant);
        applyQuadrantElevation(box);
    }
}

void QuadrantBoard::setTasks(const QVector<TaskItem> &tasks)
{
    m_q1->setTaskItems(tasks);
    m_q2->setTaskItems(tasks);
    m_q3->setTaskItems(tasks);
    m_q4->setTaskItems(tasks);
}

void QuadrantBoard::removeTask(qint64 taskId)
{
    m_q1->removeTask(taskId);
    m_q2->removeTask(taskId);
    m_q3->removeTask(taskId);
    m_q4->removeTask(taskId);
}
