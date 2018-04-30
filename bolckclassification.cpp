#include "bolckclassification.h"

BlockClassification::BlockClassification(QObject *parent) : QObject(parent)
{

}
void BlockClassification::start(QString path)
{
    float**data=openFile(path);
    //    QImage image=classfication(data,Samples,Lines,Band);
    //QImage image=relativeClssfication(data,Samples,Lines,Band);
    //QImage image=newClassification(data,Samples,Lines,Band);
    //QImage image=landFishAreaJudge(data,Samples,Lines,Band);
    QImage imageLand=getLand(data,Samples,Lines,Band);
//    QImage image=judge(data,Samples,Lines,Band);
    {
        QDateTime Systemtime = QDateTime::currentDateTime();//获取系统现在的时间
        QString str = Systemtime.toString("yyyy_MM_dd_hh_mm_ss"); //设置显示格式
//        QString fileStr="D:\\qttest\\blockClassfication"+str+".tif";
//        image.save(fileStr);
                QString landStr="D:\\qttest\\land"+str+".tif";
                imageLand.save(landStr);
    }
}

/**
 * @brief BlockClassfication::getRound 获取周围像素点的波段平均值
 * @param x
 * @param y
 * @param data
 * @param Samples
 * @param Lines
 * @param Band
 */
float* BlockClassification::getRound(int x,int y,float **data,int Samples,int Band)
{

    float *averageBand=new float[Band]{0};
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            int currentX=x+i;
            int currentY=y+j;
            for(int b=0;b<Band;b++)
            {
                averageBand[b]+=data[b][currentY*Samples+currentX];
            }

        }
    }
    for(int b=0;b<Band;b++)
    {
        averageBand[b]/=9;
    }
    return averageBand;
}

/**
 * @brief BlockClassification::classfication 边缘检测
 * @param data
 * @param Samples
 * @param Lines
 * @param Band
 * @return
 */
QImage BlockClassification::classfication(float** data,int Samples,int Lines,int Band)
{
    QImage image(Samples,Lines,QImage::Format_RGB32);
    QRgb rgb_black=qRgb(0,0,0);
    QRgb rgb_white=qRgb(255,255,255);
    float *lastAverage=new float[Band]{0};
    int threshould=1;

    for(int w=1;w<Samples-1;w++)
    {
        for(int h=1;h<Lines-1;h++)
        {
            float *averageBand=getRound(w,h,data,Samples,Band);
            for(int b=0;b<Band;b++)
            {
                if(qAbs(averageBand[b]-lastAverage[b])>=threshould)
                {
                    image.setPixel(w,h,rgb_black);
                    break;
                }else
                {
                    image.setPixel(w,h,rgb_white);
                }
            }
            delete[] lastAverage;
            lastAverage=averageBand;
        }
    }

    return image;
}

QImage BlockClassification::judge(float** data,int Samples,int Lines,int Band)
{

    float **newdata=deleteLand(data,Samples,Lines,Band);
    typeCountArray numArray=countRelative(newdata,Samples,Lines,Band);
    QImage image(Samples,Lines,QImage::Format_RGB32);
    QRgb rgb_blue=qRgb(0,0,255);
    QRgb rgb_green=qRgb(0,255,0);

    for(int w=0;w<Samples;w++)
    {
        for(int h=0;h<Lines;h++)
        {
            //if(newdata[2][h*Samples+w]>=160)
            //{
            //    qDebug()<<QStringLiteral("there maybe an error");
            //}
            //numArray[h*Samples+w].area2_count
            //qDebug()<<numArray[h*Samples+w].area1_Count;
            float sum=numArray[h*Samples+w].area1_Count+numArray[h*Samples+w].area2_Count;
            float ratio=numArray[h*Samples+w].area1_Count*1.0/sum;
            if(ratio>=0.6)
            {
                image.setPixel(w,h,rgb_blue);
            }else
            {
                image.setPixel(w,h,rgb_green);
            }
        }
    }
    delete [] numArray;
    delete []data;
    return image;

}

/**
 * @brief BlockClassification::countRelative 计算浮筏养殖区的
 * @param data
 * @param Samples
 * @param Lines
 * @param Band
 * @return
 */
