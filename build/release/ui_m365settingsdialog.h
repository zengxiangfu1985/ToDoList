/********************************************************************************
** Form generated from reading UI file 'm365settingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_M365SETTINGSDIALOG_H
#define UI_M365SETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_M365SettingsDialog
{
public:
    QFormLayout *formLayout;
    QLabel *labelTenant;
    QLineEdit *editTenant;
    QLabel *labelClient;
    QLineEdit *editClientId;
    QLabel *labelStatusTitle;
    QLabel *labelAuthStatus;
    QHBoxLayout *loginLayout;
    QPushButton *btnLogin;
    QPushButton *btnPoll;
    QLabel *labelUrlTitle;
    QLabel *labelDeviceUrl;
    QLabel *labelCodeTitle;
    QHBoxLayout *codeLayout;
    QLabel *labelUserCode;
    QPushButton *btnCopyCode;
    QPushButton *btnOpenUrl;
    QPushButton *btnSync;

    void setupUi(QDialog *M365SettingsDialog)
    {
        if (M365SettingsDialog->objectName().isEmpty())
            M365SettingsDialog->setObjectName(QString::fromUtf8("M365SettingsDialog"));
        formLayout = new QFormLayout(M365SettingsDialog);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        labelTenant = new QLabel(M365SettingsDialog);
        labelTenant->setObjectName(QString::fromUtf8("labelTenant"));

        formLayout->setWidget(0, QFormLayout::LabelRole, labelTenant);

        editTenant = new QLineEdit(M365SettingsDialog);
        editTenant->setObjectName(QString::fromUtf8("editTenant"));

        formLayout->setWidget(0, QFormLayout::FieldRole, editTenant);

        labelClient = new QLabel(M365SettingsDialog);
        labelClient->setObjectName(QString::fromUtf8("labelClient"));

        formLayout->setWidget(1, QFormLayout::LabelRole, labelClient);

        editClientId = new QLineEdit(M365SettingsDialog);
        editClientId->setObjectName(QString::fromUtf8("editClientId"));

        formLayout->setWidget(1, QFormLayout::FieldRole, editClientId);

        labelStatusTitle = new QLabel(M365SettingsDialog);
        labelStatusTitle->setObjectName(QString::fromUtf8("labelStatusTitle"));

        formLayout->setWidget(2, QFormLayout::LabelRole, labelStatusTitle);

        labelAuthStatus = new QLabel(M365SettingsDialog);
        labelAuthStatus->setObjectName(QString::fromUtf8("labelAuthStatus"));

        formLayout->setWidget(2, QFormLayout::FieldRole, labelAuthStatus);

        loginLayout = new QHBoxLayout();
        loginLayout->setObjectName(QString::fromUtf8("loginLayout"));
        btnLogin = new QPushButton(M365SettingsDialog);
        btnLogin->setObjectName(QString::fromUtf8("btnLogin"));

        loginLayout->addWidget(btnLogin);

        btnPoll = new QPushButton(M365SettingsDialog);
        btnPoll->setObjectName(QString::fromUtf8("btnPoll"));

        loginLayout->addWidget(btnPoll);


        formLayout->setLayout(3, QFormLayout::SpanningRole, loginLayout);

        labelUrlTitle = new QLabel(M365SettingsDialog);
        labelUrlTitle->setObjectName(QString::fromUtf8("labelUrlTitle"));

        formLayout->setWidget(4, QFormLayout::LabelRole, labelUrlTitle);

        labelDeviceUrl = new QLabel(M365SettingsDialog);
        labelDeviceUrl->setObjectName(QString::fromUtf8("labelDeviceUrl"));
        labelDeviceUrl->setWordWrap(true);

        formLayout->setWidget(4, QFormLayout::FieldRole, labelDeviceUrl);

        labelCodeTitle = new QLabel(M365SettingsDialog);
        labelCodeTitle->setObjectName(QString::fromUtf8("labelCodeTitle"));

        formLayout->setWidget(5, QFormLayout::LabelRole, labelCodeTitle);

        codeLayout = new QHBoxLayout();
        codeLayout->setObjectName(QString::fromUtf8("codeLayout"));
        labelUserCode = new QLabel(M365SettingsDialog);
        labelUserCode->setObjectName(QString::fromUtf8("labelUserCode"));

        codeLayout->addWidget(labelUserCode);

        btnCopyCode = new QPushButton(M365SettingsDialog);
        btnCopyCode->setObjectName(QString::fromUtf8("btnCopyCode"));

        codeLayout->addWidget(btnCopyCode);

        btnOpenUrl = new QPushButton(M365SettingsDialog);
        btnOpenUrl->setObjectName(QString::fromUtf8("btnOpenUrl"));

        codeLayout->addWidget(btnOpenUrl);


        formLayout->setLayout(5, QFormLayout::FieldRole, codeLayout);

        btnSync = new QPushButton(M365SettingsDialog);
        btnSync->setObjectName(QString::fromUtf8("btnSync"));

        formLayout->setWidget(6, QFormLayout::SpanningRole, btnSync);


        retranslateUi(M365SettingsDialog);

        QMetaObject::connectSlotsByName(M365SettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *M365SettingsDialog)
    {
        M365SettingsDialog->setWindowTitle(QCoreApplication::translate("M365SettingsDialog", "Microsoft 365", nullptr));
        labelTenant->setText(QCoreApplication::translate("M365SettingsDialog", "Tenant ID", nullptr));
        editTenant->setPlaceholderText(QCoreApplication::translate("M365SettingsDialog", "common \346\210\226\347\273\204\347\273\207\347\247\237\346\210\267 GUID", nullptr));
        labelClient->setText(QCoreApplication::translate("M365SettingsDialog", "Client ID", nullptr));
        labelStatusTitle->setText(QCoreApplication::translate("M365SettingsDialog", "\347\212\266\346\200\201", nullptr));
        btnLogin->setText(QCoreApplication::translate("M365SettingsDialog", "\350\256\276\345\244\207\347\240\201\347\231\273\345\275\225", nullptr));
        btnPoll->setText(QCoreApplication::translate("M365SettingsDialog", "\347\241\256\350\256\244\345\267\262\346\216\210\346\235\203", nullptr));
        labelUrlTitle->setText(QCoreApplication::translate("M365SettingsDialog", "\351\252\214\350\257\201 URL", nullptr));
        labelCodeTitle->setText(QCoreApplication::translate("M365SettingsDialog", "\347\224\250\346\210\267\347\240\201", nullptr));
        btnCopyCode->setText(QCoreApplication::translate("M365SettingsDialog", "\345\244\215\345\210\266", nullptr));
        btnOpenUrl->setText(QCoreApplication::translate("M365SettingsDialog", "\346\211\223\345\274\200\346\265\217\350\247\210\345\231\250", nullptr));
        btnSync->setText(QCoreApplication::translate("M365SettingsDialog", "\345\220\214\346\255\245 Outlook \346\227\227\346\240\207\351\202\256\344\273\266\344\270\272\344\273\273\345\212\241", nullptr));
    } // retranslateUi

};

namespace Ui {
    class M365SettingsDialog: public Ui_M365SettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_M365SETTINGSDIALOG_H
