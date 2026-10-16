#include "musicitemwidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>


MusicItemWidget::MusicItemWidget(QWidget *parent)
    : QWidget(parent), m_bgColor(Qt::white), m_songId(-1) // 初始化歌曲ID为-1
{
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_colorAnim = new QPropertyAnimation(this, "bgColor");
    m_colorAnim->setDuration(200);

    initWidgets();

    m_highlightColor = m_theme.highlightColor().lighter(150);

    this->setCursor(Qt::PointingHandCursor);

    connect(likeBtn,&QPushButton::clicked,this,&MusicItemWidget::onLikeClicked);
}

void MusicItemWidget::initWidgets()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(12, 8, 12, 8);
    mainLayout->setSpacing(12);

    // 序号标签
    numLabel = new QLabel();
    numLabel->setFixedWidth(30);
    numLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(numLabel);

    // 封面
    coverLabel = new QLabel();
    coverLabel->setFixedSize(40, 40);
    coverLabel->setScaledContents(true);
    coverLabel->setStyleSheet("QLabel { border: none; border-radius: 20px; }");
    mainLayout->addWidget(coverLabel);

    // 文本信息
    QVBoxLayout *textLayout = new QVBoxLayout();
    titleLabel = new QLabel();
    titleLabel->setStyleSheet("font-weight: 600; font-size: 14px; color: #202124;");
    artistLabel = new QLabel();
    artistLabel->setStyleSheet("font-size: 12px; color: #5f6368;");
    textLayout->addWidget(titleLabel);
    textLayout->addWidget(artistLabel);
    mainLayout->addLayout(textLayout, 1);

    // 专辑
    albumLabel = new QLabel();
    albumLabel->setStyleSheet("font-size: 13px; color: #5f6368;");
    mainLayout->addWidget(albumLabel, 2);

    // 喜欢按钮
    likeBtn = new QPushButton();
    likeBtn->setFixedSize(24, 24);
    likeBtn->setCursor(Qt::PointingHandCursor);
    likeBtn->setCheckable(true);
    likeBtn->setStyleSheet(
        "QPushButton { border: none; image: url(:/images/image/non-like.png); background: transparent;}"
        "QPushButton:checked { image: url(:/images/image/like.png); }"
    );

