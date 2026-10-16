#include "titlebarwidget.h"
#include "ui_titlebarwidget.h"

TitleBarWidget::TitleBarWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TitleBarWidget)
{
    ui->setupUi(this);
    connect(ui->musicbtn, &QPushButton::clicked, this, &TitleBarWidget::switchToNormal);
    connect(ui->videobtn, &QPushButton::clicked, this, &TitleBarWidget::switchToVideo);
}

TitleBarWidget::~TitleBarWidget()
{
    delete ui;
}
