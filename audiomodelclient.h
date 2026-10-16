#ifndef AUDIOMODELCLIENT_H
#define AUDIOMODELCLIENT_H

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

class AudioModelClient : public QObject
{
    Q_OBJECT

public:
    explicit AudioModelClient(QObject *parent = nullptr);
    ~AudioModelClient();

    void sendRequest(const QString &filePath, const QString &question);
    QString cropVideoIfNeeded(const QString &filePath);
    QStringList splitAudioIfNeeded(const QString &filePath);
    void askQuestionAfterUpload(const QString &question);
    void uploadAudioFragment(const QString &filePath);
    void processChunk(const QString &jsonString);
    void clearConversationHistory();
    void onUploadFinished();
    void resetConversationHistory();


signals:
    void partialResponseReceived(const QString &content);
    void responseReceived(const QString &response);
    void errorOccurred(const QString &error);
    void allRequestsFinished();


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

    QJsonArray buildMessages(const QString &question, const QString &filePath, bool isVideo);
    void sendSingleRequest(const QString &filePath, const QString &question, bool isVideo);
    void cleanupTempDir();
};

#endif // AUDIOMODELCLIENT_H
