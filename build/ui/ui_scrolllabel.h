/********************************************************************************
** Form generated from reading UI file 'scrolllabel.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCROLLLABEL_H
#define UI_SCROLLLABEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ScrollLabel
{
public:

    void setupUi(QWidget *ScrollLabel)
    {
        if (ScrollLabel->objectName().isEmpty())
            ScrollLabel->setObjectName(QStringLiteral("ScrollLabel"));
        ScrollLabel->resize(400, 300);

        retranslateUi(ScrollLabel);

        QMetaObject::connectSlotsByName(ScrollLabel);
    } // setupUi

    void retranslateUi(QWidget *ScrollLabel)
    {
        ScrollLabel->setWindowTitle(QApplication::translate("ScrollLabel", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ScrollLabel: public Ui_ScrollLabel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCROLLLABEL_H
