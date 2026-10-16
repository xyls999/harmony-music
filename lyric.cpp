#include "lyric.h"
#include "ui_lyric.h"
#include <cmath>
#include <algorithm>
#include <QtMath>

Lyric::Lyric(QWidget *parent) :
    ui(new Ui::Lyric),
    m_themeMenu(new QMenu(this)),m_themeButton(new QToolButton(this))
{
    ui->setupUi(this);
    m_networkManager = new QNetworkAccessManager(this);
    setupThemeMenu();
    createControlButtons();

    setWindowFlags(Qt::FramelessWindowHint);
    setBackgroundGradient(QColor("#1D976C"), QColor("#93F9B9"));
    connect(CoverCacheManager::instance(), &CoverCacheManager::coverLoaded, this, &Lyric::updateCover);

    initUI();
    applyAnimationToAllButtons();

    QAction* defaultThemeAction = m_themeMenu->actions().at(3);
    defaultThemeAction->setChecked(true);
    changeTheme(defaultThemeAction);

    m_volumePopup->slider->setValue(50);
    updateVolumeIcon(50);


    connect(btnMin, &QToolButton::clicked, this, &Lyric::switchnormal);
    connect(btnMax, &QToolButton::clicked, this, &Lyric::toggleMaximize);
    connect(btnClose, &QToolButton::clicked, qApp, &QApplication::quit);
}
Lyric::~Lyric()
{
    delete ui;
}

