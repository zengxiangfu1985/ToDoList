/********************************************************************************
** Form generated from reading UI file 'llmsettingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LLMSETTINGSDIALOG_H
#define UI_LLMSETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_LlmSettingsDialog
{
public:
    QFormLayout *formLayout;
    QLabel *labelProvider;
    QComboBox *comboProvider;
    QLabel *labelSavedModels;
    QComboBox *comboSavedModels;
    QLabel *labelBaseUrl;
    QLineEdit *editBaseUrl;
    QLabel *labelApiKey;
    QLineEdit *editApiKey;
    QLabel *labelModel;
    QLineEdit *editModel;
    QHBoxLayout *buttonLayout;
    QPushButton *btnTestConnection;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnSave;
    QPushButton *btnCancel;

    void setupUi(QDialog *LlmSettingsDialog)
    {
        if (LlmSettingsDialog->objectName().isEmpty())
            LlmSettingsDialog->setObjectName(QString::fromUtf8("LlmSettingsDialog"));
        formLayout = new QFormLayout(LlmSettingsDialog);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        labelProvider = new QLabel(LlmSettingsDialog);
        labelProvider->setObjectName(QString::fromUtf8("labelProvider"));

        formLayout->setWidget(0, QFormLayout::LabelRole, labelProvider);

        comboProvider = new QComboBox(LlmSettingsDialog);
        comboProvider->setObjectName(QString::fromUtf8("comboProvider"));

        formLayout->setWidget(0, QFormLayout::FieldRole, comboProvider);

        labelSavedModels = new QLabel(LlmSettingsDialog);
        labelSavedModels->setObjectName(QString::fromUtf8("labelSavedModels"));

        formLayout->setWidget(1, QFormLayout::LabelRole, labelSavedModels);

        comboSavedModels = new QComboBox(LlmSettingsDialog);
        comboSavedModels->setObjectName(QString::fromUtf8("comboSavedModels"));

        formLayout->setWidget(1, QFormLayout::FieldRole, comboSavedModels);

        labelBaseUrl = new QLabel(LlmSettingsDialog);
        labelBaseUrl->setObjectName(QString::fromUtf8("labelBaseUrl"));

        formLayout->setWidget(2, QFormLayout::LabelRole, labelBaseUrl);

        editBaseUrl = new QLineEdit(LlmSettingsDialog);
        editBaseUrl->setObjectName(QString::fromUtf8("editBaseUrl"));

        formLayout->setWidget(2, QFormLayout::FieldRole, editBaseUrl);

        labelApiKey = new QLabel(LlmSettingsDialog);
        labelApiKey->setObjectName(QString::fromUtf8("labelApiKey"));

        formLayout->setWidget(3, QFormLayout::LabelRole, labelApiKey);

        editApiKey = new QLineEdit(LlmSettingsDialog);
        editApiKey->setObjectName(QString::fromUtf8("editApiKey"));
        editApiKey->setEchoMode(QLineEdit::Password);

        formLayout->setWidget(3, QFormLayout::FieldRole, editApiKey);

        labelModel = new QLabel(LlmSettingsDialog);
        labelModel->setObjectName(QString::fromUtf8("labelModel"));

        formLayout->setWidget(4, QFormLayout::LabelRole, labelModel);

        editModel = new QLineEdit(LlmSettingsDialog);
        editModel->setObjectName(QString::fromUtf8("editModel"));

        formLayout->setWidget(4, QFormLayout::FieldRole, editModel);

        buttonLayout = new QHBoxLayout();
        buttonLayout->setObjectName(QString::fromUtf8("buttonLayout"));
        btnTestConnection = new QPushButton(LlmSettingsDialog);
        btnTestConnection->setObjectName(QString::fromUtf8("btnTestConnection"));

        buttonLayout->addWidget(btnTestConnection);

        horizontalSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        buttonLayout->addItem(horizontalSpacer);

        btnSave = new QPushButton(LlmSettingsDialog);
        btnSave->setObjectName(QString::fromUtf8("btnSave"));

        buttonLayout->addWidget(btnSave);

        btnCancel = new QPushButton(LlmSettingsDialog);
        btnCancel->setObjectName(QString::fromUtf8("btnCancel"));

        buttonLayout->addWidget(btnCancel);


        formLayout->setLayout(5, QFormLayout::SpanningRole, buttonLayout);


        retranslateUi(LlmSettingsDialog);

        QMetaObject::connectSlotsByName(LlmSettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *LlmSettingsDialog)
    {
        LlmSettingsDialog->setWindowTitle(QCoreApplication::translate("LlmSettingsDialog", "LLM \350\256\276\347\275\256", nullptr));
        labelProvider->setText(QCoreApplication::translate("LlmSettingsDialog", "\346\217\220\344\276\233\345\225\206", nullptr));
        labelSavedModels->setText(QCoreApplication::translate("LlmSettingsDialog", "\345\267\262\344\277\235\345\255\230\346\250\241\345\236\213", nullptr));
#if QT_CONFIG(tooltip)
        comboSavedModels->setToolTip(QCoreApplication::translate("LlmSettingsDialog", "\345\210\207\346\215\242\345\275\223\345\211\215\346\217\220\344\276\233\345\225\206\344\270\213\345\267\262\344\277\235\345\255\230\347\232\204\346\250\241\345\236\213\351\205\215\347\275\256", nullptr));
#endif // QT_CONFIG(tooltip)
        labelBaseUrl->setText(QCoreApplication::translate("LlmSettingsDialog", "Base URL", nullptr));
        labelApiKey->setText(QCoreApplication::translate("LlmSettingsDialog", "API Key", nullptr));
        labelModel->setText(QCoreApplication::translate("LlmSettingsDialog", "\346\250\241\345\236\213", nullptr));
        btnTestConnection->setText(QCoreApplication::translate("LlmSettingsDialog", "\346\265\213\350\257\225\350\277\236\346\216\245", nullptr));
        btnSave->setText(QCoreApplication::translate("LlmSettingsDialog", "\344\277\235\345\255\230", nullptr));
        btnCancel->setText(QCoreApplication::translate("LlmSettingsDialog", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LlmSettingsDialog: public Ui_LlmSettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LLMSETTINGSDIALOG_H
