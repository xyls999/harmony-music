#include "footwidget.h"
#include "ui_footwidget.h"
#include "mymainwindow.h"

footWidget::footWidget(QWidget *parent,Lyric *lyric)
    : DWidget(parent),m_lyric(lyric),
    ui(new Ui::footWidget)
{
    ui->setupUi(this);

    initSpeedControl();


    initUI();
    applyAnimationToAllButtons();

    updateTheme(m_theme.currentTheme());

    connect(CoverCacheManager::instance(), &CoverCacheManager::coverLoaded, this, &footWidget::updateCover);

    m_hoverTimer = new QTimer(this);
       m_hoverTimer->setSingleShot(true);
       connect(m_hoverTimer, &QTimer::timeout, [this](){
           if (m_isHovering && !m_popupVisible) {
               popup->showAboveWidget(ui->Volumebtn);
               m_popupVisible = true;
           }
       });
    ui->favorbtn->setCheckable(true);
       connect(ui->favorbtn, &QPushButton::clicked,
               this, &footWidget::on_favorbtn_clicked);

    // 连接播放器信号
    connect(m_player, &QMediaPlayer::positionChanged, this, &footWidget::updatePosition);
    connect(m_player, &QMediaPlayer::durationChanged, this, &footWidget::updateDuration);
    connect(ui->positionSlider, &QSlider::sliderMoved, this, &footWidget::setPosition);

    //倍速
    connect(m_player, &QMediaPlayer::playbackRateChanged, [=](float rate){
        ui->SpeedButton->setText(QString("%1x").arg(rate));
        if (m_speedActions.contains(rate)) {
            m_speedActions[rate]->setChecked(true);
        }
    });
    connect(ui->coverButton, &QToolButton::clicked,this, &footWidget::switchlyric);
    connect(m_lyric, &Lyric::lyricClicked, this, [this](qint64 position) {

           m_player->setPosition(static_cast<qint64>(position));

           if(m_player->state() != QMediaPlayer::PlayingState) {
               m_player->play();
           }
       });

    connect(ui->addlistbtn,&QPushButton::clicked,this,&footWidget::on_addlistbtn_clicked);

    connect(m_player, &QMediaPlayer::positionChanged,
            this, [this](qint64 pos) {
        updatePosition(pos);
        m_lyric->updateLyricPosition(pos);
                   });

    connect(ui->Volumebtn, &QPushButton::clicked,
            this, &footWidget::toggleMuteState,
            Qt::UniqueConnection);
    connect(m_playlist, &QMediaPlaylist::currentIndexChanged,this, &footWidget::handlePlaylistChanged);

    connect(m_lyric, &Lyric::lyricClicked, this, [this](qint64 pos) {
        QSignalBlocker blocker(ui->positionSlider);
        ui->positionSlider->setValue(pos);
    });

    connect(m_lyric, &Lyric::playPauseRequested,
                this, &footWidget::on_Playbtn_clicked);
    connect(m_lyric, &Lyric::previousRequested,
                this, &footWidget::on_Prebtn_clicked);
    connect(m_lyric, &Lyric::nextRequested,
                this, &footWidget::on_Nextbtn_clicked);
    connect(m_lyric, &Lyric::playModeChanged,
                this, &footWidget::on_Modebtn_clicked);
    connect(m_lyric,&Lyric::curplaylistRequested,this,&footWidget::on_listbtn_clicked);

    connect(m_lyric,&Lyric::addlistRequested,this,&footWidget::on_addlistbtn_clicked);

    connect(m_lyric, &Lyric::lyricSliderMoved,this, &footWidget::setPosition);

    connect(m_playlist, &QMediaPlaylist::currentIndexChanged,
            this, &footWidget::updatePlaylistHighlight);//当前播放列表高亮

    // 连接Lyric的音量信号
    connect(m_lyric, &Lyric::volumeChanged, this, &footWidget::handleVolumeChange);
    connect(m_lyric, &Lyric::muteToggled, this, [this](bool muted) {
        if (muted != m_isMuted) {
            toggleMuteState();
        }
    });

    connect(this, &footWidget::volumeChanged, m_lyric, &Lyric::setVolume);
    connect(this, &footWidget::muteChanged, m_lyric, &Lyric::setMuted);
}

footWidget::~footWidget()
{
    delete ui;
}

