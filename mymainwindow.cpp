#include "mymainwindow.h"

myMainWindow::myMainWindow(DatabaseHandler *db,QWidget *parent)
    : QMainWindow(parent), m_lyric(new Lyric(this)),
      m_normalWidget(new QWidget(this)),
      m_lyricContainer(new QWidget(this)),m_dbHandler(db)
{

    m_dbHandler = db;
    recommender = new Recommender(m_dbHandler);

   setWindowFlags(Qt::FramelessWindowHint);

    setAttribute(Qt::WA_TranslucentBackground, true);
    this->setStyleSheet("background: transparent;");
    m_footWidget = new footWidget(this,m_lyric);
    init_UI();
    connect(m_footWidget, &footWidget::switchlyric,
            this, &myMainWindow::toggleWindowMode);
    connect(m_lyric, &Lyric::switchnormal,
            this, &myMainWindow::toggleWindowMode);
    connect(m_lyric,&Lyric::maxwindow,this,&myMainWindow::showMaximized);
    connect(m_mainWidget,&mainpage::maxwindow,this,&myMainWindow::showMaximized);
    connect(m_mainWidget,&mainpage::minwindow,this,&myMainWindow::showMinimized);
    connect(m_lyric,&Lyric::minwindow,this,&myMainWindow::showNormal);
    connect(&m_theme, &ThemeManager::themeChanged,
                this, &myMainWindow::onThemeChanged);
        m_theme.loadSettings();


}
myMainWindow::~myMainWindow()
{
}
void myMainWindow::init_UI()
{
    setMinimumSize(1600, 1000);
    // 正常模式
    QVBoxLayout* mainLayout = new QVBoxLayout(m_normalWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    m_mainWidget = new mainpage(m_dbHandler,m_normalWidget, m_footWidget);
    mainLayout->addWidget(m_mainWidget, 1);

    // 歌词模式
    m_lyricContainer->setParent(this);
    QVBoxLayout* lyricLayout = new QVBoxLayout(m_lyricContainer);
    lyricLayout->setContentsMargins(0, 0, 0, 0);
    lyricLayout->addWidget(m_lyric);
    m_lyricContainer->hide();          // 初始隐藏


    m_footWidget->setMinimumHeight(130);
    QStatusBar* statusBar = this->statusBar();
    statusBar->setSizeGripEnabled(false);
    statusBar->setStyleSheet("background: transparent;");
    statusBar->addWidget(m_footWidget,1);

    centerwidget = new QStackedWidget(this);
    centerwidget->addWidget(m_normalWidget);
    centerwidget->addWidget(m_lyricContainer);
    setCentralWidget(centerwidget);
    centerwidget->setCurrentWidget(m_normalWidget);

}
void myMainWindow::onThemeChanged() {
    // 更新状态栏背景色
    QStatusBar* statusBar = this->statusBar();
    statusBar->setStyleSheet(
        QString("QStatusBar { background-color: %1; }")
            .arg(m_theme.backgroundColor().name())
    );

}
void myMainWindow::toggleWindowMode()
{
    m_isLyricMode = !m_isLyricMode;

    if (m_isLyricMode) {
        enterLyricMode();
    } else {
        exitLyricMode();
    }
}

void myMainWindow::enterLyricMode()
{
    statusBar()->hide();

    const int w = width();
    const int h = height();

    m_lyricContainer->setGeometry(0, h, w, h);
    m_lyricContainer->show();
    centerwidget->setCurrentWidget(m_lyricContainer);
    m_normalWidget->hide();

    QPropertyAnimation *animation = new QPropertyAnimation(m_lyricContainer, "geometry");
    animation->setDuration(100);
    animation->setStartValue(QRect(0, h, w, h));
    animation->setEndValue(QRect(0, 0, w, h));
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void myMainWindow::exitLyricMode()
{
    const int w = width();
    const int h = height();

    QPropertyAnimation *animation = new QPropertyAnimation(m_lyricContainer, "geometry");
    animation->setDuration(100);

    animation->setStartValue(QRect(0, 0, w, h));
    animation->setEndValue(QRect(0, h, w, h));

    animation->setEasingCurve(QEasingCurve::InCubic);
    connect(animation, &QPropertyAnimation::finished, [=]() {
        m_lyricContainer->hide();
        statusBar()->show();
        onThemeChanged();
        centerwidget->setCurrentWidget(m_normalWidget);
        m_normalWidget->show();
    });

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void myMainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        m_isDragging = true;
        event->accept();
    }
    QMainWindow::mousePressEvent(event);  // 保持原有事件传递
}

void myMainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        QPoint newPos = event->globalPos() - m_dragPosition;
        move(newPos);
        event->accept();
    }
    QMainWindow::mouseMoveEvent(event);
}

void myMainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        event->accept();
    }
    QMainWindow::mouseReleaseEvent(event);
}
