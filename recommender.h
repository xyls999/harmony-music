#ifndef RECOMMENDER_H
#define RECOMMENDER_H

#include <QObject>
#include <QSqlDatabase>
#include <QVector>
#include <QMap>
#include <QPair>
#include <QDateTime>
#include <QSet>
#include <QPointer>
#include <QPixmap>
#include <covercache.h>
#include "databasehandler.h"

class Recommender : public QObject
{
    Q_OBJECT
public:
    explicit Recommender(DatabaseHandler *dbHandler);
    // 计算余弦相似度
    double cosineSimilarity(const QVector<double> &vec1, const QVector<double> &vec2);

    // 计算时间衰减权重
    double timeDecayWeight(const QDateTime &referenceTime, double decayFactor);

    // 生成推荐列表
    QVector<int> generateRecommendations(int userId, int numRecommendations, double decayFactor = 0.1);

    QString getTitleBySongId(int songId);
    QPixmap getCoverBySongId(int songId);

    void updateCover(const QString& key, const QPixmap& cover);
private slots:
    void handleCoverLoaded(const QString &key, const QPixmap &cover);
signals:
    void coverLoaded(const QString &key,const QPixmap &cover);
private:
    DatabaseHandler *m_dbHandler;

    // 获取用户播放历史及其权重
    QMap<int, double> getUserPlayWeights(int userId, double decayFactor);

    // 获取用户收藏的歌曲
    QSet<int> getUserFavorites(int userId);

    // 获取所有歌曲的特征向量（只使用 genre, artist, language）
    QMap<int, QVector<double>> getAllSongFeatures();
};

#endif // RECOMMENDER_H