void footWidget::initUI()
{
    m_player = new QMediaPlayer(this);
    m_playlist = new QMediaPlaylist(this);
    m_player->setPlaylist(m_playlist);
    m_playlist->setPlaybackMode(QMediaPlaylist::Loop);
    m_player->setVolume(50);

    playlistWidget = new QWidget(this);
    playlistWidget->setWindowFlags(Qt::Popup);
    playlistWidget->setVisible(false);
    playlistWidget->setFixedSize(350, 300);
   QVBoxLayout *playlistLayout = new QVBoxLayout(playlistWidget);
   QLabel *titleLabel = new QLabel(tr("当前播放列表"), playlistWidget);
   playlistView = new QListWidget(playlistWidget);

   playlistLayout->addWidget(titleLabel);
   playlistLayout->addWidget(playlistView);

   popup = new VolumePopup(this);
   connect(popup, &VolumePopup::volumeChanged, this, &footWidget::handleVolumeChange);
   popup->hide();

   ui->Volumebtn->installEventFilter(this);
   popup->installEventFilter(this);

   ui->infoLabel->setStyleSheet("background: transparent;");
   ui->positionlabel->setStyleSheet("background: transparent;");
   ui->Modebtn->setStyleSheet("background: transparent;");
   ui->Prebtn->setStyleSheet("background: transparent;");
   ui->Nextbtn->setStyleSheet("background: transparent;");


}
void footWidget::initSpeedControl()
{
    ui->SpeedButton->setStyleSheet(R"(
        QToolButton {
            background: transparent;
            border: none;
            padding: 4px;
        }
        QToolButton::menu-indicator {
            image: none;
            width: 0px;
            height: 0px;
        }
    )");

    m_speedMenu = new QMenu(this);
    m_speedMenu->setObjectName("speedMenu"); // 为菜单添加标识
    m_speedMenu->setFixedWidth(120);

    QColor bgColor = m_theme.backgroundColor();
    bool isDark = ThemeManager::isDarkTheme(bgColor);

    QColor highcolor;
    if (isDark) {
        highcolor = bgColor.lighter(150);
    } else {
        highcolor = bgColor.darker(110);
    }
    QColor textColor = isDark ? Qt::white : Qt::black;

    // 构建动态样式表
    QString menuStyle = QString(R"(
        QMenu#speedMenu {
            background: %1;
            border: 1px solid %2;
            border-radius: 6px;
            padding: 6px 0;
            margin-top: 2px;
        }
        QMenu#speedMenu::item {
            height: 28px;
            min-width: 100px;
            border:none;
            color: %3;
            font-family: "Microsoft YaHei";
            font-size: 13px;
        }
        QMenu#speedMenu::item:hover {
            background: %4;
        }
        QMenu#speedMenu::item:selected {
            background: %5;
        }
        QMenu#speedMenu::icon {
            left: 12px; /* 调整图标位置 */
        }
    )").arg(
        bgColor.name(),               // 背景色
        bgColor.darker(120).name(),   // 边框色
        textColor.name(),                // 文本颜色
        highcolor.name(),               // 悬停颜色
        highcolor.name()             // 选中颜色
    );

    m_speedMenu->setStyleSheet(menuStyle);

    m_speedGroup = new QActionGroup(this);
    const QList<float> speeds = {0.5f, 1.0f, 1.5f, 2.0f};

    // 根据当前主题设置图标
    QString themeColor = isDark ? "white" : "black";
//    qInfo()<<"footwidget"<<isDark<<bgColor.name();
    for (float speed : speeds) {
        QAction *action = new QAction(QString("%1x").arg(speed), this);
        action->setCheckable(true);
        action->setData(speed);

        QString iconPath = QString(":/images/theme/speed-%1-%2.png")
                         .arg(QString::number(speed, 'f', 1))
                         .arg(themeColor);
        action->setIcon(QIcon(iconPath));

        m_speedMenu->addAction(action);
        m_speedGroup->addAction(action);
        m_speedActions.insert(speed, action);
    }

    ui->SpeedButton->setPopupMode(QToolButton::InstantPopup);
    ui->SpeedButton->setMenu(m_speedMenu);

    connect(m_speedGroup, &QActionGroup::triggered, [=](QAction *action){
        float speed = action->data().toFloat();
        setPlaybackSpeed(speed);
    });
}

bool footWidget::eventFilter(QObject *obj, QEvent *event) {
    if (obj == ui->Volumebtn) {
        switch (event->type()) {
        case QEvent::Enter:
            m_isHovering = true;
            m_hoverTimer->start(60);
            break;
        case QEvent::Leave:
            m_isHovering = false;
            m_hoverTimer->stop();
            if (m_popupVisible) {
                QTimer::singleShot(100, [this](){
                    if (!popup->underMouse()) {
                        popup->hide();
                        m_popupVisible = false;
                    }
                });
            }
            break;
        case QEvent::MouseMove:
            if (m_popupVisible) {
                static QElapsedTimer moveTimer;
                if (moveTimer.hasExpired(50)) {
                    popup->showAboveWidget(ui->Volumebtn);
                    moveTimer.start();
                }
            }
            break;
        default:
            break;
        }
    }
    return DWidget::eventFilter(obj, event);
}

void footWidget::setplaybtn()
{
    ui->Playbtn->setIcon(QIcon(":/images/image-hover/play-hover.png"));
    if (m_lyric && m_lyric->lyricplay()) {
        m_lyric->lyricplay()->setIcon(QIcon(":/images/icons/lyricpause.png"));
    }
}
void footWidget::toggleMuteState() {
    const bool newMuted = !m_isMuted;
    int newVolume = newMuted ? 0 : m_lastVolume;

    QSignalBlocker blocker1(popup->slider);
    QSignalBlocker blocker2(m_lyric->getvolumeSlider());

    m_player->setVolume(newVolume);
    popup->slider->setValue(newVolume);
    m_lyric->getvolumeSlider()->setValue(newVolume);

    m_isMuted = newMuted;
    updateVolumeButtonIcon();
    emit muteChanged(newMuted);
}
void footWidget::updateVolumeButtonIcon()
{
    QString theme = ThemeManager::isDarkTheme(m_theme.backgroundColor()) ? "white" : "black";
    QString iconName;

    if(m_isMuted) {
        iconName = "mute";
    } else {
        int vol = m_player->volume();
        if(vol == 0) {
            iconName = "mute";
        } else if(vol < 33) {
            iconName = "volume-low";
        } else if(vol < 66) {
            iconName = "volume-medium";
        } else {
            iconName = "volume-high";
        }
    }

    ui->Volumebtn->setIcon(QIcon(QString(":/images/theme/%1-%2.png").arg(iconName, theme)));
}

