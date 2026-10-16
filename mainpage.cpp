#include "mainpage.h"
#include "ui_mainpage.h"
#include <QScrollBar>
#include <QDirIterator>
#include <QPainter>

mainpage::mainpage(DatabaseHandler *db, QWidget *parent, footWidget *footWidget)
    : QWidget(parent)
    , ui(new Ui::mainpage)
    , m_footWidget(footWidget)
    , m_dbHandler(db)
    , m_theme(ThemeManager::instance())
{

    ui->setupUi(this);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setAttribute(Qt::WA_TranslucentBackground,true);
    this->setStyleSheet("background: white;");

    localMusicWidget = ui->localMusicWidget;
    curPlayWidget = ui->curPlayWidget;
    favorWidget = ui->favorMusicList;
    playlistwidget = ui->playlistSongsWidget;
    reconwidget =ui->reconwidget;
    videoWidget = ui->videoWidegt;

    m_defaultCover.load(":/images/image/non-music.png");
    ui->favorcover->setPixmap(m_defaultCover); // 初始显示默认封面

    initUI();
    loadVideoList();

    applyAnimationToAllButtons();
    CoverCacheManager::instance();
    coverNetworkManager = new QNetworkAccessManager(this);
    connect(coverNetworkManager, &QNetworkAccessManager::finished,
                    this, &mainpage::onCoverDownloadFinished);

        // 连接 CoverCacheManager 的信号
    connect(CoverCacheManager::instance(), &CoverCacheManager::coverLoaded,
                this, &mainpage::updateAllCovers);
    qDebug() << "加载数据库0";
    init_musicLists();

    qInfo() << "加载数据库1";
    QElapsedTimer timer;  // 创建时间统计器
    timer.start();
    loadMusicFromDatabase(localMusicWidget);
    qInfo() << "查询数据库耗时：" << timer.elapsed() << "ms"; // 打印查询耗时
    qInfo() << "加载数据库2";
    loadPlayHistory(curPlayWidget);
    qInfo() << "加载数据库3";
    loadFavorites(favorWidget);
    qInfo() << "加载数据库4";
    loadPlaylists();

    qInfo() << "加载数据库wanc";
    refreshAllCovers();

    onThemeChanged();
//    for (int i = 0; i < ui->stackedWidget->count(); ++i) {
//    QWidget *page = ui->stackedWidget->widget(i);
//    qInfo() << "Page at index" << i << "is" << page->objectName();
//    }
    ui->stackedWidget->setCurrentIndex(4);
    ui->countlabel->hide();
    ui->allplaybtn->hide();
    ui->infolabel->hide();

    connect(ui->allplaybtn, &QPushButton::clicked, this, &mainpage::onPlayAllClicked);
    connect(this, &mainpage::playMusicAtIndex,
          m_footWidget, &footWidget::playMusic);

    connect(m_footWidget->getPlaylist(), &QMediaPlaylist::currentIndexChanged,
          this, &mainpage::highlightCurrentSong);

    connect(ui->themebtn, &QToolButton::clicked,
                this, &mainpage::showThemeMenu);

    connect(this, &mainpage::musicItemLiked,
            m_footWidget, &footWidget::onMusicItemLikeChanged);

    connect(m_footWidget, &footWidget::favorBtnClicked, this, [=](int songId, bool isFavorite) {
        onMusicLiked(isFavorite, songId);
    });
    connect(ui->addNewlist, &QPushButton::clicked, this, &mainpage::onAddPlaylistClicked);

    connect(ui->listView->selectionModel(), &QItemSelectionModel::currentChanged,
                 this, &mainpage::onPlaylistSelected);

    connect(&m_theme, &ThemeManager::themeChanged,
            this, &mainpage::onThemeChanged);
    m_theme.loadSettings();
    connect(ui->minbtn,&QPushButton::clicked,this,&mainpage::minwindow);
    connect(ui->closebtn, &QPushButton::clicked, qApp, &QApplication::quit);
    connect(videoWidget,&QListWidget::itemDoubleClicked,this,&mainpage::on_vedio_doubleclicked);
    connect(videoWidget,&QListWidget::itemDoubleClicked,m_videoPlayer,&VideoPlayer::onPlaylistItemDoubleClicked);

}
mainpage::~mainpage()
{
    for (auto playlist : m_playlistCache) {
        delete playlist;
    }
    m_playlistCache.clear();
    delete ui;
}

