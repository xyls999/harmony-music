#include "videoplayer.h"
#include "ui_videoplayer.h"

VideoPlayer::VideoPlayer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::VideoPlayer)
{
    // ========== 基础初始化 ==========
    ui->setupUi(this);

//    // 清除全局样式并启用样式传播
//    qApp->setStyleSheet("");
//    qApp->setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);

    // ========== 窗口属性设置 ==========
    // 窗口居中显示
    move(QGuiApplication::primaryScreen()->availableGeometry().center() - rect().center());
    // 设置无边框窗口
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    // ========== 布局设置 ==========
    ui->centralwidget->layout()->setContentsMargins(0, 0, 0, 0);  // 去除边距
    ui->centralwidget->layout()->setSpacing(0);                  // 去除间距

    // ========== 鼠标跟踪设置 ==========
    setMouseTracking(true);                  // 主窗口启用鼠标跟踪
    ui->centralwidget->setMouseTracking(true); // 中央部件启用鼠标跟踪
    ui->graphicsView->setMouseTracking(true);  // 视频视图启用鼠标跟踪
    ui->graphicsView->setAttribute(Qt::WA_TransparentForMouseEvents, true); // 设置透明鼠标事件

    // ========== UI元素样式初始化 ==========
    // 最小化按钮样式
    ui->pushButton_Min->setStyleSheet("QPushButton { border: none; outline: none; }");
    // 主题按钮样式
    ui->pushButton_Theme->setStyleSheet("QPushButton { border: none; outline: none; }");

    // 设置logo图标
    QPixmap pixmap(":/black/视频播放器_.svg");
    ui->label->setPixmap(pixmap.scaled(ui->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->label->setScaledContents(true);

    // ========== 应用主题样式 ==========
    this->setStyleSheet(Theme::Light.MainWindow);              // 主窗口样式
    ui->titleBar->setStyleSheet(Theme::Light.TitleBar);        // 标题栏样式
    ui->controlBar->setStyleSheet(Theme::Light.ControlBar);    // 控制栏样式
    ui->horizontalSlider_Position->setStyleSheet(Theme::Light.PositionSlider); // 进度条样式
    ui->horizontalSlider_Position->installEventFilter(this);   // 为进度条安装事件过滤器

    // ========== 媒体播放器核心初始化 ==========
    player = new QMediaPlayer(this);
    player->setVolume(50);  // 默认音量50

    // 创建视频场景
    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    // 设置视频显示项
    videoItem = new QGraphicsVideoItem;
    videoItem->setSize(ui->graphicsView->size());
    scene->addItem(videoItem);
    player->setVideoOutput(videoItem);  // 绑定视频输出

    // ========== 信号与槽连接 ==========
    // 元数据可用信号
    connect(player, &QMediaPlayer::metaDataAvailableChanged, this, &VideoPlayer::onMetaDataAvailable);
    // 播放进度变化信号
    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(onPositionChanged(qint64)));
    // 总时长变化信号
    connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(onDurationChanged(qint64)));

    // 全屏按钮
    connect(ui->pushButton_Max, &QPushButton::clicked, this, &VideoPlayer::toggleFullscreen);

    // 倍速菜单弹出
    connect(ui->pushButton_Speed, &QPushButton::clicked, this, [this]() {
        speedMenu->exec(ui->pushButton_Speed->mapToGlobal(
            QPoint(0, ui->pushButton_Speed->height())));
    });

    // 历史记录面板切换
    connect(ui->pushButton_History, &QPushButton::clicked, this, &VideoPlayer::toggleHistoryVisibility);

    // 播放模式切换
    connect(ui->pushButton_Mode, &QPushButton::clicked, this, [this]() {
        switch(currentPlayMode) {
        case Sequence: currentPlayMode = Loop; break;
        case Loop: currentPlayMode = Random; break;
        case Random: currentPlayMode = Sequence; break;
        }
        updatePlayModeIcon();  // 更新模式图标
    });

    // 媒体状态变化处理
    connect(player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            switch (currentPlayMode) {
            case Sequence: on_pushButton_Next_clicked(); break;  // 顺序播放下一个
            case Loop: player->setPosition(0); player->play(); break;  // 循环当前
            case Random: on_pushButton_Next_clicked(); break;  // 随机播放下一个
            }
        }
    });

    // ========== 数据库初始化 ==========
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(dataPath);  // 创建数据目录
    }

    // 打开SQLite数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","vedio_connection");
    db.setDatabaseName(dataPath + "/playlist.db");
    if (!db.open()) {
        qWarning() << "Database Error:" << db.lastError();
    }

    // 创建播放列表表
    QSqlQuery query(QSqlDatabase::database("vedio_connection"));
    query.exec("CREATE TABLE IF NOT EXISTS playlist ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "file_path TEXT UNIQUE, "
               "title TEXT, "
               "duration INTEGER, "
               "cover BLOB)");

    // 创建播放历史表
    query.exec("CREATE TABLE IF NOT EXISTS play_history ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "file_path TEXT NOT NULL, "
               "play_time DATETIME NOT NULL)");

    // ========== 功能模块初始化 ==========
    initPlaylistUI();      // 初始化播放列表UI
    initHistoryUI();       // 初始化历史记录UI
    loadFromDatabase(listWidget);    // 从数据库加载播放列表
    setAcceptDrops(true);  // 启用拖放功能

    // 视频帧捕获探头
    QVideoProbe *probe = new QVideoProbe(this);
    if (probe->setSource(player)) {
        connect(probe, &QVideoProbe::videoFrameProbed, this, &VideoPlayer::captureFirstFrame);
    }

    initVolumeControl();  // 初始化音量控制

    // ========== 倍速菜单初始化 ==========
    speedMenu = new QMenu(this);
    speedMenu->setStyleSheet(Theme::Light.Menu);
    QActionGroup *speedGroup = new QActionGroup(this);
    QStringList speeds = {"0.5x", "0.75x", "1.0x", "1.25x", "1.5x", "2.0x"};
    for (const QString &speed : speeds) {
        QAction *action = speedMenu->addAction(speed);
        action->setCheckable(true);
        action->setActionGroup(speedGroup);
        if (speed == "1.0x") action->setChecked(true);  // 默认1.0倍速

        connect(action, &QAction::triggered, this, [this, speed]() {
            setPlaybackSpeed(speed.left(speed.length()-1).toFloat());
            ui->pushButton_Speed->setText(speed);
        });
    }

    // ========== 播放模式初始化 ==========
    currentPlayMode = Sequence;  // 默认顺序播放
    updatePlayModeIcon();        // 更新播放模式图标

    createMenu();  // 创建主菜单
}


/* 析构函数：释放资源 */
VideoPlayer::~VideoPlayer()
{
    delete ui;
}

