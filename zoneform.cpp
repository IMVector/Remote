#include "zoneform.h"
#include "ui_zoneform.h"

Zoneform::Zoneform(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Zoneform)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("空间格局"));//基于空间格局的降噪
    connect(ui->startBtn,SIGNAL(clicked()),this,SLOT(startChange()));
    connect(ui->openFileBtn,SIGNAL(clicked()),this,SLOT(openFile()));
}

Zoneform::~Zoneform()
{
    delete ui;
}
void Zoneform::startChange()
{
    loadInfo info;
    info.deleteThresould=ui->deleteThresould->text().toInt();
    info.adjIntensity=ui->adjIntensity->text().toFloat();
    info.disThresould=ui->disThresould->text().toInt();
    info.changedTh=ui->changedTh->text().toInt();
    info.geoTh=ui->geoTh->text().toInt();
    qDebug()<<info.adjIntensity<<info.changedTh<<info.deleteThresould<<info.disThresould<<info.geoTh;
    emit zoneInfoSender(info);

}
void Zoneform::openFile()
{
    QString file = QFileDialog::getOpenFileName(
                this,
                "Open Document",
                QDir::currentPath(),
                "All files(*.*);;*");
    if (!file.isNull())
    { //用户选择了文件
        emit fileSender(file);
    }
    else
    {
        QMessageBox::information(this, "Warning",QStringLiteral("       你未选中任何文件        "), QMessageBox::Ok);
    }

}
