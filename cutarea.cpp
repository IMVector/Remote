#include "cutarea.h"



CutArea::CutArea(QObject *parent) : QObject(parent)
{

}
void CutArea::mCutArea(QImage image_,int radius,int firstMixColor,int secondMixColor)
{
    image=image_;
    cut(image_,radius,firstMixColor,secondMixColor);
    QImage image;
    image.load("D://dealComplete.tif");
    expansion(image,firstMixColor,secondMixColor);

}
void CutArea::expansion(QImage image,int firstMixColor,int secondMixColor)
{
    int R[7]={255,   255,  255,    0,   0,   139,0};
    int G[7]={255,     0,  255,    0,   255, 0 ,0};
    int B[7]={255,     0  ,  0,  255,   0,   139   ,0};
    int next[8][2] = { {0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1} };
    int xgo[80] = {-4,-4,-4,-4,-4,-4,-4,-4,-4,-3,-3,-3,-3,-3,-3,-3,-3,-3,-2,-2,-2,-2,-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,
                   1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4};
    int ygo[80] = {-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,1,2,3,4,-4,-3,-2,-1,0,1,
                   2,3,4,-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,0,1,2,3,4};

    int foot=8;//侵蚀距离
    int footmax=80;//不允许侵蚀的步长与xgo和ygo一起使用
    int radius=80;//不允许侵蚀的距离
    QRgb value=qRgb(255,0,0);
    int width=image.width();
    int height=image.height();

    qDebug()<<QStringLiteral("腐蚀开始");
    for(int y=0;y<height;y++)
    {
        for(int x=0;x<width;x++)
        {
            if(QColor(image.pixel(x,y)).red()==255&&
                    QColor(image.pixel(x,y)).green()==0&&
                    QColor(image.pixel(x,y)).blue()==0)
            {

                for(int i=0;i<foot;i++)
                {
                    if(x+i<width&&x+i>=0&&y+i<height&&y+i>=0)
                        if(QColor(image.pixel(x+next[i][0],y+next[i][1])).red()==R[secondMixColor]&&
                                QColor(image.pixel(x+next[i][0],y+next[i][1])).green()==G[secondMixColor]&&
                                QColor(image.pixel(x+next[i][0],y+next[i][1])).blue()==B[secondMixColor])
                        {

                            bool flag=false;
                            for(int i=-radius;i<radius;i++)
                            {
                                for(int j=-radius;j<radius;j++)
                                {
                                    if(x+i<width&&x+i>=0&&y+j<height&&y+j>=0)
                                        if(QColor(image.pixel(x+i,y+j)).red()==R[firstMixColor]&&
                                                QColor(image.pixel(x+i,y+j)).green()==G[firstMixColor]&&
                                                QColor(image.pixel(x+i,y+j)).blue()==B[firstMixColor])
                                        {
                                            flag=true;
                                            break;
                                        }

                                }
                            }
                            if(!flag)
                            {
                                for(int i=0;i<foot;i++)
                                {
                                    if(x+i<width&&x+i>=0&&y+i<height&&y+i>=0)
                                        if(QColor(image.pixel(x+next[i][0],y+next[i][1])).red()==R[secondMixColor]&&
                                                QColor(image.pixel(x+next[i][0],y+next[i][1])).green()==G[secondMixColor]&&
                                                QColor(image.pixel(x+next[i][0],y+next[i][1])).blue()==B[secondMixColor])
                                        {
                                            if(x+next[i][0]>=0&&x+next[i][0]<width&&y+next[i][1]>=0&&y+next[i][1]<height)
                                                image.setPixel(x+next[i][0],y+next[i][1],value);
                                        }
                                }
                            }
                        }
                }


                //                bool flag=false;

                //                for(int i=0;i<footmax;i++)
                //                {
                //                    if(x+xgo[i]<width&&x+xgo[i]>=0&&y+ygo[i]<height&&y+ygo[i]>=0)
                //                    if(QColor(image.pixel(x+xgo[i],y+ygo[i])).red()==R[firstMixColor]&&
                //                            QColor(image.pixel(x+xgo[i],y+ygo[i])).green()==G[firstMixColor]&&
                //                            QColor(image.pixel(x+xgo[i],y+ygo[i])).blue()==B[firstMixColor])
                //                    {
                //                        flag=true;
                //                        break;
                //                    }
                //                }
                //                if(!flag)
                //                {
                //                    for(int i=0;i<foot;i++)
                //                    {
                //
                //                        if(QColor(image.pixel(x+next[i][0],y+next[i][1])).red()==R[secondMixColor]&&
                //                                QColor(image.pixel(x+next[i][0],y+next[i][1])).green()==G[secondMixColor]&&
                //                                QColor(image.pixel(x+next[i][0],y+next[i][1])).blue()==B[secondMixColor])
                //                        {
                //                            image.setPixel(x+next[i][0],y+next[i][1],value);
                //                        }
                //                    }
                //                }


            }

        }
    }
    image.save("D://dealComplete1.tif");
    qDebug()<<QStringLiteral("腐蚀开始50%");
    for(int y=height;y>=0;y--)
    {
        for(int x=width;x>=0;x--)
        {
            if(QColor(image.pixel(x,y)).red()==255&&
                    QColor(image.pixel(x,y)).green()==0&&
                    QColor(image.pixel(x,y)).blue()==0)
            {

                for(int i=0;i<foot;i++)
                {
                    if(x+i<width&&x+i>=0&&y+i<height&&y+i>=0)
                        if(QColor(image.pixel(x+next[i][0],y+next[i][1])).red()==R[secondMixColor]&&
                                QColor(image.pixel(x+next[i][0],y+next[i][1])).green()==G[secondMixColor]&&
                                QColor(image.pixel(x+next[i][0],y+next[i][1])).blue()==B[secondMixColor])
                        {

                            bool flag=false;
                            for(int i=-radius;i<radius;i++)
                            {
                                for(int j=-radius;j<radius;j++)
                                {
                                    if(x+i<width&&x+i>=0&&y+j<height&&y+j>=0)
                                        if(QColor(image.pixel(x+i,y+j)).red()==R[firstMixColor]&&
                                                QColor(image.pixel(x+i,y+j)).green()==G[firstMixColor]&&
                                                QColor(image.pixel(x+i,y+j)).blue()==B[firstMixColor])
                                        {
                                            flag=true;
                                            break;
                                        }

                                }
                            }
                            if(!flag)
                            {
                                for(int i=0;i<foot;i++)
                                {
                                    if(x+i<width&&x+i>=0&&y+i<height&&y+i>=0)
                                        if(QColor(image.pixel(x+next[i][0],y+next[i][1])).red()==R[secondMixColor]&&
                                                QColor(image.pixel(x+next[i][0],y+next[i][1])).green()==G[secondMixColor]&&
                                                QColor(image.pixel(x+next[i][0],y+next[i][1])).blue()==B[secondMixColor])
                                        {
                                            if(x+next[i][0]>=0&&x+next[i][0]<width&&y+next[i][1]>=0&&y+next[i][1]<height)
                                                image.setPixel(x+next[i][0],y+next[i][1],value);
                                        }
                                }
                            }
                        }
                }
            }
            //            if(QColor(image.pixel(x,y)).red()==255&&
            //                    QColor(image.pixel(x,y)).green()==255&&
            //                    QColor(image.pixel(x,y)).blue()==255)
            //            {

            //                bool flag=false;

            //                for(int i=0;i<footmax;i++)
            //                {
            //                    if(x+xgo[i]<width&&y+ygo[i]<height)
            //                        if(QColor(image.pixel(x+xgo[i],y+ygo[i])).red()==R[firstMixColor]&&
            //                                QColor(image.pixel(x+xgo[i],y+ygo[i])).green()==G[firstMixColor]&&
            //                                QColor(image.pixel(x+xgo[i],y+ygo[i])).blue()==B[firstMixColor])
            //                        {
            //                            flag=true;
            //                            break;
            //                        }
            //                }
            //                if(!flag)
            //                {
            //                    for(int i=0;i<foot;i++)
            //                    {
            //                        if(x+xgo[i]<width&&y+ygo[i]<height)
            //                            if(QColor(image.pixel(x+next[i][0],y+next[i][1])).red()==R[secondMixColor]&&
            //                                    QColor(image.pixel(x+next[i][0],y+next[i][1])).green()==G[secondMixColor]&&
            //                                    QColor(image.pixel(x+next[i][0],y+next[i][1])).blue()==B[secondMixColor])
            //                            {
            //                                if(x+next[i][0]>=0&&x+next[i][0]<width&&y+next[i][1]>=0&&y+next[i][1]<height)
            //                                    image.setPixel(x+next[i][0],y+next[i][1],value);
            //                            }
            //                    }
            //                }


            //            }

        }
    }
    image.save("D://dealComplete2.tif");
    qDebug()<<QStringLiteral("结束");
}