//初始化播放列表UI
void VideoPlayer::initPlaylistUI()
{
    // ========== 播放列表面板初始化 ==========
    playlistPanel = new QWidget(this);
    playlistPanel->setFixedSize(width(), 300);  // 设置固定高度300
    playlistPanel->move(0, height());          // 初始位置在窗口底部
    playlistPanel->setStyleSheet(Theme::Light.PlaylistPanel);
    playlistPanel->raise();  // 确保显示在控制栏上层

    // ========== 主布局设置 ==========
    QVBoxLayout *panelLayout = new QVBoxLayout(playlistPanel);
    panelLayout->setContentsMargins(4, 8, 4, 4);  // 设置边距（左、上、右、下）
    panelLayout->setSpacing(4);                   // 控件间距

    // ========== 播放列表控件初始化 ==========
    listWidget = new QListWidget(playlistPanel);
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    listWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    listWidget->setStyleSheet(Theme::Light.Playlist);

    // ========== 信号连接 ==========
    // 列表项选择变化
    connect(listWidget, &QListWidget::itemSelectionChanged, this, [this]() {
        for (int i = 0; i < listWidget->count(); ++i) {
            QListWidgetItem* item = listWidget->item(i);
            if (PlaylistItem* widget = qobject_cast<PlaylistItem*>(listWidget->itemWidget(item))) {
                widget->setSelected(item->isSelected());
                disconnect(widget, &PlaylistItem::deleteRequested, nullptr, nullptr);
                connect(widget, &PlaylistItem::deleteRequested, this, [this, item]() {
                    if (QMessageBox::question(this, "确认删除", "确定要删除此项吗？",
                                            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                        deletePlaylistItem(item);
                    }
                });
            }
        }
    });
    // 双击播放
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &VideoPlayer::onPlaylistItemDoubleClicked);

    // ========== 动画设置 ==========
    panelAnimation = new QPropertyAnimation(playlistPanel, "pos");
    panelAnimation->setDuration(300);
    panelAnimation->setEasingCurve(QEasingCurve::OutQuad);

    // ========== 顶部控制栏（搜索框+计数+清除按钮） ==========
    // 在initPlaylistUI函数中找到顶部控制栏布局部分，修改如下：
    QHBoxLayout *topControlLayout = new QHBoxLayout();
    topControlLayout->setContentsMargins(0, 0, 0, 0);
    topControlLayout->setSpacing(8);

    // 视频计数标签（最左侧）
    songCountLabel = new QLabel("0个视频", playlistPanel);
    songCountLabel->setStyleSheet(Theme::Light.CountLabel); // 增加右边距

    // 搜索框（居中）
    searchEdit = new QLineEdit(playlistPanel);
    searchEdit->setPlaceholderText("搜索视频...");
    searchEdit->setClearButtonEnabled(true);
    searchEdit->setStyleSheet(Theme::Light.SearchEdit);
    // 全部清除按钮（最右侧）
    clearAllButton = new QPushButton("全部清除", playlistPanel);
    clearAllButton->setStyleSheet(Theme::Light.ClearButton); // 确保样式与原始代码完全一致

    // 修改布局顺序
    topControlLayout->addWidget(songCountLabel);       // 最左侧
    topControlLayout->addStretch();                    // 左侧弹簧
    topControlLayout->addWidget(searchEdit);          // 居中搜索框
    topControlLayout->addStretch();                    // 右侧弹簧
    topControlLayout->addWidget(clearAllButton);      // 最右侧

    // 保持原有信号连接不变
    connect(searchEdit, &QLineEdit::textChanged, this, &VideoPlayer::onSearchTextChanged);
    connect(clearAllButton, &QPushButton::clicked, this, &VideoPlayer::clearAllPlaylistItems);


    // ========== 组装最终布局 ==========
    panelLayout->addLayout(topControlLayout);  // 添加顶部控制栏
    panelLayout->addWidget(listWidget);       // 添加播放列表
    // 更新视频计数
    updateSongCount();
}

// 播放列表界面弹出
void VideoPlayer::togglePlaylistVisibility()
{
    QPoint endPos = isPlaylistVisible ?
         QPoint(0, height()) :
         QPoint(0, height() - playlistPanel->height() - ui->controlBar->height());
    panelAnimation->setStartValue(playlistPanel->pos());
    panelAnimation->setEndValue(endPos);
    panelAnimation->start();

    isPlaylistVisible = !isPlaylistVisible;
}

//播放列表按钮点击槽函数 - 控制播放列表面板的显示/隐藏
void VideoPlayer::on_pushButton_list_clicked()
{
    closeOtherPanels(playlistPanel); // 新增
    // 获取面板和控制栏高度
    const int panelHeight = playlistPanel->height();
    const int controlBarHeight = ui->controlBar->height();

    // 动画起始位置（当前位置）
    QPoint startPos = playlistPanel->pos();
    QPoint endPos;

    if (isPlaylistVisible) {
        // 隐藏面板：移动到窗口下方
        endPos = QPoint(0, height());
    } else {
        // 显示面板：定位到控制栏上方
        endPos = QPoint(0, height() - panelHeight - controlBarHeight);

        // 确保面板宽度与窗口一致
        playlistPanel->setFixedWidth(width());
    }

    // 设置并启动动画
    panelAnimation->setStartValue(startPos);
    panelAnimation->setEndValue(endPos);
    panelAnimation->start();

    // 切换面板可见状态
    isPlaylistVisible = !isPlaylistVisible;

    // 当显示面板时，更新播放列表项宽度
    if (isPlaylistVisible) {

        updatePlaylistItemWidths();
    }
}

//删除相关
void VideoPlayer::clearAllPlaylistItems()
{
    // 确认对话框
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认清除", "确定要清除全部视频吗？",
                                QMessageBox::Yes|QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    // 清空数据库
    QSqlQuery query(QSqlDatabase::database("vedio_connection"));
    if (!query.exec("DELETE FROM playlist")) {
        qWarning() << "Clear playlist failed:" << query.lastError();
    }

    // 清空列表
    listWidget->clear();

    // 停止当前播放
    player->stop();
    ui->labelCurrentMdia->clear();

    // 重置索引
    currentPlaylistIndex = 0;

    // 更新视频计数
    updateSongCount();
}

void VideoPlayer::deletePlaylistItem(QListWidgetItem *item) {
    if (!item) return;

    QString filePath = item->data(Qt::UserRole).toString();
    QString fileName = QFileInfo(filePath).fileName();

    // 关键修复：直接通过 canonicalUrl() 获取路径
    bool isCurrentlyPlaying = (player->media().canonicalUrl().toLocalFile() == filePath);
    // 从数据库删除
    QSqlQuery query(QSqlDatabase::database("vedio_connection"));
    query.prepare("DELETE FROM playlist WHERE file_path = ?");
    query.addBindValue(filePath);
    if (!query.exec()) {
        qWarning() << "Delete failed:" << query.lastError();
        QMessageBox::warning(this, "删除失败", "删除视频失败：" + query.lastError().text());
        return;
    }
    // 从列表删除
    int row = listWidget->row(item);
    delete listWidget->takeItem(row);
    // 如果是当前播放项，彻底重置播放器
    if (isCurrentlyPlaying) {
        player->stop();
        player->setMedia(QMediaContent()); // 清除媒体
        ui->labelCurrentMdia->clear();
        currentPlayIndex = -1;
        ui->horizontalSlider_Position->setValue(0); // 重置进度条
        ui->label_Ratio->setText("00:00 / 00:00"); // 清空时间显示
        currentTheme == "深色主题" ? ui->pushButton_Player->setIcon(QIcon(":/white/开始播放 .svg")) :
                                 ui->pushButton_Player->setIcon(QIcon(":/black/开始播放.svg"));
    } else if (currentPlayIndex > row) {
        currentPlayIndex--; // 调整索引
    }

    // 更新列表序号和计数
    updateSongCount();
    for (int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem *listItem = listWidget->item(i);
        if (PlaylistItem *widget = qobject_cast<PlaylistItem *>(listWidget->itemWidget(listItem))) {
            widget->setIndex(i + 1);
        }
    }
    currentPlaylistIndex = listWidget->count();

    QMessageBox::information(this, "删除成功", QString("已成功删除视频：%1").arg(fileName));
}

//调整视频随窗口变化
// 元数据可用时调整尺寸
void VideoPlayer::onMetaDataAvailable(bool available)
{
    if (available) {updateVideoSize();}
}

void VideoPlayer::updateVideoSize()
{
    if (!videoItem) return;

    // 获取视图尺寸和视频原始尺寸
    QSize viewSize = ui->graphicsView->size();
    QSizeF nativeSize = videoItem->nativeSize();

    // 根据实际情况调整
    if (nativeSize.isEmpty()) {
        // 无元数据时直接填充视图
        videoItem->setSize(viewSize);
    } else {
        // 保持宽高比缩放
        QSizeF scaledSize = nativeSize.scaled(viewSize, Qt::KeepAspectRatio);
        videoItem->setSize(scaledSize);
    }

    // 关键：强制视图适配视频项
    ui->graphicsView->fitInView(videoItem, Qt::KeepAspectRatio);
    ui->graphicsView->scene()->setSceneRect(videoItem->boundingRect());
}

// 新增帧捕获实现
void VideoPlayer::captureFirstFrame(const QVideoFrame &frame) {
    if (frameCaptured) return;

    QVideoFrame cloneFrame(frame);
    if (cloneFrame.map(QAbstractVideoBuffer::ReadOnly)) {
        QImage image(cloneFrame.bits(),
                     cloneFrame.width(),
                     cloneFrame.height(),
                     cloneFrame.bytesPerLine(),
                     QVideoFrame::imageFormatFromPixelFormat(cloneFrame.pixelFormat()));
        firstFrame = image.copy();
        cloneFrame.unmap();
        frameCaptured = true;
    }
}

//添加文件
void VideoPlayer::applyProgressDialogStyle(QProgressDialog &progress)
{
    QString themeName = currentTheme;
    QString styleSheet;

    if (themeName == "深色主题") {
        styleSheet = Theme::Dark.ProgressDialog;
    } else {
        styleSheet = Theme::Light.ProgressDialog;
    }

    progress.setStyleSheet(styleSheet);
}

