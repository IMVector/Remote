#pragma once
#ifndef MYLABEL_H
#define MYLABEL_H

#include <QDialog>
#include <QThread>
#include <QLabel>
#include <QMainWindow>
#include <QTimer>
#include <QPaintEvent>
#include <QPainter>
class MyLabel : public QLabel

{
    Q_OBJECT

public:
    explicit MyLabel(QWidget * parent = 0);

private:
    QTimer *time;

    int img_width;//缩略图宽
    int img_height;//缩略图高


    void mousePressEvent(QMouseEvent *ev);//重写鼠标按下事件
    void resizeEvent(QResizeEvent *event);//窗口大小事件
private slots:
    void updateSize();//触发后每一秒更新一次大小

signals:
    void currentOver();
    void sendlocation(int x,int y);//发送鼠标位置
    void sendLabelSize(int labelHeight, int labelWidth);//窗口改变大小时发送大小


};

#endif // MYLABEL_H
