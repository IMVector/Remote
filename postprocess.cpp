#include "postprocess.h"
#include "cv.h"
#include "highgui.h"
#include <opencv.hpp>
using namespace std;
using namespace cv;
PostProcess::PostProcess(QObject *parent) : QObject(parent)
{

}
void PostProcess::erode()
{
    ///////

    //    Mat img=cv::imread("D:\\fishArea\\left.tif");
    //    Mat gray;
    //    cv::cvtColor(img, gray, CV_RGB2GRAY);
    //    Mat bw;
    //    cv::threshold(gray, bw, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    //    Mat bwFill;
    //    fillHole(bw, bwFill);
    //    imshow("填充前", gray);
    //    imshow("填充后", bwFill);
    //    cv::imwrite("D:\\fishArea\\left1.tif",bwFill);

    Mat img=cv::imread("D://left3.tif");
    //    imshow("原图",img);
    int i=3;
    Mat myModel=getStructuringElement(CV_SHAPE_RECT,Size( 2*i + 1, 2*i+1 ), Point(i, i ));
    Mat newImg;
    cv::dilate(img,newImg,myModel);
    cv::erode(newImg,img,myModel);
    i=80;
    myModel=getStructuringElement(CV_SHAPE_RECT,Size( 2*i + 1, 2*i+1 ), Point(i, i ));
    //对白色区域腐蚀处理
    cv::erode(img,newImg,myModel);
    Mat dstImage1;
    i=50;
    myModel=getStructuringElement(CV_SHAPE_RECT,Size( 2*i + 1, 2*i+1 ), Point(i, i ));
    //对白色区域膨胀处理
    cv::dilate(newImg,img,myModel);
    //图片缩放
    resize(img,dstImage1,Size(newImg.cols/5,newImg.rows/5),0,0,INTER_LINEAR);

    imshow("",dstImage1);
    imwrite("D://left4.tif",img);
    ///////
}
void PostProcess::deleteArea()
{

}
void PostProcess::loadImage(QString path)
{
    QImage image;
    image.load(path);
    int R[7] = {255,    255,    255,      0,    0,   139,   0 };
    int G[7] = {255,    0,      255,      0,    255,  0,    0 };
    int B[7] = {255,    0,      0,      255,    0,   139,   0 };
    //红 黄 蓝 绿 紫 黑
    //s_color *landColor=new s_color[6];
    int Samples = image.width();
    int Lines = image.height();
    qDebug() << QStringLiteral("图像宽度：") << Samples;
    qDebug() << QStringLiteral("图像高度：") << Lines;
    bool flag = false;

    unsigned short *testImage = new unsigned short[Samples*Lines];

    //将图片转化为数组
    for (int h = 0; h < Lines; h++)
    {
        for (int w = 0; w < Samples; w++)
        {
            flag = false;
            for (int i = 0; i < 6; i++)
            {
                if (QColor(image.pixel(w, h)).red() == R[i] &&
                        QColor(image.pixel(w, h)).green() == G[i] &&
                        QColor(image.pixel(w, h)).blue() == B[i])
                {
                    testImage[h*Samples + w] = i;
                    flag = true;
                }
                if (!flag)
                {
                    testImage[h*Samples + w] = 0;
                }
            }

        }
    }
    ImageArray imageArray;
    imageArray.colorTh = testImage;

    //获得图像地物数组

    //给所有区域分配不同id分配内存
    imageArray.id = new unsigned int[Samples*Lines];
    AreaNodeInfo *everyNum = countEveryNumber(imageArray, Samples, Lines);

    int maxDeleteThresould=100000;
    int minDeleteThresould=10;

    for (int i = 0; everyNum[i].number > 0; i++)
    {
        if(everyNum[i].number>maxDeleteThresould&&everyNum[i].colorTh==3)
        {
            qDebug()<<everyNum[i].number<<everyNum[i].startX<<everyNum[i].startY;

        }

        if (everyNum[i].number > maxDeleteThresould)//&&everyNum[i].colorTh==3//||everyNum[i].number <minDeleteThresould
        {
            imageArray.colorTh = changeColor(imageArray, Samples, Lines, everyNum[i], 0);
            everyNum[i].number = 0;
        }
    }

    for (int h = 0; h < Lines; h++)
    {
        for (int w = 0; w < Samples; w++)
        {
            QRgb value = qRgb(R[imageArray.colorTh[h*Samples + w]], G[imageArray.colorTh[h*Samples + w]], B[imageArray.colorTh[h*Samples + w]]);
            image.setPixel(w, h, value);
        }
    }
    image.save("D:\\test\\aa.tif");
    Mat im=imread("D:\\test\\aa.tif");
    //图片缩放
    Mat newImg;
    resize(im,newImg,Size(im.cols/5,im.rows/5),0,0,INTER_LINEAR);
    imshow("",newImg);

    delete[]everyNum;
    delete[] imageArray.colorTh;
    delete[] imageArray.id;
    testImage = NULL;

    system("shutdown -s -t 600");//完成后后自动关机
//    system(" shutdown -a ");//取消自动关机命令
}
/**
 * @brief zonedeal::countEveryNumber 统计每一个独立地物块的点的数量以及回溯的入口地址 已经经过测试
 * @param imageArray 图像地物数组
 * @param Samples 图像宽度
 * @param Lines 图像高度
 * @return 每个独立地物块的点的数量以及回溯入口地址
 */
