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
    connect(ui->listTop3, &Top3ListWidget::taskCompletedToggled, this,
            &Top3PopupDialog::taskCompletedToggled);
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