void mainpage::initUI()
{

    ui->line->setFrameShape(QFrame::HLine);
    ui->line->setFrameShadow(QFrame::Sunken);
    ui->line->setLineWidth(1);
    ui->line->setFixedWidth(150);

    ui->line_2->setFrameShape(QFrame::HLine);
    ui->line_2->setFrameShadow(QFrame::Sunken);
    ui->line_2->setLineWidth(1);
    ui->line_2->setFixedWidth(150);

    ui->serachedit->setPlaceholderText("搜索歌曲、歌手、专辑...");

    videoWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_videoPlayer = new VideoPlayer(this);
    updateSearchEdit();
    ui->serachedit->setClearButtonEnabled(true);

    connect(ui->serachedit, &QLineEdit::returnPressed, this, &mainpage::performSearch);

    connect(ui->serachedit, &QLineEdit::textChanged, [this](const QString& text) {
           static QTimer searchTimer;
           searchTimer.stop();
           if(text.length() >= 2) {
               searchTimer.singleShot(500, this, [=]{
                   performSearch();
               });
           }
       });

    connect(ui->serachedit, &QLineEdit::textEdited, [this](const QString& text){
           if(!text.isEmpty()) showSearchSuggestions();
       });

    QSettings settings;
    searchHistory = settings.value("search/history").toStringList();

    searchResultsWidget = new QListWidget(this);
    setupListWidget(searchResultsWidget);
    ui->stackedWidget->addWidget(searchResultsWidget);

    ui->serachedit->setPlaceholderText("搜索歌曲、歌手、专辑...");
    connect(ui->serachedit, &QLineEdit::returnPressed, this, &mainpage::performSearch);

    connect(ui->serachedit, &QLineEdit::textChanged, [this](const QString& text) {
        if(text.length() >= 3) {
            QTimer::singleShot(300, this, &mainpage::performSearch); // 防抖
        }
    });
    connect(m_footWidget,&footWidget::fwaddplaylist,this,&mainpage::handleAddToPlaylistRequest
            );
}
void mainpage::loadVideoList() {
    videoWidget->clear();

    m_videoPlayer->loadFromDatabase(videoWidget);

    qInfo()<<"count"<< videoWidget->count();
    update();
}
void mainpage::performSearch()
{
    QString keyword = ui->serachedit->text().trimmed();
    if(keyword.isEmpty()){
        restoreOriginalList();
        return;
    }

    saveSearchHistory(keyword);
    previousViewIndex = ui->stackedWidget->currentIndex();

    QSqlQuery query;
    query.prepare("SELECT id, file_path, title, artist, album, duration, is_cloud "
                  "FROM songs WHERE title LIKE ? OR artist LIKE ? OR album LIKE ?");
    QString likePattern = "%" + keyword + "%";
    query.addBindValue(likePattern);
    query.addBindValue(likePattern);
    query.addBindValue(likePattern);

    if(!query.exec()){
        qWarning() << "搜索失败:" << query.lastError();
        return;
    }

    if (m_playlistCache.contains(searchResultsWidget)) {
        delete m_playlistCache[searchResultsWidget];
        m_playlistCache.remove(searchResultsWidget);
    }

    QColor bgColor = m_theme.backgroundColor();
    bool isDarkTheme = ThemeManager::isDarkTheme(bgColor);

    while(query.next()){
        QListWidgetItem* item = new QListWidgetItem();
        MusicItemWidget* widget = new MusicItemWidget();

        int songId = query.value(0).toInt();
        QString filePath = query.value(1).toString();
        QString title = query.value(2).toString();
        QString artist = query.value(3).toString();
        QString album = query.value(4).toString();
        QString duration = query.value(5).toString();
        bool isCloud = query.value(6).toBool();
        QPixmap cover;
        if(QFile::exists(filePath)) {
            cover = extractCover(title);
        } else {
            cover.load(":/images/image/non-music.png");
        }

        QMap<int, bool> favoritesCache;
        QSqlQuery favQuery("SELECT song_id FROM favorites");
        if (favQuery.exec()) {
            while (favQuery.next()) {
                int songId = favQuery.value(0).toInt();
                favoritesCache[songId] = true;
            }
        }

        // 更新数据前设置主题
        widget->setThemeColor(bgColor);
        widget->updateData(
            searchResultsWidget->count(),
            cover,
            title,
            artist,
            album,
            duration,
            favoritesCache.contains(songId),
            songId,
            filePath,
            isCloud
        );

        widget->highlightText(keyword);

        connect(widget, &MusicItemWidget::liked, this, &mainpage::onMusicLiked);
        connect(widget, &MusicItemWidget::onAddToPlaylistRequested,
               this, &mainpage::handleAddToPlaylistRequest);

        item->setSizeHint(widget->sizeHint());
        searchResultsWidget->addItem(item);
        searchResultsWidget->setItemWidget(item, widget);
    }
    ui->infolabel->setText("搜索结果如下所示：");
    ui->infolabel->show();
//    ui->countlabel->hide();
    ui->stackedWidget->setCurrentWidget(searchResultsWidget);
    switchToList(searchResultsWidget);
}
void mainpage::saveSearchHistory(const QString& keyword)
{
    if(keyword.isEmpty()) return;

    // 去重并保留最新10条
    searchHistory.removeAll(keyword);
    searchHistory.prepend(keyword);
    if(searchHistory.size() > 5){
        searchHistory.removeLast();
    }

    // 保存到设置
    QSettings settings;
    settings.setValue("search/history", searchHistory);
}
void mainpage::showSearchSuggestions()
{
    if (searchHistory.isEmpty()) return;

    if (!suggestionsPopup) {
        suggestionsPopup = new QListWidget(nullptr);
        suggestionsPopup->setWindowFlags(
            Qt::ToolTip|
            Qt::FramelessWindowHint |
            Qt::NoDropShadowWindowHint
        );
        suggestionsPopup->setFocusPolicy(Qt::NoFocus);
        suggestionsPopup->setAttribute(Qt::WA_ShowWithoutActivating);
        suggestionsPopup->setEditTriggers(QAbstractItemView::NoEditTriggers);
        suggestionsPopup->installEventFilter(this);


        connect(suggestionsPopup, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
            ui->serachedit->setText(item->text());
            performSearch();
            suggestionsPopup->hide();
        });
    }
    updateSuggestionsPopupTheme();

    suggestionsPopup->clear();
    for (const QString &term : searchHistory) {
        suggestionsPopup->addItem(term);
    }

    // 设置位置
    QPoint pos = ui->serachedit->mapToGlobal(QPoint(0, ui->serachedit->height() + 2));


    if (const QScreen* screen = QGuiApplication::screenAt(pos)) {
        const QRect screenRect = screen->availableGeometry();
        const int menuHeight = suggestionsPopup->sizeHintForRow(0) * qMin(6, searchHistory.size());

        if (pos.y() + menuHeight > screenRect.bottom()) {
            pos = ui->serachedit->mapToGlobal(QPoint(0, -menuHeight - 2)); // 显示在上方
        }
    }

    suggestionsPopup->move(pos);
    suggestionsPopup->resize(ui->serachedit->width(),
                           suggestionsPopup->sizeHintForRow(0) * qMin(6, searchHistory.size()));

    suggestionsPopup->show();
    ui->serachedit->setFocus();
}
void mainpage::updateSuggestionsPopupTheme()
{
    if (!suggestionsPopup) return;

    QColor bgColor = m_theme.backgroundColor();
    QColor textColor = ThemeManager::isDarkTheme(bgColor) ? Qt::white : Qt::black;
    bool isDarkTheme = ThemeManager::isDarkTheme(bgColor);

    // 计算派生颜色
    QColor itemBgColor = isDarkTheme ? bgColor.lighter(110) : bgColor.darker(105);
    QColor hoverColor = isDarkTheme ? bgColor.lighter(120) : bgColor.darker(110);
    QColor borderColor = isDarkTheme ? bgColor.lighter(130) : bgColor.darker(115);
    QColor scrollHandleColor = m_theme.highlightColor().lighter(120);

    QString styleSheet = QString(
        "QListWidget {"
        "  background: %1;"
        "  border: 1px solid %2;"
        "  border-radius: 4px;"
        "  outline: none;"
        "  color: %3;"
        "}"
        "QListWidget::item {"
        "  padding: 6px 12px;"
        "  border-bottom: 1px solid %4;"
        "  background: transparent;"
        "}"
        "QListWidget::item:hover {"
        "  background: %5;"
        "}"
        "QListWidget::item:selected {"
        "  background: %6;"
        "  color: %3;"
        "  font-weight: bold;"
        "}"
        "QScrollBar:vertical {"
        "  width: 8px;"
        "  background: transparent;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background: %7;"
        "  min-height: 20px;"
        "  border-radius: 4px;"
        "}"
    ).arg(
        bgColor.name(),
        borderColor.name(),
        textColor.name(),
        itemBgColor.name(),    // 分隔线
        hoverColor.name(),
        hoverColor.name(),
        scrollHandleColor.name() // 滚动条
    );
    QString iconPath = QString(":/images/theme/search-%1.png").arg(isDarkTheme  ? "white" : "black");
    QPixmap iconPixmap(iconPath);
    QIcon itemIcon(iconPixmap.scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    suggestionsPopup->setStyleSheet(styleSheet);
}
void mainpage::applyAnimationToAllButtons() {

    foreach (QPushButton *button, this->findChildren<QPushButton*>()) {
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
void mainpage::keyPressEvent(QKeyEvent* event)
{
    if (suggestionsPopup && suggestionsPopup->isVisible()) {
        if (event->key() == Qt::Key_Down) {
            suggestionsPopup->setCurrentRow(0);
            suggestionsPopup->setFocus();
        }
        else if (event->key() == Qt::Key_Return) {
            if (auto item = suggestionsPopup->currentItem()) {
                ui->serachedit->setText(item->text());
                performSearch();
            }
            suggestionsPopup->hide();
        }
    }
    QWidget::keyPressEvent(event);
}
bool mainpage::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == suggestionsPopup && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (!suggestionsPopup->geometry().contains(mouseEvent->globalPos())) {
            suggestionsPopup->hide();
            return true; // 阻止事件继续传递
        }
    }
    return QWidget::eventFilter(obj, event);
}
void mainpage::restoreOriginalList()
{
    ui->stackedWidget->setCurrentIndex(previousViewIndex);
    ui->serachedit->clear();
}
void mainpage::switchToList(QListWidget *newList)
{
    if (!newList || !m_footWidget || !m_footWidget->getPlayer()) {
        qWarning()<<"切换列表参数无效";
        return;
    }

    if (currentPlayList != newList) {
        lastHighlightedItem = nullptr;
    }

    if (lastHighlightedItem && currentPlayList) {
        if (auto* oldWidget = qobject_cast<MusicItemWidget*>(currentPlayList->itemWidget(lastHighlightedItem))) {
            oldWidget->setHighlighted(false, false);
        }
        lastHighlightedItem = nullptr;
    }

    currentPlayList = newList;
    int songCount = newList->count();


    QString countText;
       if(newList == playlistwidget) {
           countText = QString("当前歌单 · %1 首").arg(songCount);
       } else if(newList == favorWidget) {
           countText = QString("收藏歌曲 · %1 首").arg(songCount);
       } else if (newList == searchResultsWidget) { // 新增条件
           countText = QString("搜索结果 · %1 首").arg(songCount);
       } else {
           countText = QString("共 %1 首").arg(songCount);
       }

    ui->countlabel->setText(countText);

    ui->countlabel->show();
    ui->allplaybtn->show();
    onThemeChanged();

    QMediaPlayer *player = m_footWidget->getPlayer();
    if (player->state() == QMediaPlayer::StoppedState || player->currentMedia().isNull()) {
        return;
    }

    const QMediaContent currentMedia = player->currentMedia();
    const QString currentIdentifier = currentMedia.canonicalUrl().isLocalFile()
        ? currentMedia.canonicalUrl().toLocalFile()
        : currentMedia.canonicalUrl().toString();

    bool found = false;
    QList<int> songIds;
    const int itemCount = newList->count();

    for (int i = 0; i < itemCount; ++i) {
        if (QListWidgetItem* item = newList->item(i)) {
            if (MusicItemWidget* widget = qobject_cast<MusicItemWidget*>(newList->itemWidget(item))) {
                songIds.append(widget->getSongId());


                if (!found && widget->getFileUrl() == currentIdentifier) {
                    highlightCurrentSong(i);
                    found = true;
                    // 不break，继续收集所有songIds
                }
            }
        }
    }

    if (QMediaPlaylist* playlist = m_footWidget->getPlaylist()) {
        playlist->setProperty("songIds", QVariant::fromValue(songIds));
    }

    if (!found) {
        qDebug() << "当前播放歌曲不在当前显示列表中";
    } else if (lastHighlightedItem) {
        newList->scrollToItem(lastHighlightedItem, QAbstractItemView::PositionAtCenter);
    }

    if (m_footWidget && m_footWidget->getPlaylist() && m_playlistCache.contains(newList)) {
        QMediaPlaylist* cachedPlaylist = m_playlistCache[newList];
        if (cachedPlaylist) {
            QMediaPlaylist::PlaybackMode currentMode = m_footWidget->getPlaylist()->playbackMode();
            cachedPlaylist->setPlaybackMode(currentMode);
        }
    }
}
void mainpage::init_musicLists()
{
    // 初始化三个列表的样式和行为
    setupListWidget(localMusicWidget);
    setupListWidget(curPlayWidget);
    setupListWidget(favorWidget);
    setupListWidget(playlistwidget);



}

void mainpage::onPlayAllClicked()
{
    QMediaPlaylist* footPlaylist = m_footWidget->getPlaylist();
    QMediaPlaylist::PlaybackMode currentMode = footPlaylist->playbackMode();

    int startIndex = 0;


    if (currentMode == QMediaPlaylist::Random) {

        int totalItems = footPlaylist->mediaCount();

        if (totalItems > 0) {

            startIndex = qrand() % totalItems; // 生成一个随机索引

        }

    }


    switch(ui->stackedWidget->currentIndex()){

    case 0: // 当前播放
        onPlayAllFromList(curPlayWidget, startIndex);
        break;

    case 1: //bendi
        onPlayAllFromList(localMusicWidget, startIndex);
        break;

    case 2: // 收藏
        onPlayAllFromList(favorWidget, startIndex);
        break;

    case 3: // 歌单
        onPlayAllFromList(playlistwidget, startIndex);
        break;

    case 5:
        onPlayAllFromList(searchResultsWidget,startIndex);
        break;

    }
}

void mainpage::onPlayAllFromList(QListWidget* targetList, int startIndex)
{
    if (!targetList || targetList->count() == 0) return;


    QMediaPlaylist* playlist = getCachedPlaylist(targetList);
    if (!playlist) {
        playlist = createPlaylistFromListWidget(targetList);
        cachePlaylist(targetList, playlist);
    }

    QMediaPlaylist* footPlaylist = m_footWidget->getPlaylist();
    QMediaPlaylist::PlaybackMode oldMode = footPlaylist->playbackMode();

    footPlaylist->clear();
    for (int i = 0; i < playlist->mediaCount(); ++i) {
        footPlaylist->addMedia(playlist->media(i));
    }

    if (QMediaPlaylist* cachedPlaylist = getCachedPlaylist(currentPlayList)) {
        footPlaylist->setProperty("songIds", cachedPlaylist->property("songIds"));
    }
    footPlaylist->setPlaybackMode(oldMode);


    switchToList(targetList);
    m_footWidget->getPlayer()->play();
    m_footWidget->setplaybtn();
    highlightCurrentSong(startIndex);
    emit playMusicAtIndex(startIndex);

}

QMediaPlaylist* mainpage::getCachedPlaylist(QListWidget* listWidget)
{
    return m_playlistCache.value(listWidget, nullptr);
}

void mainpage::cachePlaylist(QListWidget* listWidget, QMediaPlaylist* playlist)
{
    // 清理旧的播放列表
    if (m_playlistCache.contains(listWidget)) {
        delete m_playlistCache[listWidget];
    }
    m_playlistCache[listWidget] = playlist;
}

QMediaPlaylist* mainpage::createPlaylistFromListWidget(QListWidget* listWidget)
{
    QMediaPlaylist* playlist = new QMediaPlaylist(this);
    QList<int> songIds;

    for (int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem* listItem = listWidget->item(i);
        if (!listItem) continue;

        MusicItemWidget* widget = qobject_cast<MusicItemWidget*>(listWidget->itemWidget(listItem));
        if (!widget) continue;

        songIds.append(widget->getSongId());
        qInfo()<<"添加songid"<<widget->getSongId()<<widget->isCloudSong();

        playlist->addMedia(QUrl(widget->getFileUrl()));
    }

    playlist->setProperty("songIds", QVariant::fromValue(songIds));
    return playlist;
}

void mainpage::onMusicItemDoubleClicked(QListWidgetItem* item)
{
    QElapsedTimer totalTimer;
    totalTimer.start();
    if (!item || !m_footWidget || !m_footWidget->getPlaylist()) {
        qWarning() << "双击事件参数检查失败";
        return;
    }

    currentPlayList = qobject_cast<QListWidget*>(item->listWidget());
    int index = currentPlayList->row(item);
    if (!currentPlayList || index < 0 || index >= currentPlayList->count()) {
        qWarning() << "无效的列表或索引";
        return;
    }

    static QListWidget* lastList = nullptr;
    static int lastIndex = -1;
    static qint64 lastClickTime = 0;

    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    bool isSameItem = (lastList == currentPlayList &&
                      lastIndex == index &&
                      (currentTime - lastClickTime) < 1000); // 1秒内重复点击

    lastList = currentPlayList;
    lastIndex = index;
    lastClickTime = currentTime;

    if (isSameItem) {
        qDebug() << "相同项双击，跳过重复处理";
        return;
    }

    // 显示加载指示器
    QApplication::setOverrideCursor(Qt::WaitCursor);

    // 获取播放器组件
    QMediaPlayer* player = m_footWidget->getPlayer();
    QMediaPlaylist* footPlaylist = m_footWidget->getPlaylist();

    // 保存当前播放状态
    bool wasPlaying = (player->state() == QMediaPlayer::PlayingState);
    QMediaPlaylist::PlaybackMode oldMode = footPlaylist->playbackMode();

    // 获取或创建播放列表
    QMediaPlaylist* playlist = getCachedPlaylist(currentPlayList);
    if (!playlist) {
        playlist = createPlaylistFromListWidget(currentPlayList);
        cachePlaylist(currentPlayList, playlist);
    }

    // 检查是否需要重新加载列表
    bool needReload = true;
    if (footPlaylist->property("currentList").value<QListWidget*>() == currentPlayList) {
        int currentIdx = footPlaylist->currentIndex();
        if (currentIdx >= 0 && currentIdx < footPlaylist->mediaCount()) {
            QUrl currentUrl = footPlaylist->media(currentIdx).canonicalUrl();
            QUrl newUrl = playlist->media(index).canonicalUrl();
            needReload = (currentUrl != newUrl);
        }
    }

    if (needReload) {
        footPlaylist->clear();
        footPlaylist->setProperty("currentList", QVariant::fromValue(currentPlayList));
        footPlaylist->setProperty("songIds", playlist->property("songIds"));

        for (int i = 0; i < playlist->mediaCount(); ++i) {
            footPlaylist->addMedia(playlist->media(i));
        }
    }

    // 设置播放状态
    footPlaylist->setPlaybackMode(oldMode);
    footPlaylist->setCurrentIndex(index);

    // 恢复播放状态
    if (wasPlaying || !player->currentMedia().isNull()) {
        player->play();
        highlightCurrentSong(index);
        emit playMusicAtIndex(index);
        m_footWidget->setplaybtn();
    }

    // 更新界面
    m_footWidget->updatePlaylistHighlight(index);

    if (MusicItemWidget* widget = qobject_cast<MusicItemWidget*>(currentPlayList->itemWidget(item))) {
        int songId = widget->getSongId();
        QMetaObject::invokeMethod(this, [this, songId]() {
            static int lastRecordedSongId = -1;
            static QDateTime lastRecordTime;

            QDateTime now = QDateTime::currentDateTime();
            if (songId != lastRecordedSongId || lastRecordTime.secsTo(now) > 10) {
                if (m_dbHandler) {
                    m_dbHandler->addPlayRecord(songId);
                }
                lastRecordedSongId = songId;
                lastRecordTime = now;
            }
        }, Qt::QueuedConnection);
    }

    // 恢复光标
    QApplication::restoreOverrideCursor();
    qInfo() << "=== 双击事件处理完成, 总耗时:" << totalTimer.elapsed() << "ms ===";
}


bool mainpage::isSystemInDarkMode() {//判断系统亮度
    QProcess process;
    process.start("gsettings", QStringList() << "get" << "com.deepin.dde.appearance" << "gtk-theme");
    process.waitForFinished(300);
    QString output = process.readAllStandardOutput().trimmed();
    return output.contains("dark", Qt::CaseInsensitive);
}

void mainpage::showThemeMenu()
{
    QMenu menu;
    menu.setWindowFlags(menu.windowFlags() | Qt::NoDropShadowWindowHint);
    menu.setAttribute(Qt::WA_TranslucentBackground);

    QString menuStyle = R"(
        QMenu {
            font-size: 11px;
            background-color: %1;
            border: 1px solid %2;
            border-radius: 6px;
            padding: 2px;
            margin: 0;
        }
        QMenu::item {
            height: 22px;
            padding: 0 12px;
            border-radius: 3px;
            color: %3;
            min-width: 80px;
        }
        QMenu::item:selected {
            background-color: %4;
        }
        QMenu::separator {
            height: 1px;
            margin: 2px 6px;
            background: %5;
        }
    )";

    bool isDark = m_theme.isDarkTheme(m_theme.backgroundColor());
    menu.setStyleSheet(menuStyle
        .arg(m_theme.backgroundColor().name())
        .arg(isDark ? "#404040" : "#D0D0D0")
        .arg(isDark ? "#E0E0E0" : "#333333")
        .arg(m_theme.highlightColor().name())
        .arg(isDark ? "#505050" : "#E0E0E0"));

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(&menu);
    shadow->setBlurRadius(8);
    shadow->setOffset(0, 2);
    shadow->setColor(QColor(0, 0, 0, 40));
    menu.setGraphicsEffect(shadow);

    auto addMenuAction = [&](const QString& text) {
        QAction* action = new QAction(text, &menu);
        menu.addAction(action);
        return action;
    };

    QAction* lightAction = addMenuAction("亮色");
    QAction* darkAction = addMenuAction("暗色");
    QAction* blueAction = addMenuAction("默认");
    QAction* customAction = addMenuAction("自定义颜色");
    QAction* systemAction = addMenuAction("跟随系统");

    menu.insertSeparator(customAction);


    auto showColorDialog = [this]() {
        QColorDialog dialog(this);
        dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowTitleHint);
        dialog.setStyleSheet(R"(
           QColorDialog {
           background-color: white;
           border-radius: 10px;
           border: 1px solid #ccc;
           }
           QPushButton {
           min-width: 64px;
           min-height: 28px;
           border-radius: 6px;
           background-color: #e0e0e0;
           color: #333;
           padding: 4px 10px;
           }
           QPushButton:hover {
           background-color: #d0d0d0;
           }
           QPushButton:pressed {
           background-color: #c0c0c0;
           }
           QLabel {
           color: #333;
           }
           )");
        dialog.setWindowTitle("");

        if (dialog.exec() == QDialog::Accepted) {
            m_theme.applyTheme(ThemeManager::Custom, dialog.selectedColor());
            QSettings().setValue("followSystem", false);
        }
    };

    // 信号连接
    connect(lightAction, &QAction::triggered, [=](){
        m_theme.applyTheme(ThemeManager::Light);
        QSettings().setValue("followSystem", false);
    });
    connect(darkAction, &QAction::triggered, [=](){
        m_theme.applyTheme(ThemeManager::Dark);
        QSettings().setValue("followSystem", false);
    });
    connect(blueAction, &QAction::triggered, [=](){
        m_theme.applyTheme(ThemeManager::green);
        QSettings().setValue("followSystem", false);
    });
    connect(customAction, &QAction::triggered, showColorDialog);
    connect(systemAction, &QAction::triggered, [=](){
        QSettings().setValue("followSystem", true);
        bool dark = isSystemInDarkMode();
        m_theme.applyTheme(dark ? ThemeManager::Dark : ThemeManager::Light);
    });

    QPropertyAnimation* anim = new QPropertyAnimation(&menu, "windowOpacity");
    anim->setDuration(100);
    anim->setStartValue(0);
    anim->setEndValue(1);
    anim->start();

    QPoint pos = QCursor::pos() + QPoint(6, 6);
    QRect screen = QApplication::screenAt(pos)->availableGeometry();
    if (pos.x() + menu.sizeHint().width() > screen.right()) {
        pos.setX(screen.right() - menu.sizeHint().width());
    }
    if (pos.y() + menu.sizeHint().height() > screen.bottom()) {
        pos.setY(screen.bottom() - menu.sizeHint().height());
    }

    menu.exec(pos);
}
void mainpage::updateSearchResultsTheme()
{
    if (!searchResultsWidget) return;

    QColor bgColor = m_theme.backgroundColor();

    // 更新列表背景
    searchResultsWidget->setStyleSheet(QString(
        "QListWidget {"
        "  background: %1;"
        "  border: none;"
        "  outline: none;"
        "}"
        "QListWidget::item {"
        "  background: transparent;"
        "  border: none;"
        "}"
        "QScrollBar:vertical {"
        "  width: 8px;"
        "  background: transparent;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background: %2;"
        "  min-height: 20px;"
        "  border-radius: 4px;"
        "}"
    ).arg(
        bgColor.name(),
        m_theme.highlightColor().lighter(120).name()
    ));
    for (int i = 0; i < searchResultsWidget->count(); ++i) {
        QListWidgetItem* item = searchResultsWidget->item(i);
        if (auto widget = qobject_cast<MusicItemWidget*>(searchResultsWidget->itemWidget(item))) {
            widget->setThemeColor(bgColor);
        }
    }
}

