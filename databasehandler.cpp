#include "databasehandler.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseHandler::DatabaseHandler(QObject *parent) : QObject(parent),
    m_model(nullptr), m_historyModel(nullptr)
{
}

DatabaseHandler::~DatabaseHandler()
{
    if (m_model) delete m_model;
    if (m_historyModel) delete m_historyModel;
    m_db.close();
}


// 获取歌曲是否为云端文件
bool DatabaseHandler::isCloudSong(int songId) {
    QSqlQuery query;
    query.prepare("SELECT is_cloud FROM songs WHERE id = ?");
    query.addBindValue(songId);
    return query.exec() && query.next() && query.value(0).toBool();
}

// 获取歌曲路径和来源类型
QPair<QString, bool> DatabaseHandler::getSongPathAndType(int songId) {
    QSqlQuery query;
    query.prepare("SELECT file_path, is_cloud FROM songs WHERE id = ?");
    query.addBindValue(songId);

    if (query.exec() && query.next()) {
        return qMakePair(query.value(0).toString(), query.value(1).toBool());
    }
    return qMakePair(QString(), false);
}


bool DatabaseHandler::initDatabase(const QString &host,
                                 const QString &dbName,
                                 const QString &user,
                                 const QString &password,
                                 int port)
{   
    // 检查MySQL驱动是否可用
    if (!QSqlDatabase::isDriverAvailable("QMYSQL")) {
        qCritical() << "QMYSQL驱动不可用\n可用驱动:" << QSqlDatabase::drivers().join(", ");
        return false;
    }

    m_db = QSqlDatabase::addDatabase("QMYSQL");
    m_db.setHostName(host);
    m_db.setPort(port);
    m_db.setDatabaseName(dbName);
    m_db.setUserName(user);
    m_db.setPassword(password);


    // 设置连接超时
    m_db.setConnectOptions("MYSQL_OPT_CONNECT_TIMEOUT=3");
    if (!m_db.open()) {
        qCritical() << "MySQL连接失败:" << m_db.lastError().text();
        return false;
    }
    // 创建表
    if (!createTables()) {
        return false;
    }
    // 初始化模型
    m_model = new QSqlTableModel(nullptr, m_db);
    m_model->setTable("songs");
    m_model->select();
    m_historyModel = new QSqlTableModel(nullptr, m_db);
    m_historyModel->setTable("play_history");
    m_historyModel->select();
    return true;
}

bool DatabaseHandler::createTables()
{
    // 启用外键约束
    QSqlQuery foreignKeyQuery("SET FOREIGN_KEY_CHECKS=1", m_db);
    if (!foreignKeyQuery.exec()) {
        qCritical() << "无法启用外键约束:" << foreignKeyQuery.lastError().text();
        return false;
    }

    // 创建各个表
    if (!createSongsTable()) return false;
    if (!createPlayHistoryTable()) return false;
    if (!createFavoritesTable()) return false;
    if (!createPlaylistsTable()) return false;
    if (!createPlaylistSongsTable()) return false;

    return true;
}

