#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlTableModel>

class DatabaseHandler : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseHandler(QObject *parent = nullptr);
    ~DatabaseHandler();

    bool initDatabase(const QString &host,
                      const QString &dbName,
                      const QString &user,
                      const QString &password,
                      int port);

    bool createTables();
    bool createSongsTable();
    bool createPlayHistoryTable();
    bool createFavoritesTable();
    bool createPlaylistsTable();
    bool createPlaylistSongsTable();
    bool isCloudSong(int songId);
    QPair<QString, bool> getSongPathAndType(int songId);
    QSqlTableModel* getSongModel();
    QSqlTableModel* getHistoryModel();
    bool addSong(const QString &filePath,
                 bool isCloud,
                 const QString &title,
                 const QString &artist,
                 const QString &album,
                 const QString &genre,
                 const QString &language,
                 int duration);
    bool deleteSong(int songId);
    bool addPlayRecord(int songId);
    int getSongCount();
    bool updatePlayRecord(int songId);
    QSqlQueryModel* getHistorySummaryModel();
    bool addToFavorites(int songId);
    bool removeFromFavorites(int songId);
    bool isFavorite(int songId);
    QSqlTableModel* getFavoritesModel();
    bool addPlaylist(const QString &name);
    bool deletePlaylist(int playlistId);
    QSqlTableModel* getPlaylistsModel();
    bool addSongToPlaylist(int playlistId, int songId);
    bool removeSongFromPlaylist(int playlistId, int songId);
    QSqlQueryModel* getPlaylistSongsModel(int playlistId);
    void printSongsTable();
private:
    QSqlDatabase m_db;
    QSqlTableModel *m_model;
    QSqlTableModel *m_historyModel;
};

#endif // DATABASEHANDLER_H
