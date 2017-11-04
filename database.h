#ifndef DATABASE_H
#define DATABASE_H

#include <QWidget>
#include <qdebug.h>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QTableWidgetItem>
#include <QListView>
namespace Ui {
class database;
}

class database : public QWidget
{
    Q_OBJECT

public:
    explicit database(QWidget *parent = 0);
    ~database();
    void OpenDatabase(QString database, QString url, QString databaseName, QString userName, QString password);

    void Setting(QString database, QString url, QString databseName, QString userName, QString password);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_tableWidget_clicked(const QModelIndex &index);
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_comboBox_currentIndexChanged(int index);

signals:
    void sendImageName(QString imageName);
    void sendRule(QString imageName);
    void sendSeaLine(QString seaLine);
//    void sendQuality(QString quality);

private:
    Ui::database *ui;
    void warningMessage();
    void deleteFile(QString name);
};

#endif // DATABASE_H
