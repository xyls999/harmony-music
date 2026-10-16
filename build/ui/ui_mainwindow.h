/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QWidget *titleBar;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QLabel *titleLabel;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *closeButton;
    QSpacerItem *horizontalSpacer_9;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_8;
    QListWidget *listWidget_chat;
    QSpacerItem *horizontalSpacer_7;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_5;
    QPushButton *selectFileButton;
    QSpacerItem *horizontalSpacer_3;
    QLineEdit *lineEdit_question;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *sendButton;
    QSpacerItem *horizontalSpacer_6;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(436, 700);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(350, 700));
        MainWindow->setMaximumSize(QSize(436, 700));
        MainWindow->setAutoFillBackground(true);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        sizePolicy.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        titleBar = new QWidget(centralwidget);
        titleBar->setObjectName(QStringLiteral("titleBar"));
        sizePolicy.setHeightForWidth(titleBar->sizePolicy().hasHeightForWidth());
        titleBar->setSizePolicy(sizePolicy);
        titleBar->setMinimumSize(QSize(426, 35));
        titleBar->setMaximumSize(QSize(436, 35));
        horizontalLayout_2 = new QHBoxLayout(titleBar);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        titleLabel = new QLabel(titleBar);
        titleLabel->setObjectName(QStringLiteral("titleLabel"));
        QFont font;
        font.setPointSize(15);
        titleLabel->setFont(font);

        horizontalLayout_2->addWidget(titleLabel);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        closeButton = new QPushButton(titleBar);
        closeButton->setObjectName(QStringLiteral("closeButton"));
        closeButton->setMinimumSize(QSize(30, 30));
        closeButton->setMaximumSize(QSize(30, 30));
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/ai/close.png"), QSize(), QIcon::Normal, QIcon::Off);
        closeButton->setIcon(icon);
        closeButton->setIconSize(QSize(36, 36));
        closeButton->setFlat(true);

        horizontalLayout_2->addWidget(closeButton);

        horizontalSpacer_9 = new QSpacerItem(20, 10, QSizePolicy::Preferred, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_9);


        verticalLayout->addWidget(titleBar);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_8);

        listWidget_chat = new QListWidget(centralwidget);
        listWidget_chat->setObjectName(QStringLiteral("listWidget_chat"));
        listWidget_chat->setMinimumSize(QSize(380, 500));
        listWidget_chat->setMaximumSize(QSize(380, 500));

        horizontalLayout_3->addWidget(listWidget_chat);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_7);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        horizontalLayout->setContentsMargins(20, 20, 20, 20);
        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_5);

        selectFileButton = new QPushButton(centralwidget);
        selectFileButton->setObjectName(QStringLiteral("selectFileButton"));
        selectFileButton->setMinimumSize(QSize(50, 50));
        selectFileButton->setMaximumSize(QSize(50, 50));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/images/ai/upload.png"), QSize(), QIcon::Normal, QIcon::Off);
        selectFileButton->setIcon(icon1);
        selectFileButton->setIconSize(QSize(48, 48));
        selectFileButton->setFlat(true);

        horizontalLayout->addWidget(selectFileButton);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);

        lineEdit_question = new QLineEdit(centralwidget);
        lineEdit_question->setObjectName(QStringLiteral("lineEdit_question"));
        lineEdit_question->setMinimumSize(QSize(250, 0));
        lineEdit_question->setMaximumSize(QSize(250, 16777215));

        horizontalLayout->addWidget(lineEdit_question);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);

        sendButton = new QPushButton(centralwidget);
        sendButton->setObjectName(QStringLiteral("sendButton"));
        sendButton->setMinimumSize(QSize(50, 50));
        sendButton->setMaximumSize(QSize(50, 50));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/images/ai/send.png"), QSize(), QIcon::Normal, QIcon::Off);
        sendButton->setIcon(icon2);
        sendButton->setIconSize(QSize(48, 48));
        sendButton->setFlat(true);

        horizontalLayout->addWidget(sendButton);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_6);


        verticalLayout->addLayout(horizontalLayout);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        titleLabel->setText(QApplication::translate("MainWindow", "\351\237\263\350\247\206\351\242\221\346\231\272\350\203\275\346\204\237\347\237\245\347\263\273\347\273\237", nullptr));
        closeButton->setText(QString());
        selectFileButton->setText(QString());
        sendButton->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
