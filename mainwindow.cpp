#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QListWidgetItem>
#include <QLabel>
#include <QHBoxLayout>
#include <QPixmap>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QMainWindow>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollBar>
// 初始化支持的媒体格式
const QStringList MainWindow::VIDEO_FORMATS = {"mp4", "avi", "mov", "mkv"};
const QStringList MainWindow::AUDIO_FORMATS = {"mp3", "wav", "flac"};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
      audioClient(new AudioModelClient(this)),
      videoClientTwo(new VideoModelClientTwo(this)),
      hideButtonTimer(new QTimer(this)),
      m_dragging(false)
{
    ui->setupUi(this);

    // 隐藏默认标题栏
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setStyleSheet(R"(
        QMainWindow {
            background-color: #F5F5F5; /* 窗口背景颜色 */
        }
        QListWidget {
            background-color: #F5F5F5; /* 与窗口背景一致 */
            border: none;
        }
        QListWidget::item {
            margin: 5px;
        }
    )");


    hideButtonTimer->setSingleShot(true);
        hideButtonTimer->setInterval(300); // 延迟 300 毫秒

        // 连接定时器信号
        connect(hideButtonTimer, &QTimer::timeout, [this]() {
            QWidget *widget = qobject_cast<QWidget *>(sender());
            if (widget) {
                QPushButton *copyButton = widget->findChild<QPushButton *>();
                if (copyButton) {
                    copyButton->setVisible(false);
                }
            }
        });
    ui->closeButton->setText(""); // 清空文字
    ui->closeButton->setStyleSheet("QPushButton { icon-size: 20px; padding: 4px; }");
    ui->closeButton->setToolTip("关闭窗口");

    ui->titleLabel->setText("聊天窗口"); // 设置默认标题
    ui->titleLabel->setAlignment(Qt::AlignCenter);
    ui->titleLabel->setStyleSheet("font-size: 18px; color: #555;");
    ui->titleLabel->setFixedHeight(30);

    // 设置上传文件按钮的图标，并通过样式表调整图标大小
    ui->sendButton->setText("");
        ui->selectFileButton->setStyleSheet(R"(
            QPushButton {
                background-color: transparent;
                border: none;
                icon-size: 30px;
                padding: 4px;
            }
            QPushButton:hover {
                background-color: rgba(0, 0, 0, 0.1);
                border-radius: 20px;
            }
        )");
        ui->selectFileButton->setToolTip("选择文件");
    // 设置发送按钮的图标，并通过样式表调整图标大小
    // 在 MainWindow 构造函数中，找到发送按钮的初始化部分：
    ui->sendButton->setText("");
    ui->sendButton->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            border: none;
            icon-size: 30px;
            padding: 4px;
        }
        QPushButton:hover {
            background-color: rgba(0, 0, 0, 0.1);
            border-radius: 20px;
        }
    )");
    ui->sendButton->setToolTip("发送消息");
    // 在 MainWindow 构造函数中：
    QPushButton *scrollToBottomButton = new QPushButton(this);
    scrollToBottomButton->setObjectName("scrollToBottomButton");
    scrollToBottomButton->setIcon(QIcon(":/images/ai/down.png"));
    scrollToBottomButton->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            border: none;
            icon-size: 24px;
            padding: 8px;
        }
        QPushButton:hover {
            background-color: rgba(0, 0, 0, 0.1);
            border-radius: 12px;
        }
    )");
    scrollToBottomButton->setFixedSize(40, 40);
    scrollToBottomButton->setVisible(false); // 初始隐藏

    // 获取 horizontalLayout 的父部件及其布局
    QWidget *parentWidget = ui->horizontalLayout->parentWidget();
    if (!parentWidget) {
        qWarning() << "未找到 horizontalLayout 的父部件！";
        return;
    }

    QLayout *parentLayout = parentWidget->layout();
    if (!parentLayout) {
        QVBoxLayout *newLayout = new QVBoxLayout(parentWidget);
        parentWidget->setLayout(newLayout);
        parentLayout = newLayout;
    }

    // 确保布局是 QBoxLayout 子类（如 QVBoxLayout）
    if (QBoxLayout *boxLayout = qobject_cast<QBoxLayout*>(parentLayout)) {
        // 1. 从布局中移除 horizontalLayout
        boxLayout->removeItem(ui->horizontalLayout);

        // 2. 添加按钮到布局
        boxLayout->addWidget(scrollToBottomButton, 0, Qt::AlignHCenter); // 按钮居中

        // 3. 重新添加 horizontalLayout 到布局
        boxLayout->addLayout(ui->horizontalLayout);
    } else {
        qWarning() << "父布局不是 QBoxLayout 类型，无法设置对齐方式！";
        parentLayout->addWidget(scrollToBottomButton); // 无对齐参数
    }

    // 获取垂直滚动条
    QScrollBar *scrollBar = ui->listWidget_chat->verticalScrollBar();

    // 连接滚动条信号，控制按钮的可见性
    connect(scrollBar, &QScrollBar::valueChanged, [this, scrollBar, scrollToBottomButton]() {
        bool isAtBottom = (scrollBar->value() == scrollBar->maximum());
        scrollToBottomButton->setVisible(!isAtBottom); // 到达底部则隐藏按钮
    });

    // 初始状态检查
    bool initialIsAtBottom = (scrollBar->value() == scrollBar->maximum());
    scrollToBottomButton->setVisible(!initialIsAtBottom);

    // 按钮点击事件：滚动到底部
    connect(scrollToBottomButton, &QPushButton::clicked, [scrollBar]() {
        scrollBar->setValue(scrollBar->maximum()); // 滚动到底部
    });
    connect(videoClientTwo,&VideoModelClientTwo::responseReceived,
            this,&MainWindow::onResponseReceived);
    connect(videoClientTwo,&VideoModelClientTwo::errorOccurred,
            this,&MainWindow::onErrorOccurred);

    connect(audioClient, &AudioModelClient::responseReceived,
            this, &MainWindow::onResponseReceived);
    connect(audioClient, &AudioModelClient::errorOccurred,
            this, &MainWindow::onErrorOccurred);

    // 连接按下 Enter 键的信号
    connect(ui->lineEdit_question, &QLineEdit::returnPressed,
            this, &MainWindow::onReturnPressed);

    // 手动连接按钮信号（可选）
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::on_sendButton_clicked);
    connect(ui->closeButton, &QPushButton::clicked, this, &MainWindow::on_closeButton_clicked);
    userAvatarPath = ":/images/icons/user_avatar.jpg"; // 替换为实际路径
    systemAvatarPath = ":/images/ai/lyingdead.png"; // 替换为实际路径


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && event->pos().y() <= ui->titleBar->height()) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        m_dragging = true;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - m_dragPosition);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
    }
}