typeCountArray BlockClassification::countRelative(float** data,int Samples,int Lines,int Band)
{
    int block_size=300;//块边长
    int offset=30;//偏移量
    typeCountArray numArray=new typeCount[Samples*Lines];
    for(int w=0;w<Samples;w++)
    {
        for(int h=0;h<Lines;h++)
        {
            numArray[h*Samples+w].area1_Count=0;
            numArray[h*Samples+w].area2_Count=0;
        }
    }


    for(int w=0;w<Samples;w+=offset)
    {
        for(int h=0;h<Lines;h+=offset)
        {
            int xBlock=block_size;
            int yBlock=block_size;
            if(w+block_size>=Samples-1)
            {
                xBlock=Samples-w;
            }
            if(h+block_size>=Lines-1)
            {
                yBlock=Lines-h;
            }

            //起始坐标（w，h）
            //终止坐标（w+xBlock,h+yBlock）

            float averageBand=blockAverage(data,Samples,w,h,xBlock,yBlock);
            float stdev=myStdev(data,Samples,averageBand,w,h,xBlock,yBlock);
            if(processImage(data,Samples,Band,w,h,xBlock,yBlock))
            {
                numArray=blockJudgeCount(numArray,averageBand,stdev,data,Samples,w,h,xBlock,yBlock);
            }

        }
    }
    return numArray;

}
/**
 * @brief BlockClassification::relativeClssfication 对图像分块处理
 * @param data
 * @param Samples
 * @param Lines
 * @param Band
 * @return
 */
QImage BlockClassification::relativeClssfication(float** data,int Samples,int Lines,int Band)
{
    int block_size=200;//块边长
    int offset=200;

    QImage image(Samples,Lines,QImage::Format_RGB32);

    for(int w=0;w<Samples;w+=offset)
    {
        for(int h=0;h<Lines;h+=offset)
        {
            int xBlock=block_size;
            int yBlock=block_size;
            if(w+block_size>=Samples-1)
            {
                xBlock=Samples-w;
            }
            if(h+block_size>=Lines-1)
            {
                yBlock=Lines-h;
            }
            //起始坐标（w，h）
            //终止坐标（w+xBlock,h+yBlock）
            float averageBand=blockAverage(data,Samples,w,h,xBlock,yBlock);
            float stdev=myStdev(data,Samples,averageBand,w,h,xBlock,yBlock);
            //if( processImage(data,Samples,Band,w,h,xBlock,yBlock))
            //{
            //    image=blockJudge(image,averageBand,data,Samples,w,h,xBlock,yBlock);
            //}
            //qDebug()<<w<<h<<averageBand<<stdev;

            if( processImage(data,Samples,Band,w,h,xBlock,yBlock))
            {
                image=blockJudge(image,averageBand,stdev,data,Samples,w,h,xBlock,yBlock);
                if(15<stdev&&stdev<200)
                {
                    offset=200;
                }
                else
                {
                    offset=10;
                }

            }

        }
    }
    return image;
}


/**
 * @brief BlockClassification::blockJudge 判断是否是养殖区
 * @param image
 * @param averageBand
 * @param data
 * @param Samples
 * @param x
 * @param y
 * @param width
 * @param height
 * @return
 */
QImage BlockClassification::blockJudge(QImage image,float averageBand,float stdev,float**data,
                                       int Samples,int x,int y,int width,int height)
{
    QRgb rgb_blue=qRgb(0,0,255);
    QRgb rgb_green=qRgb(0,255,0);

    for(int w=0;w<width;w++)
    {
        for(int h=0;h<height;h++)
        {
            int currentX=x+w;
            int currentY=y+h;
            if(15<stdev&&stdev<200)
            {
                if(256+data[2][currentY*Samples+currentX]-data[1][currentY*Samples+currentX]>=averageBand-2)
                {
                    image.setPixel(currentX,currentY,rgb_blue);
                }
                else
                {
                    image.setPixel(currentX,currentY,rgb_green);
                }
            }
            else
            {
                image.setPixel(currentX,currentY,rgb_green);
            }
        }

    }
    return image;
}
/**
 * @brief BlockClassification::blockJudgeCount 块检测，检测某个点每次的区分地物
 * @param numArray
 * @param averageBand
 * @param stdev
 * @param data
 * @param Samples
 * @param x
 * @param y
 * @param width
 * @param height
 * @return
 */
