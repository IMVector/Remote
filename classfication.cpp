#include "classfication.h"
#include "ui_classfication.h"

classfication::classfication(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::classfication)
{
    ui->setupUi(this);
}

classfication::~classfication()
{
    delete ui;
}
