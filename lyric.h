#pragma once

#include <QWidget>
#include <QToolButton>
#include <QMediaPlayer>
#include <QFile>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QActionGroup>
#include <QMenu>
#include <QMap>
#include <QColor>
#include <QDir>
#include <QTimer>
#include <QPoint>
#include <QLabel>
#include "clickablelabel.h"
#include <QScrollArea>
#include <QPointer>
#include <QPropertyAnimation>
#include <QFileInfo>
#include <QTransform>
#include <QSvgWidget>
#include <QMovie>
#include <QGraphicsEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QWidgetAction>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QNetworkRequest>
#include <QGraphicsView>
#include <QSequentialAnimationGroup>
#include <QStyle>
#include <QSignalBlocker>
#include <QSlider>
#include "volumepopup.h"
#include <QPushButton>
#include <QRegularExpression>
#include "scrolllabel.h"
#include "aibutton.h"
#include "mainwindow.h"
#include "covercache.h"

class VolumePopup;
namespace Ui {
class Lyric;
}

class Lyric : public QWidget
{
    Q_OBJECT

public:
    explicit Lyric(QWidget *parent = nullptr);
    ~Lyric();
    void initUI();
    QSlider* lyricSlider() const;
    QPushButton *lyricplay() const;
    QPushButton *lyricmode() const;
    QLabel *lyricduration() const;
    QSlider *getvolumeSlider() const;

    void updatelyric(const QString& title, const QString& lyricPath);
    void loadLyric(const QString& title, const QString& filePath);
    void setBackgroundGradient(const QColor& color1, const QColor& color2);
    void applyAnimationToAllButtons();
    void handleVolumeChange(int volume);
    void toggleMuteState();
public slots:
    void updateLyricPosition(qint64 position);
    void setSliderPosition(int position);
    void onOpenChatWindowClicked(const QPoint &btnPos);
    //音量
    void setVolume(int volume);
    void setMuted(bool muted);
    void updateVolumeIcon(int volume);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
private slots:
    void toggleMaximize();
    void changeTheme(QAction *action);
    void updateSliderStyle(const QColor& themeColor1, const QColor& themeColor2);

signals:
    void switchnormal();
    void maxwindow();
    void minwindow();
    void lyricClicked(qint64 position);
    void playPauseRequested();
    void previousRequested();
    void nextRequested();
    void playModeChanged();
    void lyricSliderMoved(int position);
    void curplaylistRequested();
    void addlistRequested();
    //音量
    void volumeChanged(int volume);
    void muteToggled(bool muted);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void updateCover(const QString& key, const QPixmap& cover);
    void createControlButtons();
    void connectSliders();
    void processLyricData(const QByteArray &data);
     QNetworkAccessManager* m_networkManager;

    Ui::Lyric *ui;
    QPoint dragPosition;
    QToolButton *btnMin;
    QToolButton *btnMax;
    QToolButton *btnClose;
    // 音量
    VolumePopup* m_volumePopup = nullptr;
    QTimer* m_hoverTimer;
    bool m_isHovering = false;
    bool m_popupVisible = false;
    bool m_isMuted = false;
    int m_lastVolume = 50;

    AIButton *aibtn;
    MainWindow *aiwindow = nullptr;

    bool isMax = false;

    QMediaPlayer *m_player;
    QVBoxLayout *m_lyricLayout;
    QMenu *m_themeMenu;
    QToolButton *m_themeButton;

    QColor m_bgColor1 = QColor(30, 30, 30);
    QColor m_bgColor2 = QColor(60, 60, 60);

    QMap<qint64, QString> m_lyricMap;
    QList<qint64> m_lyricTimes;
    QList<QLabel*> m_lyricLabels;
    QLabel* m_currentHighlightLabel = nullptr;

    QGraphicsDropShadowEffect *m_titleShadow;
    QGraphicsDropShadowEffect *m_coverShadow;
    QGraphicsScene *m_coverScene;
    QGraphicsPixmapItem *m_coverItem;

    int findLyricIndex(qint64 position) const;
    void highlightCurrentLyric(int index);
    QString findAlbumCover(const QString &lyricPath);


    void setupThemeMenu();
    void clearLyricLayout();
    void showNoLyricHint(const QString& message);
    void handleLyricClick(ClickableLabel* label);
    void setAlbumCover(const QPixmap &cover);


    void setRotation(int angle);
    void updateCoverTransform();
};
