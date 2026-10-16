#ifndef PLAYLISTITEMWIDGET_H
#define PLAYLISTITEMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QPainter>
#include <QColor>
#include <QDebug>
#include <QStyle>
#include <QEnterEvent>
#include <QLayout>
#include <QPropertyAnimation>
#include "thememanager.h"
#include "covercache.h"

class PlaylistItemWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor bgColor READ bgColor WRITE setBgColor)
public:
    explicit PlaylistItemWidget(QWidget *parent = nullptr);
    ~PlaylistItemWidget();

    void setData(const QPixmap &cover, const QString &title,
                const QString &artist, const QString &duration);
    void setCurrentStyle(bool isCurrent);

    QString gettitle() const;
    QColor bgColor() const;
    void setBgColor(const QColor &color);
    void updateThemeStyles();

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) ;
    void leaveEvent(QEvent *event) override;

private:
    void initUI();


    QColor m_bgColor;
    bool m_isDark;
    bool m_isCurrent = false;
    bool m_isHovered = false;

    QLabel *m_numLabel;
    QLabel *m_coverLabel;
    QLabel *m_titleLabel;
    QLabel *m_artistLabel;
    QLabel *m_durationLabel;
    QString m_title;
    QPropertyAnimation *m_colorAnim;

    ThemeManager &m_theme = ThemeManager::instance();
    void setTheme(bool isDark);
    void updateCover(const QString& key, const QPixmap& cover);
};
#endif // PLAYLISTITEMWIDGET_H