void mainpage::updateSearchEdit()
{
    QColor bgColor = m_theme.backgroundColor();
    bool isdark = m_theme.isDarkTheme(bgColor);
    ui->serachedit->setStyleSheet(QString(
    "QLineEdit {"
    "border: 2px solid %1;"
    "border-radius: 15px;"
    "padding: 6px 12px;"
    "background-color: %2;"
    "font-size: 14px;"
    "}"
    "QLineEdit:focus {"
    "border: 2px solid %3;"
    "background-color: %2;"
    "}"
    "QLineEdit::clear-button {"
    "image: url(:/images/icons/cha.png);"
    "subcontrol-origin: content;"
    "subcontrol-position: right;"
    "margin-right: 8px;"
    "}"
    ).arg(
        isdark ? bgColor.lighter(110).name():bgColor.darker(110).name(),
        bgColor.name(),
        m_theme.highlightColor().name()

    ));
}
void mainpage::onThemeChanged() {

    QColor bgColor = m_theme.backgroundColor();
    QColor textColor = ThemeManager::isDarkTheme(bgColor) ? Qt::white : Qt::black;

    bool isDarkTheme = ThemeManager::isDarkTheme(bgColor);

    QColor leftWidgetColor;
    QColor lefthovercolor;
    QColor linecolor;
    if (isDarkTheme) {
        leftWidgetColor = bgColor.lighter(130); // 110% 亮度
        lefthovercolor = leftWidgetColor.darker(105);
        linecolor = leftWidgetColor.lighter(105);
    } else {
        leftWidgetColor = bgColor.darker(105); // 90% 亮度 (darker(110) 表示原色的90%)
        linecolor = leftWidgetColor.darker(105);
        lefthovercolor = leftWidgetColor.darker(105);
    }
     updateSearchEdit();
     updateSearchResultsTheme();
     updateSuggestionsPopupTheme();
     ui->reconwidget->updateTheme();

    // 应用样式
    ui->leftwidget->setStyleSheet(
        QString("background-color: %1; border: none;").arg(leftWidgetColor.name()));

    ui->countlabel->setStyleSheet(QString("color: %1; font-size: 12px;").arg(textColor.name()));

    // 生成样式表字符串
    QString style = QString(
        "QListView {"
        " background: %1;"
        " border: none;"
        " outline: 0;"
        " padding: 5px;"
        " show-decoration-selected: 1;"
        "}"
        "QListView::item {"
        " font-size: 14px;"
        " color: %2;"
        " text-align: center;"
        " padding: 8px;"
        " border: none;"
        " background: transparent;"
        "}"
        "QListView::item:hover {"
        " background: %3;"          // 悬停背景色
        "}"
        "QListView::item:selected {"
        " background: %4;"          // 选中背景色
        " color: %2;"               // 选中文字颜色
        " font-weight: bold !important;"  // 强制加粗
        "}"
        "QScrollBar:vertical {"
        " width: 8px;"
        " background: transparent;"
        "}"
        "QScrollBar::handle:vertical {"
        " background: %5;"
        " min-height: 20px;"
        " border-radius: 4px;"
        "}"
    ).arg(
        leftWidgetColor.name(),
        textColor.name(),
        lefthovercolor.name(),
        lefthovercolor.name(),
        m_theme.highlightColor().lighter(120).name()
    );

    // 应用样式前清除旧设置
    ui->listView->setStyleSheet("");
    ui->listView->setStyleSheet(style);

    // 强制刷新样式
    ui->listView->style()->polish(ui->listView);

    const QString btnStyle_1 = QString(
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
         isDarkTheme ? bgColor.lighter(110).name() : bgColor.darker(105).name(), // 悬停颜色
         isDarkTheme ? bgColor.lighter(120).name() : bgColor.darker(110).name()

    );

    const QString toolbtnStyle = QString(
    "QToolButton {"
    " background-color: %1;"
    "color: %1;"
     "border:none;"
    "}"
    "QToolButton:hover {"
    " background-color: %2;"
    "}"
    "QToolButton:pressed {"
    " background-color: %3;"
    "}"
    ).arg(
         bgColor.name(),
         isDarkTheme ? bgColor.lighter(110).name() : bgColor.darker(105).name(), // 悬停颜色
         isDarkTheme ? bgColor.lighter(120).name() : bgColor.darker(110).name()

    );

    ui->themebtn->setStyleSheet(toolbtnStyle);
    ui->minbtn->setStyleSheet(btnStyle_1);
    ui->maxbtn->setStyleSheet(btnStyle_1);
    ui->closebtn->setStyleSheet(btnStyle_1);
    const QString btnStyle_2 = QString(
    "QPushButton:hover {"
    " background-color: %4;"
    "}"
    "QPushButton:pressed {"
    " background-color: %5;"
    "}"
    ).arg(isDarkTheme ? leftWidgetColor.lighter(110).name() : leftWidgetColor.darker(105).name(), // 悬停颜色
          isDarkTheme ? leftWidgetColor.lighter(120).name() : leftWidgetColor.darker(110).name()
    );

    ui->reconbtn->setStyleSheet(btnStyle_2);
    ui->btnCurPlay->setStyleSheet(btnStyle_2);
    ui->btnvideo->setStyleSheet(btnStyle_2);
    ui->btnFlavor->setStyleSheet(btnStyle_2);
    ui->btnLocal->setStyleSheet(btnStyle_2);
    ui->addNewlist->setStyleSheet(btnStyle_2);
    ui->countlabel->setStyleSheet(QString("color: %1; font-size: 12px;").arg(textColor.name()));

    ui->line->setStyleSheet( QString("background-color: %1;").arg(linecolor.name()));
    ui->line_2->setStyleSheet( QString("background-color: %1;").arg(linecolor.name()));
    ui->listwidget->setStyleSheet(
                QString("background-color: %1;").arg(m_theme.backgroundColor().name()));
    ui->topwidget->setStyleSheet(
        QString("background-color: %1;").arg(m_theme.backgroundColor().name()));
    ui->stackedWidget->setStyleSheet( QString("background-color: %1;").arg(m_theme.backgroundColor().name()));
    QColor handleColor = m_theme.highlightColor();


     if (m_footWidget) {
           m_footWidget->updateTheme(m_theme.backgroundColor());
       }

     const QList<QListWidget*> lists = { localMusicWidget, curPlayWidget, favorWidget , playlistwidget};
     for(auto* list : lists) {
         for (int i = 0; i < list->count(); ++i) {
             QListWidgetItem* listItem = list->item(i);
             if (MusicItemWidget* widget = qobject_cast<MusicItemWidget*>(list->itemWidget(listItem))) {
                 widget->setThemeColor(m_theme.backgroundColor());
             }
         }
         // 设置更完整的样式表
         list->setStyleSheet(QString(
             "QListWidget {"
             "   background: %1;"  // 主背景色
             "   border: none;"
             "   outline: 0;"      // 移除焦点边框
             "}"
             "QListWidget::item {"
             "   background: transparent;"  // 项背景透明
             "   border: none;"
             "   padding: 2px;"
             "}"
             "QListWidget::item:hover {"
             "   background: %2;"  // 悬停色
             "}"
             "QScrollBar:vertical {"
             "   width: 8px;"
             "   background: transparent;"
             "}"
             "QScrollBar::handle:vertical {"
             "   background: %3;"  // 滚动条手柄色
             "   min-height: 20px;"
             "   border-radius: 4px;"
             "}"
             "QScrollBar:vertical {"
             "   width: 8px;"
             "   background: %1;" // 动态背景色
             "   border-radius: 4px;"
             "}"
             "QScrollBar::handle:vertical {"
             "   background: #A0A0A0;" // 固定手柄颜色
             "   min-height: 20px;"
             "   border-radius: 4px;"
             "}"
             ).arg(
                 m_theme.backgroundColor().name(),
                 m_theme.backgroundColor().lighter(120).name(),
                 m_theme.highlightColor().name()
             ));
     }

    updateProgressBarTheme();
    updateIcons();
}

