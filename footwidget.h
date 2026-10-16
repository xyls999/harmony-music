#ifndef FOOTWIDGET_H
#define FOOTWIDGET_H

#include <QLayout>
#include <QSlider>
#include <QLabel>
#include <DMenu>
#include <QMouseEvent>
#include <QEvent>
#include <DWidget>
#include "volumepopup.h"
#include <QPropertyAnimation>
#include <DGuiApplicationHelper>
#include <DPalette>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QRandomGenerator>
#include <QDebug>
#include <QToolButton>
#include <QAudioOutput>
#include <QListWidget>
#include <QSignalBlocker>
#include <QColor>
#include <QElapsedTimer>
#include "volumepopup.h"
#include "scrolllabel.h"
#include "lyric.h"
#include "playlistitemwidget.h"
#include "databasehandler.h"
#include "thememanager.h"

DWIDGET_USE_NAMESPACE

class VolumePopup;
class Lyric;
struct SongInfo {
    int id;
    QString title;
    QString artist;
    QString duration;
    QPixmap cover;
};

namespace Ui {
class footWidget;
}
class footWidget : public DWidget
{
    Q_OBJECT

public:
    explicit footWidget(QWidget *parent = nullptr, Lyric *lyric = nullptr);
    ~footWidget();
    void initUI();
    void initSpeedControl();
    void setPlaybackSpeed(float speed);
    QTimer *m_hoverTimer; // 悬停计时器
    bool m_popupVisible = false; // Popup当前状态
    QMediaPlaylist* getPlaylist() const;
    QMediaPlayer* getPlayer() const { return m_player; }
    QSlider *slider() const;
    void updateSongInfo(QString title, QString artist);
    QSlider* positionSlider() const;
    QLabel *label() const;
    void togglePlaylistWidget();
    void setFavorite(bool isFavorite);
    void updateCurrentSongFavorite();
    void setfavorbtnstate(bool liked){m_isFavorite = liked;}
    void setCurrentSongId(int id) { m_currentSongId = id; }
    int getsongId() const { return m_currentSongId; }
    void updateCover(const QString& key, const QPixmap& cover);

    void onMusicItemLikeChanged(bool liked, int songId);
    void onPlaylistCurrentIndexChanged(int index);
    bool eventFilter(QObject *obj, QEvent *event) override;
    void setplaybtn();
    void applyAnimationToAllButtons();

public slots:
    void handleVolumeChange(int volume);
    void playMusic(int index);
    void updateTheme(const QColor& bgColor);
    void updatePlaylistView();
    void updatePlaylistHighlight(int currentIndex);
signals:
    void coverClicked();
    void songInfoChanged(const QString& title, const QString& artist);
    void switchlyric();
    void playRecordRequested( int songId);
    void favorBtnClicked(int songId, bool isFavorite);
    void fwaddplaylist(int songid);
    void muteChanged( bool newMuted);
    void volumeChanged(int volume);


private slots:

    void toggleMuteState();

    void on_Playbtn_clicked();

    void on_Prebtn_clicked();

    void on_Modebtn_clicked();

    void on_Nextbtn_clicked();

    void updatePosition(qint64 position);

    void updateDuration(qint64 duration);

    void setPosition(qint64 position);

    void handlePlaylistChanged(int position);

    void on_SpeedButton_clicked();

    void on_coverButton_clicked() { emit coverClicked(); }

    void on_listbtn_clicked();

     void updateSpeedButtonIcon(const QString& theme);

     void on_favorbtn_clicked();

     void on_addlistbtn_clicked();

     void updateCoverIcon(const QPixmap& cover);

private:
    Ui::footWidget *ui;
    Lyric *m_lyric;
    VolumePopup *popup = nullptr;
    QMediaPlayer *m_player;
    QMediaPlaylist *m_playlist;
    QStringList m_songNames;
    QMenu *m_speedMenu;
    QActionGroup *m_speedGroup;
    QMap<float, QAction*> m_speedActions;
    bool m_isMuted = false;
    int m_lastVolume = 50;
    bool m_isHovering = false;
    void updateModeButtonIcon(const QString& theme);
    void updateVolumeButtonIcon();
    void showFavoriteAnimation(bool isFavorite);

    QString transformToSafeIdList(const QList<int>& ids);

    DatabaseHandler* m_dbHandler;
    footWidget* findParentFootWidget() const ;
    QWidget *playlistWidget;
    QListWidget *playlistView;
    int m_currentSongId = -1;
    bool m_isFavorite = false;
    ThemeManager& m_theme = ThemeManager::instance();
    QHash<int, SongInfo> m_songCache;

    QSet<int> m_cachedIds;
    bool loadBatchSongInfo(const QList<int>& songIds, QHash<int, SongInfo>& infos);

};

#endif // FOOTWIDGET_H