void CutArea::cut(QImage image,int mradius,int firstMixColor,int secondMixColor){

    int R[7]={255,   255,  255,    0,   0,   139,0};
    int G[7]={255,     0,  255,    0,   255, 0 ,0};
    int B[7]={255,     0  ,  0,  255,   0,   139   ,0};

    int Samples=image.width();
    int Lines=image.height();

    QRgb value=qRgb(255,0,0);
    int hNumber=Lines/mradius;//降噪块的高度方向的数量
    int wNumber=Samples/mradius;//降噪块的宽度方向的数量
    int j=0,i=0;

    double hNumberAc=(double)Lines/mradius;//降噪块的高度方向的数量
    double wNumberAc=(double)Samples/mradius;//降噪块的宽度方向的数量
    int hDeviation=0;
    int wDeviation=0;
    if(hNumber!=hNumberAc)
    {
        hDeviation=Lines%mradius;
    }
    if(wNumber!=wNumberAc)
    {
        wDeviation=Samples%mradius;
    }
    qDebug()<<QStringLiteral("裁切开始");

    for( j=0;j<hNumber;j++)
    {
        for( i=0;i<wNumber;i++)
        {
            int fMColorCount=0;
            int sMColorCount=0;

            for(int y=j*mradius;y<(j+1)*mradius;y++)
            {
                for(int x=i*mradius;x<(i+1)*mradius;x++)
                {
                    if(x<Samples&&x>=0&&y<Lines&&y>=0)
                        if(QColor(image.pixel(x,y)).red()==R[firstMixColor]&&
                                QColor(image.pixel(x,y)).green()==G[firstMixColor]&&
                                QColor(image.pixel(x,y)).blue()==B[firstMixColor])
                        {
                            fMColorCount++;
                        }
                        else if(QColor(image.pixel(x,y)).red()==R[secondMixColor]&&
                                QColor(image.pixel(x,y)).green()==G[secondMixColor]&&
                                QColor(image.pixel(x,y)).blue()==B[secondMixColor])
                        {
                            sMColorCount++;
                        }
                        else
                        {
                            image.setPixel(x,y,value);
                        }
                }

            }
            if(abs(fMColorCount-sMColorCount)>mradius*mradius/1.1||fMColorCount==0)
            {
                for(int y=j*mradius+hDeviation;y>(j-1)*mradius+hDeviation;y--)
                {
                    for(int x=i*mradius+wDeviation;x>(i-1)*mradius+wDeviation;x--)
                    {
                        if(x<Samples&&x>=0&&y<Lines&&y>=0)
                            image.setPixel(x,y,value);
                    }
                }
            }


        }
    }


   qDebug()<<QStringLiteral("裁切开始50%");

    for( j=hNumber;j>=0;j--)
    {
        for( i=wNumber;i>=0;i--)
        {
            int fMColorCount=0;
            int sMColorCount=0;

            //boxNumber=0;//初始化每个小格子的点的数量
            for(int y=j*mradius+hDeviation;y>(j-1)*mradius+hDeviation;y--)
            {
                for(int x=i*mradius+wDeviation;x>(i-1)*mradius+wDeviation;x--)
                {

                    if(x<Samples&&x>=0&&y<Lines&&y>=0)
                        if(QColor(image.pixel(x,y)).red()==R[firstMixColor]&&
                                QColor(image.pixel(x,y)).green()==G[firstMixColor]&&
                                QColor(image.pixel(x,y)).blue()==B[firstMixColor])
                        {
                            fMColorCount++;
                        }
                        else if(QColor(image.pixel(x,y)).red()==R[secondMixColor]&&
                                QColor(image.pixel(x,y)).green()==G[secondMixColor]&&
                                QColor(image.pixel(x,y)).blue()==B[secondMixColor])
                        {
                            sMColorCount++;
                        }
                        else
                        {
                            image.setPixel(x,y,value);
                        }
                }

            }

            if(abs(fMColorCount-sMColorCount)>mradius*mradius/1.1||fMColorCount==0)
            {
                for(int y=j*mradius+hDeviation;y>(j-1)*mradius+hDeviation;y--)
                {
                    for(int x=i*mradius+wDeviation;x>(i-1)*mradius+wDeviation;x--)
                    {
                        if(x<Samples&&x>=0&&y<Lines&&y>=0)
                            image.setPixel(x,y,value);

                    }
                }
            }

        }
    }
    image.save("D://dealComplete.tif");

}


