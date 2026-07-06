/********************************************************************************
** Form generated from reading UI file 'top3popupdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TOP3POPUPDIALOG_H
#define UI_TOP3POPUPDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Top3PopupDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *labelHint;
    QListWidget *listTop3;
    QPushButton *btnClose;

    void setupUi(QDialog *Top3PopupDialog)
    {
        if (Top3PopupDialog->objectName().isEmpty())
            Top3PopupDialog->setObjectName(QString::fromUtf8("Top3PopupDialog"));
        Top3PopupDialog->setMinimumSize(QSize(420, 280));
        verticalLayout = new QVBoxLayout(Top3PopupDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        labelHint = new QLabel(Top3PopupDialog);
        labelHint->setObjectName(QString::fromUtf8("labelHint"));

        verticalLayout->addWidget(labelHint);

        listTop3 = new QListWidget(Top3PopupDialog);
        listTop3->setObjectName(QString::fromUtf8("listTop3"));

        verticalLayout->addWidget(listTop3);

        btnClose = new QPushButton(Top3PopupDialog);
        btnClose->setObjectName(QString::fromUtf8("btnClose"));

        verticalLayout->addWidget(btnClose);


        retranslateUi(Top3PopupDialog);

        QMetaObject::connectSlotsByName(Top3PopupDialog);
    } // setupUi

    void retranslateUi(QDialog *Top3PopupDialog)
    {
        Top3PopupDialog->setWindowTitle(QCoreApplication::translate("Top3PopupDialog", "\344\273\212\346\227\245 Top 3", nullptr));
        labelHint->setText(QCoreApplication::translate("Top3PopupDialog", "AI \346\216\250\350\215\220\344\273\212\346\227\245\344\274\230\345\205\210\345\256\214\346\210\220\347\232\204 3 \351\241\271\344\273\273\345\212\241\357\274\232", nullptr));
        btnClose->setText(QCoreApplication::translate("Top3PopupDialog", "\345\205\263\351\227\255", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Top3PopupDialog: public Ui_Top3PopupDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TOP3POPUPDIALOG_H
