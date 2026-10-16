// playlistitemwidget.cpp
#include "playlistitemwidget.h"
#include <QPainter>
#include <QEnterEvent>

PlaylistItemWidget::PlaylistItemWidget(QWidget *parent)
    : QWidget(parent), m_bgColor(Qt::white)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_colorAnim = new QPropertyAnimation(this, "bgColor");
    m_colorAnim->setDuration(200);

    m_isDark = m_theme.isDarkTheme(m_theme.backgroundColor());

    initUI();
    updateThemeStyles();

    // 连接封面加载完成信号
    connect(CoverCacheManager::instance(), &CoverCacheManager::coverLoaded,
            this, &PlaylistItemWidget::updateCover);
}

PlaylistItemWidget::~PlaylistItemWidget()
{
    delete m_colorAnim;
}

void PlaylistItemWidget::initUI()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(12, 8, 12, 8);
    mainLayout->setSpacing(12);

    // 序号标签
    m_numLabel = new QLabel(this);
    m_numLabel->setFixedWidth(15);
    m_numLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_numLabel);

    // 封面
    m_coverLabel = new QLabel(this);
    m_coverLabel->setFixedSize(40, 40);
    m_coverLabel->setStyleSheet("border-radius: 4px;");
    m_coverLabel->setScaledContents(true);
    mainLayout->addWidget(m_coverLabel);

    // 文本信息
    QVBoxLayout *textLayout = new QVBoxLayout();
    textLayout->setSpacing(2);

    m_titleLabel = new QLabel(this);
    m_artistLabel = new QLabel(this);

    textLayout->addWidget(m_titleLabel);
    textLayout->addWidget(m_artistLabel);
    mainLayout->addLayout(textLayout, 1);

    // 时长
    m_durationLabel = new QLabel(this);
    m_durationLabel->setFixedWidth(60);
    m_durationLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    mainLayout->addWidget(m_durationLabel);

    // 设置透明
    for (auto child : findChildren<QWidget*>()) {
        child->setAttribute(Qt::WA_TranslucentBackground);
    }
}

void PlaylistItemWidget::updateThemeStyles()
{
    const QColor baseTextColor = m_isDark ? Qt::white : Qt::black;
    const QColor secondaryTextColor = m_isDark ? QColor("#A0A0A0") : QColor("#5f6368");

    m_numLabel->setStyleSheet(QString("font-size:13px;color:%1;").arg(secondaryTextColor.name()));
    m_durationLabel->setStyleSheet(QString("color:%1;").arg(secondaryTextColor.name()));

    setCurrentStyle(m_isCurrent);
}
void PlaylistItemWidget::setCurrentStyle(bool isCurrent)
{
    m_isCurrent = isCurrent;

    const QColor darkBg("#4D4D4D");
    const QColor lightBg("#EBEBEB");
    const QColor darkHighlight = m_theme.highlightColor();
    const QColor lightHighlight = m_theme.highlightColor();

    QColor targetBg;
    if (m_isCurrent) {
        targetBg = m_isDark ? darkHighlight.lighter(160) : lightHighlight.lighter(190);
    } else if (m_isHovered) {
        targetBg = m_isDark ? darkHighlight.lighter(120) : lightHighlight.lighter(150);
    } else {
        targetBg = m_isDark ? darkBg : lightBg;
    }

    const QColor titleColor = m_isCurrent ?
        (m_isDark ? Qt::white : Qt::black) :
        (m_isDark ? Qt::white : QColor("#202124"));

    const QColor artistColor = m_isCurrent ?
        QColor(titleColor.red(), titleColor.green(), titleColor.blue(), 150) :
        (m_isDark ? QColor("#A0A0A0") : QColor("#5f6368"));

    if (m_bgColor != targetBg) {
        m_colorAnim->stop();
        m_colorAnim->setStartValue(m_bgColor);
        m_colorAnim->setEndValue(targetBg);
        m_colorAnim->start();
    }

    m_titleLabel->setStyleSheet(
        QString("font-weight:%1;font-size:14px;color:%2;")
            .arg(m_isCurrent ? "600" : "500")
            .arg(titleColor.name()));

    m_artistLabel->setStyleSheet(
        QString("font-size:12px;color:%1;").arg(artistColor.name()));

    m_numLabel->setText(m_isCurrent ?
        QString("<font color='%1'>▶</font>").arg(m_isDark ? darkHighlight.name(): lightHighlight.name()) :
        QString("%1").arg(m_numLabel->text()));
}


QString PlaylistItemWidget::gettitle() const
{
    return m_title;
}

void PlaylistItemWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    painter.setBrush(m_bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect().adjusted(1,1,-1,-1), 6, 6);

    if (m_isCurrent) {
        painter.setBrush(m_isDark ? m_theme.highlightColor() :m_theme.highlightColor());
        painter.drawRoundedRect(0, 2, 3, height()-4, 1, 1);
    }

    painter.setPen(m_isDark ? QColor(255,255,255,30) : QColor(0,0,0,15));
    painter.drawLine(0, 0, width(), 0);
}
void PlaylistItemWidget::enterEvent(QEnterEvent *event)
{
    m_isHovered = true;
    setCurrentStyle(m_isCurrent);
    QWidget::enterEvent(event);
}
void PlaylistItemWidget::setTheme(bool isDark)
{
    if (m_isDark != isDark) {
        m_isDark = isDark;
        updateThemeStyles();
        update(); // 强制重绘
    }
}
void PlaylistItemWidget::leaveEvent(QEvent *event)
{
    m_isHovered = false;
    setCurrentStyle(m_isCurrent);
    QWidget::leaveEvent(event);
}

void PlaylistItemWidget::setData(const QPixmap &cover, const QString &title,
                                 const QString &artist, const QString &duration)
{
    // 使用歌曲标题作为缓存键
    QString cacheKey = title.toLower();

    QPixmap loadedCover = CoverCacheManager::instance()->getCover(cacheKey);

    if (loadedCover.isNull()) {
        // 如果没有缓存，加载默认封面
        m_coverLabel->setPixmap(QPixmap(":/images/image/non-music.png"));

        // 构建封面URL
        QString encodedName = QUrl::toPercentEncoding(title);
        QUrl coverUrl(QString("http://vip.frpee.com:13678/music/cover/%1.jpg").arg(encodedName));

        // 加载封面并更新UI
        connect(CoverCacheManager::instance(), &CoverCacheManager::coverLoaded,
                this, [this, cacheKey](const QString& key, const QPixmap& loadedCover) {
            if (key == cacheKey) {
                m_coverLabel->setPixmap(loadedCover);
            }
        });

        CoverCacheManager::instance()->loadCover(cacheKey, coverUrl);
    } else {
        // 使用缓存的封面
        m_coverLabel->setPixmap(loadedCover);
    }

    m_titleLabel->setText(title.isEmpty() ? tr("未知歌曲") : title);
    m_title = title;
    m_artistLabel->setText(artist.isEmpty() ? tr("未知歌手") : artist);
    m_durationLabel->setText(duration.isEmpty() ? "--:--" : duration);
}

QColor PlaylistItemWidget::bgColor() const
{
    return m_bgColor;
}

void PlaylistItemWidget::setBgColor(const QColor &color)
{
    if (m_bgColor != color) {
        m_bgColor = color;
        update();
    }
}

void PlaylistItemWidget::updateCover(const QString& key, const QPixmap& cover)
{
    if (m_title.toLower() == key) {
        m_coverLabel->setPixmap(cover);
    }
}
