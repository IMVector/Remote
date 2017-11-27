#ifndef ZONEDEAL_H
#define ZONEDEAL_H

#include <QObject>
#include <filedeal.h>
#include <QtCore>
typedef struct
{
    unsigned short int R;
    unsigned short int G;
    unsigned short int B;
}s_color;
typedef struct
{
    int startX;//入口x
    int startY;//入口y
    int number;//点的数量
    unsigned short int colorTh;//第n种地物
    int cycle;//一个区域的周长
    long int averageID;//为一个区域设置唯一标识
}everyLandNum;
typedef struct{
    point *p;//一个区域的点的集合
    int number;//一个区域点的数量
}Area;

class zonedeal:public QObject
{
    Q_OBJECT
public:
    zonedeal();
private:
    void main(QImage image);
    everyLandNum *countEveryNumber(unsigned short *imageArray, int Samples, int Lines);
    Area pointIterator(unsigned short *imageArray, int Samples, int Lines, everyLandNum nodeinfo);
    void linjie(unsigned short *imageArray, int Samples, int Lines);
    void getRALLink(unsigned short *imageArray, int Samples, int Lines, everyLandNum nodeinfo);
    int calAreaCycle(unsigned short *imageArray, int Samples, int Lines, Area stake, everyLandNum nodeinfo);
};

#endif // ZONEDEAL_H
