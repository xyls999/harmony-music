#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QListWidget>
#include <QTimer>
#include <QScrollBar>
#include <taglib/tag.h>
#include <QListWidget>
#include <QSqlQuery>
#include <taglib/fileref.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/flacpicture.h>
#include <taglib/flacfile.h>
#include <QStandardPaths>
#include <QMediaContent>
#include <QListWidgetItem>
#include <QToolButton>
#include <QFrame>
#include <QSqlError>
#include <QSqlQuery>
#include <QMetaObject>
#include <QPointer>
#include <QTableView>
#include <QStyledItemDelegate>
#include <QSqlError>
#include <QThread>
#include <QSequentialAnimationGroup>
#include <QProgressDialog>
#include <QInputDialog>
#include <QtConcurrent/QtConcurrentRun>
#include "thememanager.h"
#include <QColorDialog>
#include <QFutureWatcher>
#include "recommender.h"
#include <QPair>
#include <QUrl>
#include "videoplayer.h"
#include "rrecommendwidget.h"
#include "musicitemwidget.h"
#include "footwidget.h"
#include "databasehandler.h" // 添加数据库处理类的头文件
struct SongData {
    int songId;
    QUrl url;
    QString title;
    QString artist;
    Q_GADGET
    Q_PROPERTY(int songId MEMBER songId)
    Q_PROPERTY(QUrl url MEMBER url)
    Q_PROPERTY(QString title MEMBER title)
    Q_PROPERTY(QString artist MEMBER artist)
};
Q_DECLARE_METATYPE(SongData)
Q_DECLARE_METATYPE(QVector<SongData>)


namespace Ui {
class mainpage;
}

class mainpage : public QWidget
{
    Q_OBJECT

public:
    explicit mainpage(DatabaseHandler *db, QWidget *parent = nullptr, footWidget *footWidget = nullptr);
    ~mainpage();
    void initUI();
    void updateFavoriteState(bool isFavorite);
    void loadVideoList();
    void updateRecommendationUI(const QVector<int>& songIds);
    void updateFavorCover(const QPixmap &cover);


signals:
    void playMusicAtIndex(int index);
    void musicItemLiked(bool liked, int songId);
protected:
    void keyPressEvent(QKeyEvent* event);
    bool eventFilter(QObject *obj, QEvent *event) override;

public slots:
    void onMusicLiked(bool liked, int songid);
    void highlightCurrentSong(int index);
    void onThemeChanged();
    void showThemeMenu();
    void onAddPlaylistClicked();
    void handleAddToPlaylistRequest(int songId);
    void onMusicItemDoubleClicked(QListWidgetItem *item);
    void onPlayAllClicked();
    void onPlayAllFromList(QListWidget* targetList, int startIndex);
private slots:
    void on_btnLocal_clicked();
    void on_btnFlavor_clicked();
    void on_btnCurPlay_clicked();
    void performSearch();
    void restoreOriginalList();
    void showSearchSuggestions();
    void on_reconbtn_clicked();

    void on_vedio_doubleclicked();

    void on_btnvideo_clicked();

signals:
    void maxwindow();
    void minwindow();

private:
    Ui::mainpage *ui;
    footWidget *m_footWidget;
    QListWidget *localMusicWidget;
    QListWidget *curPlayWidget;
    QListWidget *favorWidget;
    QListWidget *playlistwidget;
    QWidget *reconwidget;
    QSet<int> favoriteSongs;
    QListWidgetItem* lastHighlightedItem = nullptr;
    QListWidget *searchResultsWidget;
    QStringList searchHistory;
    QListWidget *videoWidget;
    int previousViewIndex = 0;
    QListWidget *suggestionsPopup = nullptr;

    QNetworkAccessManager *coverNetworkManager;
    QMap<QString, QPixmap> coverCache;
    VideoPlayer *m_videoPlayer;

    int m_firstFavorSongId = -1;
    QPixmap m_defaultCover;

    QMediaPlaylist* getCachedPlaylist(QListWidget* listWidget);
    QMediaPlaylist* createPlaylistFromListWidget(QListWidget* listWidget);
    void cachePlaylist(QListWidget* listWidget, QMediaPlaylist* playlist);

    QListWidget *currentPlayList = nullptr;
    DatabaseHandler *m_dbHandler; // 添加数据库处理对象
    QMap<QListWidget*, QMediaPlaylist*> m_playlistCache;

    Recommender *recommender;


    bool isSystemInDarkMode();
    void createAnimationLabel(const QString &text);
    void applyAnimationToAllButtons();
    void init_musicLists();
    void  updateSuggestionsPopupTheme();
    void updateSongLikeState(int songId, bool isLiked);
    void updateProgressBarTheme();
    void setupListWidget(QListWidget *listWidget);
    void addToFavorites(int songid);
    void removeFromFavorites(int songid);
    bool isSongInFavorites(int songId);
    void loadMusicFromDatabase(QListWidget *listWidget);
    void loadPlayHistory(QListWidget *listWidget);
    void loadFavorites(QListWidget *listWidget);
    void migrateOldDatabasePaths();
    std::tuple<QString, QString, QString, QString, QPixmap> extractMusicInfo(const QFileInfo &file);
    QPixmap extractCover(const QString &songName);
    void updateAllListLikedState(int songid, bool state);
    void switchToList(QListWidget *newList);
    void updatePlayRecord(int songId);
    ThemeManager& m_theme = ThemeManager::instance();
    void  updateIcons();
    void updateSearchResultsTheme();
    void updateSearchEdit();
    bool isCurrentPlayingSong(int songId) const;
    QString getMusicLibraryBaseDir() const {
        return QStandardPaths::writableLocation(QStandardPaths::MusicLocation);

    }
    void onCoverDownloadFinished(QNetworkReply *reply);
    void updateAllCovers(const QString &coverUrl, const QPixmap &cover);
    void saveSearchHistory(const QString& keyword);
    void highlightMatches(MusicItemWidget* widget, const QString& keyword);
    void loadPlaylists();
    void onPlaylistSelected(const QModelIndex &current, const QModelIndex &previous);
    QStringList getPlaylistNames();
    int getPlaylistIdByName(const QString &name);
    void addSongToPlaylist(int playlistId, int songId);
    QFutureWatcher<QPair<QList<int>, QList<QUrl>>> m_playlistLoaderWatcher;
    int m_pendingIndex = -1;
    void refreshAllCovers();

};

#endif // MAINPAGE_H
