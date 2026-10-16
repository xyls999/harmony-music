#include "chatapp.h"
#include <QDesktopWidget> // 用于获取屏幕几何信息（适用于 Qt5）
#include <QScreen>        // 用于获取屏幕几何信息（适用于 Qt6）
#include <QLabel>
#include <QHBoxLayout>
#include <QPixmap>
#include <QMouseEvent>
#include <QApplication>

ChatApp::ChatApp(QWidget *parent)
    : QWidget(parent)
{
    // 设置窗口标题和大小
    setWindowTitle("AI 按钮窗口");
    resize(1024, 768); // 修改主窗口大小为 1024x768（您可以根据需要调整）
    setFixedSize(1024, 768); // 固定窗口大小
    // 将窗口居中显示
    centerWindow();

    // 创建一个按钮用于打开聊天窗口
    openChatWindowButton = new QPushButton(this);
    openChatWindowButton->setFixedSize(40, 40); // 设置按钮为正方形

    // 设置按钮为圆形并添加背景图片
    openChatWindowButton->setStyleSheet(
        "QPushButton {"
        "   border: none;" // 去掉边框
        "   border-radius: 40px;" // 圆形半径为宽度的一半
        "   background-color: transparent;" // 背景透明
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(0, 0, 0, 50);" // 悬停时添加轻微背景色
        "}"
    );

    // 设置按钮图标（从资源文件加载）
    QIcon chatIcon(":/icons/ai.png"); // 替换为您的资源路径
    openChatWindowButton->setIcon(chatIcon);
    openChatWindowButton->setIconSize(QSize(30, 30)); // 设置图标大小

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 添加一个空白占位符，将按钮推到右下角
    mainLayout->addStretch(); // 占位符，占据剩余空间

    // 右下角布局
    QHBoxLayout *bottomRightLayout = new QHBoxLayout();
    bottomRightLayout->addStretch(); // 左侧占位符
    bottomRightLayout->addWidget(openChatWindowButton, 0, Qt::AlignBottom | Qt::AlignRight);

    // 将右下角布局添加到主布局
    mainLayout->addLayout(bottomRightLayout);

    // 连接按钮点击信号到槽函数
    connect(openChatWindowButton, &QPushButton::clicked, this, &ChatApp::onOpenChatWindowClicked);
}

void ChatApp::centerWindow() {
    // 获取主窗口的尺寸
    QRect windowGeometry = frameGeometry();

    // 获取屏幕中心点
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QScreen *screen = QGuiApplication::primaryScreen();
    QPoint centerPoint = screen->availableGeometry().center();
#else
    QDesktopWidget *desktop = QApplication::desktop();
    QPoint centerPoint = desktop->availableGeometry().center();
#endif

    // 计算窗口左上角的位置
    windowGeometry.moveCenter(centerPoint);
    move(windowGeometry.topLeft());
}

void ChatApp::onOpenChatWindowClicked() {
    // 创建聊天窗口
    MainWindow *chatWindow = new MainWindow(); // 动态分配内存
    chatWindow->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动释放内存

    // 获取按钮的位置并计算弹出窗口的位置
    QPoint buttonPos = openChatWindowButton->mapToGlobal(QPoint(0, 0)); // 获取按钮的全局位置
    int windowWidth = 436; // 弹出窗口的宽度
    int windowHeight = 700; // 弹出窗口的高度

    // 计算弹出窗口的位置（在按钮上方）
    int x = buttonPos.x() - windowWidth;
    int y = buttonPos.y() - windowHeight;

    // 设置弹出窗口的位置和大小
    chatWindow->setGeometry(x, y, windowWidth, windowHeight);
    chatWindow->show();
}
