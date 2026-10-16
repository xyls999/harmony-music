#ifndef SCROLLLABEL_H
#define SCROLLLABEL_H

#include <QLabel>
#include <QTimer>

class ScrollLabel : public QLabel {
    Q_OBJECT
public:
    explicit ScrollLabel(QWidget* parent = nullptr);

    void setText(const QString &text);
    void setScrollEnabled(bool enable);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void updateText();

private:
    void startScrolling();
    void stopScrolling();
    void checkTextLength();

    int m_offset = 0;
    int m_maxOffset = 0;
    QString m_fullText;
    QTimer* m_timer;
    bool m_needScroll = false;
};

#endif // SCROLLLABEL_H
