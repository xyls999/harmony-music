#ifndef VIDEOMODELCLIENTTWO_H
#define VIDEOMODELCLIENTTWO_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QDebug>
#include <QPointer>
#include <QTimer>
#include <QMutex>
#include <QtMultimedia>
#include <QMediaPlayer>
#include <QMediaRecorder>
#include <QVideoEncoderSettings>
#include <QEventLoop>

class VideoModelClientTwo : public QObject
{
    Q_OBJECT

public:
    explicit VideoModelClientTwo(QObject *parent = nullptr);
    ~VideoModelClientTwo();

    void sendRequest(const QString &filePath, const QString &question);
    void askQuestionAfterUpload(const QString &question);
    void uploadAudioFragment(const QString &filePath);
    void processChunk(const QString &jsonString);
    void clearConversationHistory();
    void onUploadFinished();
    void resetConversationHistory();
    double getVideoDuration(const QString &filePath);


signals:
    void partialResponseReceived(const QString &content);
    void responseReceived(const QString &response);
    void errorOccurred(const QString &error);
    void allRequestsFinished();
    void uploadProgress(int current, int total);
    void progressUpdated(int current, int total);


private slots:
    void onReadyRead();
    void onReplyFinished();

private:
    QNetworkAccessManager *networkManager;
    QJsonArray conversationHistory;
    QString fullContent;
    QString tempDir;
    QString partialData;
    QStringList m_uploadedFilePaths;
    QString buffer;
    int totalFragments = 0;
    int uploadedFragments = 0;
    int m_videoTotalFragments = 0;
    int m_videoUploadedFragments = 0;
    QStringList m_currentVideoFragments;
    QSet<QString> m_uploadedFragments;
    QMap<QNetworkReply*, QString> m_activeUploads;
    int uploadedVideoFragments = 0;
    int totalVideoFragments = 0;


    QJsonArray buildMessages(const QString &question, const QString &filePath, bool isVideo);
    void sendSingleRequest(const QString &filePath, const QString &question, bool isVideo);
    void cleanupTempDir();
    QStringList splitVideoIfNeeded(const QString &filePath);
    QStringList splitAudioIfNeeded(const QString &filePath);
    QString cropVideoIfNeeded(const QString &filePath);
    void onVideoUploadFinished();
    void uploadVideoFragment(const QString &filePath);
    bool waitForMediaLoad(QMediaPlayer *player);
    QByteArray safeReadFile(const QString &path);


};

#endif