// 时间格式化函数
QString formatTime(qint64 milliseconds) {
    qint64 seconds = milliseconds / 1000;
    qint64 minutes = seconds / 60;
    seconds %= 60;
    return QString("%1:%2")
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0'));
}

void footWidget::togglePlaylistWidget()
{
    if (!playlistWidget) return;

    QPoint btnBottomLeft = ui->listbtn->mapToGlobal(QPoint(0, 0));
    int x = btnBottomLeft.x() - (playlistWidget->width() - ui->listbtn->width()) / 2;
    int y = btnBottomLeft.y() - playlistWidget->height();

    playlistWidget->move(x, y);

    if (playlistWidget->isVisible()) {
        playlistWidget->hide();
    } else {
        updatePlaylistView();
        playlistWidget->show();
        playlistWidget->raise();
    }
}

void footWidget::updatePlaylistView()
{
    if (!playlistView || !m_playlist) return;

    playlistView->clear();

    // 获取或同步歌曲ID列表
    QList<int> songIds = m_playlist->property("songIds").value<QList<int>>();
    if (songIds.size() != m_playlist->mediaCount()) {
        qDebug() << "Syncing songIds with playlist...";
        QSqlQuery query;
        query.prepare("SELECT id FROM songs WHERE file_path IN "
                     "(SELECT file_path FROM playlist_songs WHERE playlist_id = ?)");
        query.addBindValue(m_playlist->property("playlistId").toInt());

        if (!query.exec()) {
            qWarning() << "Failed to sync songIds:" << query.lastError().text();
            return;
        }

        songIds.clear();
        while (query.next()) {
            songIds.append(query.value(0).toInt());
        }
        m_playlist->setProperty("songIds", QVariant::fromValue(songIds));
    }

    if (songIds.isEmpty()) {
        qDebug() << "Playlist is empty";
        return;
    }

    //  批量查询歌曲信息
    QHash<int, QPair<QString, QString>> songInfoHash; // <songId, <title, artist>>
    QHash<int, QString> durationHash;

    {
        QSqlQuery query;
        QString sql = QString("SELECT id, title, artist, duration FROM songs WHERE id IN (%1)")
                         .arg(transformToSafeIdList(songIds));

        if (query.exec(sql)) {
            while (query.next()) {
                int id = query.value("id").toInt();
                songInfoHash[id] = qMakePair(
                    query.value("title").toString(),
                    query.value("artist").toString()
                );

                int seconds = query.value("duration").toInt();
                durationHash[id] = QString("%1:%2")
                    .arg(seconds / 60, 2, 10, QChar('0'))
                    .arg(seconds % 60, 2, 10, QChar('0'));
            }
        } else {
            qWarning() << "Failed to batch load song info:" << query.lastError().text();
        }
    }

    const int currentIndex = m_playlist->currentIndex();

    for (int i = 0; i < m_playlist->mediaCount(); ++i) {
        int songId = songIds.value(i, -1);
        if (songId == -1) continue;

        // 获取缓存数据
        auto info = songInfoHash.value(songId, qMakePair(tr("未知歌曲"), tr("未知歌手")));
        QString duration = durationHash.value(songId, "00:00");

        // 创建列表项
        QListWidgetItem *item = new QListWidgetItem();
        item->setSizeHint(QSize(300, 50));
        item->setData(Qt::UserRole, songId);

        QPixmap defaultCover(":/images/default-cover.png");
        PlaylistItemWidget *widget = new PlaylistItemWidget();
        widget->setData(defaultCover, info.first, info.second, duration);
        widget->updateThemeStyles();

        if (i == currentIndex) {
            widget->setCurrentStyle(true);
        }

        playlistView->addItem(item);
        playlistView->setItemWidget(item, widget);
    }
}


QString footWidget::transformToSafeIdList(const QList<int>& ids)
{
    QStringList safeIds;
    foreach(int id, ids) {
        safeIds << QString::number(id);
    }
    return safeIds.join(",");
}
void footWidget::setPlaybackSpeed(float speed)
{
    bool wasPlaying = (m_player->state() == QMediaPlayer::PlayingState);
    qint64 currentPosition = m_player->position();

    m_player->stop();
    m_player->setPlaybackRate(speed);
    bool isDark = ThemeManager::isDarkTheme(m_theme.backgroundColor());
    updateSpeedButtonIcon(isDark ? "white" : "black");

    if (wasPlaying) {
        m_player->play();
    }

    m_player->setPosition(currentPosition);

    if (m_speedActions.contains(speed)) {
        m_speedActions[speed]->setChecked(true);
        ui->SpeedButton->setText(QString("%1x").arg(speed));
    }
}


