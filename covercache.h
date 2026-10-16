#ifndef COVERCACHE_H
#define COVERCACHE_H

#include <QPixmap>
#include <QHash>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>

class CoverCacheManager : public QObject
{
    Q_OBJECT

public:
    static CoverCacheManager* instance();
    QPixmap getCover(const QString& key);
    void loadCover(const QString& key, const QUrl& url);


signals:
    void coverLoaded(const QString& key, const QPixmap& cover);

private:
    explicit CoverCacheManager(QObject* parent = nullptr);
    ~CoverCacheManager() override;

    QHash<QString, QPixmap> coverCache;
    QNetworkAccessManager* networkManager;

private slots:
    void onCoverDownloadFinished(QNetworkReply* reply);
};

#endif // COVERCACHE_H
