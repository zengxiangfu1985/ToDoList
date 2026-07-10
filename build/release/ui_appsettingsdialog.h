/********************************************************************************
** Form generated from reading UI file 'appsettingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_APPSETTINGSDIALOG_H
#define UI_APPSETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QKeySequenceEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AppSettingsDialog
{
public:
    QVBoxLayout *verticalLayout;
    QScrollArea *scrollArea;
    QWidget *scrollContent;
    QVBoxLayout *scrollContentLayout;
    QGroupBox *groupSecurity;
    QFormLayout *formSecurity;
    QLabel *labelCurrentPassword;
    QLineEdit *editCurrentPassword;
    QLabel *labelNewPassword;
    QLineEdit *editNewPassword;
    QLabel *labelConfirmPassword;
    QLineEdit *editConfirmPassword;
    QLabel *labelLockDelay;
    QHBoxLayout *layoutLockDelay;
    QSpinBox *spinLockAfterSystemLock;
    QLabel *labelLockDelayHint;
    QLabel *labelIdleLock;
    QHBoxLayout *layoutIdleLock;
    QSpinBox *spinLockOnIdle;
    QLabel *labelIdleHint;
    QLabel *labelLockOnStartup;
    QCheckBox *checkLockOnStartup;
    QLabel *labelMinimizeToTray;
    QCheckBox *checkMinimizeToTray;
    QGroupBox *groupHotkeys;
    QFormLayout *formHotkeys;
    QLabel *labelTodayTasksHotkey;
    QKeySequenceEdit *editTodayTasksHotkey;
    QLabel *labelTop3Hotkey;
    QKeySequenceEdit *editTop3Hotkey;
    QLabel *labelQuickCaptureHotkey;
    QKeySequenceEdit *editQuickCaptureHotkey;
    QCheckBox *checkQuickCaptureAutoAnalyze;
    QLabel *labelHotkeyHint;
    QGroupBox *groupFocus;
    QFormLayout *formFocus;
    QLabel *labelFocusDuration;
    QComboBox *comboFocusDuration;
    QCheckBox *checkFocusTrayCountdown;
    QGroupBox *groupTools;
    QVBoxLayout *toolsLayout;
    QPushButton *btnViewAiTrace;
    QPushButton *btnM365;
    QGroupBox *groupAppearance;
    QFormLayout *formAppearance;
    QLabel *labelLanguage;
    QComboBox *comboLanguage;
    QGroupBox *groupPrivacy;
    QVBoxLayout *privacyLayout;
    QCheckBox *checkUsageStatistics;
    QLabel *labelUsageStatisticsHint;
    QHBoxLayout *buttonRow;
    QPushButton *btnLockNow;
    QPushButton *btnClearPassword;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnSave;
    QPushButton *btnCancel;

    void setupUi(QDialog *AppSettingsDialog)
    {
        if (AppSettingsDialog->objectName().isEmpty())
            AppSettingsDialog->setObjectName(QString::fromUtf8("AppSettingsDialog"));
        verticalLayout = new QVBoxLayout(AppSettingsDialog);
        verticalLayout->setSpacing(8);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        scrollArea = new QScrollArea(AppSettingsDialog);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setWidgetResizable(true);
        scrollContent = new QWidget();
        scrollContent->setObjectName(QString::fromUtf8("scrollContent"));
        scrollContentLayout = new QVBoxLayout(scrollContent);
        scrollContentLayout->setObjectName(QString::fromUtf8("scrollContentLayout"));
        scrollContentLayout->setContentsMargins(0, 0, 0, 0);
        groupSecurity = new QGroupBox(scrollContent);
        groupSecurity->setObjectName(QString::fromUtf8("groupSecurity"));
        formSecurity = new QFormLayout(groupSecurity);
        formSecurity->setObjectName(QString::fromUtf8("formSecurity"));
        labelCurrentPassword = new QLabel(groupSecurity);
        labelCurrentPassword->setObjectName(QString::fromUtf8("labelCurrentPassword"));

        formSecurity->setWidget(0, QFormLayout::LabelRole, labelCurrentPassword);

        editCurrentPassword = new QLineEdit(groupSecurity);
        editCurrentPassword->setObjectName(QString::fromUtf8("editCurrentPassword"));
        editCurrentPassword->setEchoMode(QLineEdit::Password);

        formSecurity->setWidget(0, QFormLayout::FieldRole, editCurrentPassword);

        labelNewPassword = new QLabel(groupSecurity);
        labelNewPassword->setObjectName(QString::fromUtf8("labelNewPassword"));

        formSecurity->setWidget(1, QFormLayout::LabelRole, labelNewPassword);

        editNewPassword = new QLineEdit(groupSecurity);
        editNewPassword->setObjectName(QString::fromUtf8("editNewPassword"));
        editNewPassword->setEchoMode(QLineEdit::Password);

        formSecurity->setWidget(1, QFormLayout::FieldRole, editNewPassword);

        labelConfirmPassword = new QLabel(groupSecurity);
        labelConfirmPassword->setObjectName(QString::fromUtf8("labelConfirmPassword"));

        formSecurity->setWidget(2, QFormLayout::LabelRole, labelConfirmPassword);

        editConfirmPassword = new QLineEdit(groupSecurity);
        editConfirmPassword->setObjectName(QString::fromUtf8("editConfirmPassword"));
        editConfirmPassword->setEchoMode(QLineEdit::Password);

        formSecurity->setWidget(2, QFormLayout::FieldRole, editConfirmPassword);

        labelLockDelay = new QLabel(groupSecurity);
        labelLockDelay->setObjectName(QString::fromUtf8("labelLockDelay"));

        formSecurity->setWidget(3, QFormLayout::LabelRole, labelLockDelay);

        layoutLockDelay = new QHBoxLayout();
        layoutLockDelay->setObjectName(QString::fromUtf8("layoutLockDelay"));
        spinLockAfterSystemLock = new QSpinBox(groupSecurity);
        spinLockAfterSystemLock->setObjectName(QString::fromUtf8("spinLockAfterSystemLock"));
        spinLockAfterSystemLock->setMaximum(120);

        layoutLockDelay->addWidget(spinLockAfterSystemLock);

        labelLockDelayHint = new QLabel(groupSecurity);
        labelLockDelayHint->setObjectName(QString::fromUtf8("labelLockDelayHint"));

        layoutLockDelay->addWidget(labelLockDelayHint);


        formSecurity->setLayout(3, QFormLayout::FieldRole, layoutLockDelay);

        labelIdleLock = new QLabel(groupSecurity);
        labelIdleLock->setObjectName(QString::fromUtf8("labelIdleLock"));

        formSecurity->setWidget(4, QFormLayout::LabelRole, labelIdleLock);

        layoutIdleLock = new QHBoxLayout();
        layoutIdleLock->setObjectName(QString::fromUtf8("layoutIdleLock"));
        spinLockOnIdle = new QSpinBox(groupSecurity);
        spinLockOnIdle->setObjectName(QString::fromUtf8("spinLockOnIdle"));
        spinLockOnIdle->setMaximum(480);

        layoutIdleLock->addWidget(spinLockOnIdle);

        labelIdleHint = new QLabel(groupSecurity);
        labelIdleHint->setObjectName(QString::fromUtf8("labelIdleHint"));

        layoutIdleLock->addWidget(labelIdleHint);


        formSecurity->setLayout(4, QFormLayout::FieldRole, layoutIdleLock);

        labelLockOnStartup = new QLabel(groupSecurity);
        labelLockOnStartup->setObjectName(QString::fromUtf8("labelLockOnStartup"));

        formSecurity->setWidget(5, QFormLayout::LabelRole, labelLockOnStartup);

        checkLockOnStartup = new QCheckBox(groupSecurity);
        checkLockOnStartup->setObjectName(QString::fromUtf8("checkLockOnStartup"));

        formSecurity->setWidget(5, QFormLayout::FieldRole, checkLockOnStartup);

        labelMinimizeToTray = new QLabel(groupSecurity);
        labelMinimizeToTray->setObjectName(QString::fromUtf8("labelMinimizeToTray"));

        formSecurity->setWidget(6, QFormLayout::LabelRole, labelMinimizeToTray);

        checkMinimizeToTray = new QCheckBox(groupSecurity);
        checkMinimizeToTray->setObjectName(QString::fromUtf8("checkMinimizeToTray"));
        checkMinimizeToTray->setChecked(true);

        formSecurity->setWidget(6, QFormLayout::FieldRole, checkMinimizeToTray);


        scrollContentLayout->addWidget(groupSecurity);

        groupHotkeys = new QGroupBox(scrollContent);
        groupHotkeys->setObjectName(QString::fromUtf8("groupHotkeys"));
        formHotkeys = new QFormLayout(groupHotkeys);
        formHotkeys->setObjectName(QString::fromUtf8("formHotkeys"));
        labelTodayTasksHotkey = new QLabel(groupHotkeys);
        labelTodayTasksHotkey->setObjectName(QString::fromUtf8("labelTodayTasksHotkey"));

        formHotkeys->setWidget(0, QFormLayout::LabelRole, labelTodayTasksHotkey);

        editTodayTasksHotkey = new QKeySequenceEdit(groupHotkeys);
        editTodayTasksHotkey->setObjectName(QString::fromUtf8("editTodayTasksHotkey"));

        formHotkeys->setWidget(0, QFormLayout::FieldRole, editTodayTasksHotkey);

        labelTop3Hotkey = new QLabel(groupHotkeys);
        labelTop3Hotkey->setObjectName(QString::fromUtf8("labelTop3Hotkey"));

        formHotkeys->setWidget(1, QFormLayout::LabelRole, labelTop3Hotkey);

        editTop3Hotkey = new QKeySequenceEdit(groupHotkeys);
        editTop3Hotkey->setObjectName(QString::fromUtf8("editTop3Hotkey"));

        formHotkeys->setWidget(1, QFormLayout::FieldRole, editTop3Hotkey);

        labelQuickCaptureHotkey = new QLabel(groupHotkeys);
        labelQuickCaptureHotkey->setObjectName(QString::fromUtf8("labelQuickCaptureHotkey"));

        formHotkeys->setWidget(2, QFormLayout::LabelRole, labelQuickCaptureHotkey);

        editQuickCaptureHotkey = new QKeySequenceEdit(groupHotkeys);
        editQuickCaptureHotkey->setObjectName(QString::fromUtf8("editQuickCaptureHotkey"));

        formHotkeys->setWidget(2, QFormLayout::FieldRole, editQuickCaptureHotkey);

        checkQuickCaptureAutoAnalyze = new QCheckBox(groupHotkeys);
        checkQuickCaptureAutoAnalyze->setObjectName(QString::fromUtf8("checkQuickCaptureAutoAnalyze"));
        checkQuickCaptureAutoAnalyze->setChecked(true);

        formHotkeys->setWidget(3, QFormLayout::SpanningRole, checkQuickCaptureAutoAnalyze);

        labelHotkeyHint = new QLabel(groupHotkeys);
        labelHotkeyHint->setObjectName(QString::fromUtf8("labelHotkeyHint"));
        labelHotkeyHint->setWordWrap(true);

        formHotkeys->setWidget(4, QFormLayout::SpanningRole, labelHotkeyHint);


        scrollContentLayout->addWidget(groupHotkeys);

        groupFocus = new QGroupBox(scrollContent);
        groupFocus->setObjectName(QString::fromUtf8("groupFocus"));
        formFocus = new QFormLayout(groupFocus);
        formFocus->setObjectName(QString::fromUtf8("formFocus"));
        labelFocusDuration = new QLabel(groupFocus);
        labelFocusDuration->setObjectName(QString::fromUtf8("labelFocusDuration"));

        formFocus->setWidget(0, QFormLayout::LabelRole, labelFocusDuration);

        comboFocusDuration = new QComboBox(groupFocus);
        comboFocusDuration->setObjectName(QString::fromUtf8("comboFocusDuration"));

        formFocus->setWidget(0, QFormLayout::FieldRole, comboFocusDuration);

        checkFocusTrayCountdown = new QCheckBox(groupFocus);
        checkFocusTrayCountdown->setObjectName(QString::fromUtf8("checkFocusTrayCountdown"));
        checkFocusTrayCountdown->setChecked(true);

        formFocus->setWidget(1, QFormLayout::SpanningRole, checkFocusTrayCountdown);


        scrollContentLayout->addWidget(groupFocus);

        groupTools = new QGroupBox(scrollContent);
        groupTools->setObjectName(QString::fromUtf8("groupTools"));
        toolsLayout = new QVBoxLayout(groupTools);
        toolsLayout->setObjectName(QString::fromUtf8("toolsLayout"));
        btnViewAiTrace = new QPushButton(groupTools);
        btnViewAiTrace->setObjectName(QString::fromUtf8("btnViewAiTrace"));

        toolsLayout->addWidget(btnViewAiTrace);

        btnM365 = new QPushButton(groupTools);
        btnM365->setObjectName(QString::fromUtf8("btnM365"));

        toolsLayout->addWidget(btnM365);


        scrollContentLayout->addWidget(groupTools);

        groupAppearance = new QGroupBox(scrollContent);
        groupAppearance->setObjectName(QString::fromUtf8("groupAppearance"));
        formAppearance = new QFormLayout(groupAppearance);
        formAppearance->setObjectName(QString::fromUtf8("formAppearance"));
        labelLanguage = new QLabel(groupAppearance);
        labelLanguage->setObjectName(QString::fromUtf8("labelLanguage"));

        formAppearance->setWidget(0, QFormLayout::LabelRole, labelLanguage);

        comboLanguage = new QComboBox(groupAppearance);
        comboLanguage->setObjectName(QString::fromUtf8("comboLanguage"));

        formAppearance->setWidget(0, QFormLayout::FieldRole, comboLanguage);


        scrollContentLayout->addWidget(groupAppearance);

        groupPrivacy = new QGroupBox(scrollContent);
        groupPrivacy->setObjectName(QString::fromUtf8("groupPrivacy"));
        privacyLayout = new QVBoxLayout(groupPrivacy);
        privacyLayout->setObjectName(QString::fromUtf8("privacyLayout"));
        checkUsageStatistics = new QCheckBox(groupPrivacy);
        checkUsageStatistics->setObjectName(QString::fromUtf8("checkUsageStatistics"));
        checkUsageStatistics->setChecked(true);

        privacyLayout->addWidget(checkUsageStatistics);

        labelUsageStatisticsHint = new QLabel(groupPrivacy);
        labelUsageStatisticsHint->setObjectName(QString::fromUtf8("labelUsageStatisticsHint"));
        labelUsageStatisticsHint->setWordWrap(true);

        privacyLayout->addWidget(labelUsageStatisticsHint);


        scrollContentLayout->addWidget(groupPrivacy);

        scrollArea->setWidget(scrollContent);

        verticalLayout->addWidget(scrollArea);

        buttonRow = new QHBoxLayout();
        buttonRow->setObjectName(QString::fromUtf8("buttonRow"));
        btnLockNow = new QPushButton(AppSettingsDialog);
        btnLockNow->setObjectName(QString::fromUtf8("btnLockNow"));

        buttonRow->addWidget(btnLockNow);

        btnClearPassword = new QPushButton(AppSettingsDialog);
        btnClearPassword->setObjectName(QString::fromUtf8("btnClearPassword"));

        buttonRow->addWidget(btnClearPassword);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        buttonRow->addItem(horizontalSpacer);

        btnSave = new QPushButton(AppSettingsDialog);
        btnSave->setObjectName(QString::fromUtf8("btnSave"));

        buttonRow->addWidget(btnSave);

        btnCancel = new QPushButton(AppSettingsDialog);
        btnCancel->setObjectName(QString::fromUtf8("btnCancel"));

        buttonRow->addWidget(btnCancel);


        verticalLayout->addLayout(buttonRow);


        retranslateUi(AppSettingsDialog);

        QMetaObject::connectSlotsByName(AppSettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *AppSettingsDialog)
    {
        AppSettingsDialog->setWindowTitle(QCoreApplication::translate("AppSettingsDialog", "\350\256\276\347\275\256", nullptr));
        AppSettingsDialog->setObjectName(QCoreApplication::translate("AppSettingsDialog", "appDialog", nullptr));
        groupSecurity->setTitle(QCoreApplication::translate("AppSettingsDialog", "\345\256\211\345\205\250", nullptr));
        labelCurrentPassword->setText(QCoreApplication::translate("AppSettingsDialog", "\345\275\223\345\211\215\345\257\206\347\240\201", nullptr));
        editCurrentPassword->setPlaceholderText(QCoreApplication::translate("AppSettingsDialog", "\344\277\256\346\224\271\345\257\206\347\240\201\346\227\266\345\241\253\345\206\231", nullptr));
        labelNewPassword->setText(QCoreApplication::translate("AppSettingsDialog", "\346\226\260\345\257\206\347\240\201", nullptr));
        editNewPassword->setPlaceholderText(QCoreApplication::translate("AppSettingsDialog", "\350\207\263\345\260\221 4 \344\275\215\357\274\214\347\225\231\347\251\272\350\241\250\347\244\272\344\270\215\344\277\256\346\224\271", nullptr));
        labelConfirmPassword->setText(QCoreApplication::translate("AppSettingsDialog", "\347\241\256\350\256\244\346\226\260\345\257\206\347\240\201", nullptr));
        labelLockDelay->setText(QCoreApplication::translate("AppSettingsDialog", "\347\263\273\347\273\237\351\224\201\345\261\217\345\220\216\351\224\201\345\256\232", nullptr));
        spinLockAfterSystemLock->setSuffix(QCoreApplication::translate("AppSettingsDialog", " \345\210\206\351\222\237", nullptr));
        labelLockDelayHint->setText(QCoreApplication::translate("AppSettingsDialog", "0 = \347\253\213\345\215\263\351\224\201\345\256\232", nullptr));
        labelIdleLock->setText(QCoreApplication::translate("AppSettingsDialog", "\347\251\272\351\227\262\350\207\252\345\212\250\351\224\201\345\256\232", nullptr));
        spinLockOnIdle->setSuffix(QCoreApplication::translate("AppSettingsDialog", " \345\210\206\351\222\237", nullptr));
        labelIdleHint->setText(QCoreApplication::translate("AppSettingsDialog", "0 = \344\270\215\345\220\257\347\224\250", nullptr));
        labelLockOnStartup->setText(QCoreApplication::translate("AppSettingsDialog", "\345\220\257\345\212\250\346\227\266\351\234\200\345\257\206\347\240\201", nullptr));
        checkLockOnStartup->setText(QCoreApplication::translate("AppSettingsDialog", "\347\250\213\345\272\217\345\220\257\345\212\250\346\227\266\350\246\201\346\261\202\350\276\223\345\205\245\345\257\206\347\240\201", nullptr));
        labelMinimizeToTray->setText(QCoreApplication::translate("AppSettingsDialog", "\345\205\263\351\227\255\350\241\214\344\270\272", nullptr));
        checkMinimizeToTray->setText(QCoreApplication::translate("AppSettingsDialog", "\345\205\263\351\227\255\347\252\227\345\217\243\346\227\266\346\234\200\345\260\217\345\214\226\345\210\260\346\211\230\347\233\230", nullptr));
        groupHotkeys->setTitle(QCoreApplication::translate("AppSettingsDialog", "\345\277\253\346\215\267\351\224\256", nullptr));
        labelTodayTasksHotkey->setText(QCoreApplication::translate("AppSettingsDialog", "\344\273\212\346\227\245\344\273\273\345\212\241", nullptr));
        labelTop3Hotkey->setText(QCoreApplication::translate("AppSettingsDialog", "Top 3 \345\274\271\347\252\227", nullptr));
        labelQuickCaptureHotkey->setText(QCoreApplication::translate("AppSettingsDialog", "\351\227\252\350\256\260", nullptr));
        checkQuickCaptureAutoAnalyze->setText(QCoreApplication::translate("AppSettingsDialog", "\351\227\252\350\256\260\344\277\235\345\255\230\345\220\216\350\207\252\345\212\250 AI \345\210\206\346\236\220\344\274\230\345\205\210\347\272\247", nullptr));
        labelHotkeyHint->setText(QCoreApplication::translate("AppSettingsDialog", "\345\205\250\345\261\200\345\277\253\346\215\267\351\224\256\351\234\200\345\214\205\345\220\253 Win / Ctrl / Alt / Shift \344\271\213\344\270\200\343\200\202\347\202\271\345\207\273\350\276\223\345\205\245\346\241\206\345\220\216\346\214\211\344\270\213\347\273\204\345\220\210\351\224\256\345\215\263\345\217\257\345\275\225\345\210\266\343\200\202", nullptr));
        groupFocus->setTitle(QCoreApplication::translate("AppSettingsDialog", "Focus 25", nullptr));
        labelFocusDuration->setText(QCoreApplication::translate("AppSettingsDialog", "\347\225\252\350\214\204\346\227\266\351\225\277", nullptr));
        checkFocusTrayCountdown->setText(QCoreApplication::translate("AppSettingsDialog", "\346\211\230\347\233\230\346\217\220\347\244\272\346\230\276\347\244\272\344\270\223\346\263\250\345\200\222\350\256\241\346\227\266", nullptr));
        groupTools->setTitle(QCoreApplication::translate("AppSettingsDialog", "\345\267\245\345\205\267", nullptr));
        btnViewAiTrace->setText(QCoreApplication::translate("AppSettingsDialog", "AI \345\210\206\346\236\220\350\277\207\347\250\213", nullptr));
#if QT_CONFIG(tooltip)
        btnViewAiTrace->setToolTip(QCoreApplication::translate("AppSettingsDialog", "\346\237\245\347\234\213\346\234\200\350\277\221\344\270\200\346\254\241 AI \345\210\206\346\236\220\347\232\204\350\257\246\347\273\206\346\255\245\351\252\244\344\270\216 LLM \350\257\267\346\261\202/\345\223\215\345\272\224", nullptr));
#endif // QT_CONFIG(tooltip)
        btnM365->setText(QCoreApplication::translate("AppSettingsDialog", "Microsoft 365", nullptr));
#if QT_CONFIG(tooltip)
        btnM365->setToolTip(QCoreApplication::translate("AppSettingsDialog", "\347\231\273\345\275\225\345\271\266\345\220\214\346\255\245 Outlook \346\227\227\346\240\207\351\202\256\344\273\266", nullptr));
#endif // QT_CONFIG(tooltip)
        groupAppearance->setTitle(QCoreApplication::translate("AppSettingsDialog", "\347\225\214\351\235\242", nullptr));
        labelLanguage->setText(QCoreApplication::translate("AppSettingsDialog", "\350\257\255\350\250\200", nullptr));
        groupPrivacy->setTitle(QCoreApplication::translate("AppSettingsDialog", "\351\232\220\347\247\201", nullptr));
        checkUsageStatistics->setText(QCoreApplication::translate("AppSettingsDialog", "\345\217\221\351\200\201\345\214\277\345\220\215\344\275\277\347\224\250\347\273\237\350\256\241\357\274\210\345\270\256\345\212\251\346\224\271\350\277\233\344\272\247\345\223\201\357\274\211", nullptr));
        labelUsageStatisticsHint->setText(QCoreApplication::translate("AppSettingsDialog", "\344\273\205\344\270\212\346\212\245\347\211\210\346\234\254\345\217\267\343\200\201\345\220\257\345\212\250/\351\200\200\345\207\272\346\227\266\351\227\264\344\270\216\345\214\277\345\220\215\345\256\211\350\243\205 ID\357\274\214\344\270\215\345\220\253\344\273\273\345\212\241\345\206\205\345\256\271\344\270\216 API Key\343\200\202\345\217\257\345\234\250\350\256\276\347\275\256\344\270\255\351\232\217\346\227\266\345\205\263\351\227\255\343\200\202", nullptr));
        btnLockNow->setText(QCoreApplication::translate("AppSettingsDialog", "\347\253\213\345\215\263\351\224\201\345\256\232", nullptr));
        btnClearPassword->setText(QCoreApplication::translate("AppSettingsDialog", "\346\270\205\351\231\244\345\257\206\347\240\201", nullptr));
        btnSave->setText(QCoreApplication::translate("AppSettingsDialog", "\344\277\235\345\255\230", nullptr));
        btnSave->setObjectName(QCoreApplication::translate("AppSettingsDialog", "btnOk", nullptr));
        btnCancel->setText(QCoreApplication::translate("AppSettingsDialog", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AppSettingsDialog: public Ui_AppSettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_APPSETTINGSDIALOG_H
