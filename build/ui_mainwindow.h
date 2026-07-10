/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <ui/top3listwidget.h>
#include "ui/cyber_ai_panel.h"
#include "ui/cyber_quadrant_box.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QWidget *toolBarPanel;
    QHBoxLayout *toolbarLayout;
    QPushButton *btnTodayTasks;
    QPushButton *btnAddTask;
    QPushButton *btnDeleteTasks;
    QPushButton *btnCancelDelete;
    QPushButton *btnAiAnalyze;
    QPushButton *btnResetQuadrants;
    QPushButton *btnLlmSettings;
    QPushButton *btnHistory;
    QPushButton *btnDailyEval;
    QPushButton *btnWeeklyReport;
    QPushButton *btnSettings;
    QPushButton *btnAbout;
    QSpacerItem *horizontalSpacer;
    QLabel *labelProviderStatus;
    QSplitter *splitterMain;
    QTableView *tableTasks;
    QWidget *rightPanel;
    QVBoxLayout *rightLayout;
    QGroupBox *groupQuadrants;
    QGridLayout *gridQuadrants;
    CyberQuadrantBox *quadrantQ1;
    CyberQuadrantBox *quadrantQ2;
    CyberQuadrantBox *quadrantQ3;
    CyberQuadrantBox *quadrantQ4;
    CyberAiPanel *groupAi;
    QVBoxLayout *aiLayout;
    Top3ListWidget *listTop3;
    QTextBrowser *textAiReason;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1024, 720);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        toolBarPanel = new QWidget(centralwidget);
        toolBarPanel->setObjectName(QString::fromUtf8("toolBarPanel"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(toolBarPanel->sizePolicy().hasHeightForWidth());
        toolBarPanel->setSizePolicy(sizePolicy);
        toolbarLayout = new QHBoxLayout(toolBarPanel);
        toolbarLayout->setSpacing(8);
        toolbarLayout->setObjectName(QString::fromUtf8("toolbarLayout"));
        toolbarLayout->setContentsMargins(12, 8, 12, 8);
        btnTodayTasks = new QPushButton(toolBarPanel);
        btnTodayTasks->setObjectName(QString::fromUtf8("btnTodayTasks"));

        toolbarLayout->addWidget(btnTodayTasks);

        btnAddTask = new QPushButton(toolBarPanel);
        btnAddTask->setObjectName(QString::fromUtf8("btnAddTask"));

        toolbarLayout->addWidget(btnAddTask);

        btnDeleteTasks = new QPushButton(toolBarPanel);
        btnDeleteTasks->setObjectName(QString::fromUtf8("btnDeleteTasks"));

        toolbarLayout->addWidget(btnDeleteTasks);

        btnCancelDelete = new QPushButton(toolBarPanel);
        btnCancelDelete->setObjectName(QString::fromUtf8("btnCancelDelete"));
        btnCancelDelete->setVisible(false);

        toolbarLayout->addWidget(btnCancelDelete);

        btnAiAnalyze = new QPushButton(toolBarPanel);
        btnAiAnalyze->setObjectName(QString::fromUtf8("btnAiAnalyze"));

        toolbarLayout->addWidget(btnAiAnalyze);

        btnResetQuadrants = new QPushButton(toolBarPanel);
        btnResetQuadrants->setObjectName(QString::fromUtf8("btnResetQuadrants"));

        toolbarLayout->addWidget(btnResetQuadrants);

        btnLlmSettings = new QPushButton(toolBarPanel);
        btnLlmSettings->setObjectName(QString::fromUtf8("btnLlmSettings"));

        toolbarLayout->addWidget(btnLlmSettings);

        btnHistory = new QPushButton(toolBarPanel);
        btnHistory->setObjectName(QString::fromUtf8("btnHistory"));

        toolbarLayout->addWidget(btnHistory);

        btnDailyEval = new QPushButton(toolBarPanel);
        btnDailyEval->setObjectName(QString::fromUtf8("btnDailyEval"));

        toolbarLayout->addWidget(btnDailyEval);

        btnWeeklyReport = new QPushButton(toolBarPanel);
        btnWeeklyReport->setObjectName(QString::fromUtf8("btnWeeklyReport"));

        toolbarLayout->addWidget(btnWeeklyReport);

        btnSettings = new QPushButton(toolBarPanel);
        btnSettings->setObjectName(QString::fromUtf8("btnSettings"));

        toolbarLayout->addWidget(btnSettings);

        btnAbout = new QPushButton(toolBarPanel);
        btnAbout->setObjectName(QString::fromUtf8("btnAbout"));

        toolbarLayout->addWidget(btnAbout);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        toolbarLayout->addItem(horizontalSpacer);

        labelProviderStatus = new QLabel(toolBarPanel);
        labelProviderStatus->setObjectName(QString::fromUtf8("labelProviderStatus"));

        toolbarLayout->addWidget(labelProviderStatus);


        verticalLayout->addWidget(toolBarPanel);

        splitterMain = new QSplitter(centralwidget);
        splitterMain->setObjectName(QString::fromUtf8("splitterMain"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(1);
        sizePolicy1.setHeightForWidth(splitterMain->sizePolicy().hasHeightForWidth());
        splitterMain->setSizePolicy(sizePolicy1);
        splitterMain->setOrientation(Qt::Horizontal);
        splitterMain->setChildrenCollapsible(false);
        tableTasks = new QTableView(splitterMain);
        tableTasks->setObjectName(QString::fromUtf8("tableTasks"));
        sizePolicy1.setHeightForWidth(tableTasks->sizePolicy().hasHeightForWidth());
        tableTasks->setSizePolicy(sizePolicy1);
        tableTasks->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        tableTasks->setAlternatingRowColors(true);
        splitterMain->addWidget(tableTasks);
        rightPanel = new QWidget(splitterMain);
        rightPanel->setObjectName(QString::fromUtf8("rightPanel"));
        sizePolicy1.setHeightForWidth(rightPanel->sizePolicy().hasHeightForWidth());
        rightPanel->setSizePolicy(sizePolicy1);
        rightLayout = new QVBoxLayout(rightPanel);
        rightLayout->setSpacing(8);
        rightLayout->setObjectName(QString::fromUtf8("rightLayout"));
        rightLayout->setContentsMargins(8, 4, 8, 8);
        groupQuadrants = new QGroupBox(rightPanel);
        groupQuadrants->setObjectName(QString::fromUtf8("groupQuadrants"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(3);
        sizePolicy2.setVerticalStretch(3);
        sizePolicy2.setHeightForWidth(groupQuadrants->sizePolicy().hasHeightForWidth());
        groupQuadrants->setSizePolicy(sizePolicy2);
        gridQuadrants = new QGridLayout(groupQuadrants);
        gridQuadrants->setSpacing(10);
        gridQuadrants->setObjectName(QString::fromUtf8("gridQuadrants"));
        quadrantQ1 = new CyberQuadrantBox(groupQuadrants);
        quadrantQ1->setObjectName(QString::fromUtf8("quadrantQ1"));
        sizePolicy1.setHeightForWidth(quadrantQ1->sizePolicy().hasHeightForWidth());
        quadrantQ1->setSizePolicy(sizePolicy1);

        gridQuadrants->addWidget(quadrantQ1, 0, 0, 1, 1);

        quadrantQ2 = new CyberQuadrantBox(groupQuadrants);
        quadrantQ2->setObjectName(QString::fromUtf8("quadrantQ2"));
        sizePolicy1.setHeightForWidth(quadrantQ2->sizePolicy().hasHeightForWidth());
        quadrantQ2->setSizePolicy(sizePolicy1);

        gridQuadrants->addWidget(quadrantQ2, 0, 1, 1, 1);

        quadrantQ3 = new CyberQuadrantBox(groupQuadrants);
        quadrantQ3->setObjectName(QString::fromUtf8("quadrantQ3"));
        sizePolicy1.setHeightForWidth(quadrantQ3->sizePolicy().hasHeightForWidth());
        quadrantQ3->setSizePolicy(sizePolicy1);

        gridQuadrants->addWidget(quadrantQ3, 1, 0, 1, 1);

        quadrantQ4 = new CyberQuadrantBox(groupQuadrants);
        quadrantQ4->setObjectName(QString::fromUtf8("quadrantQ4"));
        sizePolicy1.setHeightForWidth(quadrantQ4->sizePolicy().hasHeightForWidth());
        quadrantQ4->setSizePolicy(sizePolicy1);

        gridQuadrants->addWidget(quadrantQ4, 1, 1, 1, 1);


        rightLayout->addWidget(groupQuadrants);

        groupAi = new CyberAiPanel(rightPanel);
        groupAi->setObjectName(QString::fromUtf8("groupAi"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(2);
        sizePolicy3.setVerticalStretch(2);
        sizePolicy3.setHeightForWidth(groupAi->sizePolicy().hasHeightForWidth());
        groupAi->setSizePolicy(sizePolicy3);
        aiLayout = new QVBoxLayout(groupAi);
        aiLayout->setSpacing(6);
        aiLayout->setObjectName(QString::fromUtf8("aiLayout"));
        listTop3 = new Top3ListWidget(groupAi);
        listTop3->setObjectName(QString::fromUtf8("listTop3"));
        sizePolicy1.setHeightForWidth(listTop3->sizePolicy().hasHeightForWidth());
        listTop3->setSizePolicy(sizePolicy1);

        aiLayout->addWidget(listTop3);

        textAiReason = new QTextBrowser(groupAi);
        textAiReason->setObjectName(QString::fromUtf8("textAiReason"));
        sizePolicy1.setHeightForWidth(textAiReason->sizePolicy().hasHeightForWidth());
        textAiReason->setSizePolicy(sizePolicy1);
        textAiReason->setMinimumHeight(80);

        aiLayout->addWidget(textAiReason);


        rightLayout->addWidget(groupAi);

        splitterMain->addWidget(rightPanel);

        verticalLayout->addWidget(splitterMain);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "ToDoList", nullptr));
        btnTodayTasks->setText(QCoreApplication::translate("MainWindow", "\344\273\212\346\227\245\344\273\273\345\212\241", nullptr));
#if QT_CONFIG(tooltip)
        btnTodayTasks->setToolTip(QCoreApplication::translate("MainWindow", "\346\211\271\351\207\217\350\276\223\345\205\245\344\273\212\346\227\245\344\273\273\345\212\241\357\274\214\345\217\257\345\257\274\345\205\245\346\230\250\346\227\245\346\234\252\345\256\214\346\210\220\351\241\271", nullptr));
#endif // QT_CONFIG(tooltip)
        btnAddTask->setText(QCoreApplication::translate("MainWindow", "\346\267\273\345\212\240\344\273\273\345\212\241", nullptr));
#if QT_CONFIG(tooltip)
        btnAddTask->setToolTip(QCoreApplication::translate("MainWindow", "\346\267\273\345\212\240\345\215\225\346\235\241\344\273\273\345\212\241\357\274\214\345\217\257\346\211\213\345\212\250\346\214\207\345\256\232\350\261\241\351\231\220", nullptr));
#endif // QT_CONFIG(tooltip)
        btnDeleteTasks->setText(QCoreApplication::translate("MainWindow", "\345\210\240\351\231\244\344\273\273\345\212\241", nullptr));
#if QT_CONFIG(tooltip)
        btnDeleteTasks->setToolTip(QCoreApplication::translate("MainWindow", "\345\213\276\351\200\211\345\244\232\346\235\241\344\273\273\345\212\241\345\220\216\347\241\256\350\256\244\345\210\240\351\231\244\357\274\233\345\215\225\346\235\241\345\217\257\345\217\263\351\224\256\345\210\240\351\231\244", nullptr));
#endif // QT_CONFIG(tooltip)
        btnCancelDelete->setText(QCoreApplication::translate("MainWindow", "\345\217\226\346\266\210", nullptr));
#if QT_CONFIG(tooltip)
        btnCancelDelete->setToolTip(QCoreApplication::translate("MainWindow", "\351\200\200\345\207\272\346\211\271\351\207\217\345\210\240\351\231\244\346\250\241\345\274\217", nullptr));
#endif // QT_CONFIG(tooltip)
        btnAiAnalyze->setText(QCoreApplication::translate("MainWindow", "AI \345\210\206\346\236\220\344\274\230\345\205\210\347\272\247", nullptr));
#if QT_CONFIG(tooltip)
        btnAiAnalyze->setToolTip(QCoreApplication::translate("MainWindow", "\344\275\277\347\224\250 LLM \347\224\237\346\210\220 Top 3 \346\216\250\350\215\220", nullptr));
#endif // QT_CONFIG(tooltip)
        btnResetQuadrants->setText(QCoreApplication::translate("MainWindow", "\351\207\215\347\275\256\350\261\241\351\231\220", nullptr));
#if QT_CONFIG(tooltip)
        btnResetQuadrants->setToolTip(QCoreApplication::translate("MainWindow", "\345\260\206\346\211\200\346\234\211\344\273\273\345\212\241\347\232\204\350\261\241\351\231\220\350\256\276\344\270\272\343\200\214\344\270\215\347\241\256\345\256\232\343\200\215\357\274\214\344\273\245\344\276\277\351\207\215\346\226\260 AI \345\210\222\345\210\206", nullptr));
#endif // QT_CONFIG(tooltip)
        btnLlmSettings->setText(QCoreApplication::translate("MainWindow", "\350\256\276\347\275\256 LLM", nullptr));
        btnHistory->setText(QCoreApplication::translate("MainWindow", "\345\216\206\345\217\262\350\256\260\345\275\225", nullptr));
#if QT_CONFIG(tooltip)
        btnHistory->setToolTip(QCoreApplication::translate("MainWindow", "\346\237\245\347\234\213 data \347\233\256\345\275\225\344\270\255\347\232\204\346\257\217\346\227\245\344\273\273\345\212\241\345\275\222\346\241\243", nullptr));
#endif // QT_CONFIG(tooltip)
        btnDailyEval->setText(QCoreApplication::translate("MainWindow", "\346\257\217\346\227\245\350\257\204\344\274\260", nullptr));
#if QT_CONFIG(tooltip)
        btnDailyEval->setToolTip(QCoreApplication::translate("MainWindow", "\346\237\245\347\234\213 AI \346\257\217\346\227\245\345\256\214\346\210\220\346\203\205\345\206\265\350\257\204\344\274\260\345\216\206\345\217\262", nullptr));
#endif // QT_CONFIG(tooltip)
        btnWeeklyReport->setText(QCoreApplication::translate("MainWindow", "\347\224\237\346\210\220\345\221\250\346\212\245", nullptr));
#if QT_CONFIG(tooltip)
        btnWeeklyReport->setToolTip(QCoreApplication::translate("MainWindow", "\345\213\276\351\200\211\346\234\200\350\277\221\344\270\200\345\221\250\344\273\273\345\212\241\357\274\214AI \347\224\237\346\210\220\345\267\245\344\275\234\345\221\250\346\212\245", nullptr));
#endif // QT_CONFIG(tooltip)
        btnSettings->setText(QCoreApplication::translate("MainWindow", "\350\256\276\347\275\256", nullptr));
#if QT_CONFIG(tooltip)
        btnSettings->setToolTip(QCoreApplication::translate("MainWindow", "\345\257\206\347\240\201\343\200\201\351\224\201\345\261\217\343\200\201\345\277\253\346\215\267\351\224\256\347\255\211\345\270\270\347\224\250\350\256\276\347\275\256", nullptr));
#endif // QT_CONFIG(tooltip)
        btnAbout->setText(QCoreApplication::translate("MainWindow", "\345\205\263\344\272\216", nullptr));
#if QT_CONFIG(tooltip)
        btnAbout->setToolTip(QCoreApplication::translate("MainWindow", "\347\211\210\346\234\254\345\217\267\343\200\201\346\243\200\346\237\245\346\233\264\346\226\260\344\270\216\347\246\273\347\272\277\345\215\207\347\272\247", nullptr));
#endif // QT_CONFIG(tooltip)
        labelProviderStatus->setText(QCoreApplication::translate("MainWindow", "Provider: -", nullptr));
        groupQuadrants->setTitle(QCoreApplication::translate("MainWindow", "Eisenhower \345\233\233\350\261\241\351\231\220", nullptr));
        quadrantQ1->setTitle(QCoreApplication::translate("MainWindow", "Q1 \351\207\215\350\246\201\344\270\224\347\264\247\346\200\245", nullptr));
        quadrantQ2->setTitle(QCoreApplication::translate("MainWindow", "Q2 \351\207\215\350\246\201\344\270\215\347\264\247\346\200\245", nullptr));
        quadrantQ3->setTitle(QCoreApplication::translate("MainWindow", "Q3 \347\264\247\346\200\245\344\270\215\351\207\215\350\246\201", nullptr));
        quadrantQ4->setTitle(QCoreApplication::translate("MainWindow", "Q4 \344\270\215\351\207\215\350\246\201\344\270\215\347\264\247\346\200\245", nullptr));
        groupAi->setTitle(QCoreApplication::translate("MainWindow", "AI Top 3 \346\216\250\350\215\220", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
