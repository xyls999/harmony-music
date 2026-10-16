#include <DApplication>
#include <DMainWindow>
#include <DWidgetUtil>
#include <DApplicationSettings>
#include <DTitlebar>
#include <DProgressBar>
#include <DFontSizeManager>

#include <QPropertyAnimation>
#include <QDate>
#include <QLayout>
#include <QFontDatabase>
#include "mymainwindow.h"
#include "scrolllabel.h"
#include "lyric.h"
#include "videoplayer.h"
#include "thememanager.h"
#include "databasehandler.h"
#include "recommender.h"
DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    DApplication a(argc, argv);

    a.loadTranslator();

    DatabaseHandler *m_dbHandler = new DatabaseHandler();
    QString host = "vip.frpee.com";      // 修改为你的MySQL服务器地址
    int port = 13654;
    QString dbName = "music";             // 修改为你的数据库名
    QString user = "root";               // 修改为你的MySQL用户名
    QString password = "1234";           // 修改为你的密码

    if (!m_dbHandler->initDatabase(host, dbName, user, password, port)) {
       qInfo()<<"数据库初始化失败";
            }

   myMainWindow *w = new myMainWindow(m_dbHandler);
   w->show();

    ThemeManager::instance().loadSettings();

    int fontId = QFontDatabase::addApplicationFont(":/images/ai/Alibaba-PuHuiTi-Regular.ttf");
    QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QFont customFont(fontFamily, 12);
    a.setFont(customFont);

    a.setStyleSheet(
        "QMessageBox {"
        "    background-color: #fafafa;"
        "    color: #222;"
        "    font-size: 12px;"
        "    border: 1px solid #ccc;"
        "    border-radius: 10px;"
        "    padding: 10px;"
        "}"
        "QMessageBox QPushButton {"
        "    background-color: #007BFF;"
        "    color: white;"
        "    border-radius: 5px;"
        "    padding: 8px 16px;"
        "    font-size: 14px;"
        "}"
        "QMessageBox QPushButton:hover {"
        "    background-color: #0056b3;"
        "}"
        "QMessageBox QPushButton:pressed {"
        "    background-color: #003f87;"
        "}"
    );


    Dtk::Widget::moveToCenter(w);

    return a.exec();
}
