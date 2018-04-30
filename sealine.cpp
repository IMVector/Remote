#include "sealine.h"
#include "ui_sealine.h"

#include <qlistview.h>
int *seaColor=new int[5];
int *landColor=new int[5];
sealine::sealine(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sealine)
{
    ui->setupUi(this);

    this->setMaximumSize(360,205);
    this->setMinimumSize(360,205);
    this->setWindowTitle(QStringLiteral("提取海岸线"));
    initStyle();
}

sealine::~sealine()
{
    delete ui;
    delete[]seaColor;
    delete[]landColor;
}

void sealine::initStyle()
{
    ui->comboBox->setStyleSheet("QComboBox QAbstractItemView::item{height:20px;}");
    ui->comboBox_2->setStyleSheet("QComboBox QAbstractItemView::item{height:20px;}");
    ui->comboBox_3->setStyleSheet("QComboBox QAbstractItemView::item{height:20px;}");
    ui->comboBox_4->setStyleSheet("QComboBox QAbstractItemView::item{height:20px;}");
    ui->comboBox_5->setStyleSheet("QComboBox QAbstractItemView::item{height:20px;}");
    ui->comboBox_6->setStyleSheet("QComboBox QAbstractItemView::item{height:20px;}");
    ui->comboBox_7->setStyleSheet("QComboBox QAbstractItemView::item{height:20px;}");
    ui->comboBox_8->setStyleSheet("QComboBox QAbstractItemView::item{height:20px;}");
    ui->comboBox_9->setStyleSheet("QComboBox QAbstractItemView::item{height:20px;}");
    ui->comboBox_10->setStyleSheet("QComboBox QAbstractItemView::item{height:20px;}");
    ui->comboBox->setView(new QListView());
    ui->comboBox_2->setView(new QListView());
    ui->comboBox_3->setView(new QListView());
    ui->comboBox_4->setView(new QListView());
    ui->comboBox_5->setView(new QListView());
    ui->comboBox_6->setView(new QListView());
    ui->comboBox_7->setView(new QListView());
    ui->comboBox_8->setView(new QListView());
    ui->comboBox_9->setView(new QListView());
    ui->comboBox_10->setView(new QListView());
}
void sealine::on_pushButton_clicked()//开始寻找海岸线返回陆地所有颜色和海洋所有颜色
{
    int *seaColor=this->getSeaColor();
    int *landColor=this->getLandColor();
    emit getSealine(seaColor,landColor);
}
int* sealine:: getSeaColor()
{
    return seaColor;
}
int* sealine:: getLandColor()
{
    return landColor;
}
void sealine::on_comboBox_currentIndexChanged(int index)//选择海的颜色
{
    seaColor[0]=index;
}

void sealine::on_comboBox_2_currentIndexChanged(int index)//选择海的颜色
{
    seaColor[1]=index;
}

void sealine::on_comboBox_3_currentIndexChanged(int index)//选择海的颜色
{
    seaColor[2]=index;
}

void sealine::on_comboBox_4_currentIndexChanged(int index)//选择海的颜色
{
    seaColor[3]=index;
}

void sealine::on_comboBox_9_currentIndexChanged(int index)//选择海的颜色
{
    seaColor[4]=index;
}

void sealine::on_comboBox_7_currentIndexChanged(int index)//选择陆地颜色
{
    landColor[0]=index;
}

void sealine::on_comboBox_6_currentIndexChanged(int index)//选择陆地颜色
{
    landColor[1]=index;
}

void sealine::on_comboBox_5_currentIndexChanged(int index)//选择陆地颜色
{
    landColor[2]=index;
}

void sealine::on_comboBox_8_currentIndexChanged(int index)//选择陆地颜色
{
    landColor[3]=index;
}

void sealine::on_comboBox_10_currentIndexChanged(int index)//选择陆地颜色
{
    landColor[4]=index;
}