void mainpage::updateIcons() {
    // 根据背景色切换图标
    const bool useDarkIcons = ThemeManager::isDarkTheme(m_theme.backgroundColor());
    const QString iconPrefix = useDarkIcons ? "white" : "black";

    QList<QAction*> oldActions = ui->serachedit->actions();
    for (QAction* act : oldActions) {
    ui->serachedit->removeAction(act);
    delete act;
    }

    QIcon searchIcon(QString(":/images/theme/search-%1.png").arg(iconPrefix));
    QAction* action = ui->serachedit->addAction(searchIcon, QLineEdit::LeadingPosition);

    //主题按钮
    ui->themebtn->setIcon(QIcon(
        QString(":/images/theme/theme-%1.png").arg(iconPrefix)));
    //控制按钮
    ui->minbtn->setIcon(QIcon(QString(":/images/theme/min-%1.png").arg(iconPrefix)));
    ui->maxbtn->setIcon(QIcon(QString(":/images/theme/max-%1.png").arg(iconPrefix)));
    ui->closebtn->setIcon(QIcon(QString(":/images/theme/close-%1.png").arg(iconPrefix)));

    //列表按钮
    ui->btnCurPlay->setIcon(QIcon(QString(":/images/theme/previous-%1.png").arg(iconPrefix)));
    ui->btnFlavor->setIcon(QIcon(QString(":/images/theme/favor-%1.png").arg(iconPrefix)));
    ui->btnLocal->setIcon(QIcon(QString(":/images/theme/local-%1.png").arg(iconPrefix)));
    ui->btnvideo->setIcon(QIcon(QString(":/images/theme/local-video-%1.png").arg(iconPrefix)));
    ui->addNewlist->setIcon(QIcon(QString(":/images/theme/add-%1.png").arg(iconPrefix)));


//    // 更新音乐项控件中的图标
//    const QList<QListWidget*> lists = {
//        localMusicWidget,
//        curPlayWidget,
//        favorWidget
//    };
//    for(auto* list : lists) {
//        for(int i=0; i<list->count(); ++i) {
//            if(auto* widget = qobject_cast<MusicItemWidget*>(
//                list->itemWidget(list->item(i))))
//            {
//                widget->updateIconTheme(useDarkIcons);
//            }
//        }
//    }
}
void mainpage::setupListWidget(QListWidget *listWidget)
{
    // 通用列表样式设置
    listWidget->setFrameShape(QFrame::NoFrame);
    listWidget->setSpacing(8);  // 增加间距到8px
    listWidget->setUniformItemSizes(true);
    listWidget->setSelectionMode(QAbstractItemView::NoSelection);
    listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel); // 平滑滚动
    listWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    listWidget->setAlternatingRowColors(true);


    listWidget->setMouseTracking(true);


    connect(listWidget, &QListWidget::itemDoubleClicked,
            this, &mainpage::onMusicItemDoubleClicked);

}

