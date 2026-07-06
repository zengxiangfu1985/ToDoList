#include "top3popupdialog.h"
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
}

Top3PopupDialog::~Top3PopupDialog()
{
    delete ui;
}

void Top3PopupDialog::setRecommendations(const QVector<PriorityRecommendation> &items)
{
    ui->listTop3->clear();
    for (const PriorityRecommendation &rec : items) {
        const QString line = QStringLiteral("#%1 %2\n%3").arg(rec.rank).arg(rec.title, rec.reason);
        ui->listTop3->addItem(line);
    }
}
