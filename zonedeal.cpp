#include "zonedeal.h"

zonedeal::zonedeal()
{
    //输入一个图片地址
    QImage image;
    image.load("C:\\Users\\25235\\Desktop\\aaa.tif");
    if(!image.isNull())
    {
        main(image);
    }
    else
    {
        qDebug()<<QStringLiteral("文件为空");
    }

}
void zonedeal::main(QImage image)
{
    int R[7]={ 255,  255,    0,   0,   139,0};
    int G[7]={   0,  255,    0,   255, 0 ,0};
    int B[7]={   0  ,  0,  255,   0,   139   ,0};
    //红 黄 蓝 绿 紫 黑
    //s_color *landColor=new s_color[6];
    int Samples=image.width();
    int Lines=image.height();
    qDebug()<<Samples;
    qDebug()<<Lines;
    bool flag=false;

    unsigned short int *imageArray=new unsigned short int[Samples*Lines];
    //将图片转化为数组
    for(int h=0;h<Lines;h++)
    {
        for(int w=0;w<Samples;w++)
        {
            flag=false;
            for(int i=0;i<6;i++)
            {
                if(QColor(image.pixel(w,h)).red()==R[i]&&
                        QColor(image.pixel(w,h)).green()==G[i]&&
                        QColor(image.pixel(w,h)).blue()==B[i]
                        )
                {
                    imageArray[h*Samples+w]=i;
                    flag=true;
                }
                if(!flag)
                {
                    imageArray[h*Samples+w]=0;
                }
            }

        }
    }
    //获得图像地物数组
    everyLandNum *everyNum=countEveryNumber(imageArray,Samples,Lines);
    for(int i=0;everyNum[i].number!=0;i++)
    {
        ////一个地物块的点的数量很小，可以认为它是误分区域
        //        if(everyNum[i].number<(Samples*Lines)/40)
        //        {
        //            //利用回溯算法将此地物转换成将其包围的地物
        //            //回溯的入口(everyNum[i].x,everyNum[i].y)
        //        }
        //        if(everyNum[i].colorTh==0&&everyNum[i].number>20000&&everyNum[i].number<200000)
        //        {
        //            qDebug()<<QStringLiteral("入口所在下标")<<i<<QStringLiteral("数量是")<<everyNum[i].number;
        //            point *pointSet=pointIterator(imageArray,Samples,Lines,everyNum[i]);
        //            delete[] pointSet;
        //            pointSet=NULL;
        //        }
    }
    //    for(int h=0;h<Lines;h++)
    //    {
    //        for(int w=0;w<Samples;w++)
    //        {
    //            QRgb value=qRgb(R[imageArray[h*Samples+w]],G[imageArray[h*Samples+w]],B[imageArray[h*Samples+w]]);
    //            image.setPixel(w,h,value);
    //        }
    //    }
    //    image.save("C:\\Users\\25235\\Desktop\\b.tif");

    //    linjie(imageArray,Samples,Lines);
delete[] imageArray;
imageArray=NULL;
}
/**
 * @brief zonedeal::countEveryNumber 统计每一个独立地物块的点的数量以及回溯的入口地址
 * @param imageArray 图像地物数组
 * @param Samples 图像宽度
 * @param Lines 图像高度
 * @return 每个独立地物块的点的数量以及回溯入口地址
 */
