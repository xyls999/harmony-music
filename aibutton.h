#ifndef AIBUTTON_H
#define AIBUTTON_H

#include <QPushButton>
#include <QPoint>
#include <QDebug>
#include <QPainter>

class AIButton : public QPushButton
{
    Q_OBJECT
public:
    explicit AIButton(QWidget *parent = nullptr);
    ~AIButton();

signals:
    void clickedWithPosition(const QPoint &pos);

protected:
    void paintEvent(QPaintEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;

private:
    // 初始化样式
    void initStyle();
    // 初始化图标
    void initIcon();
};

#endif // AIBUTTON_H
