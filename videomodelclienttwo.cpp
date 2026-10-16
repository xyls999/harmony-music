#include "videomodelclienttwo.h"
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
#include <cmath>      // C++ 风格头文件
#include <iostream>
#include <QDataStream>
#include <QMediaPlayer>
#include <QMediaRecorder>
#include <QElapsedTimer>



VideoModelClientTwo::VideoModelClientTwo(QObject *parent)
    : QObject(parent), networkManager(new QNetworkAccessManager(this)), fullContent("")
{
    //connect(networkManager, &QNetworkAccessManager::finished, this, &AudioModelClient::onReplyFinished);
}


void VideoModelClientTwo::uploadAudioFragment(const QString &filePath) {
    QJsonArray messages = buildMessages("这是我上传的歌曲的其中一个片段，请记住它。", filePath, false);

    QJsonObject json;
    json["model"] = "qwen-omni-turbo";
    json["messages"] = messages;
    json["stream"] = true;

    QByteArray jsonData = QJsonDocument(json).toJson();

    QUrl url("https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions");
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer sk-31170f57a7c744a89d268f6e964e54be");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = networkManager->post(request, jsonData);
    connect(reply, &QNetworkReply::finished, this, &VideoModelClientTwo::onUploadFinished); // 连接 finished 信号
}

void VideoModelClientTwo::onUploadFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Upload failed:" << reply->errorString();
        QByteArray errorResponse = reply->readAll(); // 捕获服务器响应
        qDebug() << "Server response:" << QString(errorResponse); // 打印响应内容
        qDebug() << "Error occurred:" << reply->errorString();
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

void VideoModelClientTwo::uploadVideoFragment(const QString &filePath) {
    QJsonArray messages = buildMessages("这是我上传的视频的其中一个片段，请记住它。", filePath, true);

    QJsonObject json;
    json["model"] = "qwen-omni-turbo";
    json["messages"] = messages;
    json["stream"] = true;

    QByteArray jsonData = QJsonDocument(json).toJson();

    QUrl url("https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions");
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer sk-31170f57a7c744a89d268f6e964e54be");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = networkManager->post(request, jsonData);
    connect(reply, &QNetworkReply::finished, this, &VideoModelClientTwo::onVideoUploadFinished);
}