//    // 时长
//    durationLabel = new QLabel();
//    durationLabel->setStyleSheet("color: #5f6368;");
//    durationLabel->setFixedWidth(50);
//    mainLayout->addWidget(durationLabel);
    // 添加播放列表按钮
      addToPlaylistButton = new QPushButton(this);
      addToPlaylistButton->setIcon(QIcon(":/images/theme/addlist-black.png"));
      addToPlaylistButton->setFlat(true);
      addToPlaylistButton->setFixedSize(30, 30);
      addToPlaylistButton->setCursor(Qt::PointingHandCursor);

      // 初始状态隐藏
      addToPlaylistButton->setVisible(false);

      // 设置样式表使按钮透明
      addToPlaylistButton->setStyleSheet(
          "QPushButton { background: transparent; border: none; }"
          "QPushButton:hover { background: rgba(0, 0, 0, 0.1); border-radius: 4px; }"
      );

      mainLayout->addWidget(addToPlaylistButton);
      mainLayout->addItem(new QSpacerItem(100, 0, QSizePolicy::Fixed, QSizePolicy::Minimum));
      mainLayout->addWidget(likeBtn);
      mainLayout->addItem(new QSpacerItem(50, 0, QSizePolicy::Fixed, QSizePolicy::Minimum));

      // 安装事件过滤器
      this->setAttribute(Qt::WA_Hover);
      this->installEventFilter(this);

    connect(addToPlaylistButton, &QPushButton::clicked, this, &MusicItemWidget::onAddToPlaylistClicked);

    menu = new QMenu(this);

    // 设置透明
    for (auto child : findChildren<QWidget*>()) {
        child->setAttribute(Qt::WA_TranslucentBackground);
        child->setAutoFillBackground(false);
    }

}
bool MusicItemWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == this) {
        switch (event->type()) {
        case QEvent::HoverEnter:
            addToPlaylistButton->setVisible(true);
            break;
        case QEvent::HoverLeave:
            addToPlaylistButton->setVisible(false);
            break;
        default:
            break;
        }
    }
    return QWidget::eventFilter(obj, event);
}
void MusicItemWidget::highlightText(const QString& keyword)
{
    m_highlightText = keyword;
    update();
}
void MusicItemWidget::drawHighlightedText(QPainter* painter, QLabel* label)
{
    QString text = label->text();
    if(text.isEmpty() || m_highlightText.isEmpty()) return;

    QPoint labelPos = label->mapTo(this, QPoint(0, 0));

    painter->setFont(label->font());
    QFontMetrics fm(label->font());
    int textWidth = fm.horizontalAdvance(text);
    int textHeight = fm.height();
    int yPos = labelPos.y() + (label->height() - textHeight)/2 + fm.ascent();

    int pos = 0;
    while((pos = text.indexOf(m_highlightText, pos, Qt::CaseInsensitive)) != -1) {

        int highlightWidth = fm.horizontalAdvance(text.mid(pos, m_highlightText.length()));
        int xPos = labelPos.x() + fm.horizontalAdvance(text.left(pos));

        painter->fillRect(xPos, yPos - fm.ascent(),
                         highlightWidth, textHeight,
                         m_highlightColor);

        pos += m_highlightText.length();
    }

    painter->setPen(label->palette().text().color());
    painter->drawText(labelPos.x(), yPos, text);
}
void MusicItemWidget::highlightInLabel(QLabel* label, const QString& keyword)
{
    QString text = label->text();
    if(text.isEmpty() || keyword.isEmpty()) return;

    label->setText(highlightMatch(text, keyword));
}

QString MusicItemWidget::highlightMatch(const QString& text, const QString& keyword)
{
    QString escapedText = text.toHtmlEscaped(); // 替代 Qt::escape

    if(keyword.isEmpty()) return escapedText;


    QRegularExpression regex(QRegularExpression::escape(keyword),
                           QRegularExpression::CaseInsensitiveOption);

    return escapedText.replace(regex,
        QString("<span style='"
               "background-color:#FFF176;"
               "color:black;"
               "border-radius:2px;"
               "padding:0 2px;"
               "'>\\1</span>"));
}
void MusicItemWidget::onAddToPlaylistClicked()
{
    qDebug() << "添加";
    emit onAddToPlaylistRequested(m_songId);
}
void MusicItemWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);


    QColor baseColor = m_themeColor.isValid() ? m_themeColor : Qt::white;
    bool isDarkTheme = baseColor.lightness() < 128;

    QColor bgColor = isDarkTheme ? baseColor.lighter(110) : baseColor.darker(105);
    if (m_state != Normal) {
        bgColor = m_bgColor;
    }

    // 创建渐变色
    QLinearGradient gradient(0, 0, width(), 0);
    gradient.setColorAt(0, bgColor.lighter(110));
    gradient.setColorAt(1, bgColor.darker(110));

    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 6, 6);

    // 绘制分割线
    painter.setPen(QColor(0, 0, 0, isDarkTheme ? 30 : 15));
    painter.drawLine(0, 0, width(), 0);

    drawHighlightedText(&painter, titleLabel);
    drawHighlightedText(&painter, artistLabel);
    drawHighlightedText(&painter, albumLabel);
}


