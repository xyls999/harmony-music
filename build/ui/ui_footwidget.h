/********************************************************************************
** Form generated from reading UI file 'footwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FOOTWIDGET_H
#define UI_FOOTWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "scrolllabel.h"

QT_BEGIN_NAMESPACE

class Ui_footWidget
{
public:
    QHBoxLayout *horizontalLayout_5;
    QHBoxLayout *horizontalLayout;
    QPushButton *coverButton;
    QVBoxLayout *verticalLayout_3;
    ScrollLabel *infoLabel;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *pushButton_2;
    QPushButton *Lyricbtn;
    QPushButton *Modebtn;
    QSpacerItem *horizontalSpacer_3;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *Prebtn;
    QPushButton *Playbtn;
    QPushButton *Nextbtn;
    QHBoxLayout *horizontalLayout_7;
    QSlider *positionSlider;
    QLabel *positionlabel;
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *Favorbtn_3;
    QToolButton *SpeedButton;
    QPushButton *Volumebtn;
    QVBoxLayout *verticalLayout;

    void setupUi(QWidget *footWidget)
    {
        if (footWidget->objectName().isEmpty())
            footWidget->setObjectName(QStringLiteral("footWidget"));
        footWidget->resize(1752, 1013);
        footWidget->setMaximumSize(QSize(6000, 4000));
        QIcon icon;
        icon.addFile(QStringLiteral("images/background/default.jpg"), QSize(), QIcon::Normal, QIcon::Off);
        footWidget->setWindowIcon(icon);
        footWidget->setStyleSheet(QStringLiteral(""));
        horizontalLayout_5 = new QHBoxLayout(footWidget);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        coverButton = new QPushButton(footWidget);
        coverButton->setObjectName(QStringLiteral("coverButton"));
        coverButton->setMinimumSize(QSize(150, 150));
        coverButton->setMaximumSize(QSize(150, 150));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/images/image/non-music.png"), QSize(), QIcon::Normal, QIcon::Off);
        coverButton->setIcon(icon1);
        coverButton->setIconSize(QSize(150, 150));
        coverButton->setFlat(true);

        horizontalLayout->addWidget(coverButton);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        infoLabel = new ScrollLabel(footWidget);
        infoLabel->setObjectName(QStringLiteral("infoLabel"));
        infoLabel->setMinimumSize(QSize(200, 0));
        infoLabel->setMaximumSize(QSize(200, 16777215));
        infoLabel->setStyleSheet(QStringLiteral("color:rgb(255, 255, 255);"));

        verticalLayout_3->addWidget(infoLabel);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        pushButton_2 = new QPushButton(footWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setMinimumSize(QSize(40, 40));
        pushButton_2->setMaximumSize(QSize(40, 40));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/images/image/non-like.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_2->setIcon(icon2);
        pushButton_2->setIconSize(QSize(30, 30));
        pushButton_2->setFlat(true);

        horizontalLayout_3->addWidget(pushButton_2);

        Lyricbtn = new QPushButton(footWidget);
        Lyricbtn->setObjectName(QStringLiteral("Lyricbtn"));
        Lyricbtn->setMaximumSize(QSize(40, 40));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/images/image/lyric.png"), QSize(), QIcon::Normal, QIcon::Off);
        Lyricbtn->setIcon(icon3);
        Lyricbtn->setIconSize(QSize(30, 30));
        Lyricbtn->setFlat(true);

        horizontalLayout_3->addWidget(Lyricbtn);

        Modebtn = new QPushButton(footWidget);
        Modebtn->setObjectName(QStringLiteral("Modebtn"));
        Modebtn->setMaximumSize(QSize(40, 40));
        Modebtn->setStyleSheet(QLatin1String("QPushButton\n"
"{\n"
"background-repeat:no-repeat;\n"
"background-position:center center;\n"
"border:none;\n"
"}\n"
"\n"
"QPushButton:hover{\n"
"background-repeat:no-repeat;\n"
"background-position:center center;\n"
"}\n"
"\n"
"QPushButton:pressed{\n"
"background-repeat:no-repeat;\n"
"background-position:center center;\n"
"}\n"
""));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/images/image/loop2.png"), QSize(), QIcon::Normal, QIcon::Off);
        Modebtn->setIcon(icon4);
        Modebtn->setIconSize(QSize(32, 32));

        horizontalLayout_3->addWidget(Modebtn);


        verticalLayout_3->addLayout(horizontalLayout_3);


        horizontalLayout->addLayout(verticalLayout_3);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        Prebtn = new QPushButton(footWidget);
        Prebtn->setObjectName(QStringLiteral("Prebtn"));
        Prebtn->setMaximumSize(QSize(40, 40));
        Prebtn->setStyleSheet(QLatin1String("QPushButton\n"
"{\n"
"background-image: url(:/images/image/pre.png);\n"
"background-repeat:no-repeat;\n"
"background-position:center center;\n"
"border:none;\n"
"}\n"
"QPushButton:hover{\n"
"background-image: url(:/images/image-hover/pre-hover.png);\n"
"background-repeat:no-repeat;\n"
"background-position:center center;\n"
"}\n"
"QPushButton:pressed{\n"
"background-image: url(:/images/image/pre.png);\n"
"background-repeat:no-repeat;\n"
"background-position:center center;\n"
"}\n"
""));
        Prebtn->setIconSize(QSize(24, 24));

        horizontalLayout_2->addWidget(Prebtn);

        Playbtn = new QPushButton(footWidget);
        Playbtn->setObjectName(QStringLiteral("Playbtn"));
        Playbtn->setMaximumSize(QSize(40, 40));
        Playbtn->setStyleSheet(QLatin1String("QPushButton {\n"
"    background-color: #009688;\n"
"    border-radius: 25px;\n"
"    border: none;\n"
"    min-width: 50px;\n"
"    min-height: 50px;\n"
"    padding: 0px;\n"
"}\n"
""));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/images/image-hover/pase-hover.png"), QSize(), QIcon::Normal, QIcon::Off);
        Playbtn->setIcon(icon5);
        Playbtn->setIconSize(QSize(32, 32));

        horizontalLayout_2->addWidget(Playbtn);

        Nextbtn = new QPushButton(footWidget);
        Nextbtn->setObjectName(QStringLiteral("Nextbtn"));
        Nextbtn->setMaximumSize(QSize(40, 40));
        Nextbtn->setStyleSheet(QLatin1String("QPushButton\n"
"{\n"
"background-image: url(:/images/image/next.png);\n"
"background-repeat:no-repeat;\n"
"background-position:center center;\n"
"border:none;\n"
"}\n"
"QPushButton:hover{\n"
"background-image: url(:/images/image-hover/next-hover.png);\n"
"background-repeat:no-repeat;\n"
"background-position:center center;\n"
"}\n"
"QPushButton:pressed{\n"
"background-image: url(:/images/image/next.png);\n"
"background-repeat:no-repeat;\n"
"background-position:center center;\n"
"}\n"
""));
        Nextbtn->setIconSize(QSize(24, 24));

        horizontalLayout_2->addWidget(Nextbtn);


        verticalLayout_4->addLayout(horizontalLayout_2);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        positionSlider = new QSlider(footWidget);
        positionSlider->setObjectName(QStringLiteral("positionSlider"));
        positionSlider->setMinimumSize(QSize(20, 20));
        positionSlider->setMaximumSize(QSize(5000, 50));
        positionSlider->setStyleSheet(QString::fromUtf8("/* \347\273\277\350\211\262\344\270\273\351\242\230\346\273\221\345\212\250\346\235\241 */\n"
