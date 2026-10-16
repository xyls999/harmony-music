#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "audiomodelclient.h"
#include "videomodelclienttwo.h"
#include <QLabel>
#include <QPushButton>
#include <QListWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // 鼠标事件处理（用于窗口拖动）
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    // 按钮点击事件
    void on_selectFileButton_clicked(); // 选择文件按钮
    void on_sendButton_clicked();       // 发送按钮
    void on_closeButton_clicked();      // 关闭按钮

    // 系统响应和错误处理
    void onResponseReceived(const QString &response); // 处理系统响应
    void onErrorOccurred(const QString &error);       // 处理错误信息


    // 输入框回车键事件
    void onReturnPressed();
    //void onMediaUploadFinished();

private:
    // 辅助函数
    void addMessage(const QString &text, bool isUser); // 添加消息到聊天记录
    void addMessageToChat(const QString &text, bool isUser);
    void updateButtonPosition(QPushButton *button, QListWidget *listWidget);


    // UI 成员变量
    Ui::MainWindow *ui;
    QTimer *hideButtonTimer; // 定时器，用于延迟隐藏复制按钮
    void updateTitleLabel(const QString &status); // 更新 titleLabel 的内容

    AudioModelClient *audioClient;
    VideoModelClientTwo *videoClientTwo;
    QString userAvatarPath;      // 用户头像路径
    QString systemAvatarPath;    // 系统头像路径
    QString m_lastUploadedPath;   // 记录最后一次成功上传的路径
    bool m_isUploading;           // 标记是否正在上传
    bool m_hasUploaded;
    QLabel *titleLabel;      // 标题标签，用于显示状态信息
    // 媒体路径
    QString mediaPath;


    // 窗口拖动相关变量
    bool m_dragging;
    QPoint m_dragPosition;

    // 支持的媒体格式
    static const QStringList VIDEO_FORMATS;
    static const QStringList AUDIO_FORMATS;
};

#endif // MAINWINDOW_H