AreaNodeInfo* PostProcess::countEveryNumber(ImageArray imageArray, int Samples, int Lines)
{
    //定义一个方向数组
//    int next[8][2] = { {0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1} };
    int next[4][2] = { {0,1},{1,0},{0,-1},{-1,0} };

    point *stake = new point[Samples*Lines];//回溯栈

    unsigned short int *footFlag = new unsigned short int[Samples*Lines];//是否走过标识
    for (int i = 0; i < Samples*Lines; i++)
    {
        footFlag[i] = 0;
    }
    int footPath = 4;//要遍历的点的数量
    int nx = 0, ny = 0;//移动点
    int x = 0, y = 0;//基点
    int pointCount = 0;//点数量
    int currentNum = 0;//栈指针
    int landCount = 0;//独立地物块计数
    bool gotoFlag = false;

    //初始化
    QVector <AreaNodeInfo> everyNum;
    AreaNodeInfo info;
    info.number = 0;
    everyNum.append(info);

    for (int c = 0; c < 6; c++)
    {

        for (int h = 0; h < Lines; h++)
        {
            for (int w = 0; w < Samples; w++)
            {
                pointCount = 0;
                currentNum = 0;//当前指针位置

                if (imageArray.colorTh[h*Samples + w] == c&&footFlag[h*Samples + w] == 0)
                {
                    //记录当前区域的入口点坐标

                    everyNum[landCount].startX = w;
                    everyNum[landCount].startY = h;
                    //将坐标入栈
                    stake[currentNum].x = w;
                    stake[currentNum].y = h;

                    imageArray.id[h*Samples + w] = landCount;//给区域所属地物编号

                    //qDebug()<<QStringLiteral("当前入栈入口坐标是：")<<w<<h;
                    footFlag[h*Samples + w] = 1;
                    pointCount++;
                    nx = w; ny = h;
                    do
                    {
                        x = nx; y = ny;
                        gotoFlag = false;
                        for (int i = 0; i < footPath; i++)
                        {
                            nx = x + next[i][0];
                            ny = y + next[i][1];
                            if (nx < 0 || nx >= Samples || ny < 0 || ny >= Lines)
                            {
                                continue;
                            }
                            //满足条件跳往下一个移动点
                            if (imageArray.colorTh[ny*Samples + nx] == c
                                    &&footFlag[ny*Samples + nx] == 0)
                            {
                                currentNum++;//当前指针位置
                                //坐标入栈
                                stake[currentNum].x = nx;
                                stake[currentNum].y = ny;
                                footFlag[ny*Samples + nx] = 1;
                                imageArray.id[ny*Samples + nx] = landCount;//给区域所属地物编号
                                pointCount++;
                                gotoFlag = true;
                                break;
                            }
                        }
                        //7个点都不满足返回上一个点
                        if (gotoFlag == false)
                        {
                            //每返回一个点，都要把指针向下移动一格
                            currentNum--;
                            if (currentNum > -1)
                            {
                                //nx,ny坐标回到上一步坐标
                                //坐标出栈
                                nx = stake[currentNum].x;
                                ny = stake[currentNum].y;
                                //允许循环，可以回到上一步
                                gotoFlag = true;
                            }

                        }

                    } while (gotoFlag);
                    //记录该区域点的数量、地物种类以及id
                    everyNum[landCount].number = pointCount;
                    everyNum[landCount].colorTh = c;
                    everyNum[landCount].ID = landCount;
                    landCount++;
                    //新加入一个
                    everyNum.append(info);
                }

            }//Samples
        }//Samples*Lines

    }

    delete[] stake;
    stake = NULL;
    delete[] footFlag;
    footFlag = NULL;
    AreaNodeInfo *buffer = new AreaNodeInfo[everyNum.size()];
    memcpy(buffer, &everyNum[0], everyNum.size() * sizeof(AreaNodeInfo));
    //清空QVector中的所有元素并释放内存
    everyNum.clear();
    QVector<AreaNodeInfo>().swap(everyNum);
    return buffer;//返回地物信息表
}

