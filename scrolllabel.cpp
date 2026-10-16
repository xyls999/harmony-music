#include "scrolllabel.h"
#include <QPainter>
#include <QFontMetrics>

ScrollLabel::ScrollLabel(QWidget* parent)
    : QLabel(parent), m_timer(new QTimer(this)), m_offset(0), m_needScroll(false)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(m_timer, &QTimer::timeout, this, &ScrollLabel::updateText);
    m_timer->setInterval(50);
}

void ScrollLabel::setText(const QString &text) {

    QLabel::setText(text);
    m_fullText = text;

    QFont newfont = font();
    newfont.setPointSize(18);
    setFont(newfont);

    checkTextLength();     // 计算是否需要滚动
    update();
}

void ScrollLabel::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setPen(palette().color(QPalette::WindowText));

    painter.setClipRect(rect());

    if (m_needScroll) {
        int textY = height() / 2 + fontMetrics().ascent() / 2 - 2;

        painter.drawText(-m_offset, textY, m_fullText);

        painter.drawText(-m_offset + m_maxOffset, textY, m_fullText);
    } else {
        // 直接居中
        painter.drawText(rect(), Qt::AlignCenter, m_fullText);
    }
}

void ScrollLabel::resizeEvent(QResizeEvent* event) {
    checkTextLength();
    QLabel::resizeEvent(event);
}

void ScrollLabel::setScrollEnabled(bool enable) {
    if (m_needScroll != enable) {
        m_needScroll = enable;
        enable ? startScrolling() : stopScrolling();
    }
}

void ScrollLabel::updateText() {
    if (++m_offset >= m_maxOffset) {
        m_offset = 0;
    }
    update();
}

void ScrollLabel::startScrolling() {
    QFontMetrics fm(font());
    int textWidth = fm.horizontalAdvance(m_fullText);

    if (textWidth > width()) {
        m_maxOffset = textWidth + 20;
        m_timer->start();
    } else {
        stopScrolling();
    }
}

void ScrollLabel::stopScrolling() {
    m_timer->stop();
    m_offset = 0;
    update();
}
void ScrollLabel::checkTextLength() {
    QFontMetrics fm(font());
    setScrollEnabled(fm.horizontalAdvance(m_fullText) > width());
}