void VideoModelClientTwo::onVideoUploadFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Video upload failed:" << reply->errorString();
        QByteArray errorResponse = reply->readAll();
        qDebug() << "Server response:" << QString(errorResponse);
        qDebug() << "Error occurred:" << reply->errorString();
    } else {
        qDebug() << "Video fragment upload completed.";
    }

    reply->deleteLater();

    // 更新视频分片计数器
    uploadedVideoFragments++;
    if (uploadedVideoFragments == totalVideoFragments) {
        qDebug() << "All video fragments uploaded. Sending question...";
        askQuestionAfterUpload("请分析我刚才上传的所有视频片段。");
    }
}
void VideoModelClientTwo::askQuestionAfterUpload(const QString &question) {
    QJsonArray messages = buildMessages(question, "", false);

    QJsonObject json;
    json["model"] = "qwen-omni-turbo";
    json["messages"] = messages;
    json["stream"] = true; // 启用流式输出

    QByteArray jsonData = QJsonDocument(json).toJson();

    QUrl url("https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions");
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer sk-31170f57a7c744a89d268f6e964e54be");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = networkManager->post(request, jsonData);
    connect(reply, &QNetworkReply::readyRead, this, &VideoModelClientTwo::onReadyRead); // 流式数据处理
    connect(reply, &QNetworkReply::finished, this, &VideoModelClientTwo::onReplyFinished); // 请求完成处理
}
QStringList VideoModelClientTwo::splitAudioIfNeeded(const QString &filePath) {
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

QStringList VideoModelClientTwo::splitVideoIfNeeded(const QString &filePath) {
    QFileInfo fileInfo(filePath);
    qint64 fileSize = fileInfo.size();
    double duration = getVideoDuration(filePath);
    qDebug() << "视频进入切割部分：" << filePath;

    // 如果文件符合条件（≤30秒且≤100MB），直接返回
    if (fileSize <= 20 * 1024 * 1024 && duration <= 30) {
        return QStringList{filePath};
    }

    QDir tempDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation));
    QString outputPattern = tempDir.filePath("video_fragment_%03d.mp4");

    // 计算动态分段时间
    double bitrate = (fileSize * 8.0) / duration; // 平均比特率 (bps)
    double maxDurationBySize = (20 * 1024 * 1024 * 8.0) / bitrate;
    double segmentTime = qBound(1.0, qMin(20.0, maxDurationBySize), 30.0);

    // 使用FFmpeg进行智能分片
    QProcess process;
    process.start("ffmpeg", {
        "-i", filePath,
        "-c", "copy",               // 保持原编码避免重编码
        "-f", "segment",
        "-segment_time", QString::number(segmentTime, 'f', 2),
        "-reset_timestamps", "1",
        "-segment_list_size", "0",  // 不生成列表文件
        outputPattern
    });

    if (!process.waitForFinished(30000)) {
        qWarning() << "视频分片失败:" << process.errorString();
        return QStringList{filePath};
    }

    // 收集生成的分片并排序
    QStringList fragments;
    QDir dir(tempDir.path());
    for (const QString &file : dir.entryList({"video_fragment_*.mp4"}, QDir::Files)) {
        fragments << dir.filePath(file);
    }
    fragments.sort();

    // 二次验证分片合规性
    QStringList validFragments;
    for (const QString &fragment : fragments) {
        QFileInfo fragInfo(fragment);
        double fragDuration = getVideoDuration(fragment);

        if (fragInfo.size() > 20 * 1024 * 1024 || fragDuration > 30) {
            // 递归处理不符合条件的分片
            validFragments << splitVideoIfNeeded(fragment);
            QFile::remove(fragment); // 移除临时分片
        } else {
            validFragments << fragment;
        }
    }

    return validFragments.isEmpty() ? QStringList{filePath} : validFragments;
}

double VideoModelClientTwo::getVideoDuration(const QString &filePath) {
    QProcess probeProcess;
    probeProcess.start("ffprobe", {
        "-v", "error",
        "-show_entries", "format=duration",
        "-of", "default=noprint_wrappers=1:nokey=1",
        filePath
    });

    if (!probeProcess.waitForFinished(5000)) {
        qWarning() << "获取视频时长失败";
        return 0.0;
    }

    bool ok;
    double duration = QString(probeProcess.readAllStandardOutput()).toDouble(&ok);
    return ok ? duration : 0.0;
}

VideoModelClientTwo::~VideoModelClientTwo()
{
    cleanupTempDir();
}