void mainpage::updateProgressBarTheme() {
    QColor bgColor = m_theme.backgroundColor();
    QColor highlight = m_theme.highlightColor();
    bool isDark = ThemeManager::isDarkTheme(bgColor);

    QString progressStyle = QString(
        "QProgressBar {"
        "    border: 1px solid %1;"
        "    border-radius: 4px;"
        "    background: %2;"
        "    text-align: center;"
        "    height: 6px;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: %3;"
        "    border-radius: 3px;"
        "}"
    ).arg(
        isDark ? "#606060" : "#C0C0C0",
        bgColor.name(),
        highlight.name()
    );

    localMusicWidget->verticalScrollBar()->setStyleSheet(progressStyle);


}

void mainpage::loadMusicFromDatabase(QListWidget *listWidget)
{
    QSqlTableModel* songModel = m_dbHandler->getSongModel();
    if (!songModel) {
        QMessageBox::critical(this, "错误", "无法加载歌曲模型");
        return;
    }

    QMap<int, bool> favoritesCache;
    QSqlQuery favQuery("SELECT song_id FROM favorites");
    if (favQuery.exec()) {
        while (favQuery.next()) {
            int songId = favQuery.value(0).toInt();
            favoritesCache[songId] = true;
        }
    }

    listWidget->clear();
    for (int i = 0; i < songModel->rowCount(); ++i) {
        QModelIndex index = songModel->index(i, 0);
        int songId = songModel->data(index, Qt::DisplayRole).toInt();
        QString filePath = songModel->data(songModel->index(i, 1)).toString();

        QString title = songModel->data(songModel->index(i, 2)).toString();
        QString artist = songModel->data(songModel->index(i, 3)).toString();
        QString album = songModel->data(songModel->index(i, 4)).toString();
        QString duration = songModel->data(songModel->index(i, 7)).toString();
        bool is_cloud = songModel->data(songModel->index(i, 8)).toBool();
        QPixmap cover = extractCover(title); // 使用歌曲名而不是文件路径
        QListWidgetItem *item = new QListWidgetItem();
        MusicItemWidget *widget = new MusicItemWidget();

        // 连接信号
        connect(widget, &MusicItemWidget::onAddToPlaylistRequested,
                this, &mainpage::handleAddToPlaylistRequest);
        widget->updateData(
            listWidget->count(),
            cover,
            title,
            artist,
            album,
            "",
            favoritesCache.contains(songId),
            songId,
            filePath,
            is_cloud
        );

        connect(widget, &MusicItemWidget::liked, this, [=]( bool isFavorite,int songId) {
            onMusicLiked(isFavorite, songId);
        });
        item->setSizeHint(widget->sizeHint());
        listWidget->addItem(item);
        listWidget->setItemWidget(item, widget);
    }
}

