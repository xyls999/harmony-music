#ifndef MUSICITEMWIDGET_H
#define MUSICITEMWIDGET_H

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
#include <QMenu>
#include <QDebug>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QTextCursor>
#include <QStyle>
#include <QUrl>
#include <QLayout>
#include <QRegularExpression>
#include <QHBoxLayout>
#include "thememanager.h"
#include "databasehandler.h"
namespace Ui {
class MusicItemWidget;
}

class MusicItemWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor bgColor READ bgColor WRITE setBgColor)

public:
    enum State { Normal, Hovered, Playing };

    explicit MusicItemWidget(QWidget *parent = nullptr);
    QString getTitle() const { return m_title; }
    QString getArtist() const { return m_artist; }
    QPixmap getCover() const { return m_cover; }
    int getSongId() const { return m_songId; } // 添加获取歌曲ID的方法
    QString getFilePath() const { return m_filePath; }
    bool isCloudSong() const {return m_isCloud; }

    void updateData(int index, const QPixmap &cover,
                     const QString &title, const QString &artist,
                     const QString &album, const QString &duration,
                     bool liked, int songId, const QString &filePath,
                     bool is_cloud);

    QColor bgColor() const;
    void setBgColor(const QColor &color);
    void setHighlighted(bool highlighted, bool isPlaying);
    void setLiked(bool like);
    void updateIconTheme(bool useDarkIcons);
    void setThemeColor(const QColor& themeColor);
    QUrl getFileUrl() const;
    void highlightText(const QString& keyword);

    void updateCover(const QPixmap &cover);
signals:
    void liked(bool isFavorite, int songId);
    void onAddToPlaylistRequested(int songId);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event);
private slots:
    void onLikeClicked(bool checked);
    void onAddToPlaylistClicked();
private:
    void initWidgets();

    QString m_title;
    QString m_artist;
    QPixmap m_cover;
    QColor m_themeColor;
    QColor m_highlightColor ;

    // UI
    QLabel *numLabel;
    QLabel *coverLabel;
    QLabel *titleLabel;
    QLabel *artistLabel;
    QLabel *albumLabel;
    QLabel *durationLabel;
    QPushButton *likeBtn;
    QString m_highlightText;
    QPushButton *addToPlaylistButton;
    QMenu *menu;

    State m_state = Normal;
    QColor m_bgColor;
    QPropertyAnimation *m_colorAnim;
    int m_index;
    bool m_isPlaying = false;
    ThemeManager& m_theme = ThemeManager::instance();
    DatabaseHandler* m_dbHandler;

    int m_songId; // 添加歌曲ID成员变量
    QString m_filePath;
    bool m_liked = false;
    bool m_isCloud = 0;
    void highlightInLabel(QLabel* label, const QString& keyword);
    QString highlightMatch(const QString& text, const QString& keyword);
    void drawHighlightedText(QPainter* painter, QLabel* label);
};

#endif // MUSICITEMWIDGET_H
