#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QDialog>
#include <QtCore>
#include <QProgressBar>
#include <QDialog>
#include <QtGui>


namespace Ui {
class progressBar;
}

class progressBar : public QDialog
{
    Q_OBJECT

public:
    explicit progressBar(QWidget *parent = 0);
    ~progressBar();

private slots:
    void setTotalRange(int startRange, int endRange);
    void changeValue(int value);
    void closeWindow();
private:
    Ui::progressBar *ui;
};

#endif // PROGRESSBAR_H