everyLandNum* zonedeal::countEveryNumber(unsigned short int *imageArray,int Samples,int Lines)
{

    //定义一个方向数组
    int next[8][2]={ {0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}};

    point *stake=new point[Samples*Lines];
    everyLandNum * everyNum=new everyLandNum[Samples*(Lines/2)];
    //初始化，很重要
    for(int i=0;i<Samples*5;i++)
    {
        everyNum[i].number=0;
    }
    unsigned short int *footFlag=new unsigned short int[Samples*Lines];
    for(int i=0;i<Samples*Lines;i++)
    {
        footFlag[i]=0;
    }
    int extends=1;//距离边缘的距离
    int footPath=7;//要遍历的点的数量
    int nx=0,ny=0;//移动点
    int x=0,y=0;//基点
    int pointCount=0;//点数量
    int currentNum=0;//栈指针
    int landCount=0;//独立地物块计数
    bool gotoFlag=false;

    for(int c=0;c<6;c++)
    {

        for(int h=0;h<Lines;h++)
        {
            for(int w=0;w<Samples;w++)
            {
                pointCount=0;
                currentNum=0;

                if(imageArray[h*Samples+w]==c&&footFlag[h*Samples+w]==0)
                {
                    //qDebug()<<QStringLiteral("当前入口")<<w<<h;
                    everyNum[landCount].startX=w;
                    everyNum[landCount].startY=h;
                    stake[pointCount].x=w;
                    stake[pointCount].y=h;

                    footFlag[h*Samples+w]=1;

                    currentNum++;
                    pointCount++;
                    nx=w;ny=h;
                    do
                    {
                        x=nx;y=ny;
                        gotoFlag=false;
                        for(int i=0;i<footPath;i++)
                        {
                            nx=x+next[i][0];
                            ny=y+next[i][1];
                            if(nx<extends||nx>Samples-extends||ny<extends||ny>Lines-extends)
                            {
                                continue;
                            }
                            //满足条件跳往下一个移动点
                            if(imageArray[ny*Samples+nx]==c
                                    &&footFlag[ny*Samples+nx]==0)
                            {
                                stake[pointCount].x=nx;
                                stake[pointCount].y=ny;

                                footFlag[ny*Samples+nx]=1;

                                pointCount++;
                                currentNum++;
                                gotoFlag=true;
                                break;
                            }
                        }
                        //7个点都不满足返回上一个点
                        if(gotoFlag==false)
                        {
                            //每返回一个点，都要把指针向下移动一格
                            currentNum--;
                            if(currentNum>-1)
                            {
                                //nx,ny坐标回到上一步坐标
                                nx=stake[currentNum].x;
                                ny=stake[currentNum].y;
                                //允许循环，可以回到上一步
                                gotoFlag=true;
                            }

                        }

                    } while(gotoFlag);

                    everyNum[landCount].number=pointCount;
                    everyNum[landCount].colorTh=c;
                    landCount++;
                }

            }//Samples
        }//Samples*Lines

    }



    delete[] stake;
    stake=NULL;
    delete[] footFlag;
    footFlag=NULL;
    return everyNum;
    //    delete[] everyNum;
    //    everyNum=NULL;
}
/**
 * @brief zonedeal::pointIterator 根据入口点信息，获得某一地物的所有的点的坐标
 * @param imageArray
 * @param Samples
 * @param Lines
 * @param nodeinfo
 * @return
 */