void footWidget::updatePlaylistHighlight(int currentIndex)
{
    if (!playlistView) return;

    qInfo() << "updatePlaylistHighlight" << currentIndex;

    // 清除所有高亮
    for (int i = 0; i < playlistView->count(); ++i) {
        QListWidgetItem* item = playlistView->item(i);
        if (PlaylistItemWidget* widget = qobject_cast<PlaylistItemWidget*>(
                playlistView->itemWidget(item))) {
            widget->setCurrentStyle(false);
        }
    }

    // 设置新高亮
    if (currentIndex >= 0 && currentIndex < playlistView->count()) {
        QListWidgetItem* item = playlistView->item(currentIndex);
        if (PlaylistItemWidget* widget = qobject_cast<PlaylistItemWidget*>(
                playlistView->itemWidget(item))) {
            widget->setCurrentStyle(true);
            playlistView->scrollToItem(item, QAbstractItemView::PositionAtCenter);
        }
    }

}

void footWidget::playMusic(int index)
{
    if (index < 0 || index >= m_playlist->mediaCount()) {
        qWarning() << "Invalid play index:" << index;
        return;
    }

    QList<int> songIds = m_playlist->property("songIds").value<QList<int>>();

    if (songIds.size() != m_playlist->mediaCount()) {
        qWarning() << "songIds与播放列表不同步，重新加载...";

        int playlistId = m_playlist->property("playlistId").toInt();
        QString querySql;
        if (playlistId > 0) {
            querySql = "SELECT id FROM songs WHERE file_path IN "
                      "(SELECT file_path FROM playlist_songs WHERE playlist_id = ?)";
        } else {
            querySql = "SELECT id FROM songs"; // 默认列表的查询逻辑
        }

        QSqlQuery query;
        query.prepare(querySql);
        if (playlistId > 0) query.addBindValue(playlistId);

        if (!query.exec()) {
            qCritical() << "重新加载songIds失败:" << query.lastError().text();
            return;
        }

        songIds.clear();
        while (query.next()) {
            songIds.append(query.value(0).toInt());
        }
        m_playlist->setProperty("songIds", QVariant::fromValue(songIds));
    }

    if (index >= songIds.size()) {
        qCritical() << "Song ID list mismatch with playlist";
        return;
    }
    int songId = songIds.at(index);
    m_currentSongId = songId;
    m_playlist->setCurrentIndex(index);

    QSqlQuery query;
    query.prepare("SELECT title, artist FROM songs WHERE id = ?");
    query.addBindValue(songId);

    QString title = tr("未知歌曲");
    QString artist = tr("未知歌手");

    if (query.exec()) {
        if (query.next()) {
            title = query.value("title").toString();
            artist = query.value("artist").toString();
        } else {
            qWarning() << "No song found for ID:" << songId;
        }
    } else {
        qCritical() << "Database error:" << query.lastError().text();
    }

    QString lyricPath = ("http://vip.frpee.com:13678/music/lrc/"  + title + "-" + artist + ".lrc");
    qInfo() << "Lyric path:" << lyricPath;

    if (m_lyric) {
            m_lyric->updatelyric(title, lyricPath);
    }

    QMetaObject::invokeMethod(this, [=](){
        updateSongInfo(title, artist);
//        updatePlaylistHighlight(index);
        updateCurrentSongFavorite();

    }, Qt::QueuedConnection);
}
void footWidget::on_Prebtn_clicked() {
    switch (m_playlist->playbackMode()) {
    case QMediaPlaylist::Random: {
        int randomIndex = QRandomGenerator::global()->bounded(m_playlist->mediaCount());
        playMusic(randomIndex);
        break;
    }
    default:
        playMusic(m_playlist->previousIndex());
    }
}

void footWidget::on_Nextbtn_clicked() {
    switch (m_playlist->playbackMode()) {
    case QMediaPlaylist::Random: {
        int randomIndex = QRandomGenerator::global()->bounded(m_playlist->mediaCount());
        playMusic(randomIndex);
        break;
    }
    default:
        playMusic(m_playlist->nextIndex());
    }
}
void footWidget::updatePosition(qint64 position)
{
    if(!ui->positionSlider->isSliderDown()){
        QSignalBlocker blocker1(ui->positionSlider);
        QSignalBlocker blocker2(m_lyric->lyricSlider());

        ui->positionSlider->setValue(position);
        m_lyric->lyricSlider()->setValue(position);
    }

    ui->positionlabel->setText(
        QString("%1/%2")
            .arg(formatTime(position))
            .arg(formatTime(m_player->duration()))
    );
    m_lyric->lyricduration()->setText( QString("%1/%2")
                                       .arg(formatTime(position))
                                       .arg(formatTime(m_player->duration())));
}

void footWidget::setPosition(qint64 position)
{
    if(position >= 0 && position <= m_player->duration()) {
        QSignalBlocker blocker1(ui->positionSlider);
        QSignalBlocker blocker2(m_lyric->lyricSlider());

        m_player->setPosition(position);
        ui->positionSlider->setValue(position);
        m_lyric->lyricSlider()->setValue(position);
    }
}

void footWidget::on_Playbtn_clicked()