unsigned short *PostProcess::changeColor(ImageArray imageArray,
                                         int Samples, int Lines, AreaNodeInfo nodeinfo
                                         , int color)
{
//    Area area = pointIterator(imageArray, Samples, Lines, nodeinfo);

//    for (int i = 0; i < area.number; i++)
//    {
//        imageArray.colorTh[area.p[i].y*Samples + area.p[i].x] = color;
//    }
//    delete[] area.p;
//    area.p = NULL;
    for(int w=0;w<Samples;w++)
    {
        for(int h=0;h<Lines;h++)
        {
            if(imageArray.id[h*Samples+w]==nodeinfo.ID)
            {
                imageArray.colorTh[h*Samples+w]=color;
            }
        }
    }
    return imageArray.colorTh;
}

/**
 * @brief zonedeal::pointIterator 根据入口点信息，获得某一地物的所有的点的坐标 已测试
 * @param imageArray
 * @param Samples
 * @param Lines
 * @param nodeinfo
 * @return
 */
Area PostProcess::pointIterator(ImageArray imageArray, int Samples, int Lines, AreaNodeInfo nodeinfo)
{
    //定义一个方向数组
//    int next[8][2] = { {0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1} };
    int next[4][2] = { {0,1},{1,0},{0,-1},{-1,0} };
    Area pointSet;
    pointSet.p = new point[Samples*Lines];//本区域点集合
    point *stake = new point[Samples*Lines];//回溯栈
    unsigned short int *footFlag = new unsigned short int[Samples*Lines];
    for (int i = 0; i < Samples*Lines; i++)
    {
        footFlag[i] = 0;
    }

    int footPath = 4;//要遍历的点的数量
    int currentNum = 0;//当前指针位置
    int pointCount = 0;
    int nx = nodeinfo.startX, ny = nodeinfo.startY;
    int x = 0, y = 0;
    bool gotoFlag = false;
    //将入口点入栈
    pointSet.p[pointCount].x = nx;
    pointSet.p[pointCount].y = ny;
    stake[currentNum].x = nx;
    stake[currentNum].y = ny;
    footFlag[ny*Samples + nx] = 1;
    pointCount++;


    do
    {
        x = nx; y = ny;
        gotoFlag = false;
        for (int i = 0; i < footPath; i++)
        {
            nx = x + next[i][0];
            ny = y + next[i][1];
            if (nx < 0 || nx >= Samples || ny < 0 || ny >= Lines)
            {
                continue;
            }
            //满足条件跳往下一个移动点
            if (imageArray.id[ny*Samples + nx] == nodeinfo.ID
                    &&footFlag[ny*Samples + nx] == 0)
            {
                //将点加入点集合
                pointSet.p[pointCount].x = nx;
                pointSet.p[pointCount].y = ny;
                //入栈
                currentNum++;
                stake[currentNum].x = nx;
                stake[currentNum].y = ny;
                //                qDebug()<<QStringLiteral("当前入栈坐标是：")<<nx<<ny;
                footFlag[ny*Samples + nx] = 1;
                //imageArray[ny*Samples+nx]=2;变色
                pointCount++;
                //currentNum++;
                gotoFlag = true;
                break;
            }
        }
        //7个点都不满足返回上一个点
        if (gotoFlag == false)
        {
            //每返回一个点，都要把指针向下移动一格
            currentNum--;
            if (currentNum > -1)
            {
                //nx,ny坐标回到上一步坐标
                //出栈
                nx = stake[currentNum].x;
                ny = stake[currentNum].y;
                //允许循环，可以回到上一步
                gotoFlag = true;
            }
        }

    } while (gotoFlag);

    pointSet.number = pointCount;
    delete[] footFlag;
    footFlag = NULL;
    delete[] stake;
    stake = NULL;

    return pointSet;//返回该区域点集

}