typeCountArray BlockClassification::blockJudgeCount(typeCountArray numArray,float averageBand,float stdev,float**data,
                                                    int Samples,int x,int y,int width,int height)
{

    for(int w=0;w<width;w++)
    {
        for(int h=0;h<height;h++)
        {
            int currentX=x+w;
            int currentY=y+h;
            if(15<stdev&&stdev<200)
            {
                if(256+data[2][currentY*Samples+currentX]-data[1][currentY*Samples+currentX]>=averageBand-2)
                {
                    numArray[currentY*Samples+currentX].area1_Count++;
                }
                else
                {
                    numArray[currentY*Samples+currentX].area2_Count++;
                }
            }
            else
            {
                numArray[currentY*Samples+currentX].area2_Count++;
            }
        }
    }
    return numArray;
}
/**
 * @brief BlockClassification::getLand 获取所有陆地区域
 * @param data
 * @param Samples
 * @param Lines
 * @param Band
 * @return
 */
QImage BlockClassification::getLand(float** data,int Samples,int Lines,int Band)
{
    QImage image(Samples,Lines,QImage::Format_RGB32);
    QRgb rgb_black=qRgb(0,0,0);
    QRgb rgb_white=qRgb(255,255,255);

    for(int w=0;w<Samples;w++)
    {
        for(int h=0;h<Lines;h++)
        {
            if(data[2][h*Samples+w]>=200)
            {
                image.setPixel(w,h,rgb_black);
            }
            else
            {
                image.setPixel(w,h,rgb_white);
            }
        }
    }
    return image;
}
/**
 * @brief BlockClassification::deleteLand 将陆地区域替换成与海洋相近的颜色
 * @param data
 * @param Samples
 * @param Lines
 * @param Band
 * @return
 */
float** BlockClassification::deleteLand(float** data,int Samples,int Lines,int Band)
{

    for(int w=0;w<Samples;w++)
    {
        for(int h=0;h<Lines;h++)
        {
            if(data[2][h*Samples+w]>=160||data[2][h*Samples+w]==0)
            {
                data[0][h*Samples+w]=300;
                data[1][h*Samples+w]=200;
                data[2][h*Samples+w]=100;
                data[3][h*Samples+w]=40;
            }
        }
    }
    return data;
}

/**
 * @brief preProcessImage 去除大黑边
 * @param data
 * @param Samples
 * @param Lines
 * @param Band
 * @param colorTh
 * @return
 */
float**preProcessImage(float** data,int Samples,int Lines,int Band,int colorTh)
{
    int R[7] = { 255,  255,    0,   0,   139,0 };
    int G[7] = { 0,  255,    0,   255, 0 ,0 };
    int B[7] = { 0  ,  0,  255,   0,   139   ,0 };

    for(int w=0;w<Samples;w++)
    {
        for(int h=0;h<Lines;h++)
        {
            int nullBand=0;
            for(int b=0;b<Band;b++)
            {
                if(data[b][h*Samples+w]==0)
                {
                    nullBand++;
                }
            }
            if(nullBand==Band)
            {
                data[0][h*Samples+w]=R[colorTh];
                data[1][h*Samples+w]=G[colorTh];
                data[2][h*Samples+w]=B[colorTh];
                data[3][h*Samples+w]=R[colorTh];
                //data[4][h*Samples+w]=G[colorTh];
                //data[5][h*Samples+w]=B[colorTh];
            }
        }
    }
    return data;
}
/**
 * @brief BlockClassification::processImage  判断一个区域块中是否有较多的无效点
 * @param data
 * @param Samples
 * @param Band
 * @param x
 * @param y
 * @param width
 * @param height
 * @return
 */
bool BlockClassification::processImage(float** data,int Samples,int Band,int x,int y,int width,int height)
{
    int countNull=0;
    for(int w=0;w<width;w++)
    {
        for(int h=0;h<height;h++)
        {
            int nullBand=0;

            int currentX=x+w;
            int currentY=y+h;
            for(int b=0;b<Band;b++)
            {
                if(data[b][currentY*Samples+currentX]==0)
                {
                    nullBand++;
                }
            }
            if(nullBand==Band)
            {
                countNull++;
            }
        }
    }
    if(countNull>width*height/2)
    {
        return false;
    }
    else
    {
        return true;
    }

}
/**
 * @brief BlockClassification::myStdev  求块方差（某个波段或波段差）
 * @param data
 * @param Samples
 * @param averageBand
 * @param x
 * @param y
 * @param width
 * @param height
 * @return
 */
