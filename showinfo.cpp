#include "showinfo.h"
#include "ui_showinfo.h"

showInfo::showInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::showInfo)
{
    ui->setupUi(this);
    //设置窗体关闭时自动释放内存
    this->setAttribute(Qt::WA_DeleteOnClose);
    connect(ui->confirmBtn,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->cancleBtn,SIGNAL(clicked()),this,SLOT(close()));
}

showInfo::~showInfo()
{
    delete ui;
}
void showInfo::setMessage(QString message, int type)
{
    if (type == 0) {
        ui->labIcoMain->setStyleSheet("border-image: url(:/image/info.png);");
        ui->cancleBtn->setVisible(false);
        this->setWindowTitle("提示");

    } else if (type == 1) {
        ui->labIcoMain->setStyleSheet("border-image: url(:/image/question.png);");
        this->setWindowTitle("询问");
    } else if (type == 2) {
        ui->labIcoMain->setStyleSheet("border-image: url(:/image/error.png);");
        ui->cancleBtn->setVisible(false);
        this->setWindowTitle("错误");
    }

    ui->labInfo->setText(message);
}
