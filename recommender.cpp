#include "recommender.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVector>
#include <QMap>
#include <QDateTime>
#include <cmath>
#include <algorithm>
#include <QSet>

Recommender:: Recommender(DatabaseHandler *dbHandler ){
   m_dbHandler = dbHandler;
   connect(CoverCacheManager::instance(), &CoverCacheManager::coverLoaded,
           this, &Recommender::handleCoverLoaded);
}



void Recommender::handleCoverLoaded(const QString &key, const QPixmap &cover) {
    // 将封面加载成功后通知推荐小部件更新 UI
    emit coverLoaded(key, cover);
}
// 计算时间衰减权重
double Recommender::timeDecayWeight(const QDateTime &referenceTime, double decayFactor)
{
    QDateTime now = QDateTime::currentDateTime();
    qint64 secondsAgo = now.secsTo(referenceTime);
    return exp(-decayFactor * secondsAgo / 86400.0); // 转换为天数
}
QString Recommender::getTitleBySongId(int songId)
{
    QSqlQuery query;
    query.prepare("SELECT title FROM songs WHERE id = :songId");
    query.bindValue(":songId", songId);

    if (!query.exec()) {
        qWarning() << "查询 title 失败:" << query.lastError().text();
        return "";
    }

    if (query.next()) {
        return query.value("title").toString();
    }

    qWarning() << "未找到对应的 title，songId:" << songId;
    return "";
}

QPixmap Recommender::getCoverBySongId(int songId) {
    QString title = getTitleBySongId(songId);
    if (title.isEmpty()) {
        return QPixmap(":/images/recon/loading.png");  // 默认封面
    }

    QPixmap cover = CoverCacheManager::instance()->getCover(title);
    if (cover.isNull()) {
        QUrl coverUrl(QString("http://vip.frpee.com:13678/music/cover/%1.jpg").arg(title));
        if (coverUrl.isValid()) {
            CoverCacheManager::instance()->loadCover(title, coverUrl);
        }
    }
    return cover;
}
QVector<int> Recommender::generateRecommendations(int userId, int numRecommendations, double decayFactor)
{
    // 获取用户最近播放的歌曲及其权重
    QMap<int, double> userPlayWeights = getUserPlayWeights(userId, decayFactor);

    // 获取用户收藏的歌曲
    QSet<int> userFavorites = getUserFavorites(userId);

    // 合并权重
    QMap<int, double> combinedWeights;
    for (auto it = userPlayWeights.begin(); it != userPlayWeights.end(); ++it) {
        combinedWeights[it.key()] += it.value();
    }
    for (int songId : userFavorites) {
        if (combinedWeights.contains(songId)) {
            combinedWeights[songId] += 2.0; // 收藏权重加倍
        } else {
            combinedWeights[songId] = 2.0;
        }
    }

    if (combinedWeights.isEmpty()) {
        qDebug() << "用户没有足够的播放历史或收藏，无法生成推荐";
        return QVector<int>();
    }

    // 获取所有歌曲的特征向量
    QMap<int, QVector<double>> songFeatures = getAllSongFeatures();

    // 计算每首歌曲的加权相似度
    QMap<int, double> similarityScores;
    for (auto it = combinedWeights.begin(); it != combinedWeights.end(); ++it) {
        int songId = it.key();
        double weight = it.value();

        // 获取用户权重歌曲的特征
        if (!songFeatures.contains(songId)) continue;
        QVector<double> userSongFeatures = songFeatures[songId];

        // 计算与其他所有歌曲的相似度
        for (auto featureIt = songFeatures.begin(); featureIt != songFeatures.end(); ++featureIt) {
            int targetSongId = featureIt.key();
            if (targetSongId == songId) continue; // 跳过自身

            double similarity = cosineSimilarity(userSongFeatures, featureIt.value());
            similarityScores[targetSongId] += similarity * weight;
        }
    }

    // 排序并返回推荐列表
    QVector<std::pair<int, double>> scoredSongs;
    for (auto it = similarityScores.begin(); it != similarityScores.end(); ++it) {
        scoredSongs.append({it.key(), it.value()});
    }

    // 按分数排序
    std::sort(scoredSongs.begin(), scoredSongs.end(),
              [](const std::pair<int, double> &a, const std::pair<int, double> &b) {
                  return a.second > b.second;
              });

    // 提取前 numRecommendations 首歌曲的 ID
    QVector<int> recommendations;
    for (int i = 0; i < std::min(numRecommendations, scoredSongs.size()); ++i) {
        recommendations.append(scoredSongs[i].first);
    }
    return recommendations;
}