float BlockClassification::myStdev(float**data,int Samples,float averageBand,int x,int y,int width,int height)
{
    float sum=0;//差平方
    for(int w=0;w<width;w++)
    {
        for(int h=0;h<height;h++)
        {
            int currentX=x+w;
            int currentY=y+h;
            sum+=qPow((averageBand-(256+data[2][currentY*Samples+currentX]-data[1][currentY*Samples+currentX])),2);
        }
    }
    float stdev=sum/(width*height);
    return stdev;
}

/**
 * @brief BlockClassification::blockAverage 求块平均值（某个波段或波段差）
 * @param data
 * @param Samples
 * @param x
 * @param y
 * @param width
 * @param height
 * @return
 */
float BlockClassification::blockAverage(float**data,int Samples,int x,int y,int width,int height)
{
    float averageBand=0;
    for(int w=0;w<width;w++)
    {
        for(int h=0;h<height;h++)
        {
            int currentX=x+w;
            int currentY=y+h;
            averageBand+=256+data[2][currentY*Samples+currentX]-data[1][currentY*Samples+currentX];
        }
    }
    averageBand=averageBand/(width*height);
    return averageBand;
}

//float*BlockClassification::blockAverage(float**data,int Samples,int Lines,int Band,int x,int y,int width,int height)
//{
//    float* averageBand=new float[Band]{0};
//    for(int w=0;w<width;w++)
//    {
//        for(int h=0;h<height;h++)
//        {
//            for(int b=0;b<Band;b++)
//            {
//                int currentX=x+w;
//                int currentY=y+h;
//                averageBand[b]+=data[b][currentY*Samples+currentX];
//            }
//        }
//    }
//    for(int b=0;b<Band;b++)
//    {
//        averageBand[b]=averageBand[b]/(width*height);
//        //qDebug()<<QStringLiteral("平均值")<<averageBand[b];
//    }
//    return averageBand;
//}



/**
 * @brief filedeal::openFile 获取文件的一小块区域（矩形区域） 返回原始数据
 * @param rectangle 传入矩形的左上顶点和右下顶点
 * @return 矩形区域的所有数据（以二维数组形式返回，第一维度是矩形区域的第i个点，第二维度是该点处的所有波段值）
 */
float ** BlockClassification::openFile(Points rectangle,QString filePath)
{
    int startX=rectangle[0].x,startY=rectangle[0].y;
    int xOffset=rectangle[1].x-rectangle[0].x;
    int yOffset=rectangle[1].y-rectangle[0].y;
    //qDebug()<<QStringLiteral("打开文件起始坐标")<<startX<<startY;
    //qDebug()<<QStringLiteral("打开文件结束坐标")<<rectangle[1].x<<rectangle[1].y;

    GDALAllRegister();//注册gdal驱动
    CPLSetConfigOption( "GDAL_FILENAME_IS_UTF8", "YES" );//中文路径中文名称支持
    GDALDataset *gdalData = ( GDALDataset* )GDALOpen(filePath.toStdString().c_str(), GA_ReadOnly );//打开文件
    if ( gdalData == NULL )
    {
        qWarning()<<QStringLiteral("数据集为空");
        return NULL;
    }
    int Band=gdalData->GetRasterCount();
    QList<GDALRasterBand*> bandList;
    qDeleteAll(bandList);
    bandList.clear();
    if(Band>=3)
    {
        for(int i=1;i<=Band;i++)
        {
            bandList.append(gdalData->GetRasterBand(i));//将所有的第i波段存入Qlist，以整体存入
        }
    }
    else
    {
        bandList.append(gdalData->GetRasterBand(1));
        bandList.append(gdalData->GetRasterBand(1));
        bandList.append(gdalData->GetRasterBand(1));
    }

    if ( bandList.size() == 0 )
    {
        qWarning()<<QStringLiteral("数据集为空");
        return NULL;
    }

    QList<float*> rawList;
    for(int i=0;i<Band;i++)
    {
        float *tempRaw=new float [xOffset*yOffset];//GDT_Float32
        bandList.at( i )->RasterIO( GF_Read, startX, startY, xOffset, yOffset, tempRaw, xOffset, yOffset, GDT_Float32, 0, 0 );
        rawList.append(tempRaw);
    }
    float **rawData=NULL;
    rawData=new float *[Band];
    for(int i=0;i<Band;i++)
    {
        rawData[i]=rawList.at(i);
    }
    GDALClose(gdalData);
    return rawData;
}
/**
 * @brief BlockClassification::openFile 打开原始数据集
 * @param filePath
 * @return
 */
