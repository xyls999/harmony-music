/********************************************************************************
** Form generated from reading UI file 'titlebarwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TITLEBARWIDGET_H
#define UI_TITLEBARWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QWidget>
#include "DLineEdit"
#include "DSearchEdit"

QT_BEGIN_NAMESPACE

class Ui_TitleBarWidget
{
public:
    QHBoxLayout *horizontalLayout;
    Dtk::Widget::DSearchEdit *SearchEdit;

    void setupUi(QWidget *TitleBarWidget)
    {
        if (TitleBarWidget->objectName().isEmpty())
            TitleBarWidget->setObjectName(QStringLiteral("TitleBarWidget"));
        TitleBarWidget->resize(1061, 599);
        horizontalLayout = new QHBoxLayout(TitleBarWidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        SearchEdit = new Dtk::Widget::DSearchEdit(TitleBarWidget);
        SearchEdit->setObjectName(QStringLiteral("SearchEdit"));
        SearchEdit->setAcceptDrops(false);
        SearchEdit->setAlert(false);

        horizontalLayout->addWidget(SearchEdit);


        retranslateUi(TitleBarWidget);

        QMetaObject::connectSlotsByName(TitleBarWidget);
    } // setupUi

    void retranslateUi(QWidget *TitleBarWidget)
    {
        TitleBarWidget->setWindowTitle(QApplication::translate("TitleBarWidget", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TitleBarWidget: public Ui_TitleBarWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TITLEBARWIDGET_H