void mainpage::loadPlayHistory(QListWidget *listWidget)

{
//    // 禁用更新，减少界面刷新次数
//    listWidget->setUpdatesEnabled(false);

    QString sql = "SELECT ph.song_id, s.file_path, s.title, s.artist, s.album, s.duration, "
                     "ph.play_count, ph.play_time, s.is_cloud, "
                     "CASE WHEN f.song_id IS NOT NULL THEN 1 ELSE 0 END AS is_favorite "
                     "FROM play_history ph "
                     "JOIN songs s ON ph.song_id = s.id "
                     "LEFT JOIN favorites f ON ph.song_id = f.song_id "
                     "ORDER BY ph.play_time DESC";
       QSqlQuery query(sql);
       if (!query.exec()) {
           qWarning() << "查询播放历史失败:" << query.lastError().text();  // 检查错误日志
           return;
       }

    listWidget->clear();

    // 预加载封面图片缓存
    QMap<QString, QPixmap> coverCache;

    while (query.next()) {
        int songId = query.value(0).toInt();
        QString filePath = query.value(1).toString();
        QString title = query.value(2).toString();
        QString artist = query.value(3).toString();
        QString album = query.value(4).toString();
        QString duration = query.value(5).toString();
        bool isCloud = query.value(8).toBool();
        bool isFavorite = query.value(9).toInt() == 1;

        QPixmap cover;
        if (!coverCache.contains(filePath)) {
            cover = extractCover(title);
            coverCache[filePath] = cover; // 缓存封面
        } else {
            cover = coverCache[filePath]; // 使用缓存
        }

        QListWidgetItem *item = new QListWidgetItem();
        MusicItemWidget *widget = new MusicItemWidget();
        connect(widget, &MusicItemWidget::onAddToPlaylistRequested,
                this, &mainpage::handleAddToPlaylistRequest);

        widget->updateData(
            listWidget->count(), // 这里可以提前计算总数，避免每次调用
            cover,
            title,
            artist,
            album,
            QString(""),
            isFavorite,
            songId,
            filePath,
            isCloud
        );
        connect(widget, &MusicItemWidget::liked, this, [=]( bool isFavorite,int songId) {
            onMusicLiked(isFavorite, songId);
        });
        item->setSizeHint(widget->sizeHint());
        listWidget->addItem(item);
        listWidget->setItemWidget(item, widget);
    }
}

void mainpage::loadFavorites(QListWidget *listWidget)
{
    int scrollPos = listWidget->verticalScrollBar()->value();
//    listWidget->setUpdatesEnabled(false);

    // 合并查询，减少数据库交互次数
    QString sql = "SELECT s.id, s.file_path, s.title, s.artist, s.album, s.duration, s.is_cloud, "
                  "1 AS is_favorite " // 因为是收藏列表，所以直接标记为收藏
                  "FROM favorites f "
                  "JOIN songs s ON f.song_id = s.id";
    QSqlQuery query(sql);
    if (!query.exec()) {
        QMessageBox::critical(this, "错误", "无法加载收藏: " + query.lastError().text());
        listWidget->setUpdatesEnabled(true); // 恢复更
        return;
    }

    listWidget->clear();
    // 预加载封面图片缓存
    QMap<QString, QPixmap> coverCache;

    m_firstFavorSongId = -1;

       int count = 0;
       while (query.next()) {
           int songId = query.value(0).toInt();
           QString filePath = query.value(1).toString();
           QString title = query.value(2).toString();
           QString artist = query.value(3).toString();
           QString album = query.value(4).toString();
           QString duration = query.value(5).toString();
           bool isCloud = query.value(6).toBool();
           bool isFavorite = query.value(7).toInt() == 1; // 直接从查询结果获取收藏状态

           QPixmap cover;
           if (!coverCache.contains(filePath)) {
               cover = extractCover(title);
               coverCache[filePath] = cover; // 缓存封面
           } else {
               cover = coverCache[filePath]; // 使用缓存
           }

//           // 记录第一首歌曲
//           if (count == 0) {
//               m_firstFavorSongId = songId;
//               // 尝试立即获取封面
//               QString key = QString::number(songId);
//               QPixmap cover = CoverCacheManager::instance()->getCover(key);
//               if (!cover.isNull()) {
//                   updateFavorCover(cover);
//               } else {
//                   // 触发异步加载
//                   CoverCacheManager::instance()->loadCover(
//                       key,
//                       QUrl("https://your-cover-api.com/cover/" + key)
//                   );
//               }
//           }

        QListWidgetItem *item = new QListWidgetItem();
        MusicItemWidget *widget = new MusicItemWidget();
        connect(widget, &MusicItemWidget::onAddToPlaylistRequested,
                this, &mainpage::handleAddToPlaylistRequest);

        widget->updateData(
            listWidget->count(), // 这里可以提前计算总数，避免每次调用
            cover,
            title,
            artist,
            album,
            "",
            isFavorite,
            songId,
            filePath,
            isCloud
        );

        connect(widget, &MusicItemWidget::liked, this, [=]( bool isFavorite,int songId) {
            onMusicLiked(isFavorite, songId);
        });
        widget->setThemeColor(m_theme.backgroundColor());


        item->setSizeHint(widget->sizeHint());

        listWidget->verticalScrollBar()->setValue(scrollPos);
        listWidget->addItem(item);
        listWidget->setItemWidget(item, widget);
    }
       count++;
//       // 无收藏时的处理
//       if (count == 0) {
//           updateFavorCover(m_defaultCover);
//       }
   }

//void mainpage::updateFavorCover(const QPixmap &cover)
//{
//    // 假设封面控件是 ui->favorCover（根据实际UI调整）
//    QPixmap displayCover = cover.isNull()
//        ? m_defaultCover
//        : cover.scaled(ui->favorCover->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

