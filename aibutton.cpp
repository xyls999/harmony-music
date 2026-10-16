#include "aibutton.h"
#include <QIcon>
#include <QMouseEvent>
#include <QPainter>

AIButton::AIButton(QWidget *parent)
    : QPushButton(parent)
{
    setFixedSize(40, 40);
    initStyle();
    initIcon();

    connect(this, &AIButton::clicked, this, [this](){
         emit clickedWithPosition(QCursor::pos());
     }, Qt::UniqueConnection);
}

AIButton::~AIButton() {}

void AIButton::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QPushButton::paintEvent(e);
}

void AIButton::initStyle()
{
    setStyleSheet(R"(
        QPushButton {
            border: none;
            border-radius: 20px;
            background-color: transparent;
        }
        QPushButton:hover {
            background-color: rgba(0, 0, 0, 50);
        }
        QPushButton:pressed {
            background-color: rgba(0, 0, 0, 100);
        }
    )");
}

void AIButton::initIcon()
{
    QIcon aiIcon(":/images/icons/ai.png");
    if(aiIcon.isNull()) {
        qWarning() << "Failed to load AI icon";
        return;
    }
    setIcon(aiIcon);
    setIconSize(QSize(30, 30));
}

void AIButton::mousePressEvent(QMouseEvent *e)
{
    e->accept();
    QPushButton::mousePressEvent(e);
    if(e->button() == Qt::LeftButton) {
        emit clickedWithPosition(mapToGlobal(e->pos()));
    }
}