void MusicItemWidget::setThemeColor(const QColor& themeColor) {
    m_themeColor = themeColor;
    bool isDarkTheme = themeColor.lightness() < 128;

    // 更新文本颜色
    QString textColor = isDarkTheme ? "#E0E0E0" : "#202124";
    QString secondaryColor = isDarkTheme ? "#A0A0A0" : "#5f6368";

    titleLabel->setStyleSheet(QString("font-weight: 600; font-size: 14px; color: %1;").arg(textColor));
    artistLabel->setStyleSheet(QString("font-size: 12px; color: %1;").arg(secondaryColor));
    albumLabel->setStyleSheet(QString("font-size: 13px; color: %1;").arg(secondaryColor));

    // 更新图标
    QString iconPrefix = isDarkTheme ? "white" : "black";
    likeBtn->setStyleSheet(
        QString(
            "QPushButton {"
            "   border: none;"
            "   image: url(:/images/theme/non-like-%1.png);"
            "   background: transparent;"
            "}"
            "QPushButton:checked {"
            "   image: url(:/images/theme/like-%1.png);"
            "}"
        ).arg(iconPrefix)
    );

    addToPlaylistButton->setStyleSheet(
        QString(
            "QPushButton {"
            "   border: none;"
            "   image: url(:/images/theme/addlist-%1.png);"
            "   background: transparent;"
            "}"
            "QPushButton:hover {"
            "   background: rgba(%2, %3, %4, 0.1);"
            "   border-radius: 4px;"
            "}"
        ).arg(iconPrefix)
        .arg(isDarkTheme ? "255" : "0")
        .arg(isDarkTheme ? "255" : "0")
        .arg(isDarkTheme ? "255" : "0")
    );

    m_highlightColor = m_theme.highlightColor();

    // 强制重绘
    update();
}

QUrl MusicItemWidget::getFileUrl() const
{
    return m_isCloud ?
        QUrl("http://vip.frpee.com:13678/music/audio/" + m_filePath) :
        QUrl::fromLocalFile(m_filePath);
}


QColor MusicItemWidget::bgColor() const {
    return m_bgColor;
}


void MusicItemWidget::setBgColor(const QColor &color)
{
    if (m_bgColor != color) {
        m_bgColor = color;
        update();
    }
}


void MusicItemWidget::setHighlighted(bool highlighted, bool isPlaying) {
    m_isPlaying = isPlaying;
    m_state = isPlaying ? Playing : (highlighted ? Hovered : Normal);

    QColor highlightColor = m_theme.highlightColor();
    bool isDarkTheme = m_themeColor.lightness() < 128;

    // 字体设置（保留优化）
    const QString fontFamily = "Microsoft YaHei";
    const int fontSize = 12;
    const int playingFontSize = 14;


    QColor highlightedBgColor= isDarkTheme ? highlightColor.lighter(150) : highlightColor.lighter(180);
    QColor playingBgColor = isDarkTheme ? highlightColor.lighter(160) : highlightColor.lighter(190);
    QColor normalBgColor = isDarkTheme ? m_themeColor.lighter(130) : QColor("#F5F5F5");

    QColor playingTextColor = isDarkTheme ? Qt::white : Qt::black;
    QColor highlightedTextColor = isDarkTheme ? Qt::white : Qt::black;
    QColor normalTextColor = isDarkTheme ? Qt::white : Qt::black;

    if (m_state == Playing) {
        // 播放状态 - 恢复之前的背景色
        m_bgColor = playingBgColor;
        numLabel->setText(QString("<font color='%1' size='+1'>▶</font>")
                         .arg(playingTextColor.name()));

        this->setStyleSheet(QString(
            "MusicItemWidget {"
            "   border: 2px solid %1;"
            "   border-radius: 4px;"
            "   background: %2;"
            "}"
            "QLabel {"
            "   font-family: %3;"
            "   font-size: %4px;"
            "   font-weight: bold;"
//            "   color: %5;"
            "}"
        ).arg(highlightColor.darker(120).name(),
              m_bgColor.name(),
              fontFamily,
              QString::number(playingFontSize)
/*              playingTextColor.name()*/));

        QGraphicsOpacityEffect *pulseEffect = new QGraphicsOpacityEffect(this);
        pulseEffect->setOpacity(0.9);
        this->setGraphicsEffect(pulseEffect);

        QPropertyAnimation *pulse = new QPropertyAnimation(pulseEffect, "opacity");
        pulse->setDuration(1200);
        pulse->setStartValue(0.9);
        pulse->setEndValue(0.7);
        pulse->setEasingCurve(QEasingCurve::InOutSine);
        pulse->setLoopCount(-1);
        pulse->start();

    } else if (highlighted) {
        m_bgColor = highlightedBgColor;
        numLabel->setText(QString("%1").arg(m_index+1, 2, 10, QLatin1Char('0')));

        this->setStyleSheet(QString(
            "MusicItemWidget {"
            "   background: %1;"
            "   border-radius: 4px;"
            "}"
            "QLabel {"
            "   font-family: %2;"
            "   font-size: %3px;"
            "   font-weight: 600;"
            "   color: %4;"
            "}"
        ).arg(m_bgColor.name(),
              fontFamily,
              QString::number(fontSize),
              highlightedTextColor.name()));
        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
        shadow->setBlurRadius(10);
        shadow->setColor(highlightColor);
        shadow->setOffset(2, 2);
        this->setGraphicsEffect(shadow);

    } else {
        // 默认状态
        m_bgColor = normalBgColor;
        numLabel->setText(QString("%1").arg(m_index+1, 2, 10, QLatin1Char('0')));

        this->setStyleSheet(QString(
            "MusicItemWidget { background: transparent; }"
            "QLabel {"
            "   font-family: %1;"
            "   font-size: %2px;"
            "   color: %3;"
            "}"
        ).arg(fontFamily,
              QString::number(fontSize),
              normalTextColor.name()));
         this->setGraphicsEffect(nullptr);
    }

    // 强制更新样式
    style()->unpolish(this);
    style()->polish(this);
    update();
}

