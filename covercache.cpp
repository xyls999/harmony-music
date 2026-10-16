#include "covercache.h"
#include <QMutex>
#include <QMutexLocker>

CoverCacheManager* CoverCacheManager::instance()
{
    static CoverCacheManager* instance = new CoverCacheManager();
    return instance;
}

CoverCacheManager::CoverCacheManager(QObject* parent)
    : QObject(parent), networkManager(new QNetworkAccessManager(this))
{
    connect(networkManager, &QNetworkAccessManager::finished, this, &CoverCacheManager::onCoverDownloadFinished);
}

CoverCacheManager::~CoverCacheManager()
{
    delete networkManager;
}

QPixmap CoverCacheManager::getCover(const QString& key)
{
    if(coverCache.contains(key)){
        return coverCache.value(key);
    }
    else {
        return  QPixmap(":/images/recon/loading.png");
    }

}

void CoverCacheManager::loadCover(const QString& key, const QUrl& url)
{
//    qDebug()<<"下载封面";
    if (coverCache.contains(key)) {
            emit coverLoaded(key, coverCache.value(key));  // 立即通知
            return;
        }

//    qDebug() << "Requesting cover for:" << key << "URL:" << url;
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::User, QVariant(key));
    networkManager->get(request);
}

// covercache.cpp
void CoverCacheManager::onCoverDownloadFinished(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "封面下载失败:" << reply->url() << reply->errorString();
        reply->deleteLater();
        return;
    }

    QString key = reply->request().attribute(QNetworkRequest::User).toString();
    QByteArray imageData = reply->readAll();
    reply->deleteLater();

    QPixmap cover;
    if (cover.loadFromData(imageData)) {
        coverCache.insert(key, cover);
        emit coverLoaded(key, cover); // 确保发出信号
    }
}
