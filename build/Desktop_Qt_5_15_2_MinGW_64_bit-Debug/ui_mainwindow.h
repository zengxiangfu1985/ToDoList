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
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QWidget *toolBarPanel;
    QHBoxLayout *toolbarLayout;
    QPushButton *btnAddTask;
    QPushButton *btnAiAnalyze;
    QPushButton *btnLlmSettings;
    QPushButton *btnM365;
    QPushButton *btnHistory;
    QSpacerItem *horizontalSpacer;
    QLabel *labelProviderStatus;
    QSplitter *splitterMain;
    QTableView *tableTasks;
    QWidget *rightPanel;
    QVBoxLayout *rightLayout;
    QGroupBox *groupQuadrants;
    QGridLayout *gridQuadrants;
    QGroupBox *quadrantQ1;
    QGroupBox *quadrantQ2;
    QGroupBox *quadrantQ3;
    QGroupBox *quadrantQ4;
    QGroupBox *groupAi;
    QVBoxLayout *aiLayout;
    QListWidget *listTop3;
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
        btnAddTask = new QPushButton(toolBarPanel);
        btnAddTask->setObjectName(QString::fromUtf8("btnAddTask"));

        toolbarLayout->addWidget(btnAddTask);

        btnAiAnalyze = new QPushButton(toolBarPanel);
        btnAiAnalyze->setObjectName(QString::fromUtf8("btnAiAnalyze"));

        toolbarLayout->addWidget(btnAiAnalyze);

        btnLlmSettings = new QPushButton(toolBarPanel);
        btnLlmSettings->setObjectName(QString::fromUtf8("btnLlmSettings"));

        toolbarLayout->addWidget(btnLlmSettings);

        btnM365 = new QPushButton(toolBarPanel);
        btnM365->setObjectName(QString::fromUtf8("btnM365"));

        toolbarLayout->addWidget(btnM365);

        btnHistory = new QPushButton(toolBarPanel);
        btnHistory->setObjectName(QString::fromUtf8("btnHistory"));

        toolbarLayout->addWidget(btnHistory);

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
        gridQuadrants->setSpacing(8);
        gridQuadrants->setObjectName(QString::fromUtf8("gridQuadrants"));
        quadrantQ1 = new QGroupBox(groupQuadrants);
        quadrantQ1->setObjectName(QString::fromUtf8("quadrantQ1"));
        sizePolicy1.setHeightForWidth(quadrantQ1->sizePolicy().hasHeightForWidth());
        quadrantQ1->setSizePolicy(sizePolicy1);

        gridQuadrants->addWidget(quadrantQ1, 0, 0, 1, 1);

        quadrantQ2 = new QGroupBox(groupQuadrants);
        quadrantQ2->setObjectName(QString::fromUtf8("quadrantQ2"));
        sizePolicy1.setHeightForWidth(quadrantQ2->sizePolicy().hasHeightForWidth());
        quadrantQ2->setSizePolicy(sizePolicy1);

        gridQuadrants->addWidget(quadrantQ2, 0, 1, 1, 1);

        quadrantQ3 = new QGroupBox(groupQuadrants);
        quadrantQ3->setObjectName(QString::fromUtf8("quadrantQ3"));
        sizePolicy1.setHeightForWidth(quadrantQ3->sizePolicy().hasHeightForWidth());
        quadrantQ3->setSizePolicy(sizePolicy1);

        gridQuadrants->addWidget(quadrantQ3, 1, 0, 1, 1);

        quadrantQ4 = new QGroupBox(groupQuadrants);
        quadrantQ4->setObjectName(QString::fromUtf8("quadrantQ4"));
        sizePolicy1.setHeightForWidth(quadrantQ4->sizePolicy().hasHeightForWidth());
        quadrantQ4->setSizePolicy(sizePolicy1);

        gridQuadrants->addWidget(quadrantQ4, 1, 1, 1, 1);


        rightLayout->addWidget(groupQuadrants);

        groupAi = new QGroupBox(rightPanel);
        groupAi->setObjectName(QString::fromUtf8("groupAi"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(2);
        sizePolicy3.setVerticalStretch(2);
        sizePolicy3.setHeightForWidth(groupAi->sizePolicy().hasHeightForWidth());
        groupAi->setSizePolicy(sizePolicy3);
        aiLayout = new QVBoxLayout(groupAi);
        aiLayout->setSpacing(6);
        aiLayout->setObjectName(QString::fromUtf8("aiLayout"));
        listTop3 = new QListWidget(groupAi);
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
        btnAddTask->setText(QCoreApplication::translate("MainWindow", "\346\267\273\345\212\240\344\273\273\345\212\241", nullptr));
#if QT_CONFIG(tooltip)
        btnAddTask->setToolTip(QCoreApplication::translate("MainWindow", "\345\277\253\351\200\237\346\267\273\345\212\240\345\276\205\345\212\236\344\273\273\345\212\241", nullptr));
#endif // QT_CONFIG(tooltip)
        btnAiAnalyze->setText(QCoreApplication::translate("MainWindow", "AI \345\210\206\346\236\220\344\274\230\345\205\210\347\272\247", nullptr));
#if QT_CONFIG(tooltip)
        btnAiAnalyze->setToolTip(QCoreApplication::translate("MainWindow", "\344\275\277\347\224\250 LLM \347\224\237\346\210\220 Top 3 \346\216\250\350\215\220", nullptr));
#endif // QT_CONFIG(tooltip)
        btnLlmSettings->setText(QCoreApplication::translate("MainWindow", "\350\256\276\347\275\256 LLM", nullptr));
        btnM365->setText(QCoreApplication::translate("MainWindow", "Microsoft 365", nullptr));
#if QT_CONFIG(tooltip)
        btnM365->setToolTip(QCoreApplication::translate("MainWindow", "\347\231\273\345\275\225\345\271\266\345\220\214\346\255\245 Outlook \346\227\227\346\240\207\351\202\256\344\273\266", nullptr));
#endif // QT_CONFIG(tooltip)
        btnHistory->setText(QCoreApplication::translate("MainWindow", "\345\216\206\345\217\262\350\256\260\345\275\225", nullptr));
#if QT_CONFIG(tooltip)
        btnHistory->setToolTip(QCoreApplication::translate("MainWindow", "\346\237\245\347\234\213 data \347\233\256\345\275\225\344\270\255\347\232\204\346\257\217\346\227\245\344\273\273\345\212\241\345\275\222\346\241\243", nullptr));
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
