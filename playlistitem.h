#ifndef PLAYLISTITEM_H
#define PLAYLISTITEM_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include "themes.h"

class PlaylistItem : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistItem(QWidget *parent = nullptr) : QWidget(parent), currentTheme(&Theme::Light)
    {
        // 主布局设置
        QHBoxLayout *mainLayout = new QHBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);  // 消除边距
        mainLayout->setSpacing(8);                   // 控件间距

        // 序号标签
        lblNumber = new QLabel(this);
        lblNumber->setFixedWidth(30);
        lblNumber->setAlignment(Qt::AlignCenter);
        lblNumber->setStyleSheet(Theme::Light.PlaylistItemNumber);


        // 封面标签
        lblCover = new QLabel(this);
        lblCover->setFixedSize(80, 50);
        lblCover->setStyleSheet(Theme::Light.PlaylistItemCover);

        // 文字区域（标题）
        QWidget *textArea = new QWidget(this);
        textArea->setStyleSheet("background: transparent;");
        QVBoxLayout *textLayout = new QVBoxLayout(textArea);
        textLayout->setContentsMargins(0, 0, 0, 0);

        lblTitle = new QLabel(textArea);
        lblTitle->setFixedHeight(60);  // 确保高度足够
        lblTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        lblTitle->setStyleSheet(Theme::Light.PlaylistItemTitle);

        // 时间标签
        lblDuration = new QLabel(this);
        lblDuration->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        lblDuration->setStyleSheet(Theme::Light.PlaylistItemDuration);

        // 删除按钮
        btnDelete = new QPushButton(this);
        btnDelete->setFixedSize(24, 24);
        btnDelete->setStyleSheet(Theme::Light.PlaylistItemDeleteBtn);
        btnDelete->setText("×");
        btnDelete->setVisible(false);

        // 组装布局
        textLayout->addWidget(lblTitle);
        textLayout->addStretch();

        mainLayout->addWidget(lblNumber);
        mainLayout->addWidget(lblCover);
        mainLayout->addWidget(textArea, 1);  // 文字区域扩展填充
        mainLayout->addWidget(lblDuration);
        mainLayout->addWidget(btnDelete);

        connect(btnDelete, &QPushButton::clicked, this, &PlaylistItem::deleteRequested);
    }

    void setSelected(bool selected)
    {
        const Theme::Style& theme = *currentTheme; // 使用成员变量

        if (selected) {
            this->setStyleSheet(theme.PlaylistItemSelected);
            lblNumber->setStyleSheet("color: white; background: transparent;");
            lblTitle->setStyleSheet("color: white; background: transparent;");
            lblDuration->setStyleSheet("color: white; background: transparent;");
            btnDelete->setVisible(true);
        } else {
            this->setStyleSheet(theme.PlaylistItemNormal);
            lblNumber->setStyleSheet(theme.PlaylistItemNumber);
            lblTitle->setStyleSheet(theme.PlaylistItemTitle);
            lblDuration->setStyleSheet(theme.PlaylistItemDuration);
            btnDelete->setVisible(false);
        }
    }

    void updateTheme(const Theme::Style& theme) {
        currentTheme = &theme; // 更新当前主题
        if (this->styleSheet().contains("background: #2196F3")) {
            // 当前是选中状态
            this->setStyleSheet(theme.PlaylistItemSelected);
            lblNumber->setStyleSheet("color: white; background: transparent;");
            lblTitle->setStyleSheet("color: white; background: transparent;");
            lblDuration->setStyleSheet("color: white; background: transparent;");
        } else {
            // 普通状态
            this->setStyleSheet(theme.PlaylistItemNormal);
            lblNumber->setStyleSheet(theme.PlaylistItemNumber);
            lblTitle->setStyleSheet(theme.PlaylistItemTitle);
            lblDuration->setStyleSheet(theme.PlaylistItemDuration);
        }
        lblCover->setStyleSheet(theme.PlaylistItemCover);
        btnDelete->setStyleSheet(theme.PlaylistItemDeleteBtn);
    }
    void setIndex(int index)
    {
        // 更新界面显示的序号
        lblNumber->setText(QString::number(index));
    }

    void setData(int number, const QString &title, const QString &duration, const QPixmap &cover)
    {
        lblNumber->setText(QString::number(number));
        lblTitle->setText(title);
        lblDuration->setText(duration);
        lblCover->setPixmap(cover.scaled(80, 50, Qt::KeepAspectRatioByExpanding));
    }
    QString getTitle() const { return lblTitle->text(); }
    void setDisabled(bool disabled)
    {
        lblDuration->setEnabled(!disabled);
        lblNumber->setEnabled(!disabled);
        lblCover->setEnabled(!disabled);
    }

    void setSubtitle(const QString& text)
    {
        lblTitle->setText(text);
    }


signals:
    void deleteRequested();
    void deleteConfirmed();

private:
    QLabel *lblNumber;
    QLabel *lblCover;
    QLabel *lblTitle;
    QLabel *lblDuration;
    QPushButton *btnDelete;
    const Theme::Style* currentTheme = &Theme::Light; // 默认浅色主题

};

#endif // PLAYLISTITEM_H
