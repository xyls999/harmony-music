#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QGraphicsVideoItem>
#include <QTimer>
#include <QTime>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QStyle>
#include <QMouseEvent>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QListWidgetItem>
#include <QSqlError>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QVideoFrame>
#include <QVideoProbe>
#include <QScrollBar>
#include <QMenu>
#include <QScreen>
#include <QLineEdit>
#include <QStackedWidget>
#include <QProgressDialog>

#include "playlistitem.h"
#include "themes.h"

QT_BEGIN_NAMESPACE
namespace Ui { class VideoPlayer; }
QT_END_NAMESPACE

class VideoPlayer : public QMainWindow
{
    Q_OBJECT

public:
    VideoPlayer(QWidget *parent = nullptr);
    ~VideoPlayer();
     void updateSongCount();
     void loadFromDatabase(QListWidget *list);
public slots:
     void onPlaylistItemDoubleClicked(QListWidgetItem *item);
private:
    // UI Components
    Ui::VideoPlayer *ui;
    QWidget *playlistPanel;
    QListWidget *listWidget;
    QWidget *historyPanel;
    QListWidget *historyListWidget;
    QLabel *songCountLabel;
    QLabel *historyLabel;
    QPushButton *clearAllButton;
    QPushButton *clearHistoryBtn;
    QSlider *volumeSlider;
    QLabel* volumeLabel;
    QMenu* speedMenu;
    QMenu* themeMenu;
    QMenu *mainMenu;
    QLineEdit *searchEdit;

    // Media Components
    QMediaPlayer *player;
    QGraphicsVideoItem *videoItem;
    QImage firstFrame;
    QVideoProbe *videoProbe;

    // Animation & Timing
    QPropertyAnimation *panelAnimation;
    QPropertyAnimation *historyPanelAnimation;
    QPropertyAnimation *slideAnimation;
    QTimer* clickTimer;

    // Database
    QSqlDatabase database;

    // State Variables
    bool isMaximized = false;
    bool frameCaptured = false;
    bool isPlaylistVisible = false;
    bool isHistoryVisible = false;
    bool m_isDragging = false;
    bool m_dragging = false;
    bool isMuted = false;
    bool sliderVisible = false;
    int clickCount = 0;
    int currentPlayIndex = -1;
    int currentPlaylistIndex = 0;
    int lastVolume = 50;
    QString durationTime;
    QString positionTime;
    QString currentTheme = "浅色主题";
    QPoint m_dragPosition;
    QPoint m_mousePressPos;
    QRect m_windowRectOnPress;
    QList<QPair<QString, QListWidgetItem*>> originalPlaylist;

    // Enums
    enum PlayMode {
        Sequence,
        Loop,
        Random
    };
    PlayMode currentPlayMode = Sequence;

    enum ThemeMode {
        SystemTheme,
        LightTheme,
        DarkTheme
    };

    enum ResizeEdge {
        None,
        Left, Right, Top, Bottom,
        TopLeft, TopRight, BottomLeft, BottomRight
    };
    ResizeEdge m_resizeEdge = None;

    // Initialization Methods
    void initPlaylistUI();
    void initHistoryUI();
    void initVolumeControl();
    void initThemeSystem();
    void createMenu();
    void setupThemeConnections();

    // UI Update Methods
    void updateVideoSize();
    void updateMaxIcon(bool maximized);
    void showControls();
    void updatePlaylistNumbers();
    void updatePlaylistItemWidths();

    void updateHistoryItemWidths();
    void updatePlayModeIcon();
    void updateCursorShape(const QPoint& pos);

    // Database Operations
    void saveToDatabase(const QString &path, const QString &title, qint64 duration);
    void deleteFromDatabase(const QString &path);
    void loadHistoryFromDatabase();

    // Playlist Operations
    void addFilesToPlaylist(const QStringList &files);
    void clearAllPlaylistItems();
    void deletePlaylistItem(QListWidgetItem *item);

    // Theme Management
    void applyTheme();
    void applyTheme(const QString& theme);
    void applyCurrentTheme();
    void applyProgressDialogStyle(QProgressDialog &progress);
    QString loadThemePreference();
    QString loadThemeFromDatabase();
    void saveThemePreference(const QString &theme);
    QString getIconPath(const QString& iconName) const;

    // Event Handlers
    bool eventFilter(QObject *watched, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    bool event(QEvent *event) override;

    // Utility Methods
    void togglePlaylistVisibility();
    void toggleHistoryVisibility();
    void toggleFullscreen();
    void toggleVolumeSlider(bool show);
    void closeAllPanels();
    void closeOtherPanels(QWidget* currentPanel);
    void setPlaybackSpeed(float speed);
    void showStyledMessageBox(const QString &title, const QString &message);
    void showAboutDialog();
    void onSearchTextChanged(const QString &text);
    bool isVideoAvailable(const QString& filePath) const;
    void captureFirstFrame(const QVideoFrame &frame);

private slots:
    // Media Control Slots
    void onDurationChanged(qint64 duration);
    void onPositionChanged(qint64 position);
    void onVolumeChanged(int value);
    void onMetaDataAvailable(bool available);

    // Button Click Handlers
    void on_pushButton_Add_clicked();
    void on_pushButton_Player_clicked();
    void on_pushButton_Sound_clicked();
    void on_pushButton_Min_clicked();
    void on_pushButton_Close_clicked();
    void on_pushButton_list_clicked();
    void on_pushButton_Next_clicked();
    void on_pushButton_Pre_clicked();

    // Playlist Interaction
    void onHistoryItemDoubleClicked(QListWidgetItem *item);
};

#endif // VIDEOPLAYER_H