{
    if (m_player->state() == QMediaPlayer::PlayingState) {
        m_player->pause();
        ui->Playbtn->setIcon(QIcon(":/images/image-hover/pase-hover.png"));
        m_lyric->lyricplay()->setIcon(QIcon(":/images/icons/lyricplay.png"));
        ui->Playbtn->setIconSize(QSize(32, 32));
    }
    else /*if(m_player->state()==QMediaPlayer::PausedState)*/{
        m_player->play();
        ui->Playbtn->setIcon(QIcon(":/images/image-hover/play-hover.png"));
        m_lyric->lyricplay()->setIcon(QIcon(":/images/icons/lyricpause.png"));
        ui->Playbtn->setIconSize(QSize(32, 32));
    }
//    else if(!m_playlist->isEmpty() && (m_player->state()==QMediaPlayer::StoppedState))
//    {
//        m_playlist->setCurrentIndex(0);
//        m_player->play();
//    }

}


void footWidget::updateDuration(qint64 duration)
{
    ui->positionSlider->setRange(0, duration);
    m_lyric->lyricSlider()->setRange(0,duration);
    ui->positionlabel->setText(
        "00:00 / " + formatTime(duration)
    );
    m_lyric->lyricduration()->setText(
        "00:00 / " + formatTime(duration)
       );
}
void footWidget::updateSpeedButtonIcon(const QString& theme)
{
    float currentSpeed = m_player->playbackRate();
    QString iconPath;

    if (qFuzzyCompare(currentSpeed, 0.5f)) {
        iconPath = QString(":/images/theme/speed-0.5-%1.png").arg(theme);
    } else if (qFuzzyCompare(currentSpeed, 1.0f)) {
        iconPath = QString(":/images/theme/speed-1.0-%1.png").arg(theme);
    } else if (qFuzzyCompare(currentSpeed, 1.5f)) {
        iconPath = QString(":/images/theme/speed-1.5-%1.png").arg(theme);
    } else if (qFuzzyCompare(currentSpeed, 2.0f)) {
        iconPath = QString(":/images/theme/speed-2.0-%1.png").arg(theme);
    }

    if (!iconPath.isEmpty()) {
        ui->SpeedButton->setIcon(QIcon(iconPath));
    }
}


QMediaPlaylist* footWidget::getPlaylist() const {
    return m_playlist;
}

QSlider *footWidget::slider() const
{
    return ui->positionSlider;
}

void footWidget::updateSongInfo(QString title, QString artist)
{
    QString displayText = QString("%1 - %2").arg(title).arg(artist);
    ui->infoLabel->setText(displayText);
    ui->infoLabel->setStyleSheet("background: transparent;");
    ui->infoLabel->setScrollEnabled(
        ui->infoLabel->fontMetrics().horizontalAdvance(displayText) > ui->infoLabel->width()
    );

    // 使用歌曲标题作为缓存键
    QString cacheKey = title.toLower();
    QPixmap cover = CoverCacheManager::instance()->getCover(cacheKey);

    // 如果封面为空，则尝试加载封面
    if (cover.isNull()) {
        // 构建封面URL
        QString encodedName = QUrl::toPercentEncoding(title);
        QUrl coverUrl(QString("http://vip.frpee.com:13678/music/cover/%1.jpg").arg(encodedName));

        // 加载封面并更新UI
        connect(CoverCacheManager::instance(), &CoverCacheManager::coverLoaded,
                this, [this, cacheKey](const QString& key, const QPixmap& loadedCover) {
            if (key == cacheKey) {
                updateCoverIcon(loadedCover);
            }
        });

        CoverCacheManager::instance()->loadCover(cacheKey, coverUrl);
    } else {
        // 直接使用缓存的封面
        qInfo()<<"更新了";
        updateCoverIcon(cover);
    }
}

void footWidget::updateCoverIcon(const QPixmap& cover)
{
    ui->coverButton->setIcon(QIcon(cover));
    ui->coverButton->setIconSize(QSize(100, 100));
    ui->coverButton->setStyleSheet("border: none; border-radius: 50px;");
    
}