void Lyric::initUI()
{
    //按钮+封面+当前播放
    ui->emoji->setEnabled(false);
    ui->cover->setFixedSize(300, 300);
    ui->cover->setAlignment(Qt::AlignCenter);

    m_coverShadow = new QGraphicsDropShadowEffect(this);
    m_coverShadow->setBlurRadius(25);  // 模糊半径
    m_coverShadow->setOffset(8, 8);    // 偏移量
    ui->cover->setGraphicsEffect(m_coverShadow);

    ui->cover->setStyleSheet(R"(
            QLabel {
                border: 4px solid rgba(255,255,255,0.3);
                border-radius: 150px;  /* 300/2 */
                background-color: transparent;
            }
        )");

    ui->title->setStyleSheet(R"(
            QLabel {
                font: bold 28px 'Microsoft YaHei';
                color: #FFFFFF;
                padding: 8px 20px;
                background-color: rgba(0,0,0,0.3);
                border-radius: 15px;
            }
        )");

    m_titleShadow = new QGraphicsDropShadowEffect(this);
    m_titleShadow->setBlurRadius(16);
    m_titleShadow->setColor(QColor(0,0,0,120));
    m_titleShadow->setOffset(3, 3);
    ui->title->setGraphicsEffect(m_titleShadow);

    //歌词
    QWidget *scrollContent = new QWidget;
    scrollContent->setAttribute(Qt::WA_TranslucentBackground); //背景透明
    scrollContent->setStyleSheet("background: transparent;");
    ui->lyricScrollArea->setWidget(scrollContent);
    ui->lyricScrollArea->setWidgetResizable(true);
    m_lyricLayout = new QVBoxLayout(scrollContent);
    m_lyricLayout->setAlignment(Qt::AlignCenter);

    ui->lyricScrollArea->setStyleSheet(R"(
        QScrollArea { border: none; background: transparent; }
        QScrollBar:vertical {
            width: 6px;
            background: transparent;
        }
        QScrollBar::handle:vertical {
            background: rgba(255,255,255,0.4);
            border-radius: 3px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover { background: rgba(255,255,255,0.6); }
        QScrollBar::add-line, QScrollBar::sub-line { background: none; }
    )");
    //音量
    m_volumePopup = new VolumePopup(this);
    m_volumePopup->hide();

    m_hoverTimer = new QTimer(this);
    m_hoverTimer->setSingleShot(true);
    connect(m_hoverTimer, &QTimer::timeout, [this](){
        if (m_isHovering && !m_popupVisible) {
            m_volumePopup->showAboveWidget(ui->lyricvolume);
            m_popupVisible = true;
        }
    });

    ui->lyricvolume->installEventFilter(this);
    m_volumePopup->installEventFilter(this);

    connect(m_volumePopup, &VolumePopup::volumeChanged, [this](int volume){
        QSignalBlocker blocker(ui->lyricvolume);
        handleVolumeChange(volume);
    });

    connect(ui->lyricvolume, &QPushButton::clicked,
            this, &Lyric::toggleMuteState);
    // 连接信号
     connect(ui->lyricplay, &QToolButton::clicked,
             this, &Lyric::playPauseRequested);
     connect(ui->lyricpre, &QToolButton::clicked,
             this, &Lyric::previousRequested);
     connect(ui->lyricnext, &QToolButton::clicked,
             this, &Lyric::nextRequested);
     connect(ui->lyricmode, &QToolButton::clicked,
             this, &Lyric::playModeChanged);
     connect(ui->lyriclist,&QPushButton::clicked,this,&Lyric::curplaylistRequested);

     connect(ui->lyricaddlist,&QPushButton::clicked,this,&Lyric::addlistRequested);

    //进度条
    ui->lyricSlider->setRange(0, 1000);
    ui->lyricSlider->setValue(0);
    updateSliderStyle(m_bgColor1, m_bgColor2);
    connect(ui->lyricSlider, &QSlider::sliderMoved,
            this, &Lyric::lyricSliderMoved);

    aibtn = new AIButton(this);

    ui->horizontalLayout_5->insertWidget(0,aibtn);
    connect(aibtn, &AIButton::clickedWithPosition,
               this, &Lyric::onOpenChatWindowClicked);


}

void Lyric::updateCover(const QString& key, const QPixmap& cover)
{
    if (ui->cover->property("cacheKey").toString() == key) {
        setAlbumCover(cover);
    }
}

bool Lyric::eventFilter(QObject *obj, QEvent *event) {
    if (obj == ui->lyricvolume) {
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
                    if (!m_volumePopup->underMouse()) {
                        m_volumePopup->hide();
                        m_popupVisible = false;
                    }
                });
            }
            break;
        case QEvent::MouseMove:
            if (m_popupVisible) {
                static QElapsedTimer moveTimer;
                if (moveTimer.hasExpired(50)) {
                    m_volumePopup->showAboveWidget(ui->lyricvolume);
                    moveTimer.start();
                }
            }
            break;
        default:
            break;
        }
    }
    else if (obj == m_volumePopup) {
        if (event->type() == QEvent::Leave) {
            QPoint globalPos = QCursor::pos();
            QPoint btnPos = ui->lyricvolume->mapFromGlobal(globalPos);
            if (!ui->lyricvolume->rect().contains(btnPos)) {
                m_volumePopup->hide();
                m_popupVisible = false;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

QSlider *Lyric::lyricSlider() const
{
    return ui->lyricSlider;
}

QPushButton *Lyric::lyricplay() const
{
    return ui->lyricplay;
}

QPushButton *Lyric::lyricmode() const
{
    return ui->lyricmode;
}

QLabel *Lyric::lyricduration() const
{
    return ui->lyricduration;
}

QSlider *Lyric::getvolumeSlider() const
{
    return m_volumePopup->slider;
}
void Lyric::setVolume(int volume) {
    QSignalBlocker blocker(m_volumePopup->slider);
   m_volumePopup->slider->setValue(volume);
    updateVolumeIcon(volume);
}

void Lyric::setMuted(bool muted) {
    m_isMuted = muted;
    updateVolumeIcon(m_volumePopup->slider->value());
}
void Lyric::toggleMuteState() {
    const bool newMuted = !m_isMuted;

    if (newMuted) {
        m_lastVolume = m_volumePopup->slider->value();
        m_volumePopup->slider->setValue(0);
    } else {
        int restoreVolume = m_lastVolume > 0 ? m_lastVolume : 50;
        m_volumePopup->slider->setValue(restoreVolume);
    }

    m_isMuted = newMuted;
    updateVolumeIcon(m_volumePopup->slider->value());
    emit muteToggled(newMuted);

    emit volumeChanged(m_volumePopup->slider->value());
}

void Lyric::handleVolumeChange(int volume) {

    m_volumePopup->slider->setValue(volume);
    m_volumePopup->setVolume(volume);

    m_isMuted = (volume == 0);
    updateVolumeIcon(volume);
    emit volumeChanged(volume);
}
void Lyric::updateVolumeIcon(int volume) {
    QString iconName;

    if(m_isMuted || volume == 0) {
        iconName = "mute";
    } else if(volume < 33) {
        iconName = "volume-low";
    } else if(volume < 66) {
        iconName = "volume-medium";
    } else {
        iconName = "volume-high";
    }

    ui->lyricvolume->setIcon(QIcon(QString(":/images/theme/%1-black.png")
                          .arg(iconName)));
}
void Lyric::setAlbumCover(const QPixmap& cover)
{
    QPixmap circularCover(300, 300);
    circularCover.fill(Qt::transparent);

    QPainter painter(&circularCover);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    QPainterPath path;
    path.addEllipse(0, 0, 300, 300);
    painter.setClipPath(path);

    QPixmap scaled = cover.scaled(300, 300, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    painter.drawPixmap((300 - scaled.width()) / 2, (300 - scaled.height()) / 2, scaled);

    ui->cover->setPixmap(circularCover);
}

void Lyric::setSliderPosition(int position)
{
    QSignalBlocker blocker(ui->lyricSlider);
    ui->lyricSlider->setValue(position);
}

void Lyric::onOpenChatWindowClicked(const QPoint &btnPos)
{
    if (aiwindow && !aiwindow->isHidden()) {
        aiwindow->activateWindow(); // 激活现有窗口
        return;
    }

    aiwindow = new MainWindow();
    aiwindow->setAttribute(Qt::WA_DeleteOnClose);

    connect(aiwindow, &QObject::destroyed, [this]() {
        aiwindow = nullptr;
    });

    aiwindow->setAttribute(Qt::WA_DeleteOnClose);

    const int windowWidth = 436;
    const int windowHeight = 700;
    const QPoint windowPos(
          btnPos.x() - windowWidth,
          btnPos.y() - windowHeight
       );

    aiwindow->setGeometry(QRect(windowPos, QSize(windowWidth, windowHeight)));
    aiwindow->show();
}
void Lyric::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QLinearGradient gradient(0, 0, width(), height());
    gradient.setColorAt(0, m_bgColor1);
    gradient.setColorAt(1, m_bgColor2);
    painter.fillRect(rect(), gradient);
}
void Lyric::setBackgroundGradient(const QColor& color1, const QColor& color2)
{
    m_bgColor1 = color1;
    m_bgColor2 = color2;
    update();
}
void Lyric::clearLyricLayout() {
    while (QLayoutItem* item = m_lyricLayout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
    m_lyricTimes.clear();
    m_lyricLabels.clear();
    m_currentHighlightLabel = nullptr;
}

void Lyric::loadLyric(const QString& title, const QString& filePath)
{
    clearLyricLayout();
    m_lyricMap.clear();

    // 提取歌曲名
    QString songName = QFileInfo(filePath).completeBaseName();

    // 加载封面
    QPixmap cover = CoverCacheManager::instance()->getCover(title);
    qDebug()<<title;
    if (!cover.isNull()) {
        qInfo()<<"jiazai";
        setAlbumCover(cover);
    } else {
        QUrl coverUrl(QString("http://vip.frpee.com:13678/music/cover/%1.jpg").arg(title));
        if (coverUrl.isValid()) {
            CoverCacheManager::instance()->loadCover(title, coverUrl);
        }
    }

    // 判断是否是网络路径
    if(filePath.startsWith("http://") || filePath.startsWith("https://"))
    {
        // 发起网络请求
        QNetworkRequest request;
        request.setUrl(QUrl(filePath));
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::SameOriginRedirectPolicy);

        // 同步请求（简单示例，实际建议用异步）
        QEventLoop loop;
        QNetworkReply* reply = m_networkManager->get(request);
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if(reply->error() != QNetworkReply::NoError) {
            showNoLyricHint(tr("网络错误: %1").arg(reply->errorString()));
            reply->deleteLater();
            return;
        }

        processLyricData(reply->readAll());
        reply->deleteLater();
    }
    else
    {
        // 本地文件处理
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            showNoLyricHint(tr("文件错误: %1").arg(file.errorString()));
            return;
        }
        processLyricData(file.readAll());
        file.close();
    }
}

void Lyric::processLyricData(const QByteArray &data)
{
    QColor bgColor = palette().color(QPalette::Window);
    bool isDark = bgColor.lightness() < 128;
    QString normalColor = isDark ? "rgba(255,255,255,0.6)" : "rgba(0,0,0,0.6)";
    QString highlightColor = isDark ? "white" : "black";

   QTextStream stream(const_cast<QByteArray*>(&data));
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        QRegularExpression re(R"(\[(\d+):(\d+\.?\d*)\](.*))");
        QRegularExpressionMatch match = re.match(line);

        if (match.hasMatch()) {
            int minutes = match.captured(1).toInt();
            double seconds = match.captured(2).toDouble();
            qint64 time = (minutes * 60 + seconds) * 1000;
            m_lyricMap[time] = match.captured(3).trimmed();
        }
    }

    m_lyricTimes = m_lyricMap.keys();
    for (qint64 time : m_lyricTimes) {
        // 使用自定义的可点击标签
        ClickableLabel* label = new ClickableLabel(m_lyricMap[time]);
        label->setProperty("lyricTime", time);

        label->setStyleSheet(QString(R"(
                QLabel {
                    color: %1;
                    font-size: 18px;
                    padding: 8px;
                    margin: 2px 0;
                    border-radius: 4px;
                }
                QLabel:hover, QLabel[highlighted="true"] {
                    color: %2;
                    font-weight: bold;
                    font-size: 20px;
                    background: rgba(255,255,255,0.1);
                }
            )").arg(normalColor, highlightColor));

        connect(label, &ClickableLabel::clicked, [this, label] {
            handleLyricClick(label);
        });
        connect(label, &ClickableLabel::doubleClicked, [this, label] {
            handleLyricClick(label);
        });

        m_lyricLayout->addWidget(label);
        m_lyricLabels.append(label);
    }

    if (m_lyricLabels.isEmpty()) {
        showNoLyricHint(tr("未找到有效歌词"));
    }
}

void Lyric::handleLyricClick(ClickableLabel* label)
{
    qint64 time = label->property("lyricTime").toLongLong();
    emit lyricClicked(time);

    // 添加点击动画效果
    QPropertyAnimation* anim = new QPropertyAnimation(label, "geometry");
    anim->setDuration(100);
    anim->setKeyValueAt(0, label->geometry().adjusted(2, 2, -2, -2));
    anim->setKeyValueAt(0.5, label->geometry().adjusted(-2, -2, 2, 2));
    anim->setEndValue(label->geometry());
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
int Lyric::findLyricIndex(qint64 position) const {
    if (m_lyricTimes.isEmpty()) return -1;

    int low = 0;
    int high = m_lyricTimes.size() - 1;
    int result = -1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (m_lyricTimes[mid] <= position) {
            result = mid;
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return result;
}

void Lyric::highlightCurrentLyric(int index) {
    QLabel* target = nullptr;
    if (index >= 0 && index < m_lyricLabels.size()) {
        target = m_lyricLabels[index];
    }

    if (m_currentHighlightLabel) {
        m_currentHighlightLabel->setProperty("highlighted", false);
        m_currentHighlightLabel->style()->unpolish(m_currentHighlightLabel);
        m_currentHighlightLabel->style()->polish(m_currentHighlightLabel);
        m_currentHighlightLabel->update();
    }

    if (target) {
        target->setProperty("highlighted", true);
        target->style()->unpolish(target);
        target->style()->polish(target);
        target->update();
        m_currentHighlightLabel = target;

        ui->lyricScrollArea->viewport()->update();
    }
}
void Lyric::updateLyricPosition(qint64 position) {
    int index = findLyricIndex(position);
    highlightCurrentLyric(index);

    if (index == -1) return;

    QLabel* currentLabel = m_lyricLabels[index];
    QScrollBar* scrollBar = ui->lyricScrollArea->verticalScrollBar();

    int labelCenter = currentLabel->y() + currentLabel->height()/2;
    int viewportCenter = ui->lyricScrollArea->viewport()->height()/2;
    int targetValue = labelCenter - viewportCenter;

    if (abs(scrollBar->value() - targetValue) > 20) {
        QPropertyAnimation *animation = new QPropertyAnimation(scrollBar, "value");
        animation->setDuration(300);
        animation->setStartValue(scrollBar->value());
        animation->setEndValue(targetValue);
        animation->setEasingCurve(QEasingCurve::OutQuad);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void Lyric::updateSliderStyle(const QColor& themeColor1, const QColor& themeColor2)
{
    QString style = QString(R"(
        QSlider {
            height: 8px;
            background: rgba(%1, %2, %3, 50);
            border-radius: 4px;
        }
        QSlider::groove:horizontal {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 %4, stop:1 %5);
            height: 8px;
            border-radius: 4px;
        }
        QSlider::handle:horizontal {
            background: white;
            width: 16px;
            height: 16px;
            margin: -4px 0;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.2);
        }
        QSlider::handle:horizontal:hover {
            background: #f0f0f0;
            transform: scale(1.1);
        }
        QSlider::sub-page:horizontal {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 %4, stop:1 %5);
            border-radius: 4px;
        }
    )").arg(themeColor1.red()).arg(themeColor1.green()).arg(themeColor1.blue())
     .arg(themeColor1.name()).arg(themeColor2.name());

    ui->lyricSlider->setStyleSheet(style);
}
void Lyric::showNoLyricHint(const QString& message) {
    QLabel* hintLabel = new QLabel(message);
    hintLabel->setStyleSheet(R"(
        QLabel {
            color: rgba(255,255,255,0.7);
            font-size: 16px;
            font-style: italic;
            padding: 20px;
        }
    )");
    hintLabel->setAlignment(Qt::AlignCenter);
    m_lyricLayout->addWidget(hintLabel);
    m_lyricLabels.append(hintLabel);
}
void Lyric::createControlButtons()
{
    QVBoxLayout *rightLayout = qobject_cast<QVBoxLayout*>(ui->verticalLayout_3);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 5, 20, 20);
    buttonLayout->setSpacing(20);

    buttonLayout->addStretch();

    btnMin = new QToolButton(this);
    btnMin->setObjectName("btnMin");
    btnMin->setFixedSize(32, 32);
    btnMin->setStyleSheet(R"(
        QToolButton {
            background: transparent;
            border: none;
            qproperty-icon: url(:/images/image-hover/min.png);
            qproperty-iconSize: 32px 32px;
        }
        QToolButton:hover { background: rgba(255,255,255,0.1); }
    )");

    btnMax = new QToolButton(this);
    btnMax->setObjectName("btnMax");
    btnMax->setFixedSize(32, 32);
    btnMax->setStyleSheet(R"(
        QToolButton {
            background: transparent;
            border: none;
            qproperty-icon: url(:/images/image-hover/max.png);
            qproperty-iconSize: = 40px 40px;
        }
        QToolButton:hover { background: rgba(255,255,255,0.1); }
    )");


    btnClose = new QToolButton(this);
    btnClose->setObjectName("btnClose");
    btnClose->setFixedSize(32, 32);
    btnClose->setStyleSheet(R"(
        QToolButton {
            background: transparent;
            border: none;
            qproperty-icon: url(:/images/image-hover/close.png);
            qproperty-iconSize: 24px 24px;
        }
        QToolButton:hover {background: rgba(255,255,255,0.1) }
    )");

    buttonLayout->addWidget(m_themeButton);
    buttonLayout->addWidget(btnMin);
    buttonLayout->addWidget(btnMax);
    buttonLayout->addWidget(btnClose);


    rightLayout->insertLayout(0, buttonLayout);
}
void Lyric::setupThemeMenu()
{
    QStringList themes = {
        "忧郁蓝 Dark Blue Night",
        "浪漫紫 Twilight Purple",
        "快乐橙 Sunset Orange",
        "活力绿 Forest Green"
    };

    m_themeButton->setIcon(QIcon(":/images/image-hover/theme.png"));
    m_themeButton->setPopupMode(QToolButton::InstantPopup);
    m_themeButton->setStyleSheet(R"(
        QToolButton {
            background:transparent ;
            border: none;
            qproperty-iconSize: 24px 24px;
        }
        QToolButton:hover {
            background: rgba(255,255,255,0.1);
        }
    )");
    m_themeButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    // 菜单整体样式
    m_themeMenu->setStyleSheet(R"(
        QMenu {
            background: rgba(255, 255, 255, 0.95);
            border-radius: 8px;
            padding: 6px 0;
            margin: 4px 0;
            min-width: 180px;
        }
        QMenu::item {
            height: 40px;
            padding: 0 20px;
            font: 14px "Microsoft YaHei";
            color: #333333;
            border-radius: 4px;
            margin: 2px 8px;
            transition: all 0.2s ease;
        }
        QMenu::item  :hover {
            background: rgba(0, 0, 0, 0.1);
        }
        QMenu::item:checked {
            background: rgba(0, 0, 0, 0.5);
        }
    )");
    // 主题颜色配置
    const QVector<QPair<QColor, QColor>> themeColors = {
        {QColor("#2C3E50"), QColor("#3498DB")},  // 忧郁蓝
        {QColor("#4776E6"), QColor("#8E54E9")},  // 浪漫紫
        {QColor("#FF512F"), QColor("#F09819")},  // 快乐橙
        {QColor("#1D976C"), QColor("#93F9B9")}   // 活力绿
    };

    QActionGroup *themeGroup = new QActionGroup(this);
    themeGroup->setExclusive(true);

    QMap<QAction*, QLabel*> actionToCheckIconMap;

    for(int i = 0; i < themeColors.size(); ++i) {
        QWidgetAction *action = new QWidgetAction(this);
        action->setCheckable(true);

        QWidget *itemWidget = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout(itemWidget);
        layout->setContentsMargins(8, 4, 8, 4);
        layout->setSpacing(12);

        QLabel *colorPreview = new QLabel();
        colorPreview->setFixedSize(24, 24);
        colorPreview->setStyleSheet(QString(
            "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
            "stop:0 %1, stop:1 %2);"
            "border-radius: 12px;"
        ).arg(themeColors[i].first.name(), themeColors[i].second.name()));

        QLabel *textLabel = new QLabel(themes[i].split(' ')[0]);
        textLabel->setStyleSheet("color: #333333; font-size: 14px;");

        QLabel *checkIcon = new QLabel();
        checkIcon->setPixmap(QIcon(":/images/image-hover/check.svg").pixmap(16, 16));
        checkIcon->setVisible(false); // 默认隐藏

        layout->addWidget(colorPreview);
        layout->addWidget(textLabel);
        layout->addStretch();
        layout->addWidget(checkIcon);

        action->setData(i);
        action->setDefaultWidget(itemWidget);
        actionToCheckIconMap[action] = checkIcon; // 记录映射

        themeGroup->addAction(action);
        m_themeMenu->addAction(action);
    }

    connect(themeGroup, &QActionGroup::triggered, [=](QAction *selectedAction) {
        for (auto it = actionToCheckIconMap.begin(); it != actionToCheckIconMap.end(); ++it) {
            it.value()->setVisible(false);
        }
        actionToCheckIconMap[selectedAction]->setVisible(true);
    });

    connect(themeGroup, &QActionGroup::triggered, this, &Lyric::changeTheme);

    m_themeButton->setMenu(m_themeMenu);

    QAction* defaultAction = m_themeMenu->actions().at(3);
    defaultAction->setChecked(true);
    actionToCheckIconMap[defaultAction]->setVisible(true);

}



void Lyric::changeTheme(QAction *action)
{
    static const QVector<QPair<QColor, QColor>> themeColors = {
        {QColor("#2C3E50"), QColor("#3498DB")},
        {QColor("#4776E6"), QColor("#8E54E9")},
        {QColor("#FF512F"), QColor("#F09819")},
        {QColor("#1D976C"), QColor("#93F9B9")}
    };

    int index = action->data().toInt();
    switch (index) {
    case 0:
        ui->emoji->setIcon(QIcon(":/images/icons/alone.png"));
        break;
    case 1:
        ui->emoji->setIcon(QIcon(":/images/icons/romantic.png"));
        break;
     case 2:
        ui->emoji->setIcon(QIcon(":/images/icons/happy.png"));
        break;
     case 3:
        ui->emoji->setIcon(QIcon(":/images/icons/energy.png"));
        break;
    }

    if(index >= 0 && index < themeColors.size()) {
        setBackgroundGradient(themeColors[index].first, themeColors[index].second);
        updateSliderStyle(themeColors[index].first, themeColors[index].second);
    }
    bool isDark = m_bgColor1.lightness() < 128;
    QString borderColor = isDark ? "rgba(255,255,255,0.3)" : "rgba(0,0,0,0.3)";
    ui->cover->setStyleSheet(QString(R"(
           QLabel {
               border: 3px solid %1;
               border-radius: 200px;
               background-color: transparent;
           }
       )").arg(borderColor));


    QColor shadowColor;
    if(isDark) {
        shadowColor = QColor(255, 255, 255, 60);
    } else {
        shadowColor = QColor(0, 0, 0, 80);
    }

    m_coverShadow->setColor(shadowColor);

    QString borderColor2 = isDark ? "rgba(255,255,255,0.4)" : "rgba(0,0,0,0.4)";
    ui->cover->setStyleSheet(QString(R"(
        QLabel {
            border: 4px solid %1;
            border-radius: 150px;
            background-color: transparent;
        }
    )").arg(borderColor2));
    m_volumePopup->updatepopupTheme(QColor("#EBEBEB"));
}
void Lyric::applyAnimationToAllButtons() {

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
void Lyric::toggleMaximize()
{
    if (isMax) {
        emit minwindow();
        btnMax->setIcon(QIcon(":/images/image-hover/max.png"));
        isMax =false;
    } else {
        emit maxwindow();
        btnMax->setIcon(QIcon(":/images/image-hover/restore.png"));
        isMax = true;
    }
}
void Lyric::updatelyric(const QString& title1, const QString& lyricPath)
{
        QFileInfo file(lyricPath);

        QString title = file.completeBaseName();
        ui->title->setText("🎵 正在播放: " + title);
        ui->title->setGraphicsEffect(m_titleShadow);
        ui->title->setStyleSheet(R"(
                QLabel {
                    font: bold 28px 'Microsoft YaHei';
                    color: #FFFFFF;
                    padding: 8px 20px;
                    background-color: rgba(0,0,0,0.3);
                    border-radius: 15px;
                }
            )");
        loadLyric(title1, lyricPath);


        // 动态渐变效果
        QPropertyAnimation *fade = new QPropertyAnimation(ui->title, "color");
        fade->setDuration(500);
        fade->setStartValue(QColor("#FFFFFF"));
        fade->setEndValue(QColor("#FFD700"));
        fade->setEasingCurve(QEasingCurve::OutQuad);
        fade->start(QAbstractAnimation::DeleteWhenStopped);


//        QString coverPath = ("")/*findAlbumCover(lyricPath)*/;
//        QPixmap cover(coverPath.isEmpty() ? ":/images/image/non-music.png" : coverPath);
//        setAlbumCover(cover.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
