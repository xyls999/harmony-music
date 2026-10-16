/********************************************************************************
** Form generated from reading UI file 'lyric.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LYRIC_H
#define UI_LYRIC_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "scrolllabel.h"

QT_BEGIN_NAMESPACE

class Ui_Lyric
{
public:
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout;
    QPushButton *emoji;
    ScrollLabel *title;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer;
    QLabel *cover;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *horizontalSpacer_2;
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QScrollArea *lyricScrollArea;
    QWidget *scrollAreaWidgetContents;
    QSlider *lyricSlider;
    QHBoxLayout *horizontalLayout_7;
    QHBoxLayout *horizontalLayout_6;
    QPushButton *lyricaddlist;
    QLabel *lyricduration;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *lyriclike;
    QPushButton *lyricpre;
    QPushButton *lyricplay;
    QPushButton *lyricnext;
    QPushButton *lyricmode;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *lyricvolume;
    QPushButton *lyriclist;

    void setupUi(QWidget *Lyric)
    {
        if (Lyric->objectName().isEmpty())
            Lyric->setObjectName(QStringLiteral("Lyric"));
        Lyric->resize(1238, 947);
        verticalLayout_5 = new QVBoxLayout(Lyric);
        verticalLayout_5->setSpacing(10);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 80, 50, -1);
        emoji = new QPushButton(Lyric);
        emoji->setObjectName(QStringLiteral("emoji"));
        emoji->setMinimumSize(QSize(100, 100));
        emoji->setMaximumSize(QSize(100, 100));
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/icons/energy.png"), QSize(), QIcon::Normal, QIcon::Off);
        emoji->setIcon(icon);
        emoji->setIconSize(QSize(100, 100));
        emoji->setCheckable(false);
        emoji->setChecked(false);
        emoji->setFlat(true);

        horizontalLayout->addWidget(emoji);

        title = new ScrollLabel(Lyric);
        title->setObjectName(QStringLiteral("title"));
        title->setStyleSheet(QString::fromUtf8("font: 20pt \"\345\233\275\346\240\207\344\273\277\345\256\213\";"));

        horizontalLayout->addWidget(title);


        verticalLayout_4->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        cover = new QLabel(Lyric);
        cover->setObjectName(QStringLiteral("cover"));
        cover->setMinimumSize(QSize(150, 150));
        cover->setMaximumSize(QSize(300, 300));
        cover->setPixmap(QPixmap(QString::fromUtf8(":/images/image/non-music.png")));
        cover->setScaledContents(true);

        verticalLayout->addWidget(cover);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);


        horizontalLayout_2->addLayout(verticalLayout);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout_4->addLayout(horizontalLayout_2);


        horizontalLayout_3->addLayout(verticalLayout_4);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(-1, 80, -1, 80);
        lyricScrollArea = new QScrollArea(Lyric);
        lyricScrollArea->setObjectName(QStringLiteral("lyricScrollArea"));
        lyricScrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 601, 621));
        lyricScrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout_2->addWidget(lyricScrollArea);


        verticalLayout_3->addLayout(verticalLayout_2);


        horizontalLayout_3->addLayout(verticalLayout_3);


        verticalLayout_5->addLayout(horizontalLayout_3);

        lyricSlider = new QSlider(Lyric);
        lyricSlider->setObjectName(QStringLiteral("lyricSlider"));
        lyricSlider->setOrientation(Qt::Horizontal);

        verticalLayout_5->addWidget(lyricSlider);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(5, 25, 5, 25);
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(25);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(10, -1, -1, -1);
        lyricaddlist = new QPushButton(Lyric);
        lyricaddlist->setObjectName(QStringLiteral("lyricaddlist"));
        lyricaddlist->setMinimumSize(QSize(40, 40));
        lyricaddlist->setMaximumSize(QSize(40, 40));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/images/icons/lyricaddlist.png"), QSize(), QIcon::Normal, QIcon::Off);
        lyricaddlist->setIcon(icon1);
        lyricaddlist->setIconSize(QSize(40, 40));
        lyricaddlist->setFlat(true);

        horizontalLayout_6->addWidget(lyricaddlist);

        lyricduration = new QLabel(Lyric);
        lyricduration->setObjectName(QStringLiteral("lyricduration"));
        lyricduration->setMinimumSize(QSize(80, 40));
        lyricduration->setMaximumSize(QSize(80, 40));

        horizontalLayout_6->addWidget(lyricduration);


        horizontalLayout_7->addLayout(horizontalLayout_6);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(15);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        lyriclike = new QPushButton(Lyric);
        lyriclike->setObjectName(QStringLiteral("lyriclike"));
        lyriclike->setMinimumSize(QSize(40, 40));
        lyriclike->setMaximumSize(QSize(40, 40));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/images/icons/nonlike.png"), QSize(), QIcon::Normal, QIcon::Off);
        lyriclike->setIcon(icon2);
        lyriclike->setIconSize(QSize(40, 40));
        lyriclike->setFlat(true);

        horizontalLayout_4->addWidget(lyriclike);

        lyricpre = new QPushButton(Lyric);
        lyricpre->setObjectName(QStringLiteral("lyricpre"));
        lyricpre->setMinimumSize(QSize(40, 40));
        lyricpre->setMaximumSize(QSize(40, 40));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/images/icons/lyricpre.png"), QSize(), QIcon::Normal, QIcon::Off);
        lyricpre->setIcon(icon3);
        lyricpre->setIconSize(QSize(40, 40));
        lyricpre->setFlat(true);

        horizontalLayout_4->addWidget(lyricpre);

        lyricplay = new QPushButton(Lyric);
        lyricplay->setObjectName(QStringLiteral("lyricplay"));
        lyricplay->setMinimumSize(QSize(40, 40));
        lyricplay->setMaximumSize(QSize(40, 40));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/images/icons/lyricplay.png"), QSize(), QIcon::Normal, QIcon::Off);
        lyricplay->setIcon(icon4);
        lyricplay->setIconSize(QSize(40, 40));
        lyricplay->setFlat(true);

        horizontalLayout_4->addWidget(lyricplay);

        lyricnext = new QPushButton(Lyric);
        lyricnext->setObjectName(QStringLiteral("lyricnext"));
        lyricnext->setMinimumSize(QSize(40, 40));
        lyricnext->setMaximumSize(QSize(40, 40));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/images/icons/lyricnext.png"), QSize(), QIcon::Normal, QIcon::Off);
        lyricnext->setIcon(icon5);
        lyricnext->setIconSize(QSize(40, 40));
        lyricnext->setFlat(true);

        horizontalLayout_4->addWidget(lyricnext);

        lyricmode = new QPushButton(Lyric);
        lyricmode->setObjectName(QStringLiteral("lyricmode"));
        lyricmode->setMinimumSize(QSize(40, 40));
        lyricmode->setMaximumSize(QSize(40, 40));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/images/icons/loop.png"), QSize(), QIcon::Normal, QIcon::Off);
        lyricmode->setIcon(icon6);
        lyricmode->setIconSize(QSize(40, 40));
        lyricmode->setFlat(true);

        horizontalLayout_4->addWidget(lyricmode);


        horizontalLayout_7->addLayout(horizontalLayout_4);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(20);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(-1, -1, 10, -1);
        lyricvolume = new QPushButton(Lyric);
        lyricvolume->setObjectName(QStringLiteral("lyricvolume"));
        lyricvolume->setMinimumSize(QSize(40, 40));
        lyricvolume->setMaximumSize(QSize(40, 40));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/images/icons/lyricvolume.png"), QSize(), QIcon::Normal, QIcon::Off);
        lyricvolume->setIcon(icon7);
        lyricvolume->setIconSize(QSize(40, 40));
        lyricvolume->setFlat(true);

        horizontalLayout_5->addWidget(lyricvolume);

        lyriclist = new QPushButton(Lyric);
        lyriclist->setObjectName(QStringLiteral("lyriclist"));
        lyriclist->setMinimumSize(QSize(40, 40));
        lyriclist->setMaximumSize(QSize(40, 40));
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/images/icons/lyriclist.png"), QSize(), QIcon::Normal, QIcon::Off);
        lyriclist->setIcon(icon8);
        lyriclist->setIconSize(QSize(40, 40));
        lyriclist->setFlat(true);

        horizontalLayout_5->addWidget(lyriclist);


        horizontalLayout_7->addLayout(horizontalLayout_5);


        verticalLayout_5->addLayout(horizontalLayout_7);


        retranslateUi(Lyric);

        QMetaObject::connectSlotsByName(Lyric);
    } // setupUi

    void retranslateUi(QWidget *Lyric)
    {
        Lyric->setWindowTitle(QApplication::translate("Lyric", "Form", nullptr));
        emoji->setText(QString());
        title->setProperty("text", QVariant(QApplication::translate("Lyric", "\345\275\223\345\211\215\346\222\255\346\224\276", nullptr)));
        cover->setText(QString());
        lyricaddlist->setText(QString());
        lyricduration->setText(QApplication::translate("Lyric", "111", nullptr));
        lyriclike->setText(QString());
        lyricpre->setText(QString());
        lyricplay->setText(QString());
        lyricnext->setText(QString());
        lyricmode->setText(QString());
        lyricvolume->setText(QString());
        lyriclist->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Lyric: public Ui_Lyric {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LYRIC_H
