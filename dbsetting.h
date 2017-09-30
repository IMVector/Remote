#ifndef DBSETTING_H
#define DBSETTING_H

#include <QDialog>

namespace Ui {
class dbsetting;
}

class dbsetting : public QDialog
{
    Q_OBJECT

public:
    explicit dbsetting(QWidget *parent = 0);
    ~dbsetting();
signals:
    void sendDBSetting(QString database,QString url,QString databseName,QString userName,QString password);
private slots:
    void on_confirmBtn_clicked();
    void on_cleraBtn_clicked();

private:
    Ui::dbsetting *ui;

};

#endif // DBSETTING_H
