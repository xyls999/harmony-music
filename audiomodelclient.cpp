#include "audiomodelclient.h"
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>
#include <QFile>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QEventLoop>
#include <QStandardPaths>


AudioModelClient::AudioModelClient(QObject *parent)
    : QObject(parent), networkManager(new QNetworkAccessManager(this)), fullContent("")
{
    //connect(networkManager, &QNetworkAccessManager::finished, this, &AudioModelClient::onReplyFinished);
}


void AudioModelClient::uploadAudioFragment(const QString &filePath) {
    QJsonArray messages = buildMessages("这是我上传的歌曲的其中一个片段，请记住它。", filePath, false);

    QJsonObject json;
    json["model"] = "qwen-omni-turbo";
    json["messages"] = messages;
    json["stream"] = true;

    QByteArray jsonData = QJsonDocument(json).toJson();

    QUrl url("https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions");
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer sk-ff94827cb8e94561af8155a132fd9a62");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = networkManager->post(request, jsonData);
    connect(reply, &QNetworkReply::finished, this, &AudioModelClient::onUploadFinished); // 连接 finished 信号
}

void AudioModelClient::onUploadFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Upload failed:" << reply->errorString();
        QByteArray errorResponse = reply->readAll(); // 捕获服务器响应
        qDebug() << "Server response:" << QString(errorResponse); // 打印响应内容
        emit errorOccurred(reply->errorString());
    } else {
        qDebug() << "Fragment upload completed.";
    }

    reply->deleteLater(); // 清理资源

    // 更新上传计数器
    uploadedFragments++;
    if (uploadedFragments == totalFragments) {
        qDebug() << "All fragments uploaded. Sending question...";
        askQuestionAfterUpload("请分析我刚才上传的所有音频片段。");
    }
}

void AudioModelClient::askQuestionAfterUpload(const QString &question) {
    QJsonArray messages = buildMessages(question, "", false);

    QJsonObject json;
    json["model"] = "qwen-omni-turbo";
    json["messages"] = messages;
    json["stream"] = true; // 启用流式输出

    QByteArray jsonData = QJsonDocument(json).toJson();

    QUrl url("https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions");
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer sk-ff94827cb8e94561af8155a132fd9a62");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = networkManager->post(request, jsonData);
    connect(reply, &QNetworkReply::readyRead, this, &AudioModelClient::onReadyRead); // 流式数据处理
    connect(reply, &QNetworkReply::finished, this, &AudioModelClient::onReplyFinished); // 请求完成处理
}


QString encodeAudio(const QString &audioPath) {
    QFile audioFile(audioPath);
    if (!audioFile.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开音频文件:" << audioPath;
        return "";
    }

    QByteArray fileData = audioFile.readAll();
    QString base64Audio = QString::fromUtf8(fileData.toBase64());
    return base64Audio;
}

QStringList AudioModelClient::splitAudioIfNeeded(const QString &filePath) {
    QFileInfo fileInfo(filePath);
    QDir tempDirObj(QDir::tempPath());
    if (!tempDirObj.exists()) {
        tempDirObj.mkpath(QDir::tempPath());
    }

    // 使用 FFmpeg 检查音频时长
    QProcess process;
    process.start("ffmpeg", {"-i", filePath});
    process.waitForFinished();
    QString output = process.readAllStandardError();

    // 解析音频时长
    qint64 duration = 0;
    QRegularExpression regex("Duration: (\\d+):(\\d+):(\\d+)\\.\\d+");
    QRegularExpressionMatch match = regex.match(output);
    if (match.hasMatch()) {
        int hours = match.captured(1).toInt();
        int minutes = match.captured(2).toInt();
        int seconds = match.captured(3).toInt();
        duration = hours * 3600 + minutes * 60 + seconds;
    }

    QStringList audioFragments;

    // 如果音频时长超过 3 分钟或文件大小超过 10MB，则进行切割
    if (duration > 180 || fileInfo.size() > 10 * 1024 * 1024) {
        qDebug() << "音频时长或大小超出限制，正在切割音频...";

        QString outputPattern = QDir::tempPath() + "/audio_fragment_%03d.mp3";
        process.start("ffmpeg", {"-i", filePath, "-f", "segment", "-segment_time", "120", "-c", "copy", outputPattern});
        process.waitForFinished();

        if (process.exitCode() == 0) {
            qDebug() << "音频切割成功。";

            // 获取所有切片文件
            QDir dir(QDir::tempPath());
            audioFragments = dir.entryList(QStringList() << "audio_fragment_*.mp3", QDir::Files);
            for (int i = 0; i < audioFragments.size(); ++i) {
                audioFragments[i] = QDir::tempPath() + "/" + audioFragments[i];
            }
        } else {
            qDebug() << "音频切割失败:" << process.readAllStandardError();
            audioFragments.append(filePath); // 切割失败，返回原文件
        }
    } else {
        qDebug() << "音频时长和大小符合要求，无需切割。";
        audioFragments.append(filePath); // 直接返回原文件
    }

    return audioFragments;
}

