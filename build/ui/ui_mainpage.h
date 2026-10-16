/********************************************************************************
** Form generated from reading UI file 'mainpage.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINPAGE_H
#define UI_MAINPAGE_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "DLabel"

QT_BEGIN_NAMESPACE

class Ui_mainpage
{
public:
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QPushButton *btnCurPlay;
    QPushButton *btnLocal;
    QPushButton *btnFlavor;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QPushButton *addNewlist;
    QListView *listView_2;
    Dtk::Widget::DLabel *label_3;
    QStackedWidget *stackedWidget;
    QWidget *page_0;
    QVBoxLayout *verticalLayout_6;
    QLabel *curLabel;
    QListWidget *curPlayWidget;
    QWidget *page_1;
    QVBoxLayout *verticalLayout_4;
    QLabel *localLabel;
    QListWidget *localMusicWidget;
    QWidget *page_2;
    QVBoxLayout *verticalLayout_5;
    QLabel *favorLabel;
    QListWidget *favorMusicList;

    void setupUi(QWidget *mainpage)
    {
        if (mainpage->objectName().isEmpty())
            mainpage->setObjectName(QStringLiteral("mainpage"));
        mainpage->resize(1447, 875);
        horizontalLayout_2 = new QHBoxLayout(mainpage);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(13);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(-1, -1, 10, -1);
        label = new QLabel(mainpage);
        label->setObjectName(QStringLiteral("label"));
        label->setMinimumSize(QSize(200, 0));
        label->setMaximumSize(QSize(200, 16777215));
        label->setStyleSheet(QStringLiteral("font: 75 14pt \"Noto Sans CJK JP Bold\";"));

        verticalLayout->addWidget(label);

        btnCurPlay = new QPushButton(mainpage);
        btnCurPlay->setObjectName(QStringLiteral("btnCurPlay"));
        btnCurPlay->setMinimumSize(QSize(20, 60));
        btnCurPlay->setMaximumSize(QSize(200, 60));
        btnCurPlay->setLayoutDirection(Qt::LeftToRight);
        btnCurPlay->setStyleSheet(QLatin1String("\n"
"font: 16pt \"Noto Sans CJK SC\";"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/imagess/\345\275\223\345\211\215\346\222\255\346\224\276.png"), QSize(), QIcon::Normal, QIcon::On);
        btnCurPlay->setIcon(icon);
        btnCurPlay->setIconSize(QSize(25, 25));
        btnCurPlay->setFlat(true);

        verticalLayout->addWidget(btnCurPlay);

        btnLocal = new QPushButton(mainpage);
        btnLocal->setObjectName(QStringLiteral("btnLocal"));
        btnLocal->setMinimumSize(QSize(200, 60));
        btnLocal->setMaximumSize(QSize(200, 60));
        btnLocal->setStyleSheet(QStringLiteral("font: 16pt \"Noto Sans CJK SC\";"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/imagess/24gl-playlistVideo.png"), QSize(), QIcon::Normal, QIcon::On);
        btnLocal->setIcon(icon1);
        btnLocal->setIconSize(QSize(25, 25));
        btnLocal->setFlat(true);

        verticalLayout->addWidget(btnLocal);

        btnFlavor = new QPushButton(mainpage);
        btnFlavor->setObjectName(QStringLiteral("btnFlavor"));
        btnFlavor->setMinimumSize(QSize(200, 60));
        btnFlavor->setMaximumSize(QSize(200, 60));
        btnFlavor->setStyleSheet(QStringLiteral("font: 16pt \"Noto Sans CJK SC\";"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/imagess/\346\210\221\347\232\204\346\224\266\350\227\217.png"), QSize(), QIcon::Normal, QIcon::On);
        btnFlavor->setIcon(icon2);
        btnFlavor->setIconSize(QSize(25, 25));
        btnFlavor->setFlat(true);

        verticalLayout->addWidget(btnFlavor);


        verticalLayout_3->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_2 = new QLabel(mainpage);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setMinimumSize(QSize(60, 0));
        label_2->setMaximumSize(QSize(100, 16777215));
        label_2->setLayoutDirection(Qt::RightToLeft);
        label_2->setStyleSheet(QStringLiteral("font: 75 14pt \"Noto Sans CJK JP Bold\";"));

        horizontalLayout->addWidget(label_2);

        addNewlist = new QPushButton(mainpage);
        addNewlist->setObjectName(QStringLiteral("addNewlist"));
        addNewlist->setMaximumSize(QSize(40, 40));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/imagess/\346\267\273\345\212\240.png"), QSize(), QIcon::Normal, QIcon::On);
        addNewlist->setIcon(icon3);
        addNewlist->setIconSize(QSize(22, 22));
        addNewlist->setFlat(true);

        horizontalLayout->addWidget(addNewlist);


        verticalLayout_2->addLayout(horizontalLayout);

        listView_2 = new QListView(mainpage);
        listView_2->setObjectName(QStringLiteral("listView_2"));
        listView_2->setMinimumSize(QSize(220, 200));
        listView_2->setMaximumSize(QSize(220, 2000));

        verticalLayout_2->addWidget(listView_2);

        label_3 = new Dtk::Widget::DLabel(mainpage);
        label_3->setObjectName(QStringLiteral("label_3"));

        verticalLayout_2->addWidget(label_3);


        verticalLayout_3->addLayout(verticalLayout_2);


        horizontalLayout_2->addLayout(verticalLayout_3);

        stackedWidget = new QStackedWidget(mainpage);
        stackedWidget->setObjectName(QStringLiteral("stackedWidget"));
        stackedWidget->setAutoFillBackground(false);
        page_0 = new QWidget();
        page_0->setObjectName(QStringLiteral("page_0"));
        verticalLayout_6 = new QVBoxLayout(page_0);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        curLabel = new QLabel(page_0);
        curLabel->setObjectName(QStringLiteral("curLabel"));
        curLabel->setMinimumSize(QSize(0, 30));
        curLabel->setMaximumSize(QSize(16777215, 30));
        curLabel->setStyleSheet(QLatin1String("\n"
"font: 16pt \"Noto Sans CJK SC\";"));

        verticalLayout_6->addWidget(curLabel);

        curPlayWidget = new QListWidget(page_0);
        curPlayWidget->setObjectName(QStringLiteral("curPlayWidget"));

        verticalLayout_6->addWidget(curPlayWidget);

        stackedWidget->addWidget(page_0);
        page_1 = new QWidget();
        page_1->setObjectName(QStringLiteral("page_1"));
        verticalLayout_4 = new QVBoxLayout(page_1);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        localLabel = new QLabel(page_1);
        localLabel->setObjectName(QStringLiteral("localLabel"));
        localLabel->setMinimumSize(QSize(0, 30));
        localLabel->setMaximumSize(QSize(16777215, 30));
        localLabel->setStyleSheet(QStringLiteral("font: 16pt \"Noto Sans CJK SC\";"));

        verticalLayout_4->addWidget(localLabel);

        localMusicWidget = new QListWidget(page_1);
        localMusicWidget->setObjectName(QStringLiteral("localMusicWidget"));
        localMusicWidget->setStyleSheet(QStringLiteral(""));

        verticalLayout_4->addWidget(localMusicWidget);

        stackedWidget->addWidget(page_1);
        page_2 = new QWidget();
        page_2->setObjectName(QStringLiteral("page_2"));
        verticalLayout_5 = new QVBoxLayout(page_2);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        favorLabel = new QLabel(page_2);
        favorLabel->setObjectName(QStringLiteral("favorLabel"));
        favorLabel->setMinimumSize(QSize(0, 30));
        favorLabel->setMaximumSize(QSize(16777215, 30));
        favorLabel->setStyleSheet(QLatin1String("\n"
"font: 16pt \"Noto Sans CJK SC\";"));

        verticalLayout_5->addWidget(favorLabel);

        favorMusicList = new QListWidget(page_2);
        favorMusicList->setObjectName(QStringLiteral("favorMusicList"));

        verticalLayout_5->addWidget(favorMusicList);

        stackedWidget->addWidget(page_2);

        horizontalLayout_2->addWidget(stackedWidget);


        retranslateUi(mainpage);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(mainpage);
    } // setupUi

    void retranslateUi(QWidget *mainpage)
    {
        mainpage->setWindowTitle(QApplication::translate("mainpage", "Form", nullptr));
        label->setText(QApplication::translate("mainpage", "\350\265\204\346\272\220\345\272\223", nullptr));
        btnCurPlay->setText(QApplication::translate("mainpage", "\346\234\200\350\277\221\346\222\255\346\224\276", nullptr));
        btnLocal->setText(QApplication::translate("mainpage", "\346\234\254\345\234\260\351\237\263\344\271\220", nullptr));
        btnFlavor->setText(QApplication::translate("mainpage", "\346\210\221\347\232\204\345\226\234\346\254\242", nullptr));
        label_2->setText(QApplication::translate("mainpage", "\346\210\221\347\232\204\346\255\214\345\215\225", nullptr));
        addNewlist->setText(QString());
        label_3->setText(QString());
        curLabel->setText(QApplication::translate("mainpage", "\345\275\223\345\211\215\346\222\255\346\224\276", nullptr));
        localLabel->setText(QApplication::translate("mainpage", "\346\211\200\346\234\211\350\265\204\346\272\220", nullptr));
        favorLabel->setText(QApplication::translate("mainpage", "\346\210\221\347\232\204\346\224\266\350\227\217", nullptr));
    } // retranslateUi

};

namespace Ui {
    class mainpage: public Ui_mainpage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINPAGE_H
