#ifndef RRECOMMENDWIDGET_H
#define RRECOMMENDWIDGET_H

#include <QWidget>
#include "thememanager.h"
#include <QPushButton>
#include <QEvent>
#include <QScrollBar>
#include <QScrollArea>
#include <QTimer>
#include <QHBoxLayout>
#include <QDebug>
#include <QToolButton>
#include <QPointer>
#include "databasehandler.h"
#include <QPropertyAnimation>
#include "covercache.h"
#include "recommender.h"
namespace Ui {
class rrecommendWidget;
}

class rrecommendWidget : public QWidget
{
    Q_OBJECT

public:
    explicit rrecommendWidget(QWidget *parent = nullptr);
    ~rrecommendWidget();
    void updateButtonCover(const QString &key, const QPixmap &cover);
    void playSong(int songId);
    void updateTheme();
    void applyMusicStyle(QToolButton *button);
    void updateRecommendationUI(const QVector<int> &songIds);
    void initializeRecommendations();

private:
    Ui::rrecommendWidget *ui;
    void setupHoverButtons(QScrollArea *area, QPushButton *leftBtn, QPushButton *rightBtn);
    void scrollAreaBy(QScrollArea *area, int offset);
    ThemeManager& m_theme = ThemeManager::instance();
    QVector<int> recommendedSongIds;
    Recommender *recommender;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // RRECOMMENDWIDGET_H
