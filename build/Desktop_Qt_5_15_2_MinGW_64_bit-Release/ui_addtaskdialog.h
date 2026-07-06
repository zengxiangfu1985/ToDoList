/********************************************************************************
** Form generated from reading UI file 'addtaskdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDTASKDIALOG_H
#define UI_ADDTASKDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_AddTaskDialog
{
public:
    QFormLayout *formLayout;
    QLabel *labelTitle;
    QLineEdit *editTitle;
    QLabel *labelDue;
    QDateEdit *dateDue;
    QLabel *labelQuadrant;
    QComboBox *comboQuadrant;
    QHBoxLayout *buttonLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnOk;
    QPushButton *btnCancel;

    void setupUi(QDialog *AddTaskDialog)
    {
        if (AddTaskDialog->objectName().isEmpty())
            AddTaskDialog->setObjectName(QString::fromUtf8("AddTaskDialog"));
        formLayout = new QFormLayout(AddTaskDialog);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        labelTitle = new QLabel(AddTaskDialog);
        labelTitle->setObjectName(QString::fromUtf8("labelTitle"));

        formLayout->setWidget(0, QFormLayout::LabelRole, labelTitle);

        editTitle = new QLineEdit(AddTaskDialog);
        editTitle->setObjectName(QString::fromUtf8("editTitle"));

        formLayout->setWidget(0, QFormLayout::FieldRole, editTitle);

        labelDue = new QLabel(AddTaskDialog);
        labelDue->setObjectName(QString::fromUtf8("labelDue"));

        formLayout->setWidget(1, QFormLayout::LabelRole, labelDue);

        dateDue = new QDateEdit(AddTaskDialog);
        dateDue->setObjectName(QString::fromUtf8("dateDue"));

        formLayout->setWidget(1, QFormLayout::FieldRole, dateDue);

        labelQuadrant = new QLabel(AddTaskDialog);
        labelQuadrant->setObjectName(QString::fromUtf8("labelQuadrant"));

        formLayout->setWidget(2, QFormLayout::LabelRole, labelQuadrant);

        comboQuadrant = new QComboBox(AddTaskDialog);
        comboQuadrant->setObjectName(QString::fromUtf8("comboQuadrant"));

        formLayout->setWidget(2, QFormLayout::FieldRole, comboQuadrant);

        buttonLayout = new QHBoxLayout();
        buttonLayout->setObjectName(QString::fromUtf8("buttonLayout"));
        horizontalSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        buttonLayout->addItem(horizontalSpacer);

        btnOk = new QPushButton(AddTaskDialog);
        btnOk->setObjectName(QString::fromUtf8("btnOk"));

        buttonLayout->addWidget(btnOk);

        btnCancel = new QPushButton(AddTaskDialog);
        btnCancel->setObjectName(QString::fromUtf8("btnCancel"));

        buttonLayout->addWidget(btnCancel);


        formLayout->setLayout(3, QFormLayout::SpanningRole, buttonLayout);


        retranslateUi(AddTaskDialog);

        QMetaObject::connectSlotsByName(AddTaskDialog);
    } // setupUi

    void retranslateUi(QDialog *AddTaskDialog)
    {
        AddTaskDialog->setWindowTitle(QCoreApplication::translate("AddTaskDialog", "\346\267\273\345\212\240\344\273\273\345\212\241", nullptr));
        labelTitle->setText(QCoreApplication::translate("AddTaskDialog", "\346\240\207\351\242\230", nullptr));
        labelDue->setText(QCoreApplication::translate("AddTaskDialog", "\346\210\252\346\255\242\346\227\245\346\234\237", nullptr));
        labelQuadrant->setText(QCoreApplication::translate("AddTaskDialog", "\350\261\241\351\231\220", nullptr));
        btnOk->setText(QCoreApplication::translate("AddTaskDialog", "\347\241\256\345\256\232", nullptr));
        btnCancel->setText(QCoreApplication::translate("AddTaskDialog", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AddTaskDialog: public Ui_AddTaskDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDTASKDIALOG_H