void MainWindow::on_selectFileButton_clicked()
{
    mediaPath = QFileDialog::getOpenFileName(
        this,
        tr("选择媒体文件"),
        QDir::homePath(),
        tr("媒体文件 (*.mp4 *.avi *.mov *.mkv *.mp3 *.wav *.flac)")
    );

    if (mediaPath.isEmpty()) {
        QMessageBox::warning(this, "警告", "未选择任何文件！");
    } else {
        ui->lineEdit_question->setPlaceholderText(tr("已选择文件: %1").arg(mediaPath));
    }
}

void MainWindow::on_sendButton_clicked()
{
    QString question = ui->lineEdit_question->text();
    if (!question.isEmpty()) {
        addMessage(question, true); // 显示用户的问题
        ui->lineEdit_question->clear();

        // 切换发送按钮图标为发送中状态
        ui->sendButton->setIcon(QIcon(":/images/ai/send_active.png"));

        QFileInfo fileInfo(mediaPath);
        QString extension = fileInfo.suffix().toLower();

        if (VIDEO_FORMATS.contains(extension)) {
            videoClientTwo->sendRequest(mediaPath, question);
        } else if (AUDIO_FORMATS.contains(extension)) {
            audioClient->sendRequest(mediaPath, question);
        } else {
            QMessageBox::critical(this, "错误", "不支持的媒体格式！");
            // 恢复默认图标（在错误情况下）
            ui->sendButton->setIcon(QIcon(":/images/ai/send.png"));
        }
    }
}

void MainWindow::on_closeButton_clicked()
{
    close(); // 关闭窗口
}

void MainWindow::onResponseReceived(const QString &response)
{
    addMessage(response, false); // 显示系统的回答
    // 恢复发送按钮图标
    ui->sendButton->setIcon(QIcon(":/images/ai/send.png"));
}

void MainWindow::onErrorOccurred(const QString &error)
{
    addMessage(error, false); // 显示错误信息
    // 恢复发送按钮图标
    ui->sendButton->setIcon(QIcon(":/images/ai/send.png"));
}
void MainWindow::onReturnPressed()
{
    on_sendButton_clicked(); // 模拟点击发送按钮
}

void MainWindow::addMessage(const QString &text, bool isUser)
{
    // 如果是用户消息，在消息显示之前提示“消息正在发送中……”
    if (isUser) {
        updateTitleLabel("消息正在发送中……");

        // 模拟消息发送延迟（例如网络请求）
        QTimer::singleShot(500, this, [this, text]() {
            // 消息发送完成后，添加到窗口并清除状态
            addMessageToChat(text, true);
            updateTitleLabel(""); // 清除状态
        });
    } else {
        // 系统消息直接添加到窗口
        addMessageToChat(text, false);
    }
}

