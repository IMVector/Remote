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
    int startX;
    int startY;
    int number;
    unsigned short int colorTh;
}everyLandNum;

class zonedeal:public QObject
{
    Q_OBJECT
public:
    zonedeal();
private:
    void main(QImage image);
    everyLandNum *countEveryNumber(unsigned short *imageArray, int Samples, int Lines);
    point *pointIterator(unsigned short *imageArray, int Samples, int Lines, everyLandNum nodeinfo);
    void linjie(unsigned short *imageArray, int Samples, int Lines);
};

#endif // ZONEDEAL_H
