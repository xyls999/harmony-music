// clickablelabel.cpp
#include "clickablelabel.h"
#include <QMouseEvent>

ClickableLabel::ClickableLabel(const QString& text, QWidget* parent)
    : QLabel(text, parent),
      m_clickTimer(new QTimer(this))
{
    m_clickTimer->setInterval(QApplication::doubleClickInterval());
    m_clickTimer->setSingleShot(true);

    connect(m_clickTimer, &QTimer::timeout, [this]{
        emit clicked();  // 定时器超时后触发单击
    });
}

void ClickableLabel::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if (!m_clickTimer->isActive()) {
            m_clickTimer->start();
        }
    }
    QLabel::mousePressEvent(event);
}

void ClickableLabel::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_clickTimer->stop();
        emit doubleClicked();
    }
    QLabel::mouseDoubleClickEvent(event);
}
