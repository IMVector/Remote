#include "MyLabel.h"
#include<qdebug.h>
#include<QLabel>
#include<QMouseEvent>
int labelHeight;
int labelWidth;
int sizeChangeCount=0;//记录窗口大小改变次数
MyLabel::MyLabel(QWidget *parent) :

    QLabel(parent)

{
    time=new QTimer(this);
    connect(time,SIGNAL(timeout()),this,SLOT(updateSize()));
    this->setWindowTitle(QStringLiteral("图像"));
}
void MyLabel::mousePressEvent(QMouseEvent *ev)//获取略缩图的鼠标位置
{
    int x0=ev->x();
    int y0=ev->y();

    if(ev->button()==Qt::LeftButton)
    {
        //触发鼠标按下信号记录鼠标选择的点的坐标
        emit sendlocation(x0,y0);
    }
    //鼠标右键点击事件
    if(ev->button()==Qt::RightButton)
    {
        emit currentOver();
        //右键启动输出选择的区域
    }
}

void MyLabel::updateSize()
{
    //    qInfo()<<labelHeight;
    //    qInfo()<<labelWidth;
    emit sendLabelSize(labelHeight,labelWidth);
    if(time->isActive())
    {
        time->stop();
    }
}

void MyLabel::resizeEvent(QResizeEvent *event)
{

    if(sizeChangeCount!=0)
    {
        time->start(1000);
    }
    sizeChangeCount++;
    labelHeight=event->size().height();
    labelWidth=event->size().width();
}


