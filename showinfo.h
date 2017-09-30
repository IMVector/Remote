#ifndef SHOWINFO_H
#define SHOWINFO_H

#include <QDialog>

namespace Ui {
class showInfo;
}

class showInfo : public QDialog
{
    Q_OBJECT

public:
    explicit showInfo(QWidget *parent = 0);
    ~showInfo();
    void setMessage(QString message, int type);

private:
    Ui::showInfo *ui;

};

#endif // SHOWINFO_H