void VideoModelClientTwo::sendRequest(const QString &filePath, const QString &question) {
    if (!filePath.isEmpty()) {
        if (!QFile::exists(filePath)) {
            qDebug() << "文件不存在";
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
                qDebug() << "音频切割失败";
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
                   // +++ 新增视频分片处理逻辑 +++
                   QStringList videoFragments = splitVideoIfNeeded(filePath);

                   if (videoFragments.isEmpty()) {
                       qDebug() << "视频处理失败" ;
                       return;
                   }

                   // 当分片数量>1时显示进度
                   if(videoFragments.size() > 1){
                       qDebug() << "Upload progress:" << 0 << "/" << videoFragments.size();
                   }

                   // 重置计数器
                   uploadedVideoFragments = 0;
                   totalVideoFragments = videoFragments.size();

                   // 逐个上传分片
                   for (const QString &fragment : videoFragments) {
                       qDebug() << "开始上传：" << fragment;
                       uploadVideoFragment(fragment);
                   }
                   // --- 新增结束 ---

                   if (!question.isEmpty()) {
                       QTimer::singleShot(1000, [this, question]() {
                           askQuestionAfterUpload(question);
                       });
                   }
                   qDebug() << "视频文件处理完成：" << filePath;
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

void VideoModelClientTwo::sendSingleRequest(const QString &filePath, const QString &question, bool isVideo) {
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
    request.setRawHeader("Authorization", "Bearer sk-31170f57a7c744a89d268f6e964e54be");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    qDebug() << "Sending request...";
    QNetworkReply* reply = networkManager->post(request, jsonData);
    connect(reply, &QNetworkReply::readyRead, this, &VideoModelClientTwo::onReadyRead);
    connect(reply, &QNetworkReply::finished, this, &VideoModelClientTwo::onReplyFinished);
}

QJsonArray VideoModelClientTwo::buildMessages(const QString &question,
                                            const QString &filePath,
                                            bool isVideo) {
    QJsonArray messages;

    // 合并历史消息（最多保留6条防止过长）
    int startIndex = qMax(conversationHistory.size() - 2, 0);
    for(int i = startIndex; i < conversationHistory.size(); ++i){
        messages.append(conversationHistory.at(i));
    }

    // 系统消息只在首次交互时添加
    if (conversationHistory.isEmpty()) {
        QJsonObject systemMsg;
        systemMsg["role"] = "system";
        systemMsg["content"] = QJsonArray{
            {{
                {"type", "text"},
                {"text", "请仔细分析我上传的媒体文件，回答时请：\n"
                        "1. 保持专业且简洁\n"
                        "2. 分点说明关键内容\n"
                        "3. 不要重复之前已回答的信息"}
            }}
        };
        messages.append(systemMsg);
    }

    // 构建用户消息
    QJsonObject userMsg;
    userMsg["role"] = "user";
    QJsonArray contentArray;

    if (!filePath.isEmpty()) {
        // 统一媒体文件处理
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "[File Error] Failed to open file:" << filePath
                         << "| Error:" << file.errorString();
            return QJsonArray();
        }

        // 优化大文件读取（适合分片处理）
        QByteArray fileData;
        QDataStream stream(&file);
        const qint64 bufferSize = 100*1024 * 1024; // 1MB缓冲区
        QByteArray buffer(bufferSize, 0);

        while(!stream.atEnd()){
            qint64 len = stream.readRawData(buffer.data(), bufferSize);
            fileData.append(buffer.constData(), len);
        }
        file.close();

        // 构建媒体消息（自动识别类型）
        QJsonObject mediaObj;
        if(isVideo){
            mediaObj.insert("type", "video_url");
            mediaObj.insert("video_url", QJsonObject{
                {"url", QString("data:video/mp4;base64,%1").arg(QString(fileData.toBase64()))}
            });
        }else{
            mediaObj.insert("type", "input_audio");
            mediaObj.insert("input_audio", QJsonObject{
                {"data", QString(fileData.toBase64())},
                {"format", "mp3"}
            });
        }
        contentArray.append(mediaObj);
    }

    // 添加问题文本（支持空问题用于纯文件分析）
    if(!question.isEmpty()){
        contentArray.append(QJsonObject{
            {"type", "text"},
            {"text", question}
        });
    }

    userMsg["content"] = contentArray;
    messages.append(userMsg);

    // 智能维护对话历史（避免过大）
    if(conversationHistory.size() >= 6){
        conversationHistory.removeFirst();
    }
    conversationHistory.append(userMsg);

    return messages;
}

void VideoModelClientTwo::onReadyRead() {
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

void VideoModelClientTwo::onReplyFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Request failed:" << reply->errorString();
        qDebug() << "Error occurred:" << reply->errorString();
    } else {
        // 确保只发送最终的完整回复
        qDebug() << "Final response content:" << fullContent;
        emit responseReceived(fullContent); // 发送最终结果到 UI
        fullContent.clear(); // 清空缓存内容
    }

    reply->deleteLater(); // 清理资源
}

void VideoModelClientTwo::processChunk(const QString &jsonString) {
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

void VideoModelClientTwo::cleanupTempDir() {
    QDir tempDirObj(QDir::tempPath());
    if (tempDirObj.exists()) {
        qDebug() << "Cleaning up temporary directory:" << QDir::tempPath();
        tempDirObj.removeRecursively();
    }
}

void VideoModelClientTwo::resetConversationHistory() {
    conversationHistory = QJsonArray(); // 清空对话历史
    qDebug() << "对话历史已重置。";
}