QLabel *footWidget::label() const
{
    ui->positionlabel;
}
void footWidget::updateTheme(const QColor& bgColor)
{
    updateVolumeButtonIcon();

    if(popup) {
        popup->updatepopupTheme(bgColor);  // 注意方法名匹配
        popup->setVolume(m_player->volume());
    }
    bool isDark = ThemeManager::isDarkTheme(bgColor);
    QString themeColor = isDark ? "white" : "black";

    QColor textColor = isDark ? Qt::white : Qt::black;
    QColor borderColor = isDark ? bgColor.darker(150) : bgColor.darker(120);
    QColor titleBorderColor = isDark ? bgColor.lighter(200) : bgColor.darker(130);
    QColor itemHoverColor = isDark ? bgColor.lighter(150) : bgColor.darker(110);
    QColor itemSelectedColor = isDark ? bgColor.lighter(150) : bgColor.darker(110);
    QColor selectedBorderColor = QColor("#1890ff");
    QColor itemBorderColor = isDark ? QColor("#404040") : QColor("#f0f0f0");
    QColor scrollHandleColor = isDark ? QColor("#606060") : QColor("#d9d9d9");

    const QString btnStyle= QString(
    "QPushButton {"
    " background-color: %1;"
     "color: %1;"
     "border:none;"
    "}"
    "QPushButton:hover {"
    " background-color: %2;"
    "}"
    "QPushButton:pressed {"
    " background-color: %3;"
    "}"
    ).arg(
         bgColor.name(),
         isDark? bgColor.lighter(110).name() : bgColor.darker(105).name(), // 悬停颜色
         isDark ? bgColor.lighter(120).name() : bgColor.darker(110).name()

    );
    ui->addmusicbtn->setStyleSheet(btnStyle);
    ui->favorbtn->setStyleSheet(btnStyle);
    ui->listbtn->setStyleSheet(btnStyle);
    ui->Modebtn->setStyleSheet(btnStyle);
    ui->Nextbtn->setStyleSheet(btnStyle);
    ui->Prebtn->setStyleSheet(btnStyle);
    ui->Volumebtn->setStyleSheet(btnStyle);
    QString playlistStyle = QString(R"(
        QWidget {
            background: %1;
            border: 1px solid %2;
            border-radius: 8px;
            padding: 0px;
        }
        QLabel#titleLabel {
            font-family: 'Microsoft YaHei';
            font-size: 14px;
            font-weight: 600;
            color: %3;
            padding: 12px 16px;
            border-bottom: 1px solid %4;
            background: transparent;
        }
        QListWidget {
            outline: 0;
            border: none;
            background: transparent;
            padding: 4px 0;
            color: %3;
        }
        QListWidget::item {
            height: 56px;
            padding: 0 12px;
            border-bottom: 1px solid %5;
            background: transparent;
        }
        QListWidget::item:hover {
            background: %6;
        }
        QListWidget::item:selected {
            background: %7;
            border-left: 3px solid %8;
        }
        QScrollBar:vertical {
            width: 6px;
            background: transparent;
            margin: 0;
        }
        QScrollBar::handle:vertical {
            background: %9;
            border-radius: 3px;
            min-height: 30px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
            background: none;
        }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: none;
        }
    )")
    .arg(isDark ? "#4D4D4D": "#EBEBEB",
         borderColor.name(),
         textColor.name(),
         titleBorderColor.name(),
         itemBorderColor.name(),
         itemHoverColor.name(),
         itemSelectedColor.name(),
         selectedBorderColor.name(),
         scrollHandleColor.name());

    playlistWidget->setStyleSheet(playlistStyle);
    updatePlaylistView();
    updateSpeedButtonIcon(themeColor);
    // 倍速
    QColor itemColor = isDark ? Qt::white : Qt::black;
    QColor highcolor;
    if (isDark) {
        highcolor = bgColor.lighter(150);
    } else {
        highcolor = bgColor.darker(110);
    }
    QString menuStyle = QString(R"(
           QMenu#speedMenu {
               background: %1;
               border: 1px solid %2;
               color: %3;
           }
           QMenu#speedMenu::item:hover {
               background: %4;
           }
           QMenu#speedMenu::item:selected {
               background: %5;
           }
       )").arg(
           bgColor.name(),
           bgColor.darker(120).name(),
           itemColor.name(),
           highcolor.name(),
           highcolor.name()
       );

       m_speedMenu->setStyleSheet(menuStyle);

       QString iconTheme = isDark ? "white" : "black";
       updateSpeedButtonIcon(iconTheme);

       // 更新速度选项图标
       for (auto it = m_speedActions.begin(); it != m_speedActions.end(); ++it) {
           float speed = it.key();
           QString iconPath = QString(":/images/theme/speed-%1-%2.png")
                            .arg(QString::number(speed, 'f', 1))
                            .arg(iconTheme);
           it.value()->setIcon(QIcon(iconPath));
       }



    updateModeButtonIcon(themeColor);


    QString sliderStyle = QString(
            "QSlider {"
            "   background: transparent;"
            "   min-height: 24px;"
            "}"
            "QSlider::groove:horizontal {"
            "   background: %1;"
            "   height: 4px;"
            "   border-radius: 2px;"
            "}"
            "QSlider::handle:horizontal {"
            "   background: %2;"
            "   width: 12px;"
            "   height: 12px;"
            "   margin: -4px 0;"
            "   border-radius: 6px;"
            "}"
            "QSlider::sub-page:horizontal {"
            "   background: %2;"
            "   border-radius: 2px;"
            "}"
        ).arg(
            isDark? "#505050" : "#E0E0E0",  // 轨道颜色
            isDark? "#009688" : "#009688"   // 进度和滑块颜色
        );

    ui->positionSlider->setStyleSheet(sliderStyle);
    ui->positionlabel->setStyleSheet(QString("color: %1;").arg(textColor.name()));
    ui->infoLabel->setStyleSheet(QString("color: %1;").arg(textColor.name()));

    ui->Prebtn->setIcon(QIcon(QString(":/images/theme/pre-%1.png").arg(themeColor)));
    ui->Nextbtn->setIcon(QIcon(QString(":/images/theme/next-%1.png").arg(themeColor)));
    ui->SpeedButton->setIcon(QIcon(QString(":/images/theme/speed-%1.png").arg(themeColor)));
    ui->listbtn->setIcon(QIcon(QString(":/images/theme/list-%1.png").arg(themeColor)));
    ui->addmusicbtn->setIcon(QIcon(QString(":/images/theme/addmusic-%1.png").arg(themeColor)));
    // 更新喜欢按钮图标
    ui->favorbtn->setStyleSheet(
        QString(
            "QPushButton {"
            "   border: none;"
            "   image: url(:/images/theme/non-like-%1.png);"
            "   background: transparent;"
            "}"
            "QPushButton:checked {"
            "   image: url(:/images/theme/like-%1.png);"
            "}"
        ).arg(themeColor)
                );
}

