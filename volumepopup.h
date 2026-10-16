#ifndef VOLUMEPOPUP_H
#define VOLUMEPOPUP_H

#include <QWidget>
#include <QColor>
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include "thememanager.h"
#include "footwidget.h"
class QSlider;
class QLabel;
class footWidget;


class VolumePopup : public QWidget
{
    Q_OBJECT
public:
    explicit VolumePopup(QWidget *parent = nullptr);
    QSlider *slider;
    void updatepopupTheme(const QColor &bgColor);
    void showAboveWidget(QWidget *widget);

signals:
    void volumeChanged(int volume);
protected:
    void mousePressEvent(QEvent *event);
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
public slots:
    void setVolume(int volume);

private:
    QColor m_textColor;
    QLabel *volumeLabel;
    QColor m_bgColor;
    QString m_themeSuffix;
    ThemeManager &m_theme = ThemeManager::instance();
};
#endif // VOLUMEPOPUP_H