point*zonedeal::pointIterator(unsigned short int *imageArray,int Samples,int Lines,everyLandNum nodeinfo)
{
    //定义一个方向数组
    int next[8][2]={ {0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}};
    point *stake=new point[Samples*Lines];
    unsigned short int *footFlag=new unsigned short int[Samples*Lines];
    for(int i=0;i<Samples*Lines;i++)
    {
        footFlag[i]=0;
    }
    int extends=1;//距离边缘的距离
    int footPath=7;//要遍历的点的数量
    int currentNum=0;
    int pointCount=0;
    int nx=nodeinfo.startX,ny=nodeinfo.startY;
    int x=0,y=0;
    bool gotoFlag=false;
    //将入口点入栈
    stake[pointCount].x=nx;
    stake[pointCount].y=ny;
    pointCount++;
    currentNum++;

    do
    {
        x=nx;y=ny;
        gotoFlag=false;
        for(int i=0;i<footPath;i++)
        {
            nx=x+next[i][0];
            ny=y+next[i][1];
            if(nx<extends||nx>Samples-extends||ny<extends||ny>Lines-extends)
            {
                continue;
            }
            //满足条件跳往下一个移动点
            if(imageArray[ny*Samples+nx]==nodeinfo.colorTh
                    &&footFlag[ny*Samples+nx]==0)
            {
                stake[pointCount].x=nx;
                stake[pointCount].y=ny;

                footFlag[ny*Samples+nx]=1;
                imageArray[ny*Samples+nx]=2;
                pointCount++;
                currentNum++;
                gotoFlag=true;
                break;
            }
        }
        //7个点都不满足返回上一个点
        if(gotoFlag==false)
        {
            //每返回一个点，都要把指针向下移动一格
            currentNum--;
            if(currentNum>-1)
            {
                //nx,ny坐标回到上一步坐标
                nx=stake[currentNum].x;
                ny=stake[currentNum].y;
                //允许循环，可以回到上一步
                gotoFlag=true;
            }
        }

    } while(gotoFlag);

    delete[] footFlag;
    footFlag=NULL;
    return stake;
    //    delete[] stake;
    //    stake=NULL;
}
void zonedeal::linjie(unsigned short int *imageArray,int Samples,int Lines)
{
    //统计四种地物（0 1）（0 2）（0 3）（1 2）（1 3）（2 3）邻接关系（右邻接和下邻接）
    unsigned short int path[15][2]={{0,1},{0,2},{1,2},{0,3},{1,3},{2,3}};
    int right[15][3];
    int down[15][3];
    int rightRevers[15][3];
    int downRevers[15][3];
    for(int i=0;i<15;i++)
    {
        for(int j=0;j<3;j++)
        {
            right[i][j]=0;
            down[i][j]=0;
            rightRevers[i][j]=0;
            downRevers[i][j]=0;
        }
    }
    int rightCount=0,downCount=0,rightReversCount=0,downReversCount=0;
    for(int i=0;i<6;i++)
    {
        //清空上一轮计数
        rightCount=0;
        downCount=0;
        rightReversCount=0;
        downReversCount=0;

        for(int h=0;h<Lines-1;h++)
        {
            for(int w=0;w<Samples-1;w++)
            {
                //对于第path[i][0]种地物，若他的右边的地物是path[i][1]地物则记录下第i种地物的右邻接地物，并记录右邻接地物的数量
                //声明right[15][3]={{path[i][0],path[i][1],number},.....}
                //声明down[15][3]={{path[i][0],path[i][1],number},......}

                //右邻接path[i][0]到path[i][1]
                if(imageArray[h*Samples+w]==path[i][0]&&imageArray[(h*Samples+w+1)]==path[i][1])
                {
                    right[i][0]=path[i][0];
                    right[i][1]=path[i][1];
                    right[i][2]=rightCount;
                    rightCount++;
                }
                //右邻接path[i][1]到path[i][0]
                if(imageArray[h*Samples+w]==path[i][1]&&imageArray[(h*Samples+w+1)]==path[i][0])
                {
                    rightRevers[i][0]=path[i][1];
                    rightRevers[i][1]=path[i][0];
                    rightRevers[i][2]=rightReversCount;
                    rightReversCount++;
                }
                //下邻接path[i][0]到path[i][1]
                if(imageArray[h*Samples+w]==path[i][0]&&imageArray[(h+1)*Samples+w]==path[i][1])
                {
                    down[i][0]=path[i][0];
                    down[i][1]=path[i][1];
                    down[i][2]=downCount;
                    downCount++;
                }
                //下邻接path[i][1]到path[i][0]
                if(imageArray[h*Samples+w]==path[i][1]&&imageArray[(h+1)*Samples+w]==path[i][0])
                {
                    downRevers[i][0]=path[i][1];
                    downRevers[i][1]=path[i][0];
                    downRevers[i][2]=downReversCount;
                    downReversCount++;
                }
            }
        }
    }

    //返回右邻接和下邻接表
    for(int i=0;i<6;i++)
    {
        qDebug()<<QStringLiteral("地物")<<right[i][0]<<QStringLiteral("地物")<<right[i][1]<<QStringLiteral("右邻接，邻接数量")<<right[i][2];
    }
    qDebug()<<"\n";
    for(int i=0;i<6;i++)
    {
        qDebug()<<QStringLiteral("地物")<<down[i][0]<<QStringLiteral("地物")<<down[i][1]<<QStringLiteral("下邻接，邻接数量")<<down[i][2];
    }
    qDebug()<<"\n";
    for(int i=0;i<6;i++)
    {
        qDebug()<<QStringLiteral("地物")<<rightRevers[i][0]<<QStringLiteral("地物")<<rightRevers[i][1]<<QStringLiteral("右邻接，邻接数量")<<rightRevers[i][2];
    }
    qDebug()<<"\n";
    for(int i=0;i<6;i++)
    {
        qDebug()<<QStringLiteral("地物")<<downRevers[i][0]<<QStringLiteral("地物")<<downRevers[i][1]<<QStringLiteral("下邻接，邻接数量")<<downRevers[i][2];
    }


}
///**
// * @brief zonedeal::zoneLowPoint
// * @param imageArray
// * @param Samples
// * @param Lines
// * @param right 自己是第self种地物
// * @param right 右邻接地物是第right种地物
// * @param down 右邻接地物是第down种地物
// */
//void zonedeal::zoneLowPoint(unsigned short int *imageArray,int Samples,int Lines,int self,int right,int down)
//{

//    for(int h=0;h<Lines-1;h++)
//    {
//        for(int w=0;w<Samples-1;w++)
//        {
//            //右边邻接
//            if(imageArray[h*Samples+w]==self&&imageArray[(h*Samples+w+1)]==right)
//            {

//            }
//            //下边邻接
//            if(imageArray[h*Samples+w]==self&&imageArray[(h+1)*Samples+w]==down)
//            {

//            }

//        }
//    }

//}