void VideoPlayer::on_pushButton_Add_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        "选择视频文件",
        "",
        "视频文件 (*.mp4 *.avi *.mkv *.mov);;所有文件 (*.*)"
    );

    if (files.isEmpty()) {
        return;
    }

    // 过滤掉已经在播放列表中的文件
    QStringList newFiles;
    for (const QString &file : files) {
        bool alreadyExists = false;
        for (int i = 0; i < listWidget->count(); ++i) {
            QListWidgetItem *item = listWidget->item(i);
            if (item->data(Qt::UserRole).toString() == file) {
                alreadyExists = true;
                break;
            }
        }
        if (!alreadyExists) {
            newFiles.append(file);
        }
    }

    if (newFiles.isEmpty()) {
        QMessageBox::information(this, "提示", "所有选择的文件已在播放列表中！");
        return;
    }

    // 显示添加进度对话框
    QProgressDialog progress("添加文件中...", "取消", 0, newFiles.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0); // 立即显示进度对话框
    // 应用当前主题样式
    applyProgressDialogStyle(progress);
    int addedCount = 0;
    for (const QString &file : newFiles) {
        progress.setValue(addedCount);
        progress.setLabelText(QString("正在添加: %1").arg(QFileInfo(file).fileName()));

        if (progress.wasCanceled()) {
            break;
        }

        // 先设置媒体但不播放
       player->setMedia(QUrl::fromLocalFile(file));
//        ui->labelCurrentMdia->setText(QFileInfo(file).fileName());

        // 连接 durationChanged 信号，异步获取时长
        QEventLoop loop;
        QObject::connect(player, &QMediaPlayer::durationChanged, &loop, [&](qint64 duration) {
            // 断开连接，避免重复触发
            QObject::disconnect(player, &QMediaPlayer::durationChanged, &loop, nullptr);

            if (duration <= 0) {
                qWarning() << "Invalid duration for file:" << file;
                loop.quit();
                return;
            }

            // 生成封面（确保此时视频已加载）
            QPixmap cover;
            if (!firstFrame.isNull()) {
                cover = QPixmap::fromImage(firstFrame.scaled(80, 50, Qt::KeepAspectRatioByExpanding));
            } else {
                cover.load(":/default_cover.png");
            }

            // 创建列表项
            QListWidgetItem *item = new QListWidgetItem();
            item->setData(Qt::UserRole, file);
            item->setSizeHint(QSize(0, 60));

            PlaylistItem *widget = new PlaylistItem();
            widget->setData(++currentPlaylistIndex,
                           QFileInfo(file).fileName(),
                           QTime(0, 0).addMSecs(duration).toString("hh:mm:ss"),
                           cover);

            listWidget->addItem(item);
            listWidget->setItemWidget(item, widget);
            connect(widget, &PlaylistItem::deleteRequested, this, [this, item]() {
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this, "确认删除",
                                            "确定要删除此项吗？",
                                            QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::Yes) {
                    deletePlaylistItem(item);  // 这里会触发成功提示
                }
            });

            // 保存到数据库
            saveToDatabase(file, QFileInfo(file).fileName(), duration);

            addedCount++;
            loop.quit();
        });

        // 重置帧捕获状态
        frameCaptured = false;

        // 等待获取duration
        loop.exec();
    }

    progress.setValue(newFiles.size());
    updateSongCount();

    if (addedCount > 0) {
        QMessageBox::information(this, "添加完成",
            QString("成功添加 %1 个视频文件").arg(addedCount));
    }
}

//数据库相关
void VideoPlayer::saveToDatabase(const QString &path, const QString &title, qint64 duration) {
    if (duration <= 0) {
        qWarning() << "Invalid duration for file:" << path;
        return;
    }

    // 生成封面（如果未捕获到第一帧，使用默认封面）
    QPixmap cover;
    if (!firstFrame.isNull()) {
        cover = QPixmap::fromImage(firstFrame.scaled(80, 50, Qt::KeepAspectRatioByExpanding));
    } else {

        currentTheme == "深色主题" ? cover.load(":/black/视频播放器_.svg") :
                                 cover.load(":/white/视频播放器_.svg");
        cover = cover.scaled(80, 50, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    }

    // 将 QPixmap 转为 QByteArray（二进制数据）
    QByteArray coverData;
    QBuffer buffer(&coverData);
    buffer.open(QIODevice::WriteOnly);
    cover.save(&buffer, "PNG"); // 保存为 PNG 格式

    QSqlQuery query(QSqlDatabase::database("vedio_connection"));
    query.prepare("INSERT OR REPLACE INTO playlist (file_path, title, duration, cover) "
                  "VALUES (?, ?, ?, ?)");
    query.addBindValue(path);
    query.addBindValue(title);
    query.addBindValue(duration);
    query.addBindValue(coverData); // 绑定封面数据
    if (!query.exec()) {
        qWarning() << "Save to DB failed:" << query.lastError();
    }
    updateSongCount();
}

void VideoPlayer::loadFromDatabase(QListWidget *list) {
    QSqlQuery query(QSqlDatabase::database("vedio_connection"));
    if (!query.exec("SELECT file_path, title, duration, cover FROM playlist")) {
        qWarning() << "Load failed:" << query.lastError().text();
        return;
    }

    currentPlaylistIndex = 0;
    while (query.next()) {
        QString path = query.value(0).toString();
        QString title = query.value(1).toString();
        qint64 duration = query.value(2).toLongLong();
        QByteArray coverData = query.value(3).toByteArray(); // 读取封面二进制数据


        // 从二进制数据恢复 QPixmap
        QPixmap cover;
        if (!coverData.isEmpty()) {
            cover.loadFromData(coverData, "PNG");
            cover = cover.scaled(80, 50, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        } else {
            cover.load(":/default_cover.png");
            cover = cover.scaled(80, 50, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        }
        QListWidgetItem *item = new QListWidgetItem();
        item->setData(Qt::UserRole, path);
        item->setSizeHint(QSize(0, 60));

        PlaylistItem *widget = new PlaylistItem();
        widget->setData(++currentPlaylistIndex,
                       title,
                       QTime(0, 0).addMSecs(duration).toString("hh:mm:ss"),
                       cover); // 使用从数据库加载的封面

        qInfo()<<"add";
        list->addItem(item);
        list->setItemWidget(item, widget);
        qInfo()<<"videocount"<<listWidget->count();
        updateSongCount();
    }
}

void VideoPlayer::updateSongCount()
{
    int count = listWidget->count();
    songCountLabel->setText(QString("%1 个视频").arg(count));
}

//用事件过滤器处理滑块快进操作
bool VideoPlayer::eventFilter(QObject *watched, QEvent *event)
{
    // 只处理进度条滑块的事件
    if (watched == ui->horizontalSlider_Position) {
        // 只处理鼠标按下事件
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);

            // 检查是否为左键点击
            if (mouseEvent && mouseEvent->button() == Qt::LeftButton) {
                QSlider *slider = ui->horizontalSlider_Position;

                // 如果媒体未加载，忽略事件
                if (player->duration() <= 0) {
                    return false;
                }

                // 计算点击位置对应的进度值
                int value = QStyle::sliderValueFromPosition(
                    slider->minimum(),
                    slider->maximum(),
                    mouseEvent->pos().x(),
                    slider->width()
                );

                // 确保值在合法范围内
                value = qBound(slider->minimum(), value, slider->maximum());

                // 更新UI和播放位置
                slider->setValue(value);
                player->setPosition(value);

                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

//鼠标拖动处理相关
void VideoPlayer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !isFullScreen()) {
        // 获取点击位置的全局坐标和窗口位置
        QPoint clickPos = event->pos();
        QWidget* child = childAt(clickPos);

        // 检查点击的控件是否是按钮或滑动条
        bool isButtonOrSlider = false;
        if (child) {
            // 检查控件本身或其父类是否是QAbstractButton或QAbstractSlider
            isButtonOrSlider = (
                qobject_cast<QAbstractButton*>(child) ||
                qobject_cast<QAbstractSlider*>(child)
            );
        }

        // 如果点击的不是按钮或滑动条，则允许拖动
        if (!isButtonOrSlider) {
            m_dragging = true;
            m_dragPosition = event->globalPos() - frameGeometry().topLeft();
            setCursor(Qt::ClosedHandCursor);
            event->accept();
            return;
        }
    }


    QMainWindow::mousePressEvent(event);
}

void VideoPlayer::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
        return;
    }
    QMainWindow::mouseMoveEvent(event);
}

void VideoPlayer::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_dragging) {
        m_dragging = false;
        event->accept();
        return;
    }
    QMainWindow::mouseReleaseEvent(event);
}

