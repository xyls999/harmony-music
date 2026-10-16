#ifndef TITLEBARWIDGET_H
#define TITLEBARWIDGET_H

#include <QWidget>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
namespace Ui {
class TitleBarWidget;
}

class TitleBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBarWidget(QWidget *parent = nullptr);
    ~TitleBarWidget();
signals:
    void switchToNormal();
    void switchToVideo();
private:
    Ui::TitleBarWidget *ui;
};

#endif // TITLEBARWIDGET_H