//    ui->favorCover->setPixmap(displayCover);
//}
std::tuple<QString, QString, QString, QString, QPixmap> mainpage::extractMusicInfo(const QFileInfo &file)
{
    QString title, artist, album, duration;
    QPixmap cover;

    QSqlQuery query;
    QString musicDir = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
       QString relativePath = QDir(musicDir).relativeFilePath(file.absoluteFilePath());

    query.prepare("SELECT title, artist, album, duration, cover FROM songs WHERE file_path = ?");
    query.addBindValue(relativePath);
    if (query.exec() && query.next()) {
        title = query.value(0).toString();
        artist = query.value(1).toString();
        album = query.value(2).toString();
        duration = query.value(3).toString();
        QByteArray coverData = query.value(4).toByteArray();
        if (!coverData.isEmpty()) {
            cover.loadFromData(coverData);
        }
    }

    return {title, artist, album, duration, cover};
}

QPixmap mainpage::extractCover(const QString& songName)
{

    QPixmap cover = CoverCacheManager::instance()->getCover(songName);
    if (!cover.isNull()) {

        return cover;
    }

    QString encodedName = QUrl::toPercentEncoding(songName);
    QUrl coverUrl(QString("http://vip.frpee.com:13678/music/cover/%1.jpg").arg(encodedName));
    if (!coverUrl.isValid()) {
        qDebug() << "无效的 URL:" << coverUrl.toString();
        return QPixmap(":/images/image/non-music.png");
    }

    CoverCacheManager::instance()->loadCover(songName, coverUrl);
    return QPixmap(":/images/image/non-music.png");
}

void mainpage::onCoverDownloadFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "封面下载失败:" << reply->url() << reply->errorString();
        reply->deleteLater();
        return;
    }

    QString coverUrl = reply->request().attribute(QNetworkRequest::User).toString();
    QByteArray imageData = reply->readAll();
    reply->deleteLater();

    QPixmap cover;
    if (cover.loadFromData(imageData)) {
        coverCache.insert(coverUrl, cover);

        // 更新所有使用该封面的控件
        updateAllCovers(coverUrl, cover);
//        ui->reconwidget->updateRecommendationUI();
//        qInfo()<<"调用了";
    }
}

void mainpage::updateAllCovers(const QString &coverUrl, const QPixmap &cover)
{
    const QList<QListWidget*> lists = {
        localMusicWidget,
        curPlayWidget,
        favorWidget,
        playlistwidget,
        searchResultsWidget
    };

    for (QListWidget* list : lists) {
        for (int i = 0; i < list->count(); ++i) {
            if (QListWidgetItem* item = list->item(i)) {
                if (MusicItemWidget* widget = qobject_cast<MusicItemWidget*>(list->itemWidget(item))) {
                    if (widget->getFileUrl().toString().contains(coverUrl)) {
                        widget->updateCover(cover);
                    }
                }
            }
        }
    }

}

void mainpage::onMusicLiked(bool liked, int songId)
{

    qInfo() << "收到喜欢信号，songId:" << songId << "状态:" << liked;
    if (liked) {
        createAnimationLabel("已添加到我的收藏");
        addToFavorites(songId);
    } else {
        createAnimationLabel("已从我的收藏中移除");
        removeFromFavorites(songId);
    }
    updateAllListLikedState(songId, liked); // 确保传递的是同一个 songId
}

void mainpage::addToFavorites(int songId)
{
    if (isSongInFavorites(songId)) return;

    QSqlQuery query;
    query.prepare("INSERT INTO favorites (song_id) VALUES (?)");
    query.addBindValue(songId);
    if (!query.exec()) {
        qWarning() << "添加收藏失败:" << query.lastError().text();
        return;
    }

    if(ui->stackedWidget->currentIndex() == 2) {
           QMetaObject::invokeMethod(this, [this](){
               loadFavorites(favorWidget);
           }, Qt::QueuedConnection); // 异步执行
       }

}
void mainpage::createAnimationLabel(const QString &text) {

    int duration = 300;
    QLabel* label = new QLabel(text, nullptr); // 父对象设为nullptr
    label->setStyleSheet(
        "QLabel {"
        "   background-color: rgba(50, 50, 50, 220);"
        "   color: white;"
        "   font-size: 14px;"
        "   padding: 8px 12px;"
        "   border-radius: 15px;"  // 关键圆角设置
        "   border: none;"
        "}"
    );
    label->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    label->setAttribute(Qt::WA_TranslucentBackground);
    label->adjustSize();

    QPoint centerPos = QPoint(
        this->geometry().center().x() + 450,
        this->geometry().bottom() - 100 // 底部向上100像素
    );
    label->move(centerPos);
    label->show();

    QPropertyAnimation* fadeIn = new QPropertyAnimation(label, "windowOpacity");
    fadeIn->setDuration(duration);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);

    QTimer::singleShot(2000, [=]() {
        QPropertyAnimation* fadeOut = new QPropertyAnimation(label, "windowOpacity");
        fadeOut->setDuration(duration);
        fadeOut->setStartValue(1.0);
        fadeOut->setEndValue(0.0);
        connect(fadeOut, &QPropertyAnimation::finished, label, &QLabel::deleteLater);
        fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
    });
}

