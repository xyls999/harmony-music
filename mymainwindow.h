#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include <DMainWindow>
#include <DWidget>
#include <DTitlebar>
#include <DStackedWidget>
#include <DFileDialog>
#include <QDebug>
#include <DStatusBar>
#include <QTimer>
#include <DMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QLayout>
#include <QLabel>
#include <DPushButton>
#include <QToolButton>
#include <QStackedWidget>
#include "footwidget.h"
#include "titlebarwidget.h"
#include <QPropertyAnimation>
#include <QMouseEvent>
#include "mainpage.h"
#include "lyric.h"
#include "databasehandler.h"
#include "thememanager.h"
#include "covercache.h"
#include "videoplayer.h"
#include "recommender.h"
DWIDGET_USE_NAMESPACE

namespace Ui {
class myMainWindow;
}

class myMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit myMainWindow(DatabaseHandler *db, QWidget *parent = nullptr);
    ~myMainWindow();
    void init_UI();//ui之外的处理
     void onThemeChanged();
private slots:
    void toggleWindowMode(); // 窗口模式切换

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    Ui::myMainWindow *ui;
    footWidget *m_footWidget;
    Lyric* m_lyric;

    QStackedWidget *centerwidget;
    mainpage *m_mainWidget;
    DTitlebar *m_titlebar;
    TitleBarWidget *m_TitleBarWidget;
    DStatusBar *m_statusbar;
    QWidget *m_normalLayout;
    QWidget* m_lyricContainer;
    ThemeManager& m_theme = ThemeManager::instance();

    void setupLyricMode();   // 歌词模式
    void restoreNormalMode();// 正常模式
    void enterLyricMode();
    void exitLyricMode();

    QWidget* m_normalWidget;
    VideoPlayer *m_videoWidget;
    QWidget *m_videoContainer ;

    bool m_isLyricMode = false;
    QPoint m_dragPosition;
    bool m_isDragging = false;
    DatabaseHandler *m_dbHandler;
    Recommender *recommender;

};

#endif // MYMAINWINDOW_H
