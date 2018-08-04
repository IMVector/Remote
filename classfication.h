#ifndef CLASSFICATION_H
#define CLASSFICATION_H

#include <QWidget>

namespace Ui {
class classfication;
}

class classfication : public QWidget
{
    Q_OBJECT

public:
    explicit classfication(QWidget *parent = 0);
    ~classfication();

private:
    Ui::classfication *ui;
};

#endif // CLASSFICATION_H