void MusicItemWidget::setLiked(bool like)
{
    if (m_liked != like) {
        QSignalBlocker blocker(likeBtn);
        likeBtn->setChecked(like);
        m_liked = like;
    }
}

void MusicItemWidget::updateData(int index, const QPixmap &cover, const QString &title, const QString &artist,
                                 const QString &album, const QString &duration, bool liked,
                                 int songId, const QString &filePath, bool is_cloud)
{

    // 更新序号标签
    numLabel->setText(QString("%1").arg(index + 1, 2, 10, QLatin1Char('0')));
    // 更新封面图片
    coverLabel->setPixmap(cover.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    // 更新歌曲标题
    titleLabel->setTextFormat(Qt::RichText);
    titleLabel->setText(title.isEmpty() ? "未知歌曲" : title.toHtmlEscaped());
    // 更新歌手名称
    artistLabel->setTextFormat(Qt::RichText);
    artistLabel->setText(artist.isEmpty() ? "未知歌手" : artist.toHtmlEscaped());
    // 更新专辑名称
    albumLabel->setTextFormat(Qt::RichText);
    albumLabel->setText(album.isEmpty() ? "未知专辑" : album.toHtmlEscaped());
//    // 更新歌曲时长
//    durationLabel->setText(duration.isEmpty() ? "00:00" : duration);
    // 更新喜欢按钮状态
    likeBtn->setChecked(liked);
    // 存储歌曲信息
    m_index = index;
    m_title = title;
    m_artist = artist;
    m_songId = songId;
    m_cover = cover;
    m_filePath = filePath;
    m_isCloud = is_cloud;

    }

void MusicItemWidget::enterEvent(QEvent *event)
{
    setHighlighted(true, m_isPlaying);
    QWidget::enterEvent(event);
}

void MusicItemWidget::leaveEvent(QEvent *event)
{
    setHighlighted(false, m_isPlaying);
    QWidget::leaveEvent(event);
}

void MusicItemWidget::onLikeClicked(bool checked) {
    m_liked = checked;
    likeBtn->setChecked(checked);
    emit liked(checked, m_songId); // 确保m_songId已正确设置

    qInfo() << "发射liked信号 songId:" << m_songId << "状态:" << checked;
}


void MusicItemWidget::updateCover(const QPixmap &cover)
{
    coverLabel->setPixmap(cover.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_cover = cover;
    qDebug()<<cover;
    update(); // 触发重绘
}