void MainWindow::addMessageToChat(const QString &text, bool isUser)
{
    QWidget *messageWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(messageWidget);

    // 头像
    QLabel *avatarLabel = new QLabel();
    QPixmap avatarPixmap(isUser ? userAvatarPath : systemAvatarPath);
    if (avatarPixmap.isNull()) {
        avatarPixmap = QPixmap(40, 40);
        avatarPixmap.fill(Qt::lightGray);
    }
    avatarLabel->setPixmap(avatarPixmap.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    avatarLabel->setFixedSize(40, 40);

    // 内容标签
    QLabel *contentLabel = new QLabel(text);
    contentLabel->setWordWrap(true);
    contentLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding); // 允许垂直扩展

    // 自适应宽度设置
    int maxWidth = 300; // 最大宽度（根据窗口大小调整）
    QFontMetrics fm(contentLabel->font());
    int textWidth = fm.horizontalAdvance(text) + 24; // 添加边距补偿
    if(textWidth > maxWidth) {
        contentLabel->setFixedWidth(maxWidth);
    } else {
        contentLabel->setFixedWidth(textWidth);
    }

    // 样式设置
    if (isUser) {
        contentLabel->setStyleSheet(
            "background-color: #C8E6C9;"
            "padding: 8px 12px;"
            "border-radius: 10px;"
            "color: black;"
            "font-family: 'Microsoft YaHei', sans-serif;"
            "line-height: 1.6em;"
        );
    } else {
        contentLabel->setStyleSheet(
            "background-color: #ECECEC;"
            "padding: 8px 12px;"
            "border-radius: 10px;"
            "color: black;"
            "font-family: 'Microsoft YaHei', sans-serif;"
            "line-height: 1.6em;"
        );
    }

    // 时间标签
    QLabel *timeLabel = new QLabel(QDateTime::currentDateTime().toString("HH:mm"));
    timeLabel->setStyleSheet("color: #888; font-size: 12px; margin-top: 4px;");

    // 复制按钮
    QPushButton *copyButton = new QPushButton();
    copyButton->setIcon(QIcon(":/images/ai/copy.png"));
    copyButton->setFlat(true);
    copyButton->setFixedSize(24, 24);
    copyButton->setStyleSheet("QPushButton { background-color: transparent; } QPushButton:hover { background-color: rgba(0, 0, 0, 0.1); border-radius: 12px; }");
    copyButton->setVisible(false);

    connect(copyButton, &QPushButton::clicked, [text]() {
        QApplication::clipboard()->setText(text);
    });

    // 水平布局
    QHBoxLayout *hLayout = new QHBoxLayout();

    if (isUser) {
        hLayout->addWidget(copyButton, 0, Qt::AlignLeft | Qt::AlignBottom);
        hLayout->addWidget(contentLabel, 0); // 不使用伸缩因子
        hLayout->addWidget(avatarLabel);
    } else {
        hLayout->addWidget(avatarLabel);
        hLayout->addWidget(contentLabel, 0); // 不使用伸缩因子
        hLayout->addWidget(copyButton, 0, Qt::AlignRight | Qt::AlignBottom);
    }

    hLayout->setAlignment(isUser ? Qt::AlignRight : Qt::AlignLeft);
    hLayout->setContentsMargins(10, 10, 10, 0);
    hLayout->setSpacing(8);

    // 垂直布局
    layout->addLayout(hLayout);
    layout->addWidget(timeLabel, 0, isUser ? Qt::AlignRight : Qt::AlignLeft);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    messageWidget->setLayout(layout);
    messageWidget->installEventFilter(this);

    // 列表项设置
    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(messageWidget->sizeHint());
    ui->listWidget_chat->addItem(item);
    ui->listWidget_chat->setItemWidget(item, messageWidget);
    ui->listWidget_chat->scrollToBottom();

    if (!isUser) {
        updateTitleLabel("对面正在输入中……");
        QTimer::singleShot(1000, this, [this]() { updateTitleLabel(""); });
    }
    messageWidget->installEventFilter(this);
}
void MainWindow::updateTitleLabel(const QString &status)
{
    if (ui->titleLabel) {
        if (status.isEmpty()) {
            ui->titleLabel->setText("音视频感知系统"); // 默认标题
        } else {
            ui->titleLabel->setText(status); // 更新状态
        }
    } else {
        qWarning() << "titleLabel is not initialized!";
    }
}
bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    QWidget *widget = qobject_cast<QWidget *>(obj);
    if (!widget) return false;

    // 查找子控件
    QLabel *contentLabel = widget->findChild<QLabel *>();
    QPushButton *copyButton = widget->findChild<QPushButton *>();

    if (!contentLabel || !copyButton) return false;

    if (event->type() == QEvent::Enter) {
        // 鼠标进入：缩小消息框，显示复制按钮
        copyButton->setVisible(true);

        QPropertyAnimation *animation = new QPropertyAnimation(contentLabel, "minimumWidth");
        animation->setDuration(200); // 动画持续时间
        animation->setStartValue(contentLabel->width());
        animation->setEndValue(contentLabel->width() * 0.8); // 缩小到原宽度的80%
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    } else if (event->type() == QEvent::Leave) {
        // 鼠标离开：恢复消息框大小，隐藏复制按钮
        copyButton->setVisible(false);

        QPropertyAnimation *animation = new QPropertyAnimation(contentLabel, "minimumWidth");
        animation->setDuration(200); // 动画持续时间
        animation->setStartValue(contentLabel->width());
        animation->setEndValue(contentLabel->width() / 0.8); // 恢复原宽度
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }

    return QObject::eventFilter(obj, event);
}
