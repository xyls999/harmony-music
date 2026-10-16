
#include <QLabel>
#include <QTimer>
#include <QApplication>

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickableLabel(const QString& text = "", QWidget* parent = nullptr);

signals:
    void clicked();
    void doubleClicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    QTimer* m_clickTimer;
};
