#include "dbsetting.h"
#include "ui_dbsetting.h"

dbsetting::dbsetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dbsetting)
{
    ui->setupUi(this);
    this->setWindowTitle(QString::fromLocal8Bit("数据库设置"));
}

dbsetting::~dbsetting()
{
    delete ui;
}
/**
 * @brief dbsetting::on_confirmBtn_clicked
 * 确认按钮相应事件
 */
void dbsetting::on_confirmBtn_clicked()
{
    QString database="QODBC";
    switch(ui->comboBox->currentIndex())
    {
    case 0:
        database="QODBC";
        break;
    case 1:
        database="QMYSQL";
        break;
    case 2:
        database="QSQLITE";
        break;
    }
    QString url=ui->urlEdit->text();
    QString dbname=ui->dbNameEdit->text();
    QString uname=ui->UNameEdit->text();
    QString password=ui->passwordEdit->text();
    emit sendDBSetting(database,url,dbname,uname,password);
    this->close();
}
/**
 * @brief dbsetting::on_cleraBtn_clicked
 * 清除按钮相应事件
 */
void dbsetting::on_cleraBtn_clicked()
{
    ui->urlEdit->setText("")  ;
    ui->dbNameEdit->setText("")  ;
    ui->UNameEdit->setText("")  ;
    ui->passwordEdit->setText("")  ;
}


