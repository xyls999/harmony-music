#include "rrecommendwidget.h"
#include "ui_rrecommendwidget.h"

rrecommendWidget::rrecommendWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::rrecommendWidget)
{
    ui->setupUi(this);
//    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded); // 开启纵向滚动


    bool isdark = m_theme.isDarkTheme(m_theme.backgroundColor());
    QString iconPrefix = isdark ? "white" : "black";
    QList<QPushButton*> allBtns = {ui->moodleft, ui->moodright, ui->musicleft, ui->musicright, ui->typeright, ui->typeleft};
       for (QPushButton* btn : allBtns) {
           btn->setStyleSheet(QString("QPushButton { background-color:transparent; border-radius: 15px; }"));
           btn->hide(); // 初始隐藏
       }
   QList<QToolButton*> musicbtns = {ui->Pop,ui->Blues,ui->Classical,ui->Country,ui->Disco,ui->Hiphop,ui->Jazz,ui->Metal,ui->Regga,ui->Rock};
   for (QToolButton *musicbtn : musicbtns){
       applyMusicStyle(musicbtn);
       qInfo()<<musicbtn->objectName();
   }
   initializeRecommendations();

    ui->moodleft->setIcon(QIcon(QString(":/images/theme/left-%1.png").arg(iconPrefix)));
    ui->musicleft->setIcon(QIcon(QString(":/images/theme/left-%1.png").arg(iconPrefix)));
    ui->typeleft->setIcon(QIcon(QString(":/images/theme/left-%1.png").arg(iconPrefix)));
    ui->moodright->setIcon(QIcon(QString(":/images/theme/right-%1.png").arg(iconPrefix)));
    ui->musicright->setIcon(QIcon(QString(":/images/theme/right-%1.png").arg(iconPrefix)));
    ui->typeright->setIcon(QIcon(QString(":/images/theme/right-%1.png").arg(iconPrefix)));

    setupHoverButtons(ui->musicScroll, ui->musicleft, ui->musicright);
    setupHoverButtons(ui->moodScroll, ui->moodleft, ui->moodright);
    setupHoverButtons(ui->typeScroll, ui->typeleft, ui->typeright);

     connect(CoverCacheManager::instance(), &CoverCacheManager::coverLoaded, this, &rrecommendWidget::updateButtonCover);

}
void rrecommendWidget::initializeRecommendations() {
    QVector<int> recommendedSongIds = recommender->generateRecommendations(1, 5, 0.1);
    hide(); // 初始隐藏
    updateRecommendationUI(recommendedSongIds);

    bool allCoversLoaded = true;
    for (int songId : recommendedSongIds) {
        QString key = QString::number(songId);
        if (CoverCacheManager::instance()->getCover(key).isNull()) {
            allCoversLoaded = false;
            QString title = recommender->getTitleBySongId(songId);
            QUrl coverUrl(QString("http://vip.frpee.com:13678/music/cover/%1.jpg").arg(title));

            CoverCacheManager::instance()->loadCover(title, coverUrl);
        }
    }

    if (allCoversLoaded) {
        show();
    } else {
        connect(CoverCacheManager::instance(), &CoverCacheManager::coverLoaded, this,
            [this, recommendedSongIds](const QString &key, const QPixmap &cover) {
                Q_UNUSED(key);
                Q_UNUSED(cover);
                // 检查是否所有封面加载完成
                bool allLoaded = true;
                for (int songId : recommendedSongIds) {
                    QString currentKey = recommender->getTitleBySongId(songId);
                    if (CoverCacheManager::instance()->getCover(currentKey).isNull()) {
                        allLoaded = false;
                        break;
                    }
                }
                if (allLoaded) {
                    show(); // 全部加载完成后显示
                    disconnect(CoverCacheManager::instance(), nullptr, this, nullptr); // 断开连接
                }
            }
        );
    }
}


void rrecommendWidget::updateButtonCover(const QString &key, const QPixmap &cover) {
    foreach (QPushButton *button, ui->recommendationLayout->findChildren<QPushButton*>()) {
        if (button->property("songId").toString() == key) {
            if (!cover.isNull()) {
                button->setIcon(QIcon(cover));
                button->setIconSize(QSize(100, 100));
            } else {
                // 加载失败时设置占位图
                button->setIcon(QIcon(":/images/recon/loading.png"));
            }
        }
    }
}
void rrecommendWidget::playSong(int songId) {
    qInfo() << "Playing song with ID: " << songId;
    // 根据 songId 播放歌曲
}

void rrecommendWidget::updateTheme()
{
    QString scrollBarStyle = R"(
    QScrollArea {
        border: none;
    }
    QScrollBar:vertical {
        width: 10px;
        background: #f0f0f0;
        margin: 0px;
        border-radius: 5px;
    }
    QScrollBar::handle:vertical {
        background: #a0a0a0;
        border-radius: 5px;
        min-height: 30px;
    }
    QScrollBar::handle:vertical:hover {
        background: #888888;
    }
    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
        height: 0px;
        background: none;
    }
    QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
        background: none;
    }

    QScrollBar:horizontal {
        height: 0px;
        background: transparent;
    }
    )";
    ui->scrollArea->setStyleSheet(scrollBarStyle);

}

