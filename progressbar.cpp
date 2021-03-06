﻿#include "progressbar.h"
#include "ui_progressbar.h"

progressBar::progressBar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::progressBar)
{
    ui->setupUi(this);
    this->setMaximumSize(400,150);
    this->setMinimumSize(400,150);
    this->setWindowTitle(QString::fromLocal8Bit("任务进行中"));
    this->setWindowFlags(Qt::WindowStaysOnTopHint);//保持窗口最前
    this->setAttribute(Qt::WA_DeleteOnClose, true);
}

progressBar::~progressBar()
{
    delete ui;
}
void progressBar::setTotalRange(int startRange,int endRange)
{
    ui->proBar->setRange(startRange,endRange);
}
void progressBar::changeValue(int value)
{
    ui->proBar->setValue(value);
}
void progressBar::closeWindow()
{
    this->close();
}

