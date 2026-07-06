/********************************************************************************
** Form generated from reading UI file 'updatedialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UPDATEDIALOG_H
#define UI_UPDATEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_UpdateDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *labelStatus;
    QProgressBar *progressBar;
    QTextEdit *textReleaseNotes;
    QHBoxLayout *buttonRow;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnUpgrade;
    QPushButton *btnClose;

    void setupUi(QDialog *UpdateDialog)
    {
        if (UpdateDialog->objectName().isEmpty())
            UpdateDialog->setObjectName(QString::fromUtf8("UpdateDialog"));
        verticalLayout = new QVBoxLayout(UpdateDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        labelStatus = new QLabel(UpdateDialog);
        labelStatus->setObjectName(QString::fromUtf8("labelStatus"));
        labelStatus->setWordWrap(true);

        verticalLayout->addWidget(labelStatus);

        progressBar = new QProgressBar(UpdateDialog);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(0);

        verticalLayout->addWidget(progressBar);

        textReleaseNotes = new QTextEdit(UpdateDialog);
        textReleaseNotes->setObjectName(QString::fromUtf8("textReleaseNotes"));
        textReleaseNotes->setReadOnly(true);
        textReleaseNotes->setMaximumHeight(160);

        verticalLayout->addWidget(textReleaseNotes);

        buttonRow = new QHBoxLayout();
        buttonRow->setObjectName(QString::fromUtf8("buttonRow"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        buttonRow->addItem(horizontalSpacer);

        btnUpgrade = new QPushButton(UpdateDialog);
        btnUpgrade->setObjectName(QString::fromUtf8("btnUpgrade"));

        buttonRow->addWidget(btnUpgrade);

        btnClose = new QPushButton(UpdateDialog);
        btnClose->setObjectName(QString::fromUtf8("btnClose"));

        buttonRow->addWidget(btnClose);


        verticalLayout->addLayout(buttonRow);


        retranslateUi(UpdateDialog);

        QMetaObject::connectSlotsByName(UpdateDialog);
    } // setupUi

    void retranslateUi(QDialog *UpdateDialog)
    {
        UpdateDialog->setWindowTitle(QCoreApplication::translate("UpdateDialog", "\346\243\200\346\237\245\346\233\264\346\226\260", nullptr));
        labelStatus->setText(QCoreApplication::translate("UpdateDialog", "\346\255\243\345\234\250\346\243\200\346\237\245\346\233\264\346\226\260...", nullptr));
        btnUpgrade->setText(QCoreApplication::translate("UpdateDialog", "\347\253\213\345\215\263\345\215\207\347\272\247", nullptr));
        btnClose->setText(QCoreApplication::translate("UpdateDialog", "\345\205\263\351\227\255", nullptr));
    } // retranslateUi

};

namespace Ui {
    class UpdateDialog: public Ui_UpdateDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UPDATEDIALOG_H