"QSlider {\n"
"    /* \345\205\250\345\261\200\350\256\276\347\275\256 */\n"
"    min-height: 20px;  /* \345\236\202\347\233\264\346\273\221\345\235\227\345\256\275\345\272\246 */\n"
"    min-width: 20px;   /* \346\260\264\345\271\263\346\273\221\345\235\227\351\253\230\345\272\246 */\n"
"}\n"
"\n"
"/* \346\273\221\346\247\275\350\275\250\351\201\223 */\n"
"QSlider::groove:horizontal {\n"
"    background: #E0E0E0;\n"
"    height: 6px;\n"
"    border-radius: 3px;\n"
"}\n"
"\n"
"QSlider::groove:vertical {\n"
"    background: #E0E0E0;\n"
"    width: 6px;\n"
"    border-radius: 3px;\n"
"}\n"
"\n"
"/* \345\267\262\345\241\253\345\205\205\351\203\250\345\210\206 */\n"
"QSlider::sub-page:horizontal {\n"
"    background: qlineargradient(\n"
"        x1:0, y1:0,\n"
"        x2:1, y2:0,\n"
"        stop:0 #6DD400, \n"
"        stop:1 #4CAF50\n"
"    );\n"
"    border-radius: 3px;\n"
"}\n"
"\n"
"QSlider::add-page:vertical {\n"
""
                        "    background: qlineargradient(\n"