void rrecommendWidget::applyMusicStyle(QToolButton *button) {
    QColor bgcolor = m_theme.backgroundColor();
    QColor highlightColor = m_theme.highlightColor();

    // 根据按钮类型设置颜色（示例）
    if (button->objectName() == "Rock") {
        bgcolor = QColor("#ff0000");
        highlightColor = QColor("#ff7f7f");
    } else if (button->objectName() == "Jazz") {
        bgcolor = QColor("#5c2d91");
        highlightColor = QColor("#a64ac9");
    }

    // 使用 QStringLiteral 避免字符串拼接错误
    QString musicstyle = QStringLiteral(
        "QToolButton {"
        "  background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 %1, stop:1 %2);" // 修正渐变语法
        "  border-radius: 25px;"
        "  padding: 10px;"
        "  color: #333333;"
        "  font-size: 20px;"
        "  qproperty-iconSize: 50px 50px;" // 修正属性名称为 iconSize
        "  qproperty-toolButtonStyle: ToolButtonTextUnderIcon;" // 确保枚举值正确
        "}"
        "QToolButton:hover {"
        "  background-color: %3;"
        "  font-weight: bold;"
        "  color: #000000;"
        "}"
        "QToolButton:pressed {"
        "  background-color: %3;"
        "}"
    ).arg(bgcolor.name(),
          highlightColor.name(),
          bgcolor.darker(110).name());

    button->setStyleSheet(musicstyle);
}

void rrecommendWidget::updateRecommendationUI(const QVector<int> &songIds) {
    // 清空旧内容
    QLayoutItem *item;
    while ((item = ui->recommendationLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // 为每个推荐创建按钮
    for (int songId : songIds) {
        QString key = QString::number(songId);
        QPixmap cover = CoverCacheManager::instance()->getCover(key);

        QPushButton *btn = new QPushButton();
        btn->setProperty("songId", key); // 存储标识用于后续更新

        // 设置按钮样式
        btn->setIconSize(QSize(100, 100));
        btn->setStyleSheet("...你的样式...");

        // 如果封面已存在则设置，否则等待加载
        if (!cover.isNull()) {
            btn->setIcon(QIcon(cover));
        } else {
            btn->setIcon(QIcon()); // 清空图标
        }

        // 添加点击事件
        connect(btn, &QPushButton::clicked, this, [this, songId]() {
            playSong(songId);
        });

        ui->recommendationLayout->addWidget(btn);
    }
}
void rrecommendWidget::setupHoverButtons(QScrollArea *area, QPushButton *leftBtn, QPushButton *rightBtn)
{

    connect(leftBtn, &QPushButton::clicked, this, [=]() {
        scrollAreaBy(area, -200); // 向左滚动200
    });

    connect(rightBtn, &QPushButton::clicked, this, [=]() {
        scrollAreaBy(area, 200); // 向右滚动200
    });
    area->installEventFilter(this);
    leftBtn->installEventFilter(this);
    rightBtn->installEventFilter(this);
}

void rrecommendWidget::scrollAreaBy(QScrollArea *area, int offset)
{
    QScrollBar *bar = area->horizontalScrollBar();
    int startValue = bar->value();
    int endValue = startValue + offset;

    QPropertyAnimation *animation = new QPropertyAnimation(bar, "value");
    animation->setDuration(200);
    animation->setStartValue(startValue);
    animation->setEndValue(endValue);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}
bool rrecommendWidget::eventFilter(QObject *obj, QEvent *event)
{
    auto showBtns = [&](QPushButton *left, QPushButton *right, bool show) {
        if (show) {
            left->show();
            right->show();
        } else {
            QTimer::singleShot(150, this, [=]() {
                QWidget *commonParent = left->parentWidget();
                if (!left->underMouse() && !right->underMouse() &&
                    !commonParent->underMouse()) {
                    left->hide();
                    right->hide();
                }
            });
        }
    };
    // 封装一个检测区域的函数
    auto handleHover = [&](QScrollArea *area, QPushButton *left, QPushButton *right) {
        bool inArea = area->underMouse();
        bool inLeft = left->underMouse();
        bool inRight = right->underMouse();
        showBtns(left, right, inArea || inLeft || inRight);
    };

    // 检查是哪个区域或按钮
    if (obj == ui->musicScroll || obj == ui->musicleft || obj == ui->musicright) {
        if (event->type() == QEvent::Enter || event->type() == QEvent::Leave) {
            handleHover(ui->musicScroll, ui->musicleft, ui->musicright);
        }
    } else if (obj == ui->moodScroll || obj == ui->moodleft || obj == ui->moodright) {
        if (event->type() == QEvent::Enter || event->type() == QEvent::Leave) {
            handleHover(ui->moodScroll, ui->moodleft, ui->moodright);
        }
    } else if (obj == ui->typeScroll || obj == ui->typeleft || obj == ui->typeright) {
        if (event->type() == QEvent::Enter || event->type() == QEvent::Leave) {
            handleHover(ui->typeScroll, ui->typeleft, ui->typeright);
        }
    }

    return QWidget::eventFilter(obj, event);
}

rrecommendWidget::~rrecommendWidget()
{
    delete ui;
}
