#include "scrollabel.h"
#include "ui_scrollabel.h"

ScrolLabel::ScrolLabel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScrolLabel)
{
    ui->setupUi(this);
}

ScrolLabel::~ScrolLabel()
{
    delete ui;
}
