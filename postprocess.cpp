#include "postprocess.h"


PostProcess::PostProcess(QObject *parent) : QObject(parent)
{

}
//void PostProcess::getRoundCircle(int x,int y)
//{
//    int ridius=100;
//    for(int w=x-50;w<x+50;w++)
//    {
//        for(int h=y-50;h<y+50;h++)
//        {
//            point point1;
//            point point2;
//            point1.x=x;
//            point1.y=y;
//            point2.x=w;
//            point2.y=h;
//            if(distance(point1,point2)<=100)

//        }
//    }
//}


//void PostProcess::fillHoll(QImage image)
//{
//    for(int w=0;w<image.width();w++)
//    {
//        for(int h=0;h<image.height();h++)
//        {

//        }
//    }


//}

///**
// * @brief PostProcess::distance 计算两点之间距离
// * @param p1
// * @param p2
// * @return
// */
//float PostProcess::distance(point p1, point p2)
//{
//    return sqrt(((p1.x - p2.x)*(p1.x - p2.x)) + ((p1.y - p2.y)*(p1.y - p2.y)));
//}