AudioModelClient::~AudioModelClient()
{
    cleanupTempDir();
}

QString AudioModelClient::cropVideoIfNeeded(const QString &filePath) {
    QFileInfo fileInfo(filePath);
    QString outputFilePath = "/home/lyingdead/" + fileInfo.fileName();
    // 创建临时目录
    QDir tempDirObj(QDir::tempPath());
    if (!tempDirObj.exists()) {
        tempDirObj.mkpath(QDir::tempPath());
    }

    // 使用 ffmpeg 检查视频时长
    QProcess process;
    process.start("ffmpeg", {"-i", filePath});
    process.waitForFinished();
    QString output = process.readAllStandardError();

    // 解析视频时长
    qint64 duration = 0;
    QRegularExpression regex("Duration: (\\d+):(\\d+):(\\d+)\\.\\d+");
    QRegularExpressionMatch match = regex.match(output);
    if (match.hasMatch()) {
        int hours = match.captured(1).toInt();
        int minutes = match.captured(2).toInt();
        int seconds = match.captured(3).toInt();
        duration = hours * 3600 + minutes * 60 + seconds;
    }

    // 如果视频时长超过 38 秒，裁剪视频
    if (duration > 38) {
        qDebug() << "视频时长超过 38 秒，正在裁剪...";
        process.start("ffmpeg", {"-i", filePath, "-ss", "0", "-t", "38", "-c", "copy", outputFilePath});
        process.waitForFinished();
        if (process.exitCode() == 0) {
            qDebug() << "视频裁剪成功，保存为:" << outputFilePath;
            return outputFilePath;
        } else {
            qDebug() << "视频裁剪失败:" << process.readAllStandardError();
            return filePath; // 裁剪失败，返回原文件
        }
    } else {
        qDebug() << "视频时长符合要求，无需裁剪。";
        return filePath; // 视频时长符合要求，直接返回原文件
    }
}

void AudioModelClient::sendRequest(const QString &filePath, const QString &question) {
    // 判断是否需要上传文件
    if (!filePath.isEmpty()) {
        // 新文件上传逻辑
        if (!QFile::exists(filePath)) {
            emit errorOccurred("文件不存在！");
            return;
        }

        QFileInfo fileInfo(filePath);
        QString extension = fileInfo.suffix().toLower();

        bool isVideo = (extension == "mp4" || extension == "avi" || extension == "mov" || extension == "mkv");
        bool isAudio = (extension == "mp3" || extension == "wav");

        if (isAudio) {
            // 检查并切割音频
            QStringList audioFragments = splitAudioIfNeeded(filePath);
            if (audioFragments.isEmpty()) {
                emit errorOccurred("音频切割失败！");
                return;
            }

            // 逐一上传音频片段
            for (const QString &fragment : audioFragments) {
                uploadAudioFragment(fragment);
            }

            // 所有音频片段上传完成后，提交问题（如果有）
            if (!question.isEmpty()) {
                QTimer::singleShot(1000, [this, question]() { askQuestionAfterUpload(question); });
            }
            qDebug() << "音频文件上传完成：" << filePath;
        } else if (isVideo) {
            // 检查并裁剪视频
            QString finalFilePath = cropVideoIfNeeded(filePath);
            sendSingleRequest(finalFilePath, question, true);
            qDebug() << "视频文件上传完成：" << filePath;
        }
    } else {
        // 后续对话，不传递文件路径，仅发送问题
        if (question.isEmpty()) {
            emit errorOccurred("请输入问题！");
            return;
        }
        sendSingleRequest(QString(), question, false);
        qDebug() << "发送问题：" << question;
    }
}

void AudioModelClient::sendSingleRequest(const QString &filePath, const QString &question, bool isVideo) {
    QJsonArray messages = buildMessages(question, filePath, isVideo);

    // 构建最终的 JSON 对象
    QJsonObject json;
    json["model"] = "qwen-omni-turbo";
    json["messages"] = messages;
    json["stream"] = true; // 启用流式输出

    QByteArray jsonData = QJsonDocument(json).toJson();
    //qDebug() << "Generated JSON object:" << QString(jsonData);

    // 设置请求
    QUrl url("https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions");
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer sk-ff94827cb8e94561af8155a132fd9a62");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    qDebug() << "Sending request...";
    QNetworkReply* reply = networkManager->post(request, jsonData);
    connect(reply, &QNetworkReply::readyRead, this, &AudioModelClient::onReadyRead);
    connect(reply, &QNetworkReply::finished, this, &AudioModelClient::onReplyFinished);
}

