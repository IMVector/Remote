#ifndef LOGINFO_H
#define LOGINFO_H

#include <QWidget>
#include <QtCore>
#include <QMessageBox>
#include <QCloseEvent>

namespace Ui {
class Loginfo;
}

class Loginfo : public QWidget
{
    Q_OBJECT

public:
    explicit Loginfo(QWidget *parent = 0);
    ~Loginfo();

signals:
    void sendMsg(QString msg);
private:
    Ui::Loginfo *ui;
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
private slots:
    void showLog(QString msg);
};

#endif // LOGINFO_H