bool DatabaseHandler::createSongsTable()
{
    QSqlQuery query(m_db);
    QString sql = "CREATE TABLE IF NOT EXISTS songs ("
                     "id INT AUTO_INCREMENT PRIMARY KEY, "
                     "file_path VARCHAR(512) NOT NULL, "
                     "title VARCHAR(255), "
                     "artist VARCHAR(255), "
                     "album VARCHAR(255), "
                     "genre VARCHAR(255), "
                     "language VARCHAR(100), "
                     "duration INT, "
                     "is_cloud TINYINT(1) NOT NULL DEFAULT 0, "  // 新增：0=本地,1=云端
                     "UNIQUE KEY (file_path)) "
                     "ENGINE=InnoDB DEFAULT CHARSET=utf8mb4";

    if (!query.exec(sql)) {
        qCritical() << "创建歌曲表失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseHandler::createPlayHistoryTable()
{
    QSqlQuery query(m_db);
    QString sql = "CREATE TABLE IF NOT EXISTS play_history ("
                 "id INT AUTO_INCREMENT PRIMARY KEY, "
                 "song_id INT NOT NULL, "
                 "play_count INT DEFAULT 1, "
                 "play_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
                 "FOREIGN KEY (song_id) REFERENCES songs(id) ON DELETE CASCADE) "
                 "ENGINE=InnoDB DEFAULT CHARSET=utf8mb4";

    if (!query.exec(sql)) {
        qCritical() << "创建播放记录表失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseHandler::createFavoritesTable()
{
    QSqlQuery query(m_db);
    QString sql = "CREATE TABLE IF NOT EXISTS favorites ("
                 "id INT AUTO_INCREMENT PRIMARY KEY, "
                 "song_id INT NOT NULL, "
                 "FOREIGN KEY (song_id) REFERENCES songs(id) ON DELETE CASCADE, "
                 "UNIQUE KEY (song_id)) "
                 "ENGINE=InnoDB DEFAULT CHARSET=utf8mb4";

    if (!query.exec(sql)) {
        qCritical() << "创建收藏表失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseHandler::createPlaylistsTable()
{
    QSqlQuery query(m_db);
    QString sql = "CREATE TABLE IF NOT EXISTS playlists ("
                 "id INT AUTO_INCREMENT PRIMARY KEY, "
                 "name VARCHAR(255) NOT NULL, "
                 "UNIQUE KEY (name)) "
                 "ENGINE=InnoDB DEFAULT CHARSET=utf8mb4";

    if (!query.exec(sql)) {
        qCritical() << "创建歌单表失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseHandler::createPlaylistSongsTable()
{
    QSqlQuery query(m_db);
    QString sql = "CREATE TABLE IF NOT EXISTS playlist_songs ("
                 "playlist_id INT NOT NULL, "
                 "song_id INT NOT NULL, "
                 "FOREIGN KEY (playlist_id) REFERENCES playlists(id) ON DELETE CASCADE, "
                 "FOREIGN KEY (song_id) REFERENCES songs(id) ON DELETE CASCADE, "
                 "PRIMARY KEY (playlist_id, song_id)) "
                 "ENGINE=InnoDB DEFAULT CHARSET=utf8mb4";

    if (!query.exec(sql)) {
        qCritical() << "创建歌单歌曲关系表失败:" << query.lastError().text();
        return false;
    }
    return true;
}

QSqlTableModel* DatabaseHandler::getSongModel()
{
    if (!m_model) {
        m_model = new QSqlTableModel(nullptr, m_db);
        m_model->setTable("songs");
        if (!m_model->select()) {
            qWarning() << "无法加载歌曲模型:" << m_model->lastError().text();
        }
    }
    return m_model;
}

void DatabaseHandler::printSongsTable()
{
    QSqlQuery query("SELECT * FROM songs");
    if (!query.exec()) {
        qWarning() << "无法查询歌曲表:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        int songId = query.value(0).toInt();
        QString filePath = query.value(1).toString();
        QString title = query.value(2).toString();
        QString artist = query.value(3).toString();
        QString album = query.value(4).toString();
        QString genre = query.value(5).toString();
        QString language = query.value(6).toString();
        int duration = query.value(7).toInt();

    }
}

QSqlTableModel* DatabaseHandler::getHistoryModel()
{
    return m_historyModel;
}

bool DatabaseHandler::addSong(const QString &filePath,
                            bool isCloud,
                            const QString &title,
                            const QString &artist,
                            const QString &album,
                            const QString &genre,
                            const QString &language,
                            int duration)
{
    QSqlQuery query;
    query.prepare("INSERT INTO songs "
                 "(file_path, is_cloud, title, artist, album, genre, language, duration) "
                 "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(filePath);
    query.addBindValue(isCloud);  // 新增云端标识
    query.addBindValue(title);
    query.addBindValue(artist);
    query.addBindValue(album);
    query.addBindValue(genre);
    query.addBindValue(language);
    query.addBindValue(duration);

    return query.exec();
}

bool DatabaseHandler::deleteSong(int songId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM songs WHERE id = ?");
    query.addBindValue(songId);

    if (!query.exec()) {
        qWarning() << "删除歌曲失败:" << query.lastError().text();
        return false;
    }

    m_model->select();
    m_historyModel->select();
    return true;
}

int DatabaseHandler::getSongCount()
{
    QSqlQuery query("SELECT COUNT(*) FROM songs");
    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

bool DatabaseHandler::addPlayRecord(int songId)
{
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT id FROM play_history WHERE song_id = ?");
    checkQuery.addBindValue(songId);

    if (!checkQuery.exec()) {
        qWarning() << "查询播放记录失败:" << checkQuery.lastError().text();
        return false;
    }

    if (checkQuery.next()) {
        return updatePlayRecord(songId);
    } else {
        QSqlQuery query;
        query.prepare("INSERT INTO play_history (song_id) VALUES (?)");
        query.addBindValue(songId);

        if (!query.exec()) {
            qWarning() << "添加播放记录失败:" << query.lastError().text();
            return false;
        }
    }

    m_historyModel->select();
    return true;
}

bool DatabaseHandler::updatePlayRecord(int songId)
{
    QSqlQuery query;
    query.prepare("UPDATE play_history SET "
                 "play_count = play_count + 1, "
                 "play_time = CURRENT_TIMESTAMP "
                 "WHERE song_id = ?");
    query.addBindValue(songId);

    if (!query.exec()) {
        qWarning() << "更新播放记录失败:" << query.lastError().text();
        return false;
    }

    m_historyModel->select();
    return true;
}

QSqlQueryModel* DatabaseHandler::getHistorySummaryModel()
{
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery("SELECT song_id, SUM(play_count) as total_plays, MAX(play_time) as last_play_time "
                   "FROM play_history "
                   "GROUP BY song_id "
                   "ORDER BY last_play_time DESC");

    model->setHeaderData(0, Qt::Horizontal, tr("歌曲ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("播放次数"));
    model->setHeaderData(2, Qt::Horizontal, tr("最后播放时间"));

    return model;
}

bool DatabaseHandler::addToFavorites(int songId)
{
    QSqlQuery query;
    query.prepare("INSERT IGNORE INTO favorites (song_id) VALUES (?)");
    query.addBindValue(songId);

    if (!query.exec()) {
        qWarning() << "添加收藏失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseHandler::removeFromFavorites(int songId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM favorites WHERE song_id = ?");
    query.addBindValue(songId);

    if (!query.exec()) {
        qWarning() << "取消收藏失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseHandler::isFavorite(int songId)
{
    QSqlQuery query;
    query.prepare("SELECT 1 FROM favorites WHERE song_id = ?");
    query.addBindValue(songId);

    if (!query.exec()) {
        qWarning() << "查询收藏状态失败:" << query.lastError().text();
        return false;
    }
    return query.next();
}

QSqlTableModel* DatabaseHandler::getFavoritesModel()
{
    QSqlTableModel *model = new QSqlTableModel(nullptr, m_db);
    model->setTable("favorites");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    return model;
}


bool DatabaseHandler::addPlaylist(const QString &name)
{
    QSqlQuery query;
    query.prepare("INSERT IGNORE INTO playlists (name) VALUES (?)");
    query.addBindValue(name);

    if (!query.exec()) {
        qWarning() << "添加歌单失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseHandler::deletePlaylist(int playlistId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM playlists WHERE id = ?");
    query.addBindValue(playlistId);

    if (!query.exec()) {
        qWarning() << "删除歌单失败:" << query.lastError().text();
        return false;
    }
    return true;
}


QSqlTableModel* DatabaseHandler::getPlaylistsModel()
{
    QSqlTableModel *model = new QSqlTableModel(nullptr, m_db);
    model->setTable("playlists");
    model->select();

    model->setHeaderData(model->fieldIndex("name"), Qt::Horizontal, tr("歌单名称"));

    return model;
}


bool DatabaseHandler::addSongToPlaylist(int playlistId, int songId)
{
    QSqlQuery query;
    query.prepare("INSERT IGNORE INTO playlist_songs (playlist_id, song_id) VALUES (?, ?)");
    query.addBindValue(playlistId);
    query.addBindValue(songId);

    if (!query.exec()) {
        qWarning() << "向歌单添加歌曲失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseHandler::removeSongFromPlaylist(int playlistId, int songId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM playlist_songs WHERE playlist_id = ? AND song_id = ?");
    query.addBindValue(playlistId);
    query.addBindValue(songId);

    if (!query.exec()) {
        qWarning() << "从歌单删除歌曲失败:" << query.lastError().text();
        return false;
    }
    return true;
}


QSqlQueryModel* DatabaseHandler::getPlaylistSongsModel(int playlistId)
{
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(QString("SELECT s.id, s.file_path, s.title, s.artist, s.album, s.duration "
                            "FROM playlist_songs ps "
                            "JOIN songs s ON ps.song_id = s.id "
                            "WHERE ps.playlist_id = %1").arg(playlistId));

    model->setHeaderData(0, Qt::Horizontal, tr("歌曲ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("文件路径"));
    model->setHeaderData(2, Qt::Horizontal, tr("标题"));
    model->setHeaderData(3, Qt::Horizontal, tr("艺术家"));
    model->setHeaderData(4, Qt::Horizontal, tr("专辑"));
    model->setHeaderData(5, Qt::Horizontal, tr("时长"));

    return model;
}