//处理播放列表随最大化和退出最大化变化操作
void VideoPlayer::updatePlaylistItemWidths()
{
    // 计算考虑滚动条后的有效宽度
    int scrollBarWidth = listWidget->verticalScrollBar()->isVisible() ?
        listWidget->verticalScrollBar()->width() : 0;
    int effectiveWidth = playlistPanel->width() - scrollBarWidth - 20; // 减去边距

    // 更新所有列表项
    for(int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem* item = listWidget->item(i);
        item->setSizeHint(QSize(effectiveWidth, 60));

        // 确保自定义widget也更新
        if(PlaylistItem* widget = qobject_cast<PlaylistItem*>(listWidget->itemWidget(item))) {
            widget->setFixedWidth(effectiveWidth);
        }
    }
}

void VideoPlayer::toggleFullscreen()
{
    if (isFullScreen()) {
        // 退出全屏
        showNormal();
        updateMaxIcon(false);

        // 恢复窗口模式宽度
        playlistPanel->setFixedWidth(this->width() * 0.8); // 80%窗口宽度

        // 强制更新列表项宽度
        updatePlaylistItemWidths();

    } else {

        showFullScreen();
        updateMaxIcon(true);

        // 设置为全屏宽度
        playlistPanel->setFixedWidth(this->width());

        // 强制更新列表项宽度
        updatePlaylistItemWidths();
    }

    // 更新位置
    QPoint pos = playlistPanel->pos();
    pos.setY(isPlaylistVisible ?
            height() - playlistPanel->height() - ui->controlBar->height() :
            height());
    playlistPanel->move(pos);
}

void VideoPlayer::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (sliderVisible)
    {
    //重新计算位置
        QPoint btnCenter = ui->pushButton_Sound->rect().center();
        QPoint globalPos = ui->pushButton_Sound->mapToGlobal(btnCenter);
        QPoint windowPos = mapFromGlobal(globalPos);

        int x = windowPos.x() - volumeSlider->width()/2;
        int y = windowPos.y() - volumeSlider->height() - 30;

        volumeSlider->move(x, y);
        volumeLabel->move(x + (volumeSlider->width() - volumeLabel->width())/2, y - 28);
     }
    // 更新播放列表宽度
    playlistPanel->setFixedWidth(isFullScreen() ? width() : width());
    // 更新所有列表项宽度
    updatePlaylistItemWidths();
    // 更新位置
    QPoint pos = playlistPanel->pos();
    pos.setY(isPlaylistVisible ?
            height() - playlistPanel->height() - ui->controlBar->height() :
            height());
    playlistPanel->move(pos);
    updateVideoSize();

    // 新增：更新历史面板宽度
    historyPanel->setFixedWidth(isFullScreen() ? width() : width());
    // 新增：更新历史列表项宽度
    updateHistoryItemWidths();

    // 更新历史面板位置
    QPoint historyPos = historyPanel->pos();
    historyPos.setY(isHistoryVisible ?
                  height() - historyPanel->height() - ui->controlBar->height() :
                  height());
    historyPanel->move(historyPos);
}

void VideoPlayer::on_pushButton_Min_clicked()
{
    this->showMinimized();
}

void VideoPlayer::on_pushButton_Close_clicked()
{
    this->close();
}

//倍速
void VideoPlayer::setPlaybackSpeed(float speed)
{
    if (player)
    {
        player->setPlaybackRate(speed);
    }
}

//音量调节相关
void VideoPlayer::on_pushButton_Sound_clicked()
{

    toggleVolumeSlider(!sliderVisible);
}

// 初始化音量控制组件
void VideoPlayer::initVolumeControl()
{
    // 创建音量滑块
    volumeSlider = new QSlider(Qt::Vertical, this); // 改为垂直滑块
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(50);
    //volumeSlider->setFixedSize(30, 100); // 设置适当大小
    volumeSlider->setFixedSize(ui->pushButton_Sound->width(), 100);
    volumeSlider->setStyleSheet(Theme::Light.VolumeSlider);
    volumeSlider->hide(); // 初始隐藏
    // 添加音量显示标签
        volumeLabel = new QLabel(this);
        volumeLabel->setAlignment(Qt::AlignCenter);
        volumeLabel->setStyleSheet(
            "background: rgba(0,0,0,150);"
            "color: white;"
            "border-radius: 4px;"
            "padding: 2px 6px;"
            "font-size: 12px;"
        );
        volumeLabel->hide();
        volumeLabel->setFixedSize(40, 24);
    // 创建动画效果
    slideAnimation = new QPropertyAnimation(volumeSlider, "pos", this);
    slideAnimation->setDuration(200);
    slideAnimation->setEasingCurve(QEasingCurve::OutQuad);

    connect(volumeSlider, &QSlider::valueChanged, this, &VideoPlayer::onVolumeChanged);
    player->setVolume(50);

}

void VideoPlayer::toggleVolumeSlider(bool show)
{
    if (sliderVisible == show) return;

    sliderVisible = show;

    // 计算按钮中心点上方位置
    QPoint btnCenter = ui->pushButton_Sound->rect().center();
    QPoint globalPos = ui->pushButton_Sound->mapToGlobal(btnCenter);
    QPoint windowPos = mapFromGlobal(globalPos);

    // 调整滑块位置（上移20px）
    int x = windowPos.x() - volumeSlider->width()/2;
    int y = windowPos.y() - volumeSlider->height() - 30;

    // 先断开之前的动画完成连接
    disconnect(slideAnimation, &QPropertyAnimation::finished, nullptr, nullptr);

    if (show) {
        volumeSlider->move(x, y);
        volumeSlider->show();
        volumeSlider->raise();

        // 设置标签位置
        volumeLabel->move(x + (volumeSlider->width() - volumeLabel->width())/2, y - 28);
        volumeLabel->show();
        volumeLabel->setText(QString::number(player->volume()) + "%");

        // 滑入动画
        slideAnimation->setStartValue(QPoint(x, y + 20));
        slideAnimation->setEndValue(QPoint(x, y));
        slideAnimation->start();
    } else {
        // 滑出动画
        slideAnimation->setStartValue(volumeSlider->pos());
        slideAnimation->setEndValue(volumeSlider->pos() + QPoint(0, 20));
        connect(slideAnimation, &QPropertyAnimation::finished, this, [this]() {
            volumeSlider->hide();
            volumeLabel->hide();
        }, Qt::UniqueConnection); // 使用UniqueConnection避免重复连接
        slideAnimation->start();
    }
}
// 音量改变处理
void VideoPlayer::onVolumeChanged(int value)
{
    player->setVolume(value);
    volumeLabel->setText(QString::number(value) + "%"); // 实时更新标签
}

void VideoPlayer::on_pushButton_Pre_clicked()
{
    if (listWidget->count() == 0) return;

    int newIndex = -1;

    switch (currentPlayMode) {
    case Sequence: // 顺序播放 - 默认模式
        newIndex = currentPlayIndex - 1;
        if (newIndex < 0) {
            // 如果已经是第一首，根据用户偏好可以选择：
            // 1. 不处理（停在第一首）
            // newIndex = 0;

            // 2. 循环到最后一首（更符合用户预期）
            newIndex = listWidget->count() - 1;
        }
        break;

    case Loop: // 循环播放当前项
        newIndex = currentPlayIndex;
        break;

    case Random: // 随机播放
        do {
            newIndex = QRandomGenerator::global()->bounded(listWidget->count());
        } while (newIndex == currentPlayIndex && listWidget->count() > 1); // 避免重复
        break;
    }

    QListWidgetItem *item = listWidget->item(newIndex);
    if (item) {
        onPlaylistItemDoubleClicked(item);
    }
}

void VideoPlayer::on_pushButton_Next_clicked()
{
    if (listWidget->count() == 0) return;

    int newIndex = -1;

    switch (currentPlayMode) {
    case Sequence: // 顺序播放 - 默认模式
        newIndex = currentPlayIndex + 1;
        if (newIndex >= listWidget->count()) {
            newIndex = 0; // 循环到开头
        }
        break;

    case Loop: // 循环播放当前项
        newIndex = currentPlayIndex;
        break;

    case Random: // 随机播放
        do {
            newIndex = QRandomGenerator::global()->bounded(listWidget->count());
        } while (newIndex == currentPlayIndex && listWidget->count() > 1); // 避免重复
        break;
    }

    QListWidgetItem *item = listWidget->item(newIndex);
    if (item) {
        onPlaylistItemDoubleClicked(item);
    }
}

