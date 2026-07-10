#include "top3popupdialog.h"
#include "ui/top3listwidget.h"
#include "ui_top3popupdialog.h"

#include "../utils/app_theme.h"

#include <QPushButton>

Top3PopupDialog::Top3PopupDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Top3PopupDialog)
{
    ui->setupUi(this);
    AppTheme::styleDialog(this);
    setWindowTitle(tr("今日 Top 3"));
    setWindowFlags(windowFlags() | Qt::Tool | Qt::WindowStaysOnTopHint);
    connect(ui->btnClose, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->btnFocus25, &QPushButton::clicked, this, [this]() {
        const int row = ui->listTop3->currentRow();
        if (row < 0 && ui->listTop3->count() > 0)
            ui->listTop3->setCurrentRow(0);
        const qint64 taskId = ui->listTop3->taskIdAtRow(ui->listTop3->currentRow());
        if (taskId > 0)
            emit focusRequested(taskId);
    });
    connect(ui->listTop3, &Top3ListWidget::taskCompletedToggled, this,
            &Top3PopupDialog::taskCompletedToggled);
    connect(ui->listTop3, &Top3ListWidget::focusRequested, this, &Top3PopupDialog::focusRequested);
}

Top3PopupDialog::~Top3PopupDialog()
{
    delete ui;
}

void Top3PopupDialog::setRecommendations(const QVector<PriorityRecommendation> &items,
                                         const QHash<qint64, bool> &completedById)
{
    ui->listTop3->setRecommendations(items, completedById);
}