float** BlockClassification::openFile(QString filePath)
{

    GDALAllRegister();//注册gdal驱动
    CPLSetConfigOption( "GDAL_FILENAME_IS_UTF8", "YES" );//中文路径中文名称支持
    GDALDataset *gdalData = ( GDALDataset* )GDALOpen(filePath.toStdString().c_str(), GA_ReadOnly );//打开文件
    if ( gdalData == NULL )
    {
        qWarning()<<QStringLiteral("数据集为空");
        return NULL;
    }
    Band=gdalData->GetRasterCount();
    Samples=gdalData->GetRasterXSize();
    Lines=gdalData->GetRasterYSize();

    QList<GDALRasterBand*> bandList;
    qDeleteAll(bandList);
    bandList.clear();
    if(Band>=3)
    {
        for(int i=1;i<=Band;i++)
        {
            bandList.append(gdalData->GetRasterBand(i));//将所有的第i波段存入Qlist，以整体存入
        }
    }
    else
    {
        bandList.append(gdalData->GetRasterBand(1));
        bandList.append(gdalData->GetRasterBand(1));
        bandList.append(gdalData->GetRasterBand(1));
    }

    if ( bandList.size() == 0 )
    {
        qWarning()<<QStringLiteral("数据集为空");
        return NULL;
    }

    int picWidth = bandList.at( 0 )->GetXSize();
    int picHeight = bandList.at( 0 )->GetYSize();

    QList<float*> rawList;
    for(int i=0;i<Band;i++)
    {
        float *tempRaw=new float [picHeight*picWidth];//GDT_Float32
        bandList.at( i )->RasterIO( GF_Read, 0, 0, picWidth, picHeight, tempRaw, picWidth, picHeight, GDT_Float32, 0, 0 );
        rawList.append(tempRaw);
    }
    float **rawData=NULL;
    rawData=new float *[Band];
    for(int i=0;i<Band;i++)
    {
        rawData[i]=rawList.at(i);
    }
    GDALClose(gdalData);
    return rawData;
}
/**
 * @brief BlockClassification::preProcessData 对原始数据进行处理
 * @param rawData
 * @param Samples
 * @param Lines
 * @param Band
 * @return
 */
float** BlockClassification::preProcessData(float **rawData,int Samples,int Lines,int Band)
{
    for(int h=0;h<Lines;h++)
    {
        for(int w=0;w<Samples;w++)
        {
            for(int b=0;b<Band;b++)
            {
                rawData[b][h*Samples+w]=rawData[b][h*Samples+w]*100;//将原始数据放大100倍
            }
        }
    }
    return rawData;
}

/**
 * @brief BlockClassification::newClassification 基于回溯的分类
 * @param data
 * @param Samples
 * @param Lines
 * @param Band
 * @return
 */