void VideoPlayer::onSearchTextChanged(const QString &text) {
    QString keyword = text.trimmed().toLower();

    int visibleCount = 0;
    for(int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem *item = listWidget->item(i);
        if(PlaylistItem *widget = qobject_cast<PlaylistItem*>(listWidget->itemWidget(item))) {
            QString title = widget->getTitle().toLower();
            bool match = title.contains(keyword);
            item->setHidden(!match);
            if(match) visibleCount++;
        }
    }

    // 修复：当搜索为空时恢复原始计数显示
    if (keyword.isEmpty()) {
        songCountLabel->setText(QString("%1 个视频").arg(listWidget->count()));
    } else {
        //songCountLabel->setText(QString("%1 个视频（找到 %2 个）").arg(listWidget->count()).arg(visibleCount)); // 注意这里的半角括号
        songCountLabel->setText(QString("找到 %1 个").arg(visibleCount)); // 注意这里的半角括号
    }
}

void VideoPlayer::initHistoryUI()
{
    // ========== 历史列表面板初始化 ==========
    historyPanel = new QWidget(this);
    historyPanel->setFixedSize(width(), 300);
    historyPanel->move(0, height());
    historyPanel->setStyleSheet(Theme::Light.HistoryPanel);
    historyPanel->raise();

    // ========== 主布局设置 ==========
    QVBoxLayout *mainLayout = new QVBoxLayout(historyPanel);
    mainLayout->setContentsMargins(4, 8, 4, 4);
    mainLayout->setSpacing(4);

    // ========== 顶部控制栏布局 ==========
    QHBoxLayout *topControlLayout = new QHBoxLayout();
    topControlLayout->setContentsMargins(0, 0, 0, 0);
    topControlLayout->setSpacing(8);

    // 左侧：历史播放标签
    historyLabel = new QLabel("历史播放", historyPanel);
    historyLabel->setStyleSheet(Theme::Light.HistoryLabel);

    // 右侧：清除历史按钮
    clearHistoryBtn = new QPushButton("清除历史", historyPanel);
    clearHistoryBtn->setStyleSheet(Theme::Light.ClearButton);

    // 将控件添加到顶部布局
    topControlLayout->addWidget(historyLabel);
    topControlLayout->addStretch(); // 添加弹簧使按钮靠右
    topControlLayout->addWidget(clearHistoryBtn);

    // ========== 历史列表控件 ==========
    historyListWidget = new QListWidget(historyPanel);
    // 在历史列表控件的初始化代码中设置样式表
    historyListWidget->setSelectionMode(QAbstractItemView::SingleSelection);  // 设置单选模式
    historyListWidget->setStyleSheet(Theme::Light.HistoryList);
    // 确保列表项宽度适应
    connect(historyListWidget->verticalScrollBar(), &QScrollBar::rangeChanged, this, [this](){
        updateHistoryItemWidths();
    });

    // ========== 组装最终布局 ==========
    mainLayout->addLayout(topControlLayout);  // 添加顶部控制栏
    mainLayout->addWidget(historyListWidget); // 添加历史列表

    // ========== 按钮信号连接 ==========
    connect(clearHistoryBtn, &QPushButton::clicked, this, [this](){
        if (QMessageBox::question(this, "确认清除", "确定要清除所有播放历史吗？",
                                QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            QSqlQuery("DELETE FROM play_history");
            historyListWidget->clear();
        }
    });
    // ========== 信号连接 ==========
    connect(historyListWidget, &QListWidget::itemSelectionChanged, this, [this]() {
            for (int i = 0; i < historyListWidget->count(); ++i) {
                QListWidgetItem* item = historyListWidget->item(i);
                if (PlaylistItem* widget = qobject_cast<PlaylistItem*>(historyListWidget->itemWidget(item))) {
                    widget->setSelected(item->isSelected());
                    // 连接删除信号
                    disconnect(widget, &PlaylistItem::deleteRequested, nullptr, nullptr);
                    connect(widget, &PlaylistItem::deleteRequested, this, [this, item]() {
                        if (QMessageBox::question(this, "确认删除", "确定要删除此历史记录吗？",
                                                QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                            QString path = item->data(Qt::UserRole).toString();
                            QSqlQuery query(QSqlDatabase::database("vedio_connection"));
                            query.prepare("DELETE FROM play_history WHERE file_path = ?");
                            query.addBindValue(path);
                            if (query.exec()) {
                                delete historyListWidget->takeItem(historyListWidget->row(item));
                            }
                        }
                    });
                }
            }
        });
    // 历史项双击播放
    connect(historyListWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item){
        // 先清除其他项的选中状态
        for (int i = 0; i < historyListWidget->count(); ++i) {
            QListWidgetItem* otherItem = historyListWidget->item(i);
            otherItem->setSelected(false);
            if (PlaylistItem* widget = qobject_cast<PlaylistItem*>(historyListWidget->itemWidget(otherItem))) {
                widget->setSelected(false);
            }
        }

        // 设置当前项为选中状态
        item->setSelected(true);
        if (PlaylistItem* widget = qobject_cast<PlaylistItem*>(historyListWidget->itemWidget(item))) {
            widget->setSelected(true);
        }
        QString path = item->data(Qt::UserRole).toString();

        if (!isVideoAvailable(path)) {
            QMessageBox::StandardButton reply = QMessageBox::question(this, "视频不可用",
                "该视频已从播放列表删除，是否重新添加？",
                QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::Yes) {
                // 触发添加文件流程
                QString newPath = QFileDialog::getOpenFileName(this, "重新选择视频文件",
                    QFileInfo(path).path(), "视频文件 (*.mp4 *.avi *.mkv *.mov);;所有文件 (*.*)");

                if (!newPath.isEmpty()) {
                    // 添加回播放列表
                    on_pushButton_Add_clicked();
                }
            }
            return;
        }
        // 插入新的播放记录
        QSqlQuery query(QSqlDatabase::database("vedio_connection"));
        query.prepare("INSERT INTO play_history (file_path, play_time) VALUES (?, datetime('now', 'localtime'))");
        query.addBindValue(path);
        query.exec();

        // 刷新历史列表
        loadHistoryFromDatabase();
        // 正常播放流程
        player->setMedia(QUrl::fromLocalFile(path));
        player->play();
        // 更新当前播放索引和UI
        ui->labelCurrentMdia->setText(QFileInfo(path).fileName());
        currentTheme == "深色主题" ? ui->pushButton_Player->setIcon(QIcon(":/white/暂停播放.svg")) :
                                 ui->pushButton_Player->setIcon(QIcon(":/black/暂停播放.svg"));
        ui->pushButton_Player->setIconSize(QSize(25, 25));

        // 选中播放列表中对应的项
        for (int i = 0; i < listWidget->count(); ++i) {
            QListWidgetItem *playlistItem = listWidget->item(i);
            if (playlistItem->data(Qt::UserRole).toString() == path) {
                listWidget->setCurrentItem(playlistItem);
                currentPlayIndex = i;
                break;
            }
        }
    });

    // ========== 动画设置 ==========
    historyPanelAnimation = new QPropertyAnimation(historyPanel, "pos");
    historyPanelAnimation->setDuration(300);
    historyPanelAnimation->setEasingCurve(QEasingCurve::OutQuad);
}

void VideoPlayer::loadHistoryFromDatabase() {
    historyListWidget->clear();

    QSqlQuery query(QSqlDatabase::database("vedio_connection"));
        query.prepare(R"(
            SELECT
                h.file_path,
                h.play_time,
                p.title,
                p.cover
            FROM play_history h
            LEFT JOIN playlist p ON h.file_path = p.file_path
            ORDER BY h.play_time DESC
            LIMIT 100
        )");

    if (!query.exec()) {
        qWarning() << "Load history failed:" << query.lastError();
        return;
    }

    int index = 1;
    while (query.next()) {
        QString path = query.value(0).toString();
        bool isAvailable = isVideoAvailable(path);
        QDateTime playTime = query.value(1).toDateTime();
        QString title = query.value(2).toString();
        QByteArray coverData = query.value(3).toByteArray();

        // 处理无效时间
        if (!playTime.isValid()) {
            qWarning() << "Invalid play_time for:" << path;
            playTime = QDateTime::currentDateTime();
        }

        QPixmap cover;
        if (!coverData.isEmpty()) {
            cover.loadFromData(coverData, "PNG");
            cover = cover.scaled(80, 50, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        } else {
            cover.load(":/default_cover.png");
            cover = cover.scaled(80, 50, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        }

        QListWidgetItem *item = new QListWidgetItem();
        item->setData(Qt::UserRole, path);
        item->setSizeHint(QSize(0, 60));

        PlaylistItem *widget = new PlaylistItem();
        widget->setData(
            index++,
            title.isEmpty() ? QFileInfo(path).fileName() : title,
            playTime.toString("yyyy-MM-dd HH:mm:ss"), // 完整时间格式
            cover
        );
        widget->setDisabled(!isAvailable);  // 禁用不可用项
                if (!isAvailable) {
                    widget->setSubtitle("[已删除] " + widget->getTitle() );
                    widget->setStyleSheet("QLabel { color: #999; }");
                }

        historyListWidget->addItem(item);
        historyListWidget->setItemWidget(item, widget);
        widget->updateTheme(currentTheme == "深色主题" ? Theme::Dark : Theme::Light);

    }

}

void VideoPlayer::toggleHistoryVisibility()
{
    closeOtherPanels(historyPanel);
    QPoint endPos = isHistoryVisible ?
        QPoint(0, height()) :
        QPoint(0, height() - historyPanel->height() - ui->controlBar->height());

    historyPanelAnimation->setStartValue(historyPanel->pos());
    historyPanelAnimation->setEndValue(endPos);
    historyPanelAnimation->start();

    if (!isHistoryVisible) {
        loadHistoryFromDatabase();
        historyPanel->setFixedWidth(width()); // 确保宽度与窗口一致
        updateHistoryItemWidths(); // 更新列表项宽度
    }

    isHistoryVisible = !isHistoryVisible;
}

void VideoPlayer::onHistoryItemDoubleClicked(QListWidgetItem *item)
{
    QString path = item->data(Qt::UserRole).toString();

    // 更新播放历史记录
    QSqlQuery query(QSqlDatabase::database("vedio_connection"));
    query.prepare("INSERT INTO play_history (file_path, play_time) VALUES (?, datetime('now', 'localtime'))");
    query.addBindValue(path);
    query.exec();

    // 刷新历史列表
    loadHistoryFromDatabase();

    // 播放选中的视频
    player->setMedia(QUrl::fromLocalFile(path));
    player->play();
    ui->labelCurrentMdia->setText(QFileInfo(path).fileName());

    // 更新播放按钮状态
    ui->pushButton_Player->setIcon(QIcon(":/new/prefix1/images/暂停1.svg"));
    ui->pushButton_Player->setIconSize(QSize(25, 25));

    // 查找并选中播放列表中对应的项
    for (int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem *playlistItem = listWidget->item(i);
        if (playlistItem->data(Qt::UserRole).toString() == path) {
            currentPlayIndex = i;
            listWidget->setCurrentItem(playlistItem);
            break;
        }
    }
}

void VideoPlayer::updateHistoryItemWidths()
{
    // 计算考虑滚动条后的有效宽度
    int scrollBarWidth = historyListWidget->verticalScrollBar()->isVisible() ?
        historyListWidget->verticalScrollBar()->width() : 0;
    int effectiveWidth = historyPanel->width() - scrollBarWidth - 20; // 减去边距

    // 更新所有历史列表项
    for(int i = 0; i < historyListWidget->count(); ++i) {
        QListWidgetItem* item = historyListWidget->item(i);
        item->setSizeHint(QSize(effectiveWidth, 60));

        // 确保自定义widget也更新
        if(PlaylistItem* widget = qobject_cast<PlaylistItem*>(historyListWidget->itemWidget(item))) {
            widget->setFixedWidth(effectiveWidth);
        }
    }
}

bool VideoPlayer::isVideoAvailable(const QString& filePath) const
{
    QSqlQuery query(QSqlDatabase::database("vedio_connection"));
    query.prepare("SELECT COUNT(*) FROM playlist WHERE file_path = ?");
    query.addBindValue(filePath);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

//按动其他键面板消失
bool VideoPlayer::event(QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        const QPoint globalPos = mouseEvent->globalPos();

        // 只处理窗口内的点击事件
        if (this->geometry().contains(globalPos)) {
            if (!playlistPanel->geometry().contains(mapFromGlobal(globalPos)) &&
                !historyPanel->geometry().contains(mapFromGlobal(globalPos)) &&
                !volumeSlider->geometry().contains(mapFromGlobal(globalPos)))
            {
                closeAllPanels();
            }
        }
    }
    // 移除WindowDeactivate事件的处理
    return QMainWindow::event(event);
}

void VideoPlayer::closeAllPanels()
{
    if (isPlaylistVisible) {
        togglePlaylistVisibility();
    }
    if (isHistoryVisible) {
        toggleHistoryVisibility();
    }
    if (sliderVisible) {
        toggleVolumeSlider(false);
    }
}

void VideoPlayer::closeOtherPanels(QWidget* currentPanel) {
    if (isPlaylistVisible && currentPanel != playlistPanel) {
        togglePlaylistVisibility();
    }
    if (isHistoryVisible && currentPanel != historyPanel) {
        toggleHistoryVisibility();
    }
    if (sliderVisible && currentPanel != volumeSlider) {
        toggleVolumeSlider(false);
    }
}

//拖动添加文件
void VideoPlayer::dragEnterEvent(QDragEnterEvent *event)
{
    // 检查拖入的内容是否包含文件
    if (event->mimeData()->hasUrls()) {
        // 检查每个URL是否是有效的视频文件
        const QList<QUrl> urls = event->mimeData()->urls();
        for (const QUrl &url : urls) {
            QString filePath = url.toLocalFile();
            QString suffix = QFileInfo(filePath).suffix().toLower();
            if (suffix == "mp4" || suffix == "avi" || suffix == "mkv" || suffix == "mov") {
                event->acceptProposedAction();
                return;
            }
        }
    }
    event->ignore();
}

void VideoPlayer::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasUrls()) {
        event->ignore();
        return;
    }

    QStringList files;
    const QList<QUrl> urls = event->mimeData()->urls();
    for (const QUrl &url : urls) {
        QString filePath = url.toLocalFile();
        QString suffix = QFileInfo(filePath).suffix().toLower();
        if (suffix == "mp4" || suffix == "avi" || suffix == "mkv" || suffix == "mov") {
            files.append(filePath);
        }
    }

    if (files.isEmpty()) {
        QMessageBox::information(this, "提示", "没有有效的视频文件被拖放！");
        return;
    }

    // 使用已有的添加文件逻辑
    addFilesToPlaylist(files);
}

void VideoPlayer::addFilesToPlaylist(const QStringList &files)
{
    // 过滤掉已经在播放列表中的文件
    QStringList newFiles;
    for (const QString &file : files) {
        bool alreadyExists = false;
        for (int i = 0; i < listWidget->count(); ++i) {
            QListWidgetItem *item = listWidget->item(i);
            if (item->data(Qt::UserRole).toString() == file) {
                alreadyExists = true;
                break;
            }
        }
        if (!alreadyExists) {
            newFiles.append(file);
        }
    }

    if (newFiles.isEmpty()) {
        QMessageBox::information(this, "提示", "所有选择的文件已在播放列表中！");
        return;
    }

    // 显示添加进度对话框
    QProgressDialog progress("添加文件中...", "取消", 0, newFiles.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);

    int addedCount = 0;
    for (const QString &file : newFiles) {
        progress.setValue(addedCount);
        progress.setLabelText(QString("正在添加: %1").arg(QFileInfo(file).fileName()));

        if (progress.wasCanceled()) {
            break;
        }

        // 先设置媒体但不播放
        player->setMedia(QUrl::fromLocalFile(file));
        ui->labelCurrentMdia->setText(QFileInfo(file).fileName());

        // 连接 durationChanged 信号，异步获取时长
        QEventLoop loop;
        QObject::connect(player, &QMediaPlayer::durationChanged, &loop, [&](qint64 duration) {
            QObject::disconnect(player, &QMediaPlayer::durationChanged, &loop, nullptr);

            if (duration <= 0) {
                qWarning() << "Invalid duration for file:" << file;
                loop.quit();
                return;
            }

            // 生成封面
            QPixmap cover;
            if (!firstFrame.isNull()) {
                cover = QPixmap::fromImage(firstFrame.scaled(80, 50, Qt::KeepAspectRatioByExpanding));
            } else {
                cover.load(":/default_cover.png");
            }

            // 创建列表项
            QListWidgetItem *item = new QListWidgetItem();
            item->setData(Qt::UserRole, file);
            item->setSizeHint(QSize(0, 60));

            PlaylistItem *widget = new PlaylistItem();
            widget->setData(++currentPlaylistIndex,
                           QFileInfo(file).fileName(),
                           QTime(0, 0).addMSecs(duration).toString("hh:mm:ss"),
                           cover);

            listWidget->addItem(item);
            listWidget->setItemWidget(item, widget);
            connect(widget, &PlaylistItem::deleteRequested, this, [this, item]() {
                if (QMessageBox::question(this, "确认删除", "确定要删除此项吗？",
                                        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                    deletePlaylistItem(item);
                }
            });

            // 保存到数据库
            saveToDatabase(file, QFileInfo(file).fileName(), duration);

            addedCount++;
            loop.quit();
        });

        // 重置帧捕获状态
        frameCaptured = false;

        // 等待获取duration
        loop.exec();
    }

    progress.setValue(newFiles.size());
    updateSongCount();

    if (addedCount > 0) {
        QMessageBox::information(this, "添加完成",
            QString("成功添加 %1 个视频文件").arg(addedCount));
    }
}

/* 槽函数：更新总时长显示 */
void VideoPlayer::onDurationChanged(qint64 duration)
{
    ui->horizontalSlider_Position->setMaximum(duration);
    ui->label_Ratio->setStyleSheet(Theme::Light.CountLabel);
    int secs = duration / 1000;
    int hours = secs / 3600;
    int mins = (secs % 3600) / 60;
    secs %= 60;
    durationTime = QString("%1:%2:%3")
        .arg(hours, 2, 10, QLatin1Char('0'))
        .arg(mins, 2, 10, QLatin1Char('0'))
        .arg(secs, 2, 10, QLatin1Char('0'));
    ui->label_Ratio->setText(positionTime + " / " + durationTime);
}

/* 槽函数：更新当前播放位置显示 */
void VideoPlayer::onPositionChanged(qint64 position)
{
    if (ui->horizontalSlider_Position->isSliderDown()) return; // 拖动时忽略自动更新

    ui->horizontalSlider_Position->setSliderPosition(position);
    ui->label_Ratio->setStyleSheet(Theme::Light.CountLabel);
    int secs = position / 1000;
    int hours = secs / 3600;
    int mins = (secs % 3600) / 60;
    secs %= 60;
    positionTime = QString("%1:%2:%3")
        .arg(hours, 2, 10, QLatin1Char('0'))
        .arg(mins, 2, 10, QLatin1Char('0'))
        .arg(secs, 2, 10, QLatin1Char('0'));
    ui->label_Ratio->setText(positionTime + " / " + durationTime);
}

//双击播放，点击播放键
void VideoPlayer::onPlaylistItemDoubleClicked(QListWidgetItem *item)
{
    QString path = item->data(Qt::UserRole).toString();
    QSqlQuery query(QSqlDatabase::database("vedio_connection"));
        query.prepare("INSERT INTO play_history (file_path, play_time) VALUES (?, datetime('now', 'localtime'))");
        query.addBindValue(path);
        query.exec();

    // 刷新历史列表
    loadHistoryFromDatabase();
    player->setMedia(QUrl::fromLocalFile(path));
    player->play();
    ui->labelCurrentMdia->setText(QFileInfo(path).fileName());
    currentPlayIndex = listWidget->row(item); // 强制更新当前播放索引
    currentTheme == "深色主题" ? ui->pushButton_Player->setIcon(QIcon(":/white/暂停播放.svg")) :
                             ui->pushButton_Player->setIcon(QIcon(":/black/暂停播放.svg"));
    ui->pushButton_Player->setIconSize(QSize(25, 25));
}

void VideoPlayer::on_pushButton_Player_clicked()
{
    if (player->state() == QMediaPlayer::PlayingState)
    {
        // 如果正在播放，则暂停
        player->pause();
        currentTheme == "深色主题" ? ui->pushButton_Player->setIcon(QIcon(":/white/开始播放 .svg")) :
                                 ui->pushButton_Player->setIcon(QIcon(":/black/开始播放.svg"));
    }
    else
    {
        if (player->mediaStatus() == QMediaPlayer::NoMedia && listWidget->count() > 0)
        {
            // 如果没有加载媒体但播放列表不为空，播放第一项
            QListWidgetItem* firstItem = listWidget->item(0);
            onPlaylistItemDoubleClicked(firstItem);
        }
        else
        {
            // 否则正常播放
            player->play();
        }
        currentTheme == "深色主题" ? ui->pushButton_Player->setIcon(QIcon(":/white/暂停播放.svg")) :
                                 ui->pushButton_Player->setIcon(QIcon(":/black/暂停播放.svg"));
    }
    ui->pushButton_Player->setIconSize(QSize(25, 25));
}

//更新图标
void VideoPlayer::updatePlayModeIcon()
{
    QString iconPath;
    QString toolTip;

    if (currentTheme == "深色主题")
    {
        switch(currentPlayMode)
        {
        case Sequence:
            iconPath = ":/white/顺序播放.svg";
            toolTip = tr("顺序播放");
            break;
        case Loop:
            iconPath = ":/white/循环播放.svg";
            toolTip = tr("循环播放");
            break;
        case Random:
            iconPath = ":/white/随机播放.svg";
            toolTip = tr("随机播放");
            break;
        }
    }
    else
    {
        switch(currentPlayMode)
        {
        case Sequence:
            iconPath = ":/black/顺序播放.svg";
            toolTip = tr("顺序播放");
            break;
        case Loop:
            iconPath = ":/black/循环播放.svg";
            toolTip = tr("循环播放");
            break;
        case Random:
            iconPath = ":/black/随机播放.svg";
            toolTip = tr("随机播放");
            break;
        }
    }

    ui->pushButton_Mode->setIcon(QIcon(iconPath));
    ui->pushButton_Mode->setToolTip(toolTip);
    ui->pushButton_Mode->setAccessibleName(toolTip);
    ui->pushButton_Mode->setIconSize(QSize(25, 25));
}

void VideoPlayer::updateMaxIcon(bool isFullscreen)
{
    QString iconPath;

    if (currentTheme == "深色主题") {
        // 深色主题图标
        if (isFullscreen) {
            iconPath = ":/white/退出最大化.svg";  // 深色主题的"退出全屏"图标
        } else {
            iconPath = ":/white/最大化.svg";     // 深色主题的"最大化"图标
        }
    } else {
        // 浅色主题图标
        if (isFullscreen) {
            iconPath = ":/black/退出最大化.svg";  // 浅色主题的"退出全屏"图标
        } else {
            iconPath = ":/black/最大化 .svg";      // 浅色主题的"最大化"图标
        }
    }

    // 设置图标
    ui->pushButton_Max->setIcon(QIcon(iconPath));
    ui->pushButton_Max->setIconSize(QSize(25, 25));  // 统一图标大小
}

void VideoPlayer::createMenu() {
    // 创建主菜单
    mainMenu = new QMenu(this);
    mainMenu->setStyleSheet(Theme::Light.Menu);

    // 1. 主题切换子菜单
    themeMenu = new QMenu("切换主题", mainMenu);
    //themeMenu->setIcon(QIcon(":/new/prefix1/images/更换主题.svg"));
    themeMenu->setStyleSheet(Theme::Light.Menu);

    // 2. 说明选项
    QAction *aboutAction = new QAction("说明", mainMenu);
    //aboutAction->setIcon(QIcon(":/new/prefix1/images/info_icon.svg"));
    connect(aboutAction, &QAction::triggered, this, &VideoPlayer::showAboutDialog);

    // 添加到主菜单
    mainMenu->addMenu(themeMenu);
    mainMenu->addSeparator();
    mainMenu->addAction(aboutAction);

    // 设置主题菜单
    setupThemeConnections();

    // 连接菜单按钮
    connect(ui->pushButton_Theme, &QPushButton::clicked, this, [this]() {
        this->mainMenu->exec(ui->pushButton_Theme->mapToGlobal(QPoint(0, ui->pushButton_Theme->height())));
    });
}

// 添加主题相关实现
void VideoPlayer::applyTheme(const QString &themeName)
{
    currentTheme = themeName;
    const Theme::Style& theme = (themeName == "深色主题") ? Theme::Dark : Theme::Light;
    qApp->setStyleSheet(theme.MessageBox);
    if (themeName == "深色主题")
    {
        this->setStyleSheet(Theme::Dark.MainWindow);
        ui->graphicsView->setStyleSheet(Theme::Dark.MainWindow);
        ui->titleBar->setStyleSheet(Theme::Dark.TitleBar);
        ui->controlBar->setStyleSheet(Theme::Dark.ControlBar);
        ui->horizontalSlider_Position->setStyleSheet(Theme::Dark.PositionSlider);
        ui->label_Ratio->setStyleSheet(Theme::Dark.CountLabel);
        volumeSlider->setStyleSheet(Theme::Dark.VolumeSlider);

        playlistPanel->setStyleSheet(Theme::Dark.PlaylistPanel);
        listWidget->setStyleSheet(Theme::Dark.Playlist);
        searchEdit->setStyleSheet(Theme::Dark.SearchEdit);
        songCountLabel->setStyleSheet(Theme::Dark.CountLabel);
        clearAllButton->setStyleSheet(Theme::Dark.ClearButton);

        historyPanel->setStyleSheet(Theme::Dark.HistoryPanel);
        historyListWidget->setStyleSheet(Theme::Dark.HistoryList);
        historyLabel->setStyleSheet(Theme::Dark.HistoryLabel);
        clearHistoryBtn->setStyleSheet(Theme::Dark.ClearButton);

        speedMenu->setStyleSheet(Theme::Dark.Menu);
        mainMenu->setStyleSheet(Theme::Dark.Menu);
        themeMenu->setStyleSheet(Theme::Dark.Menu);
        // 更新按钮样式
        QPixmap pixmap(":/white/视频播放器_.svg");
        ui->label->setPixmap(pixmap.scaled(ui->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->pushButton_Min->setIcon(QIcon(":/white/最小化 .svg"));
        ui->pushButton_Close->setIcon(QIcon(":/white/关闭.svg"));
        ui->pushButton_Max->setIcon(QIcon(":/white/最大化.svg"));
        ui->pushButton_Theme->setIcon(QIcon(":/white/更多 .svg"));
        ui->pushButton_Pre->setIcon(QIcon(":/white/上一个.svg"));
        if (player->state() == QMediaPlayer::PlayingState)
        {
            ui->pushButton_Player->setIcon(QIcon(":/white/暂停播放.svg"));
        }
        else
        {
            ui->pushButton_Player->setIcon(QIcon(":/white/开始播放 .svg"));
        }
        ui->pushButton_Next->setIcon(QIcon(":/white/下一个 下一首.svg"));
        ui->pushButton_Add->setIcon(QIcon(":/white/添加文件.svg"));
        ui->pushButton_Sound->setIcon(QIcon(":/white/声音.svg"));
        ui->pushButton_list->setIcon(QIcon(":/white/播放列表.svg"));
        ui->pushButton_History->setIcon(QIcon(":/white/历史记录 .svg"));
        ui->pushButton_Mode->setIcon(QIcon(":/white/顺序播放.svg")); // 播放模式图标
    }
    else
    {
        // 浅色主题
        this->setStyleSheet(Theme::Light.MainWindow);
        ui->graphicsView->setStyleSheet(Theme::Light.MainWindow);
        ui->titleBar->setStyleSheet(Theme::Light.TitleBar);
        ui->controlBar->setStyleSheet(Theme::Light.ControlBar);
        ui->horizontalSlider_Position->setStyleSheet(Theme::Light.PositionSlider);
        volumeSlider->setStyleSheet(Theme::Light.VolumeSlider);
        ui->label_Ratio->setStyleSheet(Theme::Light.CountLabel);

        playlistPanel->setStyleSheet(Theme::Light.PlaylistPanel);
        listWidget->setStyleSheet(Theme::Light.Playlist);
        searchEdit->setStyleSheet(Theme::Light.SearchEdit);
        songCountLabel->setStyleSheet(Theme::Light.CountLabel);
        clearAllButton->setStyleSheet(Theme::Light.ClearButton);

        historyPanel->setStyleSheet(Theme::Light.HistoryPanel);
        historyListWidget->setStyleSheet(Theme::Light.HistoryList);
        historyLabel->setStyleSheet(Theme::Light.HistoryLabel);
        clearHistoryBtn->setStyleSheet(Theme::Light.ClearButton);

        speedMenu->setStyleSheet(Theme::Light.Menu);
        mainMenu->setStyleSheet(Theme::Light.Menu);

        QPixmap pixmap(":/black/视频播放器_.svg");
        ui->label->setPixmap(pixmap.scaled(ui->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->pushButton_Min->setIcon(QIcon(":/black/最小化 .svg"));
        ui->pushButton_Close->setIcon(QIcon(":/black/关闭.svg"));
        ui->pushButton_Max->setIcon(QIcon(":/black/最大化 .svg"));
        ui->pushButton_Theme->setIcon(QIcon(":/black/更多.svg"));
        ui->pushButton_Pre->setIcon(QIcon(":/black/上一个.svg"));
        if (player->state() == QMediaPlayer::PlayingState)
        {
            ui->pushButton_Player->setIcon(QIcon(":/black/暂停播放.svg"));
        }
        else
        {
            ui->pushButton_Player->setIcon(QIcon(":/black/开始播放.svg"));
        }
        ui->pushButton_Next->setIcon(QIcon(":/black/下一个 .svg"));
        ui->pushButton_Add->setIcon(QIcon(":/black/添加文件.svg"));
        ui->pushButton_list->setIcon(QIcon(":/black/播放列表.svg"));
        ui->pushButton_History->setIcon(QIcon(":/black/历史记录 .svg"));
        ui->pushButton_Sound->setIcon(QIcon(":/black/声音.svg"));
        ui->pushButton_Mode->setIcon(QIcon(":/black/顺序播放.svg")); // 播放模式图标
    }
    // 更新所有播放列表项
    for(int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem* item = listWidget->item(i);
        if(PlaylistItem* widget = qobject_cast<PlaylistItem*>(listWidget->itemWidget(item))) {
            widget->updateTheme(theme);
        }
    }

    // 更新所有历史列表项
    for(int i = 0; i < historyListWidget->count(); ++i) {
        QListWidgetItem* item = historyListWidget->item(i);
        if(PlaylistItem* widget = qobject_cast<PlaylistItem*>(historyListWidget->itemWidget(item))) {
            widget->updateTheme(theme);
        }
    }

    // 保存偏好
    saveThemePreference(themeName);
    update();
}

void VideoPlayer::setupThemeConnections() {
    // 连接主题菜单
    QStringList themes = {"深色主题", "浅色主题", "跟随系统"};
    QActionGroup *themeGroup = new QActionGroup(this);

    for (const QString &theme : themes) {
        QAction *action = themeMenu->addAction(theme);
        action->setCheckable(true);
        action->setActionGroup(themeGroup);
        if (theme == currentTheme) {
            action->setChecked(true);
        }
        connect(action, &QAction::triggered, this, [this, theme]() {
            applyTheme(theme);
        });
    }
}

void VideoPlayer::saveThemePreference(const QString &theme)
{
    QSettings settings;
    settings.setValue("Theme/Current", theme);
}

QString VideoPlayer::loadThemePreference()
{
    QSettings settings;
    return settings.value("Theme/Current", "浅色主题").toString();
}

void VideoPlayer::applyCurrentTheme()
{
    QString theme = loadThemePreference();
    applyTheme(theme);
    qApp->processEvents();
    style()->unpolish(this);
    style()->polish(this);
}

void VideoPlayer::showAboutDialog()
{
    QMessageBox::about(this, "说明",
        "<h2>视频播放器</h2>"
        "<p>功能:</p>"
        "<ul>"
        "<li>支持多种视频格式播放</li>"
        "<li>播放列表管理</li>"
        "<li>播放历史记录（播放时间显示）</li>"
        "<li>多种主题切换</li>"
        "<li>倍速播放</li>"
        "</ul>"
        "<p>使用说明:</p>"
        "<p>拖放视频文件到窗口可快速添加到播放列表</p>"
        "<p>双击列表项播放，单击列表项进行删除</p>"

                       );

}