void footWidget::updateModeButtonIcon(const QString& theme)
{
    switch(m_playlist->playbackMode()) {
    case QMediaPlaylist::Random:
        ui->Modebtn->setIcon(QIcon(QString(":/images/theme/random-%1.png").arg(theme)));
        m_lyric->lyricmode()->setIcon(QIcon(QString(":/images/theme/random-black.png")));

        break;
    case QMediaPlaylist::CurrentItemInLoop:
        ui->Modebtn->setIcon(QIcon(QString(":/images/theme/reloop-%1.png").arg(theme)));
        m_lyric->lyricmode()->setIcon(QIcon(QString(":/images/theme/reloop-black.png")));
        break;
    case QMediaPlaylist::Loop:
        ui->Modebtn->setIcon(QIcon(QString(":/images/theme/loop-%1.png").arg(theme)));
        m_lyric->lyricmode()->setIcon(QIcon(QString(":/images/theme/loop-black.png")));
        break;
    }
}

void footWidget::on_Modebtn_clicked()
{
    if(m_playlist->playbackMode() == QMediaPlaylist::Loop){
        m_playlist->setPlaybackMode(QMediaPlaylist::Random);
    }
    else if(m_playlist->playbackMode() == QMediaPlaylist::Random){
        m_playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    }
    else {
        m_playlist->setPlaybackMode(QMediaPlaylist::Loop);
    }

    bool isDark = ThemeManager::isDarkTheme(m_theme.backgroundColor());
    updateModeButtonIcon(isDark ? "white" : "black");

    // 更新工具提示
    QString toolTip;
    switch(m_playlist->playbackMode()) {
    case QMediaPlaylist::Random:
        toolTip = u8"随机播放";
        break;
    case QMediaPlaylist::CurrentItemInLoop:
        toolTip = u8"单曲循环";
        break;
    case QMediaPlaylist::Loop:
        toolTip = u8"列表循环";
        break;
    }
    ui->Modebtn->setToolTip(toolTip);
    m_lyric->lyricmode()->setToolTip(toolTip);
}

void footWidget::handleVolumeChange(int volume) {
    QSignalBlocker blocker1(popup->slider);
    QSignalBlocker blocker2(m_lyric->getvolumeSlider());

    m_player->setVolume(volume);
    popup->slider->setValue(volume);
    m_lyric->getvolumeSlider()->setValue(volume);

    m_isMuted = (volume == 0);
    updateVolumeButtonIcon();
    emit volumeChanged(volume);
}
void footWidget::on_SpeedButton_clicked()
{
    m_speedMenu->adjustSize();

    QPoint buttonPos = ui->SpeedButton->mapToGlobal(QPoint(0, 0));

    QPoint menuPos(
        buttonPos.x(),
        buttonPos.y() - m_speedMenu->sizeHint().height()
    );

    m_speedMenu->exec(menuPos);
}


void footWidget::on_listbtn_clicked()
{
    togglePlaylistWidget();
}

void footWidget::handlePlaylistChanged(int index)
{
    if (index < 0 || index >= m_playlist->mediaCount()) {
        qWarning() << "Invalid play index:" << index;
        return;
    }

    QList<int> songIds = m_playlist->property("songIds").value<QList<int>>();

    if (songIds.size() != m_playlist->mediaCount()) {
        qWarning() << "songIds与播放列表不同步，重新加载...";

        int playlistId = m_playlist->property("playlistId").toInt();
        QString querySql;
        if (playlistId > 0) {
            querySql = "SELECT id FROM songs WHERE file_path IN "
                      "(SELECT file_path FROM playlist_songs WHERE playlist_id = ?)";
        } else {
            querySql = "SELECT id FROM songs"; // 默认列表的查询逻辑
        }

        QSqlQuery query;
        query.prepare(querySql);
        if (playlistId > 0) query.addBindValue(playlistId);

        if (!query.exec()) {
            qCritical() << "重新加载songIds失败:" << query.lastError().text();
            return;
        }

        songIds.clear();
        while (query.next()) {
            songIds.append(query.value(0).toInt());
        }
        m_playlist->setProperty("songIds", QVariant::fromValue(songIds));
    }

    if (index >= songIds.size()) {
        qCritical() << "Song ID list mismatch with playlist";
        return;
    }
    int songId = songIds.at(index);
    m_currentSongId = songId;
    m_playlist->setCurrentIndex(index);

    QSqlQuery query;
    query.prepare("SELECT title, artist FROM songs WHERE id = ?");
    query.addBindValue(songId);

    QString title = tr("未知歌曲");
    QString artist = tr("未知歌手");

    if (query.exec()) {
        if (query.next()) {
            title = query.value("title").toString();
            artist = query.value("artist").toString();
        } else {
            qWarning() << "No song found for ID:" << songId;
        }
    } else {
        qCritical() << "Database error:" << query.lastError().text();
    }

    QString lyricPath = ("http://vip.frpee.com:13678/music/lrc/"  + title + "-" + artist + ".lrc");
    qInfo() << "Lyric path:" << lyricPath;
    
    if (index >= 0 && index < m_playlist->mediaCount()) {
        QList<int> songIds = m_playlist->property("songIds").value<QList<int>>();
        m_currentSongId = songIds.value(index, -1);
        updateCurrentSongFavorite();
        
        if (m_lyric) {
                m_lyric->updatelyric(title, lyricPath);
        }        
        updatePlaylistHighlight(index);  // 确保高亮显示更新
        updateSongInfo(title, artist);

        
    }
}

