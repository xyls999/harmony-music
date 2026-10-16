#ifndef SCROLLABEL_H
#define SCROLLABEL_H

#include <QWidget>

namespace Ui {
class ScrolLabel;
}

class ScrolLabel : public QWidget
{
    Q_OBJECT

public:
    explicit ScrolLabel(QWidget *parent = nullptr);
    ~ScrolLabel();

private:
    Ui::ScrolLabel *ui;
};

#endif // SCROLLABEL_H