QJsonArray AudioModelClient::buildMessages(const QString &question, const QString &filePath, bool isVideo) {
    QJsonArray messages;

    // 添加历史对话记录
    for (const QJsonValue &msg : conversationHistory) {
        messages.append(msg);
    }

    // 如果对话历史为空，添加系统消息
    if (conversationHistory.isEmpty()) {
        QJsonObject systemMessage;
        systemMessage["role"] = "system";
        systemMessage["content"] = QJsonArray{
            QJsonObject{{"type", "text"}, {"text", "接下来我将上传一首歌曲，请你记住它，并且在回答下一个问题是不需要重复之前回答的问题内容。"}}
        };
        messages.append(systemMessage);
    }

    // 构建用户消息
    QJsonObject userMessage;
    userMessage["role"] = "user";
    QJsonArray contentArray;

    // 检查是否需要上传新文件
    if (!filePath.isEmpty()) {
        qDebug() << "文件路径不为空，处理文件内容上传。";

        // 打开文件并读取内容
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            emit errorOccurred("无法打开文件！");
            return QJsonArray();
        }

        QByteArray fileData = file.readAll();
        QString fileBase64 = QString(fileData.toBase64());
        file.close();

        // 根据文件类型构建消息内容
        if (isVideo) {
            // 视频文件
            contentArray.append(QJsonObject{
                {"type", "video_url"},
                {"video_url", QJsonObject{
                    {"url", QString("data:;base64,%1").arg(fileBase64)}
                }}
            });
        } else {
            // 音频文件
            contentArray.append(QJsonObject{
                {"type", "input_audio"},
                {"input_audio", QJsonObject{
                    {"data", QString("data:;base64,%1").arg(fileBase64)},
                    {"format", "mp3"} // 假设音频格式为 MP3
                }}
            });
        }
    }

    // 添加问题文本
    contentArray.append(QJsonObject{
        {"type", "text"},
        {"text", question}
    });

    // 设置用户消息的内容
    userMessage["content"] = contentArray;
    messages.append(userMessage);

    // 更新对话历史
    conversationHistory.append(userMessage);

    return messages;
}

void AudioModelClient::onReadyRead() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    QByteArray responseData = reply->readAll();
    //qDebug() << "Streamed response data:" << responseData;

    QStringList lines = QString(responseData).split("\n");
    for (const QString& line : lines) {
        QString trimmedLine = line.trimmed();

        if (trimmedLine.startsWith("data:")) {
            QString jsonStr = trimmedLine.mid(5).trimmed(); // 去掉 "data: " 前缀
            if (jsonStr.isEmpty()) continue;

            processChunk(jsonStr); // 处理 JSON 数据块
        } else if (trimmedLine == "[DONE]") {
            qDebug() << "Stream finished.";
        }
    }
}

void AudioModelClient::onReplyFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Request failed:" << reply->errorString();
        emit errorOccurred(reply->errorString());
    } else {
        // 确保只发送最终的完整回复
        qDebug() << "Final response content:" << fullContent;
        emit responseReceived(fullContent); // 发送最终结果到 UI
        fullContent.clear(); // 清空缓存内容
    }

    reply->deleteLater(); // 清理资源
}

void AudioModelClient::processChunk(const QString &jsonString) {
    // 检查是否是流结束标记
    if (jsonString.trimmed() == "[DONE]") {
        qDebug() << "Stream finished.";
        return;
    }

    // 将新数据追加到缓冲区
    buffer += jsonString;

    // 尝试解析 JSON
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(buffer.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        // 如果解析失败，说明数据还不完整，继续等待下一块数据
        qDebug() << "Incomplete JSON data, waiting for more chunks...";
        return;
    }

    // 清空缓冲区，准备处理下一个数据块
    buffer.clear();

    if (!jsonDoc.isObject()) {
        qWarning() << "Unexpected JSON format: not an object.";
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();

    // 提取 choices 数组
    QJsonArray choices = jsonObject.value("choices").toArray();
    if (choices.isEmpty()) {
        qWarning() << "No choices found in JSON chunk.";
        return;
    }

    // 遍历 choices 数组
    for (const QJsonValue &choiceValue : choices) {
        QJsonObject choice = choiceValue.toObject();

        // 提取 delta 对象
        QJsonObject delta = choice.value("delta").toObject();
        QString content = delta.value("content").toString();

        if (!content.isEmpty()) {
            //qDebug() << "Received content chunk:" << content;

            // 将内容追加到完整响应中
            fullContent += content;

        }
    }
}

void AudioModelClient::cleanupTempDir() {
    QDir tempDirObj(QDir::tempPath());
    if (tempDirObj.exists()) {
        qDebug() << "Cleaning up temporary directory:" << QDir::tempPath();
        tempDirObj.removeRecursively();
    }
}

void AudioModelClient::resetConversationHistory() {
    conversationHistory = QJsonArray(); // 清空对话历史
    qDebug() << "对话历史已重置。";
}