QImage BlockClassification::newClassification(float** data,int Samples,int Lines,int Band)
{
    QImage image(Samples,Lines,QImage::Format_RGB32);
    QRgb rgb_black=qRgb(0,0,0);
    QRgb rgb_white=qRgb(255,255,255);
    //定义一个方向数组
    int next[8][2] = { {0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1} };

    point *stake = new point[Samples*Lines];//回溯栈
    unsigned short int *footFlag = new unsigned short int[Samples*Lines];//是否走过标识

    for (int i = 0; i < Samples*Lines; i++)
    {
        footFlag[i] = 0;
    }
    int footPath = 8;//要遍历的点的数量
    int nx = 0, ny = 0;//移动点
    int x = 0, y = 0;//基点
    int currentNum = 0;//栈指针
    bool gotoFlag = false;


    int random=0;
    for (int h = 1; h < Lines-1; h++)
    {
        for (int w = 1; w < Samples-1; w++)
        {
            currentNum = 0;//当前指针位置
            random++;
            if (isSatisfy(data,Samples,Lines,Band,w,h)&&footFlag[h*Samples + w] == 0)
            {

                if(random%2==0)
                {
                    image.setPixel(w,h,rgb_black);
                }
                else
                {
                    image.setPixel(w,h,rgb_white);
                }

                //将坐标入栈
                stake[currentNum].x = w;
                stake[currentNum].y = h;

                footFlag[h*Samples + w] = 1;

                nx = w; ny = h;
                do
                {
                    x = nx; y = ny;
                    gotoFlag = false;
                    for (int i = 0; i < footPath; i++)
                    {
                        nx = x + next[i][0];
                        ny = y + next[i][1];
                        if (nx < 1 || nx >= Samples-1 || ny < 1 || ny >= Lines-1)
                        {
                            continue;
                        }
                        //满足条件跳往下一个移动点
                        if (!isSatisfy(data,Samples,Lines,Band,nx,ny)
                                &&footFlag[ny*Samples + nx] == 0)
                        {
                            currentNum++;//当前指针位置
                            //坐标入栈
                            if(random%2==0)
                            {
                                image.setPixel(w,h,rgb_black);
                            }
                            else
                            {
                                image.setPixel(w,h,rgb_white);
                            }
                            stake[currentNum].x = nx;
                            stake[currentNum].y = ny;
                            footFlag[ny*Samples + nx] = 1;

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
                            gotoFlag = true;
                        }

                    }

                } while (gotoFlag);
                //记录该区域点的数量、地物种类以及id

            }

        }//Samples
    }//Samples*Lines

    delete[] stake;
    stake = NULL;
    delete[] footFlag;
    footFlag = NULL;
    return image;
}

/**
 * @brief BlockClassification::isSatisfy 判断是否是突变点
 * @param data
 * @param Samples
 * @param Lines
 * @param Band
 * @return
 */