void mainpage::removeFromFavorites(int songId)
{
    if (!isSongInFavorites(songId)) return;

    QSqlQuery query;
    query.prepare("DELETE FROM favorites WHERE song_id = ?");
    query.addBindValue(songId);
    if (!query.exec()) {
        qWarning() << "取消收藏失败:" << query.lastError().text();
        return;
    }

    // 更新收藏列表
    loadFavorites(favorWidget);
}

 bool mainpage::isSongInFavorites(int songId)
 {
        QSqlQuery query;
        query.prepare("SELECT 1 FROM favorites WHERE song_id = ?");
        query.addBindValue(songId);
        if (!query.exec()) {
            qWarning() << "查询收藏状态失败:" << query.lastError().text();
            return false;
        }
        return query.next(); // 如果查询到记录，返回 true
 }

 void mainpage::highlightCurrentSong(int index)
 {
     if(!currentPlayList || index < 0) return;

     // 清除旧高亮
     if(lastHighlightedItem) {
         if(auto* oldWidget = qobject_cast<MusicItemWidget*>(
             currentPlayList->itemWidget(lastHighlightedItem))) {
             oldWidget->setHighlighted(false, false);
         }
     }

     // 设置新高亮
     if(index < currentPlayList->count()) {
         QListWidgetItem *currentItem = currentPlayList->item(index);
         if(auto* widget = qobject_cast<MusicItemWidget*>(
             currentPlayList->itemWidget(currentItem))) {

             widget->setHighlighted(true, m_footWidget->getPlayer()->state() == QMediaPlayer::PlayingState);
             lastHighlightedItem = currentItem;
             currentPlayList->scrollToItem(currentItem);
             m_footWidget->updateSongInfo(widget->getTitle(), widget->getArtist());
             m_footWidget->setCurrentSongId(widget->getSongId());
         }
     }
 }
 void mainpage::updateAllListLikedState(int songId, bool state)
 {
     qInfo() << "更新所有列表的喜欢状态，songId:" << songId << "state:" << state;

     const QList<QListWidget*> lists = { localMusicWidget, curPlayWidget, favorWidget };
     for (QListWidget* list : lists) {
         for (int i = 0; i < list->count(); ++i) {
             QListWidgetItem* item = list->item(i);
             if (auto* w = qobject_cast<MusicItemWidget*>(list->itemWidget(item))) {
                 qDebug() << "检查列表项，songId:" << w->getSongId();
                 if (w->getSongId() == songId) {
                     w->setLiked(state);

                     // 特殊处理收藏列表
                     if (list == favorWidget && !state) {
                         list->takeItem(i);
                         delete item;
                         i--; // 调整索引
                     }
                 }
             }
         }
     }

     if (m_footWidget->getsongId()== songId) {

         m_footWidget->onMusicItemLikeChanged(state, songId);
     }


     if (ui->stackedWidget->currentIndex() == 2 && state) {
         QMetaObject::invokeMethod(this, [this](){
             loadFavorites(favorWidget);
         }, Qt::QueuedConnection);
     }
 }
 void mainpage::loadPlaylists()
 {
     QSqlTableModel* playlistsModel = m_dbHandler->getPlaylistsModel();
     if (!playlistsModel) {
         QMessageBox::critical(this, "错误", "无法加载歌单数据");
         return;
     }

     // 设置显示列和ID角色
     playlistsModel->setHeaderData(1, Qt::Horizontal, tr("歌单名称"));
     ui->listView->setModel(playlistsModel);
     ui->listView->setCursor(Qt::PointingHandCursor);
     ui->listView->setModelColumn(1); // 显示名称列

 }
 void mainpage::onPlaylistSelected(const QModelIndex &current, const QModelIndex &previous)
 {
     ui->infolabel->setText("我的歌单");
     ui->infolabel->show();
     Q_UNUSED(previous)

         if (!current.isValid()) {
             loadMusicFromDatabase(ui->localMusicWidget);
             ui->stackedWidget->setCurrentIndex(1);
             return;
         }

         // 1. 首先获取模型
         QAbstractItemModel* model = ui->listView->model();
         int row = current.row();
         int playlistId = model->data(model->index(row, 0)).toInt();

         qDebug() << "Selected playlist ID:" << playlistId; // 调试输出

         if (playlistId <= 0) {
             qWarning() << "Invalid playlist ID:" << playlistId;
             return;
         }

         ui->stackedWidget->setCurrentIndex(3);


         QSqlQueryModel* playlistSongsModel = m_dbHandler->getPlaylistSongsModel(playlistId);
         if (!playlistSongsModel || playlistSongsModel->rowCount() == 0) {
             qWarning() << "Failed to load playlist songs or playlist is empty";
//             QMessageBox::information(this, "提示", "该歌单为空");
             createAnimationLabel("该歌单为空");
             return;
         }

         QMap<int, bool> favoritesCache;
             QSqlQuery favQuery("SELECT song_id FROM favorites");
             if (favQuery.exec()) {
                 while (favQuery.next()) {
                     int songId = favQuery.value(0).toInt();
                     favoritesCache[songId] = true;
                 }
             }


         ui->playlistSongsWidget->clear();
     int rowCount = 0;

     for (int i = 0; i < playlistSongsModel->rowCount(); ++i) {
         int songId = playlistSongsModel->data(playlistSongsModel->index(i, 0)).toInt();
         QString filePath = playlistSongsModel->data(playlistSongsModel->index(i, 1)).toString();
         QString title = playlistSongsModel->data(playlistSongsModel->index(i, 2)).toString();
         QString artist = playlistSongsModel->data(playlistSongsModel->index(i, 3)).toString();
         QString album = playlistSongsModel->data(playlistSongsModel->index(i, 4)).toString();
         QString duration = playlistSongsModel->data(playlistSongsModel->index(i, 5)).toString();
         bool isCloud = playlistSongsModel->data(playlistSongsModel->index(i, 6)).toBool();//这里不知道为什么是false
         QPixmap cover = extractCover(title);

         QListWidgetItem* item = new QListWidgetItem();
         MusicItemWidget* widget = new MusicItemWidget();

         widget->updateData(
             rowCount++,
             cover,
             title,
             artist,
             album,
             duration,
             favoritesCache.contains(songId),
             songId,
             filePath,
             true
         );

         connect(widget, &MusicItemWidget::liked, this, &mainpage::onMusicLiked);

         item->setSizeHint(widget->sizeHint());
         ui->playlistSongsWidget->addItem(item);
         ui->playlistSongsWidget->setItemWidget(item, widget);
     }


//     QMediaPlaylist* playlist = createPlaylistFromListWidget(playlistwidget);
//     cachePlaylist(playlistwidget, playlist);
     switchToList(playlistwidget);
     qInfo()<<"已切换";

 }
 void mainpage::onAddPlaylistClicked()
 {
     // 弹出对话框让用户输入歌单名称
     bool ok;
     QString playlistName = QInputDialog::getText(this, tr("创建歌单"), tr("歌单名称:"), QLineEdit::Normal, "", &ok);
     if (ok && !playlistName.isEmpty()) {
         // 添加歌单到数据库
         if (m_dbHandler->addPlaylist(playlistName)) {
             loadPlaylists();
         } else {
//             QMessageBox::critical(this, "错误", "歌单创建失败！");
             createAnimationLabel("歌单创建失败");
         }
     }
 }


 QStringList mainpage::getPlaylistNames()
 {
     QStringList names;
     QSqlQuery query;
     query.exec("SELECT name FROM playlists");
     while (query.next()) {
         names.append(query.value(0).toString());
     }
     return names;
 }

 // 通过名称获取歌单ID
 int mainpage::getPlaylistIdByName(const QString &name)
 {
     QSqlQuery query;
     query.prepare("SELECT id FROM playlists WHERE name = ?");
     query.addBindValue(name);
     if (query.exec() && query.next()) {
         return query.value(0).toInt();
     }
     return -1;
 }

 // 添加歌曲到歌单
 void mainpage::addSongToPlaylist(int playlistId, int songId)
 {
     m_dbHandler->addSongToPlaylist(playlistId, songId); // 假设DatabaseHandler有一个这样的函数
 }

 void mainpage::handleAddToPlaylistRequest(int songId)
 {
     qDebug() << "添加歌曲";
     QStringList playlists = getPlaylistNames();
     if (playlists.isEmpty()) {
//         QMessageBox::information(this, "提示", "您还没有歌单，可以创建一个！");
         createAnimationLabel("您还没有歌单，可以创建一个");
         onAddPlaylistClicked();
         return;
     }

     QMenu menu(this);
     for (const QString& playlistName : playlists) {
         QAction* action = menu.addAction(playlistName);
         action->setData(playlistName);
     }

     QAction* newPlaylistAction = menu.addAction(tr("新建歌单"));
     newPlaylistAction->setData("新建歌单");

     QAction* selectedAction = menu.exec(QCursor::pos());
     if (!selectedAction) return;

     QString selectedPlaylistName = selectedAction->data().toString();
     if (selectedPlaylistName == "新建歌单") {
         onAddPlaylistClicked();
     } else {
         int playlistId = getPlaylistIdByName(selectedPlaylistName);
         if (playlistId != -1) {
             addSongToPlaylist(playlistId, songId);
//             QMessageBox::information(this, "成功", "歌曲已添加到歌单！");
             createAnimationLabel("歌曲已添加到歌单");
         }
     }
 }

 void mainpage::on_btnFlavor_clicked()
{
    ui->infolabel->setText("我的收藏");
    ui->infolabel->show();
    ui->listView->clearSelection();
    loadFavorites(favorWidget);
    ui->stackedWidget->setCurrentIndex(2);
    switchToList(favorWidget);
}

void mainpage::on_btnLocal_clicked()
{
    ui->infolabel->setText("本地歌曲");
    ui->infolabel->show();
    ui->listView->clearSelection();
    loadMusicFromDatabase(localMusicWidget);
    ui->stackedWidget->setCurrentIndex(1);
    switchToList(localMusicWidget);
}

void mainpage::on_btnCurPlay_clicked()
{
    ui->infolabel->setText("历史播放");
    ui->infolabel->show();
    ui->listView->clearSelection();
    loadPlayHistory(curPlayWidget);
    ui->stackedWidget->setCurrentIndex(0);
    switchToList(curPlayWidget);
}

void mainpage::on_reconbtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

void mainpage::refreshAllCovers()
{
    qDebug()<<"重新加载";
    QSqlTableModel* songModel = m_dbHandler->getSongModel();
    if (!songModel) {
        QMessageBox::critical(this, "错误", "无法加载歌曲模型");
        return;
    }

    // 获取所有歌曲的标题
    QStringList songTitles;
    for (int i = 0; i < songModel->rowCount(); ++i) {
        QModelIndex index = songModel->index(i, 2); // 假设标题在第3列
        QString title = songModel->data(index).toString();
        songTitles.append(title);
    }

    // 遍历所有歌曲标题并重新加载封面
    for (const QString& title : songTitles) {
        CoverCacheManager::instance()->loadCover(title, QUrl("http://vip.frpee.com:13678/music/cover/" + QUrl::toPercentEncoding(title) + ".jpg"));
    }
}

void mainpage::on_vedio_doubleclicked()
{

    this->showMinimized();
    m_videoPlayer->setAttribute(Qt::WA_DeleteOnClose);
    m_videoPlayer->show();
}

void mainpage::on_btnvideo_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}
