// volumePopup.cpp
#include "volumepopup.h"
#include <QScreen>
#include <QGuiApplication>

VolumePopup::VolumePopup(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);

    // 使用最小尺寸 + 布局适配
    setMinimumSize(26, 140);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);  // 增加内边距


    volumeLabel = new QLabel(this);
    volumeLabel->setAlignment(Qt::AlignCenter);
    volumeLabel->setMinimumWidth(24);
    volumeLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // 字体设置（带备用字体）
    QFont labelFont("Microsoft YaHei", 9, QFont::Bold);
    labelFont.setStyleHint(QFont::SansSerif);  // 备用字体
    volumeLabel->setFont(labelFont);

    mainLayout->addWidget(volumeLabel);

    QWidget *sliderContainer = new QWidget(this);
    sliderContainer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QHBoxLayout *sliderLayout = new QHBoxLayout(sliderContainer);
    sliderLayout->setContentsMargins(0, 0, 0, 0);

    slider = new QSlider(Qt::Vertical, this);
    slider->setRange(0, 100);
    slider->setFixedSize(24, 100);  // 固定滑动条尺寸

    sliderLayout->addStretch();
    sliderLayout->addWidget(slider);
    sliderLayout->addStretch();

    mainLayout->addWidget(sliderContainer);

    connect(slider, &QSlider::valueChanged, [this](int value) {
//        qInfo() << "[VolumePopup] 滑块值变化：" << value;
        volumeLabel->setText(QString::number(value));
        emit volumeChanged(value);});

}

void VolumePopup::updatepopupTheme(const QColor &bgColor)
{
    m_bgColor = bgColor;
    m_textColor = m_theme.isDarkTheme(bgColor) ? Qt::white : Qt::black;

    QString style = QString(R"(
        QWidget {
            background: %1;
            border-radius: 8px;
            border: 1px solid %2;
        }
        QLabel {
            color: %3;
            background: transparent;
        }
        QSlider::groove:vertical {
            width: 4px;
            background: %4;
            border-radius: 2px;
            margin: 0 6px;
        }
        QSlider::handle:vertical {
            height: 12px;
            width: 12px;
            margin: 0 -4px;
            background: %3;
            border-radius: 6px;
        }
        QSlider::add-page:vertical {
            background: %5;
        }
    )").arg(
        bgColor.name(),
        bgColor.darker(120).name(),
        m_textColor.name(),
        bgColor.darker(130).name(),
        m_textColor == Qt::white ? "rgba(255,255,255,0.2)" : "rgba(0,0,0,0.2)"
    );

    setStyleSheet(style);
    adjustSize();
}


void VolumePopup::showAboveWidget(QWidget* widget)
{
    // 获取全局坐标
    QPoint btnTopCenter = widget->mapToGlobal(QPoint(widget->width()/2, 0));

    int x = btnTopCenter.x() - width()/2;
    int y = btnTopCenter.y() - height() - 5;

    move(x, y);
    if (auto *fw = qobject_cast<footWidget*>(widget->parentWidget())) {
        if (fw->getPlayer()) {
            int currentVolume = fw->getPlayer()->volume();
            slider->setValue(currentVolume);
            volumeLabel->setText(QString::number(currentVolume));

           }
       }

     show();
}
void VolumePopup::mousePressEvent(QEvent *event)
{
    event->accept();
}

void VolumePopup::setVolume(int volume)
{
    QSignalBlocker blocker(slider);
    slider->setValue(volume);

    if(volume <= 0) {
        volumeLabel->setText("静音");
        volumeLabel->setStyleSheet("color: " + m_textColor.name() + "; opacity: 0.7;");
    } else {
        volumeLabel->setText(QString("%1%").arg(volume));
        volumeLabel->setStyleSheet("color: " + m_textColor.name() + ";");
    }

    QPalette pal = volumeLabel->palette();
    pal.setColor(QPalette::WindowText, m_textColor);
    volumeLabel->setPalette(pal);
}
void VolumePopup::enterEvent(QEvent *event) {
    if (auto *fw = qobject_cast<footWidget*>(parentWidget())) {
        fw->eventFilter(this, event);
    }
}

void VolumePopup::leaveEvent(QEvent *event) {
    if (auto *fw = qobject_cast<footWidget*>(parentWidget())) {
        fw->eventFilter(this, event);
    }
}