bool BlockClassification::isSatisfy(float** data,int Samples,int Lines,int Band,int x,int y)
{
    float *lastAverage=new float[Band]{0};
    int threshould=2;
    float *averageBand=getRound(x,y,data,Samples,Band);
    for(int b=0;b<Band;b++)
    {
        if(qAbs(averageBand[b]-lastAverage[b])>=threshould)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

//void BolckClassification::classification(int Samples,int Lines)
//{
//}
//void BolckClassification::cutImage(int Samples,int Lines,int width,int height)
//{
//    int wNum=Samples/width;
//    int hNum=Lines/height;
//    int startX=0,startY=0;

//    QList<Points> rectangleList;
//    Points rectangle=new point[2];
//    for(int h=0;h<hNum-1;h++)
//    {
//        for(int w=0;w<wNum-1;w++)
//        {
//            rectangle[0].x=w*width;
//            rectangle[0].y=h*height;
//            rectangle[1].x=(w+1)*width;
//            rectangle[1].y=(h+1)*height;
//            rectangleList.append(rectangle);
//        }
//    }

//    for(int h=0;h<hNum-1;h++)
//    {
//        rectangle[0].x=(wNum-1)*width;
//        rectangle[0].y=h*height;
//        rectangle[1].x=Samples;
//        rectangle[1].y=(h+1)*height;
//    }

//}
//void BolckClassification::openImage()
//{
//    GDALAllRegister();//注册gdal驱动
//    CPLSetConfigOption( "GDAL_FILENAME_IS_UTF8", "YES" );//中文路径中文名称支持
//    GDALDataset *gdalData = ( GDALDataset* )GDALOpen(fileName.toStdString().c_str(), GA_ReadOnly );//打开文件
//    if ( gdalData == NULL )
//    {
//        qWarning()<<QStringLiteral("数据集为空");
//        return ;
//    }

////    Band=gdalData->GetRasterCount();
////    Samples=gdalData->GetRasterXSize();
////    Lines=gdalData->GetRasterYSize();
////    qInfo()<<QStringLiteral("波段数量：")<<Band;
////    qInfo()<<QStringLiteral("图像宽度：")<<Samples;
////    qInfo()<<QStringLiteral("图像高度：")<<Lines;
//}

//QImage BlockClassification::newClassfication(float** data,int Samples,int Lines,int Band)
//{
//    QImage image(Samples,Lines,QImage::Format_RGB32);
//    QRgb rgb_black=qRgb(0,0,0);
//    QRgb rgb_white=qRgb(255,255,255);
//    QRgb current_rgb;
//    int threshould=2;
//    bool flag=false;
//    float *lastAverage=new float[Band]{0};

//    for(int w=1;w<Samples-1;w++)
//    {
//        for(int h=1;h<Lines-1;h++)
//        {
//            float *averageBand=getRound(w,h,data,Samples,Lines,Band);
//            for(int b=0;b<Band;b++)
//            {
//                if(qAbs(averageBand[b]-lastAverage[b])>=threshould)
//                {
//                    if(flag)
//                    {
//                        current_rgb=rgb_black;
//                        flag=false;
//                    }
//                    else
//                    {
//                        current_rgb=rgb_white;
//                        flag=true;
//                    }
//                    image.setPixel(w,h,current_rgb);
//                    break;
//                }else
//                {
//                    image.setPixel(w,h,current_rgb);
//                }
//            }
//            delete[] lastAverage;
//            lastAverage=averageBand;
//        }
//    }
//    return image;
//}

QImage BlockClassification::landFishAreaJudge(float** data,int Samples,int Lines,int Band)
{
    typeCountArray numArray=countLandFishAreaRelative(data,Samples,Lines,Band);
    QImage image(Samples,Lines,QImage::Format_RGB32);
    QRgb rgb_blue=qRgb(0,0,255);
    QRgb rgb_green=qRgb(0,255,0);

    for(int w=0;w<Samples;w++)
    {
        for(int h=0;h<Lines;h++)
        {
            float sum=numArray[h*Samples+w].area1_Count+numArray[h*Samples+w].area2_Count;
            float ratio=numArray[h*Samples+w].area1_Count*1.0/sum;
            if(ratio>=0.3)
            {
                image.setPixel(w,h,rgb_blue);
            }else
            {
                image.setPixel(w,h,rgb_green);
            }
        }
    }
    delete [] numArray;
    delete []data;
    return image;

}


/**
 * @brief BlockClassification::countRelative 计算沿海养殖区的
 * @param data
 * @param Samples
 * @param Lines
 * @param Band
 * @return
 */
typeCountArray BlockClassification::countLandFishAreaRelative(float** data,int Samples,int Lines,int Band)
{
    int block_size=300;//块边长
    int offset=30;//偏移量
    typeCountArray numArray=new typeCount[Samples*Lines];
    for(int w=0;w<Samples;w++)
    {
        for(int h=0;h<Lines;h++)
        {
            numArray[h*Samples+w].area1_Count=0;
            numArray[h*Samples+w].area2_Count=0;
        }
    }

    for(int w=0;w<Samples;w+=offset)
    {
        for(int h=0;h<Lines;h+=offset)
        {
            int xBlock=block_size;
            int yBlock=block_size;
            if(w+block_size>=Samples-1)
            {
                xBlock=Samples-w;
            }
            if(h+block_size>=Lines-1)
            {
                yBlock=Lines-h;
            }

            //起始坐标（w，h）
            //终止坐标（w+xBlock,h+yBlock）

            float averageBand=blockAverage(data,Samples,w,h,xBlock,yBlock);
            float stdev=myStdev(data,Samples,averageBand,w,h,xBlock,yBlock);
            if(processImage(data,Samples,Band,w,h,xBlock,yBlock))
            {
                numArray=blockLandFishAreaJudgeCount(numArray,averageBand,stdev,data,Samples,w,h,xBlock,yBlock);
            }

        }
    }
    return numArray;

}


typeCountArray BlockClassification::blockLandFishAreaJudgeCount(typeCountArray numArray,float averageBand,float stdev,float**data,
                                                                int Samples,int x,int y,int width,int height)
{

    for(int w=0;w<width;w++)
    {
        for(int h=0;h<height;h++)
        {
            int currentX=x+w;
            int currentY=y+h;
            if(15<stdev&&stdev<200)
            {
                if(256+data[2][currentY*Samples+currentX]-data[1][currentY*Samples+currentX]<=averageBand)
                {
                    numArray[currentY*Samples+currentX].area1_Count++;
                }
                else
                {
                    numArray[currentY*Samples+currentX].area2_Count++;
                }
            }
            else
            {
                numArray[currentY*Samples+currentX].area2_Count++;
            }

        }
    }
    return numArray;
}