"        x1:0, y1:0,\n"
"        x2:0, y2:1,\n"
"        stop:0 #6DD400,\n"
"        stop:1 #4CAF50\n"
"    );\n"
"    border-radius: 3px;\n"
"}\n"
"\n"
"/* \346\273\221\345\235\227\346\211\213\346\237\204 */\n"
"QSlider::handle {\n"
"    background: #FFFFFF;\n"
"    border: 2px solid #4CAF50;\n"
"    width: 16px;\n"
"    height: 16px;\n"
"    border-radius: 8px;\n"
"    margin: -6px 0;  /* \350\260\203\346\225\264\345\236\202\347\233\264\346\273\221\345\235\227\344\275\215\347\275\256 */\n"
"    box-shadow: 0 2px 4px rgba(0,0,0,0.2);\n"
"}\n"
"\n"
"/* \346\202\254\345\201\234\347\212\266\346\200\201 */\n"
"QSlider::handle:hover {\n"
"    background: #F5F5F5;\n"
"    border-color: #66BB6A;\n"
"}\n"
"\n"
"/* \346\214\211\344\270\213\347\212\266\346\200\201 */\n"
"QSlider::handle:pressed {\n"
"    background: #E0F7FA;\n"
"    border-color: #81C784;\n"
"    box-shadow: 0 1px 2px rgba(0,0,0,0.2);\n"
"}\n"
"\n"
"/* \347\246\201\347\224\250\347\212\266\346\200\201 */\n"
"QSlider:d"
                        "isabled {\n"
"    background: transparent;\n"
"}\n"
"\n"
"QSlider::sub-page:disabled {\n"
"    background: #BDBDBD;\n"
"}\n"
"\n"
"QSlider::handle:disabled {\n"
"    background: #EEEEEE;\n"
"    border-color: #BDBDBD;\n"
"}"));
        positionSlider->setOrientation(Qt::Horizontal);

        horizontalLayout_7->addWidget(positionSlider);

        positionlabel = new QLabel(footWidget);
        positionlabel->setObjectName(QStringLiteral("positionlabel"));
        positionlabel->setStyleSheet(QStringLiteral("color:rgb(255, 255, 255);"));

        horizontalLayout_7->addWidget(positionlabel);


        verticalLayout_4->addLayout(horizontalLayout_7);


        horizontalLayout->addLayout(verticalLayout_4);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));

        horizontalLayout->addLayout(verticalLayout_2);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(30);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        Favorbtn_3 = new QPushButton(footWidget);
        Favorbtn_3->setObjectName(QStringLiteral("Favorbtn_3"));
        Favorbtn_3->setMaximumSize(QSize(50, 50));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/images/image/addFromLocal.png"), QSize(), QIcon::Normal, QIcon::Off);
        Favorbtn_3->setIcon(icon6);
        Favorbtn_3->setIconSize(QSize(30, 30));
        Favorbtn_3->setFlat(true);

        horizontalLayout_4->addWidget(Favorbtn_3);

        SpeedButton = new QToolButton(footWidget);
        SpeedButton->setObjectName(QStringLiteral("SpeedButton"));
        SpeedButton->setMinimumSize(QSize(50, 50));
        SpeedButton->setMaximumSize(QSize(50, 50));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/images/image/speed.png"), QSize(), QIcon::Normal, QIcon::Off);
        SpeedButton->setIcon(icon7);
        SpeedButton->setIconSize(QSize(40, 40));

        horizontalLayout_4->addWidget(SpeedButton);

        Volumebtn = new QPushButton(footWidget);
        Volumebtn->setObjectName(QStringLiteral("Volumebtn"));
        Volumebtn->setMaximumSize(QSize(50, 50));
        Volumebtn->setStyleSheet(QLatin1String("\n"
"        QToolButton::menu-indicator {\n"
"            image: none;\n"
"            width: 0px;\n"
"            height: 0px;\n"
"        }\n"
"\n"
"    "));
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/images/image-hover/music-voice-hover.png"), QSize(), QIcon::Normal, QIcon::Off);
        Volumebtn->setIcon(icon8);
        Volumebtn->setIconSize(QSize(30, 30));
        Volumebtn->setFlat(true);

        horizontalLayout_4->addWidget(Volumebtn);


        horizontalLayout->addLayout(horizontalLayout_4);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));

        horizontalLayout->addLayout(verticalLayout);


        horizontalLayout_5->addLayout(horizontalLayout);


        retranslateUi(footWidget);

        QMetaObject::connectSlotsByName(footWidget);
    } // setupUi

    void retranslateUi(QWidget *footWidget)
    {
        footWidget->setWindowTitle(QApplication::translate("footWidget", "MainWindow", nullptr));
        coverButton->setText(QString());
        infoLabel->setProperty("text", QVariant(QString()));
        pushButton_2->setText(QString());
        Lyricbtn->setText(QString());
        Modebtn->setText(QString());
        Prebtn->setText(QString());
        Playbtn->setText(QString());
        Nextbtn->setText(QString());
        positionlabel->setText(QApplication::translate("footWidget", "00:00/00:00", nullptr));
        Favorbtn_3->setText(QString());
        SpeedButton->setText(QApplication::translate("footWidget", "...", nullptr));
#ifndef QT_NO_TOOLTIP
        Volumebtn->setToolTip(QApplication::translate("footWidget", "\351\237\263\351\207\217\346\216\247\345\210\266", nullptr));
#endif // QT_NO_TOOLTIP
        Volumebtn->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class footWidget: public Ui_footWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FOOTWIDGET_H