void footWidget::updateCurrentSongFavorite() {
    if (m_currentSongId != -1) {
        QSqlQuery query;
        query.prepare("SELECT 1 FROM favorites WHERE song_id = ?");
        query.addBindValue(m_currentSongId);
        if (query.exec() && query.next()) {
            m_isFavorite = true;
        } else {
            m_isFavorite = false;
        }

        QSignalBlocker blocker(ui->favorbtn);
        ui->favorbtn->setChecked(m_isFavorite);
    }
}
void footWidget::on_favorbtn_clicked() {
    if (m_currentSongId == -1) return;

    bool newState = !ui->favorbtn->isChecked();
    ui->favorbtn->setChecked(newState);
    emit favorBtnClicked(m_currentSongId, newState);
    qInfo()<<"on_favorbtn_clicked"<<m_currentSongId;
}

void footWidget::onMusicItemLikeChanged(bool liked, int songId) {
    if (songId == m_currentSongId) {
        QSignalBlocker blocker(ui->favorbtn);
        ui->favorbtn->setChecked(liked);
        showFavoriteAnimation(liked);
    }
}

void footWidget::updateCover(const QString& key, const QPixmap& cover)
{
    if (ui->coverButton->property("cacheKey").toString() == key) {
        ui->coverButton->setIcon(QIcon(cover));
        ui->coverButton->setIconSize(QSize(40, 40));
    }
}


void footWidget::showFavoriteAnimation(bool isFavorite)
{
    QPropertyAnimation *anim = new QPropertyAnimation(ui->favorbtn, "iconSize");
    anim->setDuration(200);
    anim->setEasingCurve(QEasingCurve::OutBack);

    if(isFavorite) {
        anim->setStartValue(QSize(24, 24));
        anim->setEndValue(QSize(32, 32));
    } else {
        anim->setStartValue(QSize(32, 32));
        anim->setEndValue(QSize(24, 24));
    }
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void footWidget::onPlaylistCurrentIndexChanged(int index) {
    if (index == -1) {
        // 播放列表为空时的处理
        updateSongInfo("", "");
        m_currentSongId = -1;
        return;
    }

    // 从播放列表属性中获取数据
    QVariant songIdsVar = m_playlist->property("songIds");
    QVariant titlesVar = m_playlist->property("titles");
    QVariant artistsVar = m_playlist->property("artists");

    if (songIdsVar.isValid() && titlesVar.isValid() && artistsVar.isValid()) {
        QList<int> songIds = songIdsVar.value<QList<int>>();
        QList<QString> titles = titlesVar.value<QList<QString>>();
        QList<QString> artists = artistsVar.value<QList<QString>>();

        if (index < songIds.size()) {
            m_currentSongId = songIds[index];
            updateSongInfo(titles[index], artists[index]);
        }
    }
}
void footWidget::applyAnimationToAllButtons() {

    foreach (QPushButton *button, this->findChildren<QPushButton*>()) {
        button->setCursor(Qt::PointingHandCursor);
        connect(button, &QPushButton::clicked, this, [this, button]() {
            // 添加点击动画：按钮缩小
            QPropertyAnimation* shrinkAnimation = new QPropertyAnimation(button, "geometry");
            shrinkAnimation->setDuration(100);
            shrinkAnimation->setEasingCurve(QEasingCurve::OutQuad);
            QRect startRect = button->geometry();
            QRect endRect = startRect.adjusted(-3, -3, 3, 3); // 缩小按钮
            shrinkAnimation->setStartValue(startRect);
            shrinkAnimation->setEndValue(endRect);

            QPropertyAnimation* restoreAnimation = new QPropertyAnimation(button, "geometry");
            restoreAnimation->setDuration(100);
            restoreAnimation->setEasingCurve(QEasingCurve::OutQuad);
            restoreAnimation->setStartValue(endRect);
            restoreAnimation->setEndValue(startRect); // 恢复原尺寸

            QSequentialAnimationGroup* animationGroup = new QSequentialAnimationGroup(this);
            animationGroup->addAnimation(shrinkAnimation);
            animationGroup->addAnimation(restoreAnimation);

            // 启动动画
            animationGroup->start(QAbstractAnimation::DeleteWhenStopped);
        });
    }
}

void footWidget::on_addlistbtn_clicked()
{
    emit fwaddplaylist(m_currentSongId);
}