// 获取用户播放历史及其权重
QMap<int, double> Recommender::getUserPlayWeights(int userId, double decayFactor)
{
    QMap<int, double> playWeights;

    QSqlQuery query;
    query.prepare("SELECT song_id, SUM(play_count) AS total_play_count, MAX(play_time) AS last_play_time  "
                 "FROM play_history "
                 "GROUP BY song_id "
                 "ORDER BY last_play_time DESC");

    if (!query.exec()) {
        qWarning() << "获取播放历史失败:" << query.lastError().text();
        return playWeights;
    }

    while (query.next()) {
        int songId = query.value(0).toInt();
        int playCount = query.value(1).toInt();
        QDateTime lastPlayTime = query.value(2).toDateTime();

        // 计算时间衰减权重
        double timeWeight = timeDecayWeight(lastPlayTime, decayFactor);

        // 综合播放次数和时间衰减
        playWeights[songId] = playCount * timeWeight;
    }

    return playWeights;
}

// 获取用户收藏的歌曲
QSet<int> Recommender::getUserFavorites(int userId)
{
    QSet<int> favoriteSongs;

    QSqlQuery query;
    query.prepare("SELECT song_id FROM favorites");

    if (!query.exec()) {
        qWarning() << "获取收藏失败:" << query.lastError().text();
        return favoriteSongs;
    }

    while (query.next()) {
        int songId = query.value(0).toInt();
        favoriteSongs.insert(songId);
    }

    return favoriteSongs;
}

// 获取所有歌曲的特征向量（只使用 genre, artist, language）
QMap<int, QVector<double>> Recommender::getAllSongFeatures()
{
    QMap<int, QVector<double>> songFeatures;
    QMap<QString, int> genreMap;
    QMap<QString, int> artistMap;
    QMap<QString, int> languageMap;

    // 1. 获取所有 genre、artist、language 的唯一值
    QSqlQuery uniqueQuery;
    if (!uniqueQuery.exec("SELECT DISTINCT genre, artist, language FROM songs")) {
        qWarning() << "获取唯一特征失败:" << uniqueQuery.lastError().text();
        return songFeatures;
    }

    while (uniqueQuery.next()) {
        QString genre = uniqueQuery.value(0).toString();
        QString artist = uniqueQuery.value(1).toString();
        QString language = uniqueQuery.value(2).toString();

        if (!genre.isEmpty() && !genreMap.contains(genre)) {
            genreMap[genre] = genreMap.size();
        }
        if (!artist.isEmpty() && !artistMap.contains(artist)) {
            artistMap[artist] = artistMap.size();
        }
        if (!language.isEmpty() && !languageMap.contains(language)) {
            languageMap[language] = languageMap.size();
        }
    }

    // 2. 为每首歌曲构建特征向量
    QSqlQuery songQuery("SELECT id, genre, artist, language FROM songs");
    if (!songQuery.exec()) {
        qWarning() << "获取歌曲特征失败:" << songQuery.lastError().text();
        return songFeatures;
    }

    while (songQuery.next()) {
        int songId = songQuery.value(0).toInt();
        QString genre = songQuery.value(1).toString();
        QString artist = songQuery.value(2).toString();
        QString language = songQuery.value(3).toString();

        // 初始化特征向量（确保大小正确）
        int totalFeatures = genreMap.size() + artistMap.size() + languageMap.size();
        QVector<double> features(totalFeatures, 0.0);

        // 设置 genre 特征（检查索引是否有效）
        if (genreMap.contains(genre)) {
            int index = genreMap[genre];
            if (index >= 0 && index < features.size()) {
                features[index] = 1.0;
            }
        }

        // 设置 artist 特征
        if (artistMap.contains(artist)) {
            int index = genreMap.size() + artistMap[artist];
            if (index >= 0 && index < features.size()) {
                features[index] = 1.0;
            }
        }

        // 设置 language 特征
        if (languageMap.contains(language)) {
            int index = genreMap.size() + artistMap.size() + languageMap[language];
            if (index >= 0 && index < features.size()) {
                features[index] = 1.0;
            }
        }

        songFeatures[songId] = features;
    }

    return songFeatures;
}
// 计算两首歌曲的相似度（使用余弦相似度）
double Recommender::cosineSimilarity(const QVector<double> &vec1, const QVector<double> &vec2)
{
    double dotProduct = 0.0;
    double normA = 0.0;
    double normB = 0.0;

    for (int i = 0; i < vec1.size(); ++i) {
        dotProduct += vec1[i] * vec2[i];
        normA += vec1[i] * vec1[i];
        normB += vec2[i] * vec2[i];
    }

    if (normA == 0 || normB == 0)
        return 0.0;

    return dotProduct / (sqrt(normA) * sqrt(normB));
}
