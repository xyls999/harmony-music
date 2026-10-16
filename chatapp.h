#ifndef CHATAPP_H
#define CHATAPP_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include "mainwindow.h" // 包含 MainWindow 的头文件

class ChatApp : public QWidget {
    Q_OBJECT

public:
    explicit ChatApp(QWidget *parent = nullptr);

private slots:
    void onOpenChatWindowClicked();

private:
    QPushButton *openChatWindowButton;
    void centerWindow();
};

#endif // CHATAPP_H
