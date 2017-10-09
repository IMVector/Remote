#include "filedeal.h"
//1650 202
int thisCount=0;
int ERRORCount=0;
QString filePathName=NULL;
int Band=0;
int Samples=0;
int Lines=0;
int pointCount[6]={0};//样本区域点的个数
int tempCount=0;//当前样本数量
int current=0;//当前是第几个样本
int changer=0;
int changeg=0;
int changeb=0;
int samplePointNum;//选择的样本区域的点的数量
int linePointNb=0;//最长海岸线点的数量
bool mytempChange=false;//非标准rgb
bool sorted=false;
bool combineFlag=false;
QString lineImage[2];
point visiualDrawP;//上一个鼠标所在点的坐标
Points tempPoint=(point*)malloc(2*sizeof(point));//找到数据的矩形区域
Points areaPoint=(point*)malloc(100*sizeof(point));
simples *dataCopyArea =NULL;
QImage rawImage;
QImage partImage;
int R[7]={255,   255,  255,    0,   0,   139,0};
int G[7]={255,     0,  255,    0,   255, 0 ,0};
int B[7]={255,     0  ,  0,  255,   0,   139   ,0};

////////////////////////////////////////////////////////////////////
unsigned short int markColor[10000][10000];   //count(max)=561875  有563行
int xRoad[24] = {-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,0,0,0,0,1,1,1,1,1,2,2,2,2,2};
int yRoad[24] = {-2,-1,0,1,2,-2,-1,0,1,2,-2,-1,1,2,-2,-1,0,1,2,-2,-1,0,1,2};

filedeal::filedeal()
{
    visiualDrawP.x=0;
    visiualDrawP.y=0;
}

///获取图像波段和文件信息
void filedeal::openPathFile(QString fileName)
{
    qDebug()<<fileName;
    filePathName=fileName;
    GDALAllRegister();//注册gdal驱动
    CPLSetConfigOption( "GDAL_FILENAME_IS_UTF8", "YES" );//中文路径中文名称支持
    gdalData = ( GDALDataset* )GDALOpen(fileName.toStdString().c_str(), GA_ReadOnly );//打开文件
    if ( gdalData == NULL )
    {
        qDebug()<<"ERROR";
        return ;
    }
    Band=0;
    Samples=0;
    Lines=0;
    Band=gdalData->GetRasterCount();
    Samples=gdalData->GetRasterXSize();
    Lines=gdalData->GetRasterYSize();
    qDebug()<<"Band"<<Band;
    qDebug()<<"Samples"<<Samples;
    qDebug()<<"Lines"<<Lines;

    GDALClose(gdalData);
    emit bandToUi(Band);
    emit sendSize(Samples,Lines);

    if(Samples>Lines)
    {
        scale=(double)(QApplication::desktop()->height()/4)/Lines;
    }
    else
    {
        scale=(double)(QApplication::desktop()->width()/12)/Samples;
    }
    QSqlQuery query;
    QString sqlInsertStr=QString("insert into RemoteSensingImage values('%1')").arg(fileName);
    query.exec(sqlInsertStr);
}
///获得rgb信息并显示图片
void filedeal::getRGB(int r, int g, int b)
{
    openFile(r,g,b);
    point p;
    p.x=0;
    p.y=0;
    if(Samples>500&&Lines>500)
    {
        visiualdraw(p,500,500,rawImage);
    }
    else if(Samples>500&&Lines<500)
    {
        currentWidth=Lines;
        visiualdraw(p,500,Lines,rawImage);
    }
    else if(Samples<500&&Lines>500)
    {
        currentWidth=Samples;
        visiualdraw(p,Samples,500,rawImage);
    }
    else
    {
        currentWidth=Samples;
        currentHeight=Lines;
        visiualdraw(p,Samples,Lines,rawImage);
    }

    sorted=false;
}
////得到鼠标位置画框并刷新
void filedeal::getMouse(int x, int y)
{
    if(sorted==true||selectEnable==true)
    {
        point p;
        p.x=x*(1/scale);
        p.y=y*(1/scale);
        visiualdraw(p,currentHeight,currentWidth,partImage);
    }
    else if(selectEnable==false)
    {
        point p;
        p.x=x*(1/scale);
        p.y=y*(1/scale);
        visiualdraw(p,currentHeight,currentWidth,rawImage);
    }
}


///返回原始数据
float ** filedeal::openFile()
{
    GDALAllRegister();//注册gdal驱动
    CPLSetConfigOption( "GDAL_FILENAME_IS_UTF8", "YES" );//中文路径中文名称支持
    gdalData = ( GDALDataset* )GDALOpen(filePathName.toStdString().c_str(), GA_ReadOnly );//打开文件
    if ( gdalData == NULL )
    {
        qDebug()<<"ERROR";
        return NULL;
    }
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
        qDebug()<<"ERROR out";
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


///显示图片
void filedeal::openFile(int r,int g,int b)
{
    if(r>Band||g>Band||b>Band)
    {
        r=1,g=2,b=3;
    }
    GDALAllRegister();//注册gdal驱动
    CPLSetConfigOption( "GDAL_FILENAME_IS_UTF8", "YES" );//中文路径中文名称支持
    gdalData = ( GDALDataset* )GDALOpen(filePathName.toStdString().c_str(), GA_ReadOnly );//打开文件

    if ( gdalData == NULL )
    {
        qDebug()<<"ERROR";
        return;
    }
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
    int picWidth = bandList.at( 0 )->GetXSize();
    int picHeight = bandList.at( 0 )->GetYSize();

    unsigned char *processData_band1;
    unsigned char *processData_band2;
    unsigned char *processData_band3;
    float *tempRaw_band1=new float [picHeight*picWidth];
    float *tempRaw_band2=new float [picHeight*picWidth];
    float *tempRaw_band3=new float [picHeight*picWidth];
    bandList.at( r )->RasterIO( GF_Read, 0, 0, picWidth, picHeight, tempRaw_band1, picWidth, picHeight, GDT_Float32, 0, 0 );
    bandList.at( g )->RasterIO( GF_Read, 0, 0, picWidth, picHeight, tempRaw_band2, picWidth, picHeight, GDT_Float32, 0, 0 );
    bandList.at( b )->RasterIO( GF_Read, 0, 0, picWidth, picHeight, tempRaw_band3, picWidth, picHeight, GDT_Float32, 0, 0 );
    processData_band1= picSketch( tempRaw_band1, bandList.at( r ), picWidth * picHeight, bandList.at( r )->GetNoDataValue() );
    processData_band2= picSketch( tempRaw_band2, bandList.at( g ), picWidth * picHeight, bandList.at( g )->GetNoDataValue() );
    processData_band3= picSketch( tempRaw_band3, bandList.at( b ), picWidth * picHeight, bandList.at( b )->GetNoDataValue() );

    GDALClose(gdalData);
    delete[] tempRaw_band1;
    delete[] tempRaw_band2;
    delete[] tempRaw_band3;
    tempRaw_band1=NULL;
    tempRaw_band2=NULL;
    tempRaw_band3=NULL;
    QImage image(Samples, Lines, QImage::Format_RGB32);
    QRgb value;
    image.fill(Qt::white);//将图片背景填充为白色
    //以下r，g，b均不是形参的rgb意义

    for( int y=0;y<Lines;y++)
    {
        for(int x=0;x<Samples;x++)
        {
            r=processData_band1[y * Samples + x];
            g=processData_band2[y * Samples + x];
            b=processData_band3[y * Samples + x];
            value = qRgb(r, g, b);
            image.setPixel(x,y,value);
        }
    }
    rawImage=image;
    delete[] processData_band1;
    delete[] processData_band2;
    delete[] processData_band3;
    processData_band1=NULL;
    processData_band2=NULL;
    processData_band3=NULL;

}
////图像线性拉伸(将rgb转化到0-255内)
unsigned char* filedeal::picSketch( float* buffer , GDALRasterBand* currentBand, int bandSize, double noValue )
{
    unsigned char* normalBand = new unsigned char[bandSize];
    double max, min;
    double minmax[2];

    currentBand->ComputeRasterMinMax( 1, minmax );//当前波段的最大和最小值
    min = minmax[0];//最小值
    max = minmax[1];//最大值
    if( min <= noValue && noValue <= max )// noValue是图像中的异常值即不在0-255范围内的
    {
        min = 0;
    }
    for ( int i = 0; i < bandSize; i++ )
    {
        if ( buffer[i] > max )
        {
            normalBand[i] = 255;
        }
        else if (min<=buffer[i]&&buffer[i] <= max  )
        {
            normalBand[i] = static_cast<uchar>( 255 - 255 * ( max - buffer[i] ) / ( max - min ) );
        }
        else
        {
            normalBand[i] = 0;
        }
    }
    return normalBand;//返回经过拉伸的8位图像缓存
}

///显示小红框及略缩图
/// p:鼠标坐标点
/// visiualHeight视区高度
/// visiualWidth视区宽度
void filedeal::visiualdraw(point p,int visiualHeight,int visiualWidth,QImage visiualImage)
{
    int startX=0,startY=0;
    QRgb value;
    QImage image=visiualImage;
    int r=255, g=0, b=0;//用来接收颜色
    value = qRgb(r, g, b);
    visiualDrawP=p;
    //得到鼠标所在点的坐标
    //如果x小于25则x的起始点为0
    if(p.x<=visiualWidth/2)
    {
        startX=0;
    }
    else if(p.x>(Samples-visiualWidth/2))
    {
        startX=Samples-visiualWidth-1;
    }
    else
    {
        startX=p.x-visiualWidth/2;
    }
    //如果y小于25则y的起始点为0
    if(p.y<=visiualHeight/2)
    {
        startY=0;
    }
    else if(p.y>(Lines-visiualHeight/2))
    {
        startY=Lines-visiualHeight-1;
    }
    else
    {
        startY=p.y-visiualHeight/2;
    }

    //如果x>=25 and y>=25 x=x-25,y=y-25 得到矩形可视框的起点坐标
    //load_image(startX,startY);///区域显示
    //显示大图
    loadPartImage(startX,startY,visiualHeight,visiualWidth,visiualImage);
    //startY,startX;//起点的样本位置
    //将一维坐标转换成二维坐标

    int x=startX;
    int y=startY;
    int w=0;
    while(w<visiualWidth)
    {
        for(int i=0;i<1/scale;i++)
        {
            image.setPixel(x, y+i, value);
        }
        for(int i=1/scale;i>0;i--)
        {
            image.setPixel(x, y+visiualHeight-i, value);
        }
        x++;
        w++;
    }
    w=0;
    x=startX;
    y=startY;
    while(w<visiualHeight)
    {

        for(int i=0;i<1/scale;i++)
        {
            image.setPixel(x+i, y, value);
        }
        for(int i=1/scale;i>0;i--)
        {
            image.setPixel(x+visiualWidth-i, y, value);
        }
        y++;
        w++;
    }

    //矩形可视框的终点坐标是x=x+50,y=y+50
    //显示文件
    visiualX=startX;
    visiualY=startY;
    emit sendVisiualP(startX,startY);
    emit sendImageToUi(image,1);
}
/////////image.color(pixelIndex(x,y));
void filedeal::loadPartImage(int startX,int startY,int visiualHeight,int visiualWidth,QImage visiualImage)//图像细节显示模块
{
    QImage image(visiualWidth, visiualHeight, QImage::Format_RGB32);
    QColor value;
    image.fill(Qt::white);//将图片背景填充为白色

    //startY,startX;//起点的样本位置
    //将一维坐标转换成二维坐标
    //显示文件pixelIndex(int x, int y)不支持32-bppimage
    //详情看QT文档
    //qDebug()<<  MainImage.pixelColor(startX,startY);
    int x=0;
    int y=0;
    for(int h=startY;h<startY+visiualHeight;h++)
    {

        for(int w=startX;w<startX+visiualWidth;w++)
        {
            value=visiualImage.pixelColor(w,h);
            image.setPixelColor(x%visiualWidth,y,value);
            x++;
        }
        y++;
    }
    emit sendImageToUi(image,2);
}


////显示框resize事件处理
void filedeal::sizeChange(int height, int width)
{
    currentHeight=height;
    currentWidth=width;

    if(selectEnable||sorted)
    {
        visiualdraw(visiualDrawP,height,width,partImage);
    }
    else
    {
        visiualdraw(visiualDrawP,height,width,rawImage);
    }
}
///判断点是否在区域中核心算法
//point_number 是点的数量,mpoints是点的坐标集合，testx和testy为要判断的点坐标
int filedeal::judgeArea (int point_number,Points mpoints, int testx, int testy)
{
    int i, j, c = 0;
    for (i = 0, j = point_number-1; i < point_number; j = i++) {
        if ( ( (mpoints[i].y>testy) != (mpoints[j].y>testy) ) &&
             (testx <(mpoints[j].x-mpoints[i].x) * (testy-mpoints[i].y) / (mpoints[j].y-mpoints[i].y) + mpoints[i].x) )
            c = !c;
    }
    return c;
}


////样本选择前的初始化（仅初始化一次）分配内存
void filedeal::startSelectDate()
{
    if(selectEnable==false)
    {
        partImage=rawImage;
        dataCopyArea=new Node*[6];
        for(int i=0;i<6;i++)
        {
            dataCopyArea[i]=new Node[50000];
        }
    }
    selectEnable=true;//样本选择开启
}
////显示样本选择时的线
void filedeal::getData_image(Points points)
{
    QRgb value;
    QTime time;
    time.start();

    if(points[0].x>points[1].x)
    {
        tempPoint[0].x=points[1].x;
        tempPoint[1].x=points[0].x;
    }
    else
    {
        tempPoint[0].x=points[0].x;
        tempPoint[1].x=points[1].x;
    }
    if(points[0].y>points[1].y)
    {
        tempPoint[0].y=points[1].y;
        tempPoint[1].y=points[0].y;
    }
    else
    {
        tempPoint[0].y=points[0].y;
        tempPoint[1].y=points[1].y;
    }
    //double k,b;
    //k=(double)(points[1].y-points[0].y)/(points[1].x-points[0].x);
    //b=points[0].y-points[0].x*k;
    //qDebug()<<"k:"<<k;
    //qDebug()<<"b:"<<b;
    //判断点是否在线上
    for(int h=tempPoint[0].y;h<tempPoint[1].y;h++)
    {
        for(int w=tempPoint[0].x;w<tempPoint[1].x;w++)
        {
            //if(h==qCeil(k*w+b)||h==qFloor(k*w+b))
            if(judgeArea(2,points,w,h))
            {
                value=qRgb(R[current+1],G[current+1],B[current+1]);
                partImage.setPixelColor(w,h,value);
            }
        }
    }
    qDebug()<<time.elapsed()<<"ms";
    loadPartImage(visiualX,visiualY,currentHeight,currentWidth,partImage);
}
////找能把所有点放进去的矩形,提高效率
void filedeal::findEdge(Points selectPoint,int number)
{
    samplePointNum=number;
    int i;
    for(i=0;i<number;i++)
    {
        areaPoint[i]=selectPoint[i];
    }
    int minx;
    int miny;
    int maxx;
    int maxy;
    //找x的最小值
    minx=selectPoint[0].x;
    for(i=0;i<number;i++)
    {
        //        qDebug()<<selectPoint[i].x;
        if(minx>selectPoint[i].x)
            minx=selectPoint[i].x;
    }
    tempPoint[0].x=minx;
    //找y的最小值
    miny=selectPoint[0].y;
    for(i=0;i<number;i++)
    {
        if(miny>selectPoint[i].y)
            miny=selectPoint[i].y;
    }
    tempPoint[0].y=miny;
    //找x的最大值
    maxx=selectPoint[0].x;
    for(i=0;i<number;i++)
    {
        if(maxx<selectPoint[i].x)
            maxx=selectPoint[i].x;
    }
    tempPoint[1].x=maxx;
    //找y的最大值
    maxy=selectPoint[0].y;
    for(i=0;i<number;i++)
    {
        if(maxy<selectPoint[i].y)
            maxy=selectPoint[i].y;
    }
    tempPoint[1].y=maxy;
    getAimData(tempPoint,openFile());
}
////从文件获取数据
void filedeal::getAimData(Points mPoints,float **data)
{
    QTime time;
    time.start();
    QRgb value;
    value = qRgb(R[current+1], G[current+1], B[current+1]);
    //传入待测试的点
    for(int h=mPoints[0].y;h<mPoints[1].y;h++)
    {
        for(int w=mPoints[0].x;w<mPoints[1].x;w++)
        {
            if(judgeArea(samplePointNum,areaPoint,w,h))//判断点是否在区域中
            {
                //样本在区域中
                partImage.setPixel(w, h, value);
                for(int i=0;i<Band;i++)
                {
                    dataCopyArea[current][pointCount[current]].bands[i]=data[i][h*Samples+w];
                }
                pointCount[current]++;
            }
        }
    }
    qDebug()<<time.elapsed()<<"ms";

    visiualdraw(visiualDrawP,currentHeight,currentWidth,partImage);
    emit dataDetails(pointCount[current]);
    for(int i=0;i<Band;i++)
    {
        delete []data[i];
        data[i]=NULL;
    }
    delete[] data;
    data=NULL;
}
void filedeal::saveSample(QString name)
{
    //1、名字
    //2、样本一数量
    //3、样本二数量
    //4、样本三数量
    //5、样本四数量
    //6、样本五数量
    //7、样本六数量

    QString simpleName=filePathName+name;

    QFile infoWrite(simpleName.toStdString().c_str());
    infoWrite.open(QIODevice::WriteOnly|QIODevice::Append);
    QTextStream infoOut(&infoWrite);
    QString infoContent;
    simpleName.append("data");

    infoContent.append(simpleName);
    infoContent.append(",");
    for(int i=0;i<6;i++)
    {
        infoContent.append(QString::number(pointCount[i]));
        infoContent.append(",");
    }
    infoOut<<infoContent;  //输出
    infoWrite.close();

    QFile dataWrite(simpleName.toStdString().c_str());
    dataWrite.open(QIODevice::WriteOnly|QIODevice::Append);
    QTextStream dataOut(&dataWrite);
    QString dataContent;
    for(int i=0;i<6;i++)//第i个样本
    {
        for(int j=0;j<pointCount[i];j++)//第j个点
        {
            for(int k=0;k<Band;k++)//第k个波段
            {
                dataContent.append(QString::number(dataCopyArea[i][j].bands[k]));
                dataContent.append(",");
            }
        }
    }
    dataOut<<dataContent;
    dataWrite.close();
}
/**
 * @brief filedeal::simpleInfo
 * @param name
 * @param number
 * 读取完样本后直接分类
 */
void filedeal::simpleInfo(QString name, int *number)
{
    int geoNumber=0;
    for(int i=0;i<6;i++)
    {
        if(number[i]!=0)
        {
            geoNumber++;
        }
    }
    for(int i=0;i<6;i++)
    {
        if(number[i]!=0)
        {
            emit fileDataDetails(number[i],geoNumber);//将样本数量发送到UI
        }
        else
        {
            qDebug()<<"this data is null";
        }
    }

    QFile read(name.toStdString().c_str());
    read.open(QFile::ReadOnly);
    QTextStream in(&read);
    QString oldString;
    in>>oldString;
    QStringList dataList=oldString.split(",");
    int count=0;
    simples data=new Node[300000];
    QString tempStr;
    for(int i=0;i<dataList.size()-1;i++)
    {
        tempStr=dataList.at(i);
        data[count].bands[i%Band]=tempStr.toUInt();
        if(i%Band==Band-1)
        {
            count++;
        }
    }
    qDebug()<<"data size is"<<dataList.size();
    read.close();
    emit sendData(data,count);
}

void filedeal::lineMouse(int x, int y)
{
    if(combineFlag==true)
    {
        moveLine(visiualX+x,visiualY+y);
        //发送标准点坐标
    }
}
/**
 * @brief filedeal::startSort
 * 开始分类
 */
void filedeal::startSort()
{
    int count=0;
    simples data=new Node[300000];
    for(int i=0;i<6;i++)
    {
        for(int j=0;j<pointCount[i];j++)
        {
            data[count]=dataCopyArea[i][j];
            count++;
        }
    }
    delete[] dataCopyArea;
    dataCopyArea=NULL;
    selectEnable=false;
    emit sendData(data,count);
}
////改变currentCount
void filedeal::currentFunction(int currentCount)
{
    current=currentCount;
}
////删除下标为index的样本
void filedeal::deleteDataFun(int currentIndex)
{
    current=currentIndex;
    pointCount[current]=0;
    qDebug()<<"currentIndex:"<<currentIndex;
}
////解析分类结果，将图片变色
void filedeal::parser(QString ruleName)
{
    sorted=true;
    float **rawData=NULL;
    rawData=openFile();
    if(!rawImage.isNull())
    {
        partImage=rawImage;
    }
    else
    {
        openFile(1,2,3);
        partImage=rawImage;
    }

    QRgb value;
    QVector<QString>str(30);
    QVector<QStringList>strList(30);
    QFile read(ruleName.toStdString().c_str());
    read.open(QFile::ReadOnly);
    QTextStream in(&read);
    QString oldString;
    in>>oldString;
    QStringList List=oldString.split("#end#");
    emit sendFeature(List);
    for(int i=0;i<List.size()-1;i++)
    {
        str[i]=List.at(i);
        qDebug()<<str[i];
    }

    for(int i=0;i<List.size()-1;i++)
    {
        strList[i]=str[i].split(",");
    }

    //字符串已经全部解析
    QString tempStr;
    int bandI[30],bandJ[30],bandK[30];
    int compare[30];
    double Threshold[30];
    int geoName[30];
    int i=0,j=0,p=0,q=0,flag[30]={0};
    bool thisbool=true;
    int geoNameCount=0;
    for(i=0;i<strList.size();i++)
    {
        q=0;
        for(j=0;j<strList[i].size()-7;j++)
        {
            tempStr=strList[i].at(j);
            if((j+1)%6==1)
            {
                bandI[q]=tempStr.toInt(); //bandi
            }
            if((j+1)%6==2)
            {
                bandJ[q]=tempStr.toInt();  //bandj
            }
            if((j+1)%6==3)
            {
                bandK[q]=tempStr.toInt();//bandk
            }
            if((j+1)%6==4)
            {
                QString str=strList[i].at(j+6);
                compare[q]=str.toInt();//>or<   要用j+5来计算
            }
            if((j+1)%6==5)
            {
                Threshold[q]=tempStr.toDouble(); //Threshold
                q++;
            }
            if((j+1)%6==0)
            {
                qDebug()<< bandI[q-1]<<"i";
                qDebug()<< bandJ[q-1]<<"j";
                qDebug()<< bandK[q-1]<<"k";
                qDebug()<<compare[q-1]<<"compare";
                qDebug()<<  Threshold[q-1]<<"  Threshold[j]";
            }
            //            qDebug()<<strList[i].at(j);
        }
        if(j!=0)
        {
            QString geoStr=strList[i].at(j+6);
            geoName[geoNameCount]=geoStr.toInt();
            qDebug()<<geoName[geoNameCount]<<"geoName";
            qDebug()<<"--------------------------------";
            for(int k=0;k<Samples*Lines;k++)
            {
                memset(flag,0,30*sizeof(int));
                for(p=0;p<q;p++)
                {
                    if(bandJ[p]!=-1&&bandK[p]!=-1&&compare[p]==-2)
                    {
                        if((rawData[bandI[p]][k]+rawData[bandK[p]][k])*1.0/2-rawData[bandJ[p]][k]<Threshold[p])
                            //                        if((rawData[k].bands[bandI[p]]+rawData[k].bands[bandK[p]])*1.0/2-rawData[k].bands[bandJ[p]]<Threshold[p])
                        {
                            flag[p]=1;
                        }else
                        {
                            flag[p]=0;
                        }
                    }
                    else if(bandJ[p]!=-1&&bandK[p]!=-1&&compare[p]==2)
                    {
                        if((rawData[bandI[p]][k]+rawData[bandK[p]][k])*1.0/2-rawData[bandJ[p]][k]>Threshold[p])
                            //                        if((rawData[k].bands[bandI[p]]+rawData[k].bands[bandK[p]])*1.0/2-rawData[k].bands[bandJ[p]]>Threshold[p])
                        {
                            flag[p]=1;
                        }else
                        {
                            flag[p]=0;
                        }
                    }
                    else if(bandJ[p]!=-1&&bandK[p]==-1&&compare[p]==-2)
                    {
                        if(rawData[bandI[p]][k]-rawData[bandJ[p]][k]<Threshold[p])
                            //                        if(rawData[k].bands[bandI[p]]-rawData[k].bands[bandJ[p]]<Threshold[p])
                        {
                            flag[p]=1;
                        }else
                        {
                            flag[p]=0;
                        }
                    }
                    else if(bandJ[p]!=-1&&bandK[p]==-1&&compare[p]==2)
                    {
                        if(rawData[bandI[p]][k]-rawData[bandJ[p]][k]>Threshold[p])
                            //                        if(rawData[k].bands[bandI[p]]-rawData[k].bands[bandJ[p]]>Threshold[p])
                        {
                            flag[p]=1;
                        }else
                        {
                            flag[p]=0;
                        }
                    }
                    else if(bandJ[p]==-1&&compare[p]==-2)
                    {
                        if(rawData[bandI[p]][k]<Threshold[p])
                            //                        if(rawData[k].bands[bandI[p]]<Threshold[p])
                        {
                            flag[p]=1;
                        }else
                        {
                            flag[p]=0;
                        }
                    }
                    else if(bandJ[p]==-1&&compare[p]==2)
                    {
                        if(rawData[bandI[p]][k]>Threshold[p])
                            //                        if(rawData[k].bands[bandI[p]]>Threshold[p])
                        {
                            flag[p]=1;
                        }else
                        {
                            flag[p]=0;
                        }
                    }
                }//判断是否要变色循环
                thisbool=true;
                for(p=0;p<q;p++)
                {
                    if(flag[p]!=1)
                    {
                        thisbool=false;
                    }
                }
                if(thisbool)
                {
                    value = qRgb(R[geoName[geoNameCount]], G[geoName[geoNameCount]], B[geoName[geoNameCount]]);
                    partImage.setPixel(k%Samples,k/Samples,value);
                    //                    value=qRgb(255,255,255);
                    //                    mImage.setPixel(k%Samples,k/Samples,value);
                    //                    data_copy[k].bands[changer]=R[geoName[geoNameCount]];
                    //                    data_copy[k].bands[changeg]=G[geoName[geoNameCount]];
                    //                    data_copy[k].bands[changeb]=B[geoName[geoNameCount]];
                    //                    markColor[k/Samples][k%Samples]=geoName[geoNameCount];
                }
            }//样本循环
            geoNameCount++;
        }
    }
    visiualdraw(visiualDrawP,currentHeight,currentWidth,partImage);
    for(int i=0;i<Band;i++)
    {
        delete []rawData[i];
        rawData[i]=NULL;
    }
    delete[] rawData;
    rawData=NULL;
    //    int time=0;
    //    int roadCount=0;
    //    for(time=1;time<=5;time++)
    //    {
    //        roadCount=13;
    //        for(i=Lines-3;i>=2;i--)
    //            for(j=Samples-3;j>=2;j--)
    //            {
    //                int geoID[4]={0,0,0,0},currentX,currentY;
    //                for(int f=0 ; f<24 ; f++)
    //                {
    //                    currentX = i+xRoad[f];
    //                    currentY = j+yRoad[f];
    //                    geoID[markColor[currentX][currentY]]++;

    //                    if(geoID[markColor[currentX][currentY]]==roadCount)
    //                    {
    //                        int numberthis=i*Samples+j;//二维坐标一维化
    //                        data_copy[numberthis].bands[changer]=R[markColor[currentX][currentY]];
    //                        data_copy[numberthis].bands[changeg]=G[markColor[currentX][currentY]];
    //                        data_copy[numberthis].bands[changeb]=B[markColor[currentX][currentY]];
    //                    }
    //                }
    //            }
    //    }
    //    FILE *fp;
    //    fp=fopen("D://Extend","wb");
    //    int g;
    //    //    for(j=0;j<Band;j++)
    //    //    {
    //    for(int i=0;i<Samples*Lines;i++)
    //    {
    //        if(  data_copy[i].bands[changer]==255&&
    //             data_copy[i].bands[changeg]==0&&
    //             data_copy[i].bands[changeb]==0)
    //        {
    //            g=1;
    //        }
    //       else if( data_copy[i].bands[changer]==0&&
    //                data_copy[i].bands[changeg]==255&&
    //                data_copy[i].bands[changeb]==0)
    //        {
    //            g=2;
    //        }
    //       else if(data_copy[i].bands[changer]==0&&
    //                data_copy[i].bands[changeg]==0&&
    //                data_copy[i].bands[changeb]==255)
    //        {
    //            g=3;
    //        }
    //        else{
    //            g=2;
    //        }
    //        fwrite(&g,1,1,fp);
    //        //    }
    //    }
    //    fclose(fp);
}

//////////////////////////////////海岸线提取///////////////////////////////////////////////////////
unsigned short int mark[10000][10000];   //count(max)=561875  有563行
//定义了一个二维数组，将每一个点设成一个坐标。如第一个点为change[0][0] ,以此类推。
unsigned short int longLineMark[10000][10000] ={0};//记录最长的海岸线
// 去除噪点用的图像
int flag = 12;
int x_go[8] = {1,-1,0,0,1,1,-1,-1};
int y_go[8] = {0,0,1,-1,1,-1,1,-1};

int x1_go[24] = {-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,0,0,0,0,1,1,1,1,1,2,2,2,2,2};
int y1_go[24] = {-2,-1,0,1,2,-2,-1,0,1,2,-2,-1,1,2,-2,-1,0,1,2,-2,-1,0,1,2};

int x2_go[48] = {-3,-3,-3,-3,-3,-3,-3,-2,-2,-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,1,1,1,1,1,1,1,2,2,2,2,2,2,2,3,3,3,3,3,3,3};
int y2_go[48] = {-3,-2,-1,0,1,2,3,-3,-2,-1,0,1,2,3,-3,-2,-1,0,1,2,3,-3,-2,-1,1,2,3,-3,-2,-1,0,1,2,3,-3,-2,-1,0,1,2,3,-3,-2,-1,0,1,2,3};

int x3_go[80] = {-4,-4,-4,-4,-4,-4,-4,-4,-4,-3,-3,-3,-3,-3,-3,-3,-3,-3,-2,-2,-2,-2,-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,
                 1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4};
int y3_go[80] = {-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,1,2,3,4,-4,-3,-2,-1,0,1,
                 2,3,4,-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,0,1,2,3,4};

//定义一个方向数组
int next[8][2]={{-1,1}, //右上
                {0,1},//右
                {1,1}, //右下
                {1,0},//下
                {1,-1},//左下
                {0,-1},//左
                {-1,-1},//左上
                {-1,0}};//上

long int count1,maxLine;
int *sum=NULL;
int *regist_x=NULL;
int *regist_y=NULL;

void filedeal:: seaLineGet(int *seaColor,int *landColor)
{
    sum=new int[Samples*Lines];
    regist_x=new int[Samples*Lines];
    regist_y=new int[Samples*Lines];

    QImage image(Samples, Lines, QImage::Format_RGB32);
    QRgb value;
    image.fill(Qt::white);//将图片背景填充为白色
    int final=0;
    count1=0,maxLine=0;
    memset(sum,0,Samples*Lines*sizeof(int));
    memset(regist_x ,0,Samples*Lines*sizeof(int));
    memset(regist_y ,0,Samples*Lines*sizeof(int));
    memset(mark,0,Samples*Lines*sizeof(unsigned short int));
    memset(longLineMark,0,Samples*Lines*sizeof(unsigned short int));
    int i,j,r,g,b;
    QDateTime Systemtime = QDateTime::currentDateTime();//获取系统现在的时间
    QString str = Systemtime.toString("yyyy_MM_dd_hh_mm_ss"); //设置显示格式
    QString fileStr=filePathName;
    fileStr=fileStr+str;

    int areaAR=0,areaAG=0,areaAB=0,areaBR=0,areaBG=0,areaBB=0;
    for(int k=0;k<5;k++)
    {
        switch(landColor[k])
        {
        case 1:
            areaAR=255;
            areaAG=0;
            areaAB=0;
            break;
        case 2:
            areaAR=255;
            areaAG=255;
            areaAB=0;
            break;
        case 3:
            areaAR=0;
            areaAG=0;
            areaAB=255;
            break;
        case 4:
            areaAR=0;
            areaAG=255;
            areaAB=0;
            break;
        case 5:
            areaAR=139;
            areaAG=0;
            areaAB=139;
            break;
        case 6:
            areaAR=0;
            areaAG=0;
            areaAB=0;
            break;
        default:
            areaAR=300;
            areaAG=300;
            areaAB=300;
            break;
        }
        switch(seaColor[k])
        {
        case 1:
            areaBR=255;
            areaBG=0;
            areaBB=0;
            break;
        case 2:
            areaBR=255;
            areaBG=255;
            areaBB=0;
            break;
        case 3:
            areaBR=0;
            areaBG=0;
            areaBB=255;
            break;
        case 4:
            areaBR=0;
            areaBG=255;
            areaBB=0;
            break;
        case 5:
            areaBR=139;
            areaBG=0;
            areaBB=139;
            break;
        case 6:
            areaBR=0;
            areaBG=0;
            areaBB=0;
            break;
            areaBR=300;
            areaBG=300;
            areaBB=300;
        }
        qDebug()<<areaAR<<"    "<<areaAG<<"   "<<areaAB<<"A";
        qDebug()<<areaBR<<"    "<<areaBG<<"   "<<areaBB<<"B";
        for(int h=0;h<Lines;h++)
        {
            for(int w=0;w<Samples;w++)
            {
                if(QColor(partImage.pixel(w,h)).red()==areaAR
                        &&QColor(partImage.pixel(w,h)).green()==areaAG
                        &&QColor(partImage.pixel(w,h)).blue()==areaAB)
                {
                    mark[h][w]=1;//长宽对调
                }
                else if(QColor(partImage.pixel(w,h)).red()==areaBR
                        &&QColor(partImage.pixel(w,h)).green()==areaBG
                        &&QColor(partImage.pixel(w,h)).blue()==areaBB)
                {
                    mark[h][w]=2;//长宽对调
                }
            }
        }
        //        for(i=0;i<Samples*Lines;i++)
        //        {
        //            if(data_copy[i].bands[changer]==areaAR&&data_copy[i].bands[changeg]==areaAG&&data_copy[i].bands[changeb]==areaAB)
        //            {
        //                mark[i/Samples][(i-1)%Samples]=1;
        //            }
        //            else if(data_copy[i].bands[changer]==areaBR&&data_copy[i].bands[changeg]==areaBG&&data_copy[i].bands[changeb]==areaBB)
        //            {
        //                mark[i/Samples][(i-1)%Samples]=2;
        //            }
        //            else
        //            {
        //                mark[i/Samples][(i-1)%Samples]=1;
        //            }
        //        }
    }
    //淤泥与养殖区的交界线是海岸线或海水与养殖区的交界线是海岸线
    qDebug()<<"normal in mark";
    /***************************去噪声点 9宫格*********************************************/
    /* int time=0;
     for(time=1;time<=5;time++) {
      if(time<=2)flag=4;
      else flag=5;
    for(i=1123;i>=1;i--)
        for(j=1994;j>=1;j--) {

        int diw[4]={0,0,0,0},x_,y_,f;
        for(int f=0 ; f<8 ; f++)
        {
            x_ = i+x_go[f];
            y_ = j+y_go[f];
            diw[change[x_][y_]]++;

            if(diw[change[x_][y_]]==flag)
            {
                change[i][j] = change[x_][y_];

            }
        }
            }
           }        */
    /*****************************25宫格**************************************/
    //    int time=0;
    //    for(time=1;time<=2;time++)
    //    {
    //        flag=13;
    //        for(i=Lines-2;i>=1;i--)
    //            for(j=Samples-2;j>=1;j--)
    //            {
    //                int diw[4]={0,0,0,0},x_,y_;
    //                for(int f=0 ; f<24 ; f++)
    //                {
    //                    x_ = i+x1_go[f];
    //                    y_ = j+y1_go[f];
    //                    diw[mark[x_][y_]]++;
    //                    if(diw[mark[x_][y_]]==flag)
    //                    {
    //                        mark[i][j] = mark[x_][y_];
    //                    }
    //                }
    //            }
    //    }
    int time=0;
    for(time=1;time<=1;time++)
    {
        flag=42;
        for(i=Lines-5;i>=4;i--)
            for(j=Samples-5;j>=4;j--)
            {

                int diw[4]={0,0,0,0},x_,y_;
                for(int f=0 ; f<80 ; f++)
                {
                    x_ = i+x3_go[f];
                    y_ = j+y3_go[f];
                    diw[mark[x_][y_]]++;

                    if(diw[mark[x_][y_]]==flag)
                    {
                        mark[i][j] = mark[x_][y_];
                    }
                }
            }
    }

    qDebug()<<"normal in down point";
    /******************************************49宫格***************************************/
    //    int time=0;
    //    for(time=1;time<=1;time++)
    //    {
    //        flag=24;
    //        for(i=1121;i>=3;i--)
    //            for(j=1992;j>=3;j--)
    //            {

    //                int diw[4]={0,0,0,0},x_,y_;
    //                for(int f=0 ; f<48 ; f++)
    //                {
    //                    x_ = i+x2_go[f];
    //                    y_ = j+y2_go[f];
    //                    diw[mark[x_][y_]]++;

    //                    if(diw[mark[x_][y_]]==flag)
    //                    {
    //                        mark[i][j] = mark[x_][y_];
    //                    }
    //                }
    //            }

    //    }

    /****************************修改数据（寻找符合规则的线）************************************************/
    for(i=1;i<=Lines-2;i++)
    {
        for(j=1;j<=Samples-2;j++)
        {
            if((mark[i][j]==2&&mark[i-1][j-1]==1)||
                    (mark[i][j]==2&&mark[i-1][j+1]==1)||
                    (mark[i][j]==2&&mark[i-1][j]==1)||
                    (mark[i][j]==2&&mark[i+1][j-1]==1)||
                    (mark[i][j]==2&&mark[i+1][j]==1)||
                    (mark[i][j]==2&&mark[i+1][j+1]==1)||
                    (mark[i][j]==2&&mark[i][j-1]==1)||
                    (mark[i][j]==2&&mark[i][j+1]==1) )
            {
                longLineMark[i][j]=1;           //   chang1=1记录的是符合规则的线
                //                                r=0;b=0;g=0; //黑
                //                                value = qRgb(r, g, b);
                //                                image.setPixel(j,i, value);
            }
        }
    }//for
    //        fileStr.append(".tif");
    //        image.save(fileStr);

    //    for(i=2;i<=Lines-2;i++)
    //    {
    //        for(j=2;j<=Samples-2;j++)
    //        {
    //            if((mark[i][j]==2&&mark[i-1][j-1]==1)||
    //                    (mark[i][j]==2&&mark[i-1][j+1]==1)||
    //                    (mark[i][j]==2&&mark[i-1][j]==1)||
    //                    (mark[i][j]==2&&mark[i+1][j-1]==1)||
    //                    (mark[i][j]==2&&mark[i+1][j]==1)||
    //                    (mark[i][j]==2&&mark[i+1][j+1]==1)||
    //                    (mark[i][j]==2&&mark[i][j-1]==1)||
    //                    (mark[i][j]==2&&mark[i][j+1]==1)||//以后为新加
    //                    (mark[i][j]==2&&mark[i-2][j-2]==1)||
    //                    (mark[i][j]==2&&mark[i-2][j-1]==1)||
    //                    (mark[i][j]==2&&mark[i-2][j]==1)||
    //                    (mark[i][j]==2&&mark[i-2][j+1]==1)||
    //                    (mark[i][j]==2&&mark[i-2][j+1]==1)||
    //                    (mark[i][j]==2&&mark[i-2][j+2]==1)||
    //                    (mark[i][j]==2&&mark[i-1][j+2]==1)||
    //                    (mark[i][j]==2&&mark[i][j+2]==1)||
    //                    (mark[i][j]==2&&mark[i+1][j+2]==1)||
    //                    (mark[i][j]==2&&mark[i+2][j+2]==1)||
    //                    (mark[i][j]==2&&mark[i+2][j+1]==1)||
    //                    (mark[i][j]==2&&mark[i+2][j]==1)||
    //                    (mark[i][j]==2&&mark[i+2][j-1]==1)||
    //                    (mark[i][j]==2&&mark[i+2][j-2]==1)||
    //                    (mark[i][j]==2&&mark[i+1][j-2]==1)||
    //                    (mark[i][j]==2&&mark[i][j-2]==1)||
    //                    (mark[i][j]==2&&mark[i-1][j-2]==1) )
    //            {
    //                longLineMark[i][j]=1;           //   chang1=1记录的是符合规则的线
    //            }
    //        }
    //    }
    qDebug()<<"longLineMark complete";

    for(i=1;i<=Lines-2;i++)
    {
        for(j=1;j<=Samples-2;j++)
        {
            mark[i][j]=0;
        }
        //此时的change是book
    }   //寻找最长的线


    /******************************************修改数据************************************************************************/
    qDebug()<<"before dfs";

    for(i=1;i<=Lines-2;i++)
    {
        for(j=1;j<=Samples-2;j++)
        {
            count1++;//用作计数的
            regist_x[count1]=i;
            regist_y[count1]=j;
            mark[i][j]=1;   //此时的change是book
            dfs(i,j);
        }
    }

    qDebug()<<count1<<"count1";
    qDebug()<< regist_x[count1]<<"regist_x[count1]";
    qDebug()<< regist_y[count1]<<"regist_x[count1]";
    qDebug()<<maxLine<<"max";

    maxLine=0;
    for(i=1;i<=count1;i++)
    {
        if(maxLine<sum[i])
        {
            maxLine=sum[i];
            final=i;
        }
    }

    qDebug()<<final<<"final";

    for(i=1;i<=Lines-2;i++)
    {
        for(j=1;j<=Samples-2;j++)
        {
            mark[i][j]=0;//change重置为0，在画最长的过程中依旧为book
        }
    }

    qDebug()<<regist_x[final]<<"regist_x[final]";
    qDebug()<<regist_y[final]<<"regist_y[final]";

    mark[regist_x[final]][regist_y[final]]=1;
    longLineMark[regist_x[final]][regist_y[final]]=2;     //change1=2记录的是最长的线
    redfs(regist_x[final],regist_y[final]);

    for(i=1;i<=Lines-2;i++)
    {
        for(j=1;j<=Samples-2;j++)
        {
            if(longLineMark[i][j]!=2)
            {
                longLineMark[i][j]=0;
            }
        }
    }


    for(i=1;i<=Lines-2;i++)
    {
        for(j=1;j<=Samples-2;j++)
        {
            if(longLineMark[i][j]==2 )
            {
                r=0;b=0;g=0; //黑
                value = qRgb(r, g, b);
                image.setPixel(j,i, value);
            }
        }
    }
    partImage=image;
    visiualdraw(visiualDrawP,currentHeight,currentWidth,partImage);
    fileStr.append(".tif");
    image.save(fileStr);
    QSqlQuery query;
    QString sqlInsertStr=QString("insert into RemoteSensingSeaLine values('%1','%2')").arg(fileStr).arg(filePathName);
    query.exec(sqlInsertStr);
    delete[] sum;
    delete[] regist_x;
    delete[] regist_y;
}
void filedeal::dfs(int x,int y)
{

    int k,tx,ty;

    for(k=0;k<=7;k++)
    {
        tx=x+next[k][0];
        ty=y+next[k][1];
        //判断是否越界
        if(tx<1||tx>Lines-1||ty<1||ty>Samples-1)
            continue;
        if(longLineMark[tx][ty]==1&&mark[tx][ty]==0)   //此时的change是book
        {
            sum[count1]++;	//将每根线的线长存到sum数组中
            mark[tx][ty]=1;
            dfs(tx,ty);
        }
    }
    return;

}
void filedeal:: redfs(int x,int y)
{
    int k,tx,ty;

    for(k=0;k<=7;k++)
    {
        tx=x+next[k][0];
        ty=y+next[k][1];
        //判断是否越界
        if(tx<1||tx>Lines-1||ty<1||ty>Samples-1)
            continue;
        if(longLineMark[tx][ty]>0&&mark[tx][ty]==0)   //此时的change是book
        {
            mark[tx][ty]=1;
            longLineMark[tx][ty]=2;
            redfs(tx,ty);
        }
    }
    return;

}
///////////////////////////////////////////////海岸线完/////////////////////////////////////////////////
////开始降噪
void filedeal::lowPointsStart()
{
    /*if(mytempChange==true){
        for(int i=0;i<Samples*Lines;i++)
        {
            if(rawData[i].bands[changer]==1)
            {
                data_copy[i].bands[0]=255;
                data_copy[i].bands[1]=0;
                data_copy[i].bands[2]=0;
            }
            if(rawData[i].bands[changer]==2)
            {
                data_copy[i].bands[0]=255;
                data_copy[i].bands[1]=255;
                data_copy[i].bands[2]=0;
            }
            if(rawData[i].bands[changer]==3)
            {
                data_copy[i].bands[0]=0;
                data_copy[i].bands[1]=0;
                data_copy[i].bands[2]=255;
            }
            if(rawData[i].bands[changer]==4)
            {
                data_copy[i].bands[0]=0;
                data_copy[i].bands[1]=255;
                data_copy[i].bands[2]=0;
            }
        }
        mytempChange=false;
    }*/

    for(int time=0;time<2;time++)
    {

        for(int i=18;i>=1;i--)
        {
            lowBadPoints(i*5,i*5);
        }
        for(int i=1;i<=18;i++)
        {
            lowBadPoints(i*5,i*5);
        }
        visiualdraw(visiualDrawP,currentHeight,currentWidth,partImage);
    }
}
////降噪函数
void filedeal::lowBadPoints(int w,int h){
    qDebug()<<"lowPoints in";
    //    int h=Lines/hNumber;//降噪块的高度
    //    int w=Samples/wNumber;//降噪块的宽度
    QRgb value;
    int hNumber=Lines/h;//降噪块的高度方向的数量
    int wNumber=Samples/w;//降噪块的宽度方向的数量
    int boxNumber=0;
    int color[7]={0};
    int j=0,i=0;
    for( j=0;j<hNumber;j++)
    {
        for( i=0;i<wNumber;i++)
        {
            //每个小格子开始的x坐标和y坐标
            for(int c=0;c<6;c++)
            {
                color[c]=0;//初始化color
            }
            boxNumber=0;
            //boxNumber=0;//初始化每个小格子的点的数量
            for(int y=j*h;y<(j+1)*h;y++)
            {
                for(int x=i*w;x<(i+1)*w;x++)
                {
                    int centerY=(double)(j*h+(j+1)*h)/2;//圆心y坐标
                    int centerX=(double)(i*w+(i+1)*w)/2;//圆心x坐标
                    int radius=centerX-i*w;//半径
                    if((x-centerX)*(x-centerX)+(y-centerY)*(y-centerY)<=radius*radius)//点在内切圆内部
                    {
                        //                        int location=y*Samples+x;//二维坐标一维化
                        if(QColor(partImage.pixel(x,y)).red()==255&&
                                QColor(partImage.pixel(x,y)).green()==0&&
                                QColor(partImage.pixel(x,y)).blue()==0)
                        {
                            color[0]++;
                            boxNumber++;
                            //红色++
                        }
                        if(QColor(partImage.pixel(x,y)).red()==255&&
                                QColor(partImage.pixel(x,y)).green()==255&&
                                QColor(partImage.pixel(x,y)).blue()==0)
                        {
                            color[1]++;
                            boxNumber++;
                            //黄色++
                        }
                        if(QColor(partImage.pixel(x,y)).red()==0&&
                                QColor(partImage.pixel(x,y)).green()==0&&
                                QColor(partImage.pixel(x,y)).blue() ==255)
                        {
                            color[2]++;
                            boxNumber++;
                            //蓝色++
                        }
                        if(QColor(partImage.pixel(x,y)).red()==0&&
                                QColor(partImage.pixel(x,y)).green()==255&&
                                QColor(partImage.pixel(x,y)).blue() ==0)
                        {
                            color[3]++;
                            boxNumber++;
                            //绿色++
                        }
                        if(QColor(partImage.pixel(x,y)).red()==139&&
                                QColor(partImage.pixel(x,y)).green()==0&&
                                QColor(partImage.pixel(x,y)).blue() ==139)
                        {
                            color[4]++;
                            boxNumber++;
                            //紫色++
                        }
                        if(QColor(partImage.pixel(x,y)).red()==0&&
                                QColor(partImage.pixel(x,y)).green()==0&&
                                QColor(partImage.pixel(x,y)).blue() ==0)
                        {
                            color[5]++;
                            boxNumber++;
                            //黑色++
                        }
                    }
                }

            }
            //一个小宫格的内容全部遍历
            int tempNumber=color[0];
            int tempIndex=1;
            for(int c=0;c<6;c++)
            {
                if(color[c]>tempNumber)
                {
                    tempNumber=color[c];
                    tempIndex=c+1;
                }
            }
            if(boxNumber==0)
            {
                qDebug()<<"there is /0";
                continue;
            }
            //            qDebug()<<"tempNumber/boxNumber:"<<(double)tempNumber/boxNumber<<"tempIndex:"<<tempIndex;
            int centerY=(double)(j*h+(j+1)*h)/2;//圆心y坐标
            int centerX=(double)(i*w+(i+1)*w)/2;//圆心x坐标
            int radius=centerX-i*w;//半径
            if(((double)tempNumber/boxNumber)>=0.987)//直径100时98.7基本不动边界//直径50时96基本不动边界
            {
                //                qDebug()<<tempIndex;
                for(int y=j*h;y<(j+1)*h;y++)
                {
                    for(int x=i*w;x<(i+1)*w;x++)
                    {
                        if((x-centerX)*(x-centerX)+(y-centerY)*(y-centerY)<=radius*radius)//点在内切圆内部
                        {
                            //                            int tempLocation=y*Samples+x;//二维坐标一维化
                            value = qRgb(R[tempIndex],G[tempIndex], B[tempIndex]);
                            partImage.setPixel(x,y,value);
                            //                            data_copy[tempLocation].bands[changer]=R[tempIndex];
                            //                            data_copy[tempLocation].bands[changeg]=G[tempIndex];
                            //                            data_copy[tempLocation].bands[changeb]=B[tempIndex];
                        }
                        // data_copy[tempLocation].bands[changer]=R[0];
                        // data_copy[tempLocation].bands[changeg]=G[0];
                        // data_copy[tempLocation].bands[changeb]=B[0];
                    }
                }
            }

        }
    }

    //////////////////////////////////////////////////new
    double hNumberAc=(double)Lines/h;//降噪块的高度方向的数量
    double wNumberAc=(double)Samples/w;//降噪块的宽度方向的数量
    int hDeviation=0;
    int wDeviation=0;
    if(hNumber!=hNumberAc)
    {
        hDeviation=Lines%h;
    }
    if(wNumber!=wNumberAc)
    {
        wDeviation=Samples%w;
    }
    for( j=hNumber;j>0;j--)
    {
        for( i=wNumber;i>0;i--)
        {
            //每个小格子开始的x坐标和y坐标
            for(int c=0;c<6;c++)
            {
                color[c]=0;//初始化color
            }
            boxNumber=0;
            //boxNumber=0;//初始化每个小格子的点的数量
            for(int y=j*h+hDeviation-1;y>(j-1)*h+hDeviation;y--)
            {
                for(int x=i*w+wDeviation-1;x>(i-1)*w+wDeviation;x--)
                {
                    int centerY=(double)(j*h+hDeviation+(j-1)*h+hDeviation)/2;//圆心y坐标
                    int centerX=(double)(i*w+wDeviation+(i-1)*w+wDeviation)/2;//圆心x坐标
                    int radius=centerX-(i*w+wDeviation);//半径
                    if((x-centerX)*(x-centerX)+(y-centerY)*(y-centerY)<=radius*radius)//点在内切圆内部
                    {
                        //                            int location=y*Samples+x;//二维坐标一维化
                        //                            qDebug()<<location<<"location";
                        if(QColor(partImage.pixel(x,y)).red()==255&&
                                QColor(partImage.pixel(x,y)).green()==0&&
                                QColor(partImage.pixel(x,y)).blue()==0)
                        {
                            color[0]++;
                            boxNumber++;
                            //红色++
                        }
                        if(QColor(partImage.pixel(x,y)).red()==255&&
                                QColor(partImage.pixel(x,y)).green()==255&&
                                QColor(partImage.pixel(x,y)).blue()==0)
                        {
                            color[1]++;
                            boxNumber++;
                            //黄色++
                        }
                        if(QColor(partImage.pixel(x,y)).red()==0&&
                                QColor(partImage.pixel(x,y)).green()==0&&
                                QColor(partImage.pixel(x,y)).blue() ==255)
                        {
                            color[2]++;
                            boxNumber++;
                            //蓝色++
                        }
                        if(QColor(partImage.pixel(x,y)).red()==0&&
                                QColor(partImage.pixel(x,y)).green()==255&&
                                QColor(partImage.pixel(x,y)).blue() ==0)
                        {
                            color[3]++;
                            boxNumber++;
                            //绿色++
                        }
                        if(QColor(partImage.pixel(x,y)).red()==139&&
                                QColor(partImage.pixel(x,y)).green()==0&&
                                QColor(partImage.pixel(x,y)).blue() ==139)
                        {
                            color[4]++;
                            boxNumber++;
                            //紫色++
                        }
                        if(QColor(partImage.pixel(x,y)).red()==0&&
                                QColor(partImage.pixel(x,y)).green()==0&&
                                QColor(partImage.pixel(x,y)).blue() ==0)
                        {
                            color[5]++;
                            boxNumber++;
                            //黑色++
                        }
                    }
                }

            }
            //一个小宫格的内容全部遍历
            int tempNumber=color[0];
            int tempIndex=1;
            for(int c=0;c<6;c++)
            {
                if(color[c]>tempNumber)
                {
                    tempNumber=color[c];
                    tempIndex=c+1;
                }
            }
            if(boxNumber==0)
            {
                qDebug()<<"there is /0";
                continue;
            }
            //            qDebug()<<"tempNumber/boxNumber:"<<(double)tempNumber/boxNumber<<"tempIndex:"<<tempIndex;
            int centerY=(double)(j*h+hDeviation+(j-1)*h+hDeviation)/2;//圆心y坐标
            int centerX=(double)(i*w+wDeviation+(i-1)*w+wDeviation)/2;//圆心x坐标
            int radius=centerX-(i*w+wDeviation);//半径
            if(((double)tempNumber/boxNumber)>=0.987)//直径100时98.7基本不动边界//直径50时96基本不动边界
            {
                for(int y=j*h+hDeviation-1;y>(j-1)*h+hDeviation;y--)
                {
                    for(int x=i*w+wDeviation-1;x>(i-1)*w+wDeviation;x--)
                    {
                        if((x-centerX)*(x-centerX)+(y-centerY)*(y-centerY)<=radius*radius)//点在内切圆内部
                        {
                            value = qRgb(R[tempIndex],G[tempIndex], B[tempIndex]);
                            partImage.setPixel(x,y,value);
                        }
                        // data_copy[tempLocation].bands[changer]=R[0];
                        // data_copy[tempLocation].bands[changeg]=G[0];
                        // data_copy[tempLocation].bands[changeb]=B[0];
                    }
                }
            }

        }
    }
    //        qDebug()<<i<<"i";
    //        qDebug()<<j<<"j";

}



////保存图像tif文件
void filedeal::saveTif()
{
    QDateTime Systemtime = QDateTime::currentDateTime();//获取系统现在的时间
    QString str = Systemtime.toString("yyyy_MM_dd_hh_mm_ss"); //设置显示格式
    QString fileStr=filePathName;
    fileStr=fileStr+"Rule"+str+".tif";
    partImage.save(fileStr.toStdString().c_str());
    //.toStdString().c_str()
}
////保存二进制文件
void filedeal::saveBinary()
{
    //保存二进制文件
    QDateTime Systemtime = QDateTime::currentDateTime();//获取系统现在的时间
    QString str = Systemtime.toString("yyyy_MM_dd_hh_mm_ss"); //设置显示格式
    QString fileStr=filePathName;
    fileStr=fileStr+"Binary"+str;
    FILE *fp;
    fp=fopen(fileStr.toLocal8Bit().data(),"wb");
    if(fp==NULL)
    {
        qDebug()<<"ERROR";
        return;
    }
    int g;
    for(int y=0;y<Lines;y++)
    {
        for(int x=0;x<Samples;x++)
        {
            if(QColor(partImage.pixel(x,y)).red()==255&&
                    QColor(partImage.pixel(x,y)).green()==0&&
                    QColor(partImage.pixel(x,y)).blue()==0)
            {
                g=1;
            }
            else if(QColor(partImage.pixel(x,y)).red()==255&&
                    QColor(partImage.pixel(x,y)).green()==255&&
                    QColor(partImage.pixel(x,y)).blue()==0)
            {
                g=2;
            }
            else if(QColor(partImage.pixel(x,y)).red()==0&&
                    QColor(partImage.pixel(x,y)).green()==0&&
                    QColor(partImage.pixel(x,y)).blue()==255)
            {
                g=3;
            }
            else if(QColor(partImage.pixel(x,y)).red()==0&&
                    QColor(partImage.pixel(x,y)).green()==255&&
                    QColor(partImage.pixel(x,y)).blue()==0)
            {
                g=4;
            }
            else if(QColor(partImage.pixel(x,y)).red()==139&&
                    QColor(partImage.pixel(x,y)).green()==0&&
                    QColor(partImage.pixel(x,y)).blue()==139)
            {
                g=5;
            }
            else if(QColor(partImage.pixel(x,y)).red()==0&&
                    QColor(partImage.pixel(x,y)).green()==0&&
                    QColor(partImage.pixel(x,y)).blue()==0)
            {
                g=6;
            }
            fwrite(&g,1,1,fp);
        }
    }
    fclose(fp);
}
void filedeal::moveLine(int x,int y)
{
    point standPoint;
    //    standPoint.x=x;
    //    standPoint.y=y;
    //    1650 192
    standPoint.x=x;
    standPoint.y=y;
    qDebug()<<"location"<<x<<" "<<y;
    move(loadLine(lineImage[0],lineImage[1]),standPoint,linePointNb);
    //    oldMove(loadLine(lineImage[0],lineImage[1]),standPoint,linePointNb);
}
point** filedeal::loadLine(QString imageStrF,QString imageStrS)
{
    QImage imageF;
    QImage imageS;
    imageF.load(imageStrF);
    imageS.load(imageStrS);
    Samples=imageF.width();
    Lines=imageF.height();
    qDebug()<<Samples;
    qDebug()<<Lines;
    //emit sendSize(Samples,Lines);
    //fLine是靠近海的那条海岸线
    //sLine是靠近陆地的那条海岸线

    point *fLine=NULL;
    point *sLine=NULL;

    if(Samples>Lines)
    {
        fLine=new point[Samples*10];
        sLine=new point[Samples*10];
    }
    else
    {
        fLine=new point[Lines*10];
        sLine=new point[Lines*10];
    }
    int x=0,y=0;
    for(int h=0;h<Lines;h++)
    {
        for(int w=0;w<Samples;w++)
        {
            //有线画到总图
            if(QColor(imageF.pixel(w,h)).red()==0&&
                    QColor(imageF.pixel(w,h)).green()==0&&
                    QColor(imageF.pixel(w,h)).blue()==0)
            {
                fLine[x].x=w;
                fLine[x].y=h;
                x++;
            }

            //有线画到总图
            if(QColor(imageS.pixel(w,h)).red()==0&&
                    QColor(imageS.pixel(w,h)).green()==0&&
                    QColor(imageS.pixel(w,h)).blue()==0)
            {
                sLine[y].x=w;
                sLine[y].y=h;
                y++;
            }

        }
    }
    //是否保存数组
    if(1)
    {
        FILE *fp;
        fp=fopen("D://Line1","wb");
        if(fp==NULL)
        {
            return NULL;
        }
        for(int i=0;i<x;i++)
        {
            fprintf(fp,"%d\n",fLine[i].x);
            fprintf(fp,"%d\n",fLine[i].y);
        }
        fclose(fp);

        FILE *fp1;
        fp1=fopen("D://Line1Y","wb");
        if(fp1==NULL)
        {
            return NULL;
        }
        for(int i=0;i<x;i++)
        {
            fprintf(fp1,"%d\n",sLine[i].x);
            fprintf(fp1,"%d\n",sLine[i].y);
        }
        fclose(fp1);
    }

    int min=0;
    int max=0;
    if(x>y)
    {
        min=y;
        max=x;
    }
    else
    {
        min=x;
        max=y;
    }
    linePointNb=max;
    point **lineSet=NULL;
    lineSet=new point*[2];
    lineSet[0]=fLine;
    lineSet[1]=sLine;
    return lineSet;
}
/**
 * @brief filedeal::combineLine 海岸线合并
 * @param imageStrF
 * @param imageStrS
 */
void filedeal::combineLine(QString imageStrF,QString imageStrS)
{
    combineFlag=true;
    lineImage[0]=imageStrF;
    lineImage[1]=imageStrS;
    QImage imageF;
    QImage imageS;
    imageF.load(imageStrF);
    imageS.load(imageStrS);
    Samples=imageF.width();
    Lines=imageF.height();
    qDebug()<<Samples;
    qDebug()<<Lines;
    //emit sendSize(Samples,Lines);
    //fLine是靠近海的那条海岸线
    //sLine是靠近陆地的那条海岸线

    point *fLine=NULL;
    point *sLine=NULL;

    if(Samples>Lines)
    {
        fLine=new point[Samples*10];
        sLine=new point[Samples*10];
        scale=(double)(QApplication::desktop()->height()/4)/Lines;//要想正常显示略缩图必须首先设置缩放率
    }
    else
    {
        fLine=new point[Lines*10];
        sLine=new point[Lines*10];
        scale=(double)(QApplication::desktop()->width()/12)/Samples;//要想正常显示略缩图必须首先设置缩放率
    }

    int x=0,y=0;
    for(int h=0;h<Lines;h++)
    {
        for(int w=0;w<Samples;w++)
        {
            //有线画到总图
            if(QColor(imageF.pixel(w,h)).red()==0&&
                    QColor(imageF.pixel(w,h)).green()==0&&
                    QColor(imageF.pixel(w,h)).blue()==0)
            {
                fLine[x].x=w;
                fLine[x].y=h;
                x++;
            }

            //有线画到总图
            if(QColor(imageS.pixel(w,h)).red()==0&&
                    QColor(imageS.pixel(w,h)).green()==0&&
                    QColor(imageS.pixel(w,h)).blue()==0)
            {
                sLine[y].x=w;
                sLine[y].y=h;
                y++;
            }

        }
    }
    int min=0;
    int max=0;
    if(x>y)
    {
        min=y;
        max=x;
    }
    else
    {
        min=x;
        max=y;
    }
    linePointNb=max;
    qDebug()<<"min"<<min;
    qDebug()<<"max"<<max;
    //    sLine=reorderSLine(fLine,sLine,min);
    //    for(int i=0;i<min;i++)
    //    {
    //        qDebug()<<"fLine:"<<i<<" x:"<<fLine[i].x<<" y:"<<fLine[i].y;
    //        qDebug()<<"sLine:"<<i<<" x:"<<sLine[i].x<<" y:"<<sLine[i].y<<"\n";
    //    }
    QImage image(Samples, Lines, QImage::Format_RGB32);
    QRgb valueF;
    QRgb valueS;
    QRgb valueT;
    image.fill(Qt::white);//将图片背景填充为白色
    valueF=qRgb(0,0,255);
    valueS=qRgb(255,0,0);
    valueT=qRgb(0,255,0);
    for(int i=0;i<max;i++)
    {
        image.setPixel(fLine[i].x,fLine[i].y,valueF);
        image.setPixel(sLine[i].x,sLine[i].y,valueS);

    }

    rawImage=image;
    partImage=image;
    visiualdraw(visiualDrawP,currentHeight,currentWidth,partImage);
}
/**
 * @brief filedeal::move 计算出所有移动后的点
 * @param lineSet
 * @param standPoint
 * @param number
 */
void filedeal::move(point**lineSet,point standPoint,int number)
{
    int currentIndex=0;
    point *fLineP=lineSet[0];
    point *sLineP=lineSet[1];
    sLineP=reorderSLine(fLineP,sLineP,number);//将第二条海岸线上的点数组重排序
    point *tLine=new point[number];

    QSet<int> indexSet;
    for(int i=0;i<number;i++)
    {
        indexSet.insert(i);
    }

    currentIndex=minDistanceIndex(standPoint,sLineP,number,indexSet);//获取离标准点最近的第二条海岸线上的点
    //    //小于index的
    //    for(int i=currentIndex;i>0;i--)
    //    {
    //        currentIndex=minDistanceIndex(standPoint,sLineP,number,indexSet);//获取离标准点最近的第二条海岸线上的点
    //        indexSet.erase(indexSet.find(currentIndex));//去重复点
    //        double rat=newRatio(standPoint,currentIndex,sLineP[currentIndex],fLineP);
    //        //double rat=ratio(standPoint,sLineP[currentIndex],fLineP,number);
    //        tLine[i]=calculatePoint(fLineP[i],sLineP[i],rat);
    //        if((tLine[i].x-sLineP[i].x)*(sLineP[i].x-fLineP[i].x)<0)
    //        {
    //            tLine[i]=sLineP[i];
    //        }
    //        standPoint=tLine[i];//重新指定标准点
    //    }
    //    for(int i=currentIndex;i<number;i++)
    //    {
    //        currentIndex=minDistanceIndex(standPoint,sLineP,number,indexSet);//获取离标准点最近的第二条海岸线上的点
    //        indexSet.erase(indexSet.find(currentIndex));//去重复点
    //        double  rat=newRatio(standPoint,currentIndex,sLineP[currentIndex],fLineP);
    //        tLine[i]=calculatePoint(fLineP[i],sLineP[i],rat);
    //        if((tLine[i].x-sLineP[i].x)*(sLineP[i].x-fLineP[i].x)<0)
    //        {
    //            tLine[i]=sLineP[i];
    //        }
    //        standPoint=tLine[i];//重新指定标准点
    //    }
    currentIndex=minDistanceIndex(standPoint,fLineP,number,indexSet);//获取离标准点最近的第二条海岸线上的点
    indexSet.erase(indexSet.find(currentIndex));//去重复点
    double  rat=newRatio(standPoint,currentIndex,sLineP[currentIndex],fLineP);

    for(int i=0;i<number;i++)
    {
        tLine[i]=calculatePoint(fLineP[i],sLineP[i],rat);
        if((tLine[i].x-sLineP[i].x)*(sLineP[i].x-fLineP[i].x)<0)
        {
            tLine[i]=sLineP[i];
        }
        //        standPoint=tLine[i];//重新指定标准点
    }
    QImage image(Samples, Lines, QImage::Format_RGB32);
    QRgb valueF;
    QRgb valueS;
    QRgb valueT;
    image.fill(Qt::white);//将图片背景填充为白色
    valueF=qRgb(0,0,255);
    valueS=qRgb(255,0,0);
    valueT=qRgb(0,0,0);
    for(int i=0;i<number;i++)
    {
        //        image.setPixel(fLineP[i].x,fLineP[i].y,valueF);
        //        image.setPixel(sLineP[i].x,sLineP[i].y,valueS);
        image.setPixel(tLine[i].x,tLine[i].y,valueT);
        //qDebug()<<"tLine"<<i<<"    "<<tLine[i].x<<"   "<<tLine[i].y;
    }
    rawImage=image;
    partImage=image;
    visiualdraw(visiualDrawP,currentHeight,currentWidth,image);
    //        unLinkPoint(tLine,number);
    //        rawImage=partImage;
    //        visiualdraw(visiualDrawP,currentHeight,currentWidth,partImage);
    averageLine(image,Samples,Lines);
}
/**
 * @brief filedeal::unLinkPoint 判断点是否连在海岸线上
 * @param Line
 * @param number
 * @return
 */
int * filedeal::unLinkPoint(point* Line,int number)
{
    //(x,y)
    //(x+1,y)
    //(x-1,y)
    //(x,y+1)
    //(x,y-1)
    //(x+1,y+1)
    //(x+1,y-1)
    //(x-1,y+1)
    //(x-1,y-1)
    bool flag=false;
    int count=0;
    int *index=new int[number];
    point p;
    for(int i=0;i<number;i++)
    {
        p=Line[i];
        flag=false;
        for(int j=-1;j<2;j++)
        {
            for(int k=-1;k<2;k++)
            {
                p.x=Line[i].x+j;
                p.y=Line[i].y+k;
                if(p.x!=Line[i].x&&p.y!=Line[i].y)
                {
                    if(pExistLine(p,Line,number))
                    {
                        j=2;
                        k=2;
                        flag=true;
                    }
                }
            }
        }
        if(!flag)
        {
            index[count]=i;
            count++;
        }
    }
    qDebug()<<"number"<<number;
    qDebug()<<"不在线上的点的数量是："<<count;
    for(int i=0;i<count;i++)
    {
        int lineIndex=pToLminIndex(Line[index[i]],Line,number);//返回不在海岸线上的点与海岸线上距离最近的点的index
        partImage=linkLine(partImage,Line[lineIndex],Line[index[i]]);
    }
    return index;
}
int filedeal::pToLminIndex(point alonePoint,point *Line,int number)
{
    int minIndex=0;
    double min=0;
    double temp=0;
    min=distance(alonePoint,Line[0]);
    for(int i=0;i<number;i++)
    {
        if(alonePoint.x!=Line[i].x&&alonePoint.y!=Line[i].y)
        {

            temp=distance(alonePoint,Line[i]);
            if(temp<min)
            {
                minIndex=i;
                min=temp;
            }
        }
    }
    return minIndex;
}

/**
 * @brief filedeal::linkLine 将孤立两点连接成线
 * @param image
 * @param p1 点1
 * @param p2 点2
 * @return 返回连接后的图像
 */

QImage filedeal::linkLine(QImage image,point p1,point p2)
{
    //    if(distance(p1,p2)>150)
    //    {
    //        return image;
    //    }
    point minP,maxP;
    QRgb valueF;
    valueF=qRgb(0,0,0);
    if(p1.x<p2.x)
    {
        minP.x=p1.x;
        maxP.x=p2.x;
    }
    else
    {
        minP.x=p2.x;
        maxP.x=p1.x;
    }

    if(p1.y<p2.y)
    {
        minP.y=p1.y;
        maxP.y=p2.y;
    }
    else
    {
        minP.y=p2.y;
        maxP.y=p1.y;
    }
    //    qDebug()<<"p1"<<p1.x<<"p1"<<p1.y;
    //    qDebug()<<"p2"<<p2.x<<"p2"<<p2.y<<"\n";
    //    qDebug()<<"minX"<<minP.x<<"maxX"<<maxP.x;
    //    qDebug()<<"minY"<<minP.y<<"maxY"<<maxP.y<<"\n";
    for(int h=minP.y;h<maxP.y;h++)
    {
        for(int w=minP.x;w<maxP.x;w++)
        {
            point test;
            test.x=w;
            test.y=h;

            if(judgePoint(p1,p2,test))
            {
                image.setPixel(w,h,valueF);
            }
        }
    }
    return image;
}

/**
 * @brief filedeal::pExistLine 判断点是否在海岸线上（子函数，父函数是unLinkPoint）
 * @param p
 * @param Line
 * @param number
 * @return
 */
bool filedeal::pExistLine(point p,point* Line,int number)
{
    for(int i=0;i<number;i++)
    {
        if(p.x==Line[i].x&&p.y==Line[i].y)
        {
            return true;
        }
    }
    return false;
}
/**
 * @brief filedeal::distance 求两点之间的距离
 * @param p1
 * @param p2
 * @return 两点之间的距离
 */
double filedeal::distance(point p1,point p2)
{
    return sqrt(((p1.x-p2.x)*(p1.x-p2.x))+((p1.y-p2.y)*(p1.y-p2.y)));
}
/**
 * @brief filedeal::reorderSLine 找两海岸线之间的最短的点然后对第二条海岸线数组重新排序
 * @param fLineP
 * @param sLineP
 * @param number
 * @return 返回重新排序的数组
 */
point* filedeal::reorderSLine(point *fLineP,point *sLineP,int number)
{
    double minDistance=0;
    double tempDistance=0;
    int minIndex=0;
    //    bool exist=true;
    //下标集合
    QSet<int> indexSet;
    for(int i=0;i<number;i++)
    {
        indexSet.insert(i);
    }
    //下标遍历器
    QSet<int>::iterator it;

    point *tLine=new point[number];
    for(int i=0;i<number;i++)
    {
        minIndex=0;
        minDistance=distance(fLineP[i],sLineP[minIndex]);
        for(int j=0;j<number;j++)
        {
            tempDistance=distance(fLineP[i],sLineP[j]);
            if(tempDistance<minDistance)
            {
                minDistance=tempDistance;
                minIndex=j;
                //                                it=indexSet.find(j);
                //                                if(it!=indexSet.end())
                //                                {
                //                                    //还存在
                //                                    exist=true;
                //                                }
                //                                else
                //                                {
                //                                    //不存在
                //                                    exist=false;
                //                                }
                //                                if(exist)
                //                                {
                //                                    minDistance=tempDistance;
                //                                    minIndex=j;
                //                                }

            }
        }
        //        indexSet.erase(indexSet.find(minIndex));
        //        if(distance(fLineP[i],sLineP[minIndex])>30)
        //        {
        //            qDebug()<<"th"<<i<<"maybe ERROR"<<"finalminDistance"<<minDistance<<" "<<minIndex<<"\n";
        //        }
        tLine[i]=sLineP[minIndex];
    }
    return tLine;
}

/**
 * @brief filedeal::slope   求斜率
 * @param p1
 * @param p2
 * @return 返回斜率
 *
 */
double filedeal::slope(point p1,point p2)
{
    double k;
    if(p1.x-p2.x!=0)
    {
        k=(p1.y-p2.y)/(p1.x-p2.x);
    }
    else
    {
        return 2.2222222;//返回一个不正常的数
    }
    return k;
}
/**
 * @brief filedeal::newRatio （不考虑标准点所在直线）直接标准点到第二条海岸线的最近距离，第二条海岸线到第一条海岸线最近的点
 * @param standPoint
 * @param sIndex
 * @param sLineP
 * @param fLine
 * @param number
 * @return
 */
double filedeal::newRatio(point standPoint,int sIndex,point sLineP,point *fLine)
{
    double ratio=0;
    if(distance(standPoint,sLineP)!=0.0&&distance(sLineP,fLine[sIndex])!=0.0)
    {
        ratio=distance(standPoint,sLineP)/distance(sLineP,fLine[sIndex]);
    }
    else
    {
        ratio=1;
    }
    return ratio;
}
/**
 * @brief filedeal::ratio
 * @param standPoint
 * @param sLine
 * @param fLine
 * @return 返回比例 （standPoint-sLinePoint）/(sLinePoint-fLinePoint)
 */
double filedeal::ratio(point standPoint,point sLine,point *fLine,int number)
{
    int fIndex=0;
    double ratio=0;
    int count=0;
    double k=slope(standPoint,sLine);//直线l1的斜率
    int pointStake[100]={0};
    if(k!=2.2222222)
    {
        //斜率存在时
        for(int i=0;i<number;i++)
        {
            //点斜式：y-y0=k*(x-x0),满足条件则是交点

            ///找过标准点与（离标准点最近的第二条海岸上的点）的直线l1  与 第一条海岸线的交点下标

            if(fLine[i].y-standPoint.y==qCeil(k*(fLine[i].x-standPoint.x))||fLine[i].y-standPoint.y==qFloor(k*(fLine[i].x-standPoint.x)))
            {
                fIndex=i;
                pointStake[count]=i;
                count++;
            }
        }
    }
    else
    {
        //斜率不存在时
        for(int i=0;i<number;i++)
        {
            if(fLine[i].x==standPoint.x)
            {
                fIndex=i;
                pointStake[count]=i;
                count++;
            }
        }

    }
    double minDistance=0,tempDistance=0;
    minDistance=distance(sLine,fLine[pointStake[0]]);
    for(int i=0;i<count;i++)
    {
        tempDistance=distance(sLine,fLine[pointStake[i]]);
        if(tempDistance<minDistance)
        {
            fIndex=pointStake[i];
        }
    }
    if(distance(sLine,fLine[fIndex])!=0.0&&distance(standPoint,sLine)!=0.0)
    {
        ratio=distance(standPoint,sLine)/distance(sLine,fLine[fIndex]);
        //qDebug()<<"standPoint.x"<<standPoint.x<<"standPoint.y"<<standPoint.y;
        //qDebug()<<"fLine.x"<<fLine[fIndex].x<<"fLine.y"<<fLine[fIndex].y;
        //qDebug()<<"distance"<<distance(standPoint,sLine)<<" distance(sLine[sIndex],fLine[fIndex])"<<distance(sLine,fLine[fIndex]);
        //qDebug()<<"ratio"<<ratio;
    }
    else
    {
        ratio=1;
    }

    return ratio;
}
/**
 * @brief filedela::oldMinDistanceIndex
 * @param standPoint
 * @param sLine
 * @param number
 * @return 返回标准点距离第二条海岸线最近的第二条海岸线上的点
 */
int filedeal::oldMinDistanceIndex(point standPoint,point *sLine,int number)
{
    int minIndex=0;
    double min=0;
    double temp=0;
    min=distance(standPoint,sLine[0]);
    for(int i=0;i<number;i++)
    {
        temp=distance(standPoint,sLine[i]);
        if(temp<min)
        {
            minIndex=i;
            min=temp;
        }
    }
    return minIndex;
}
/**
 * @brief filedeal::minDistanceIndex
 * @param standPoint
 * @param fLine
 * @param number
 * @param indexSet 没被选中的点的下标集合
 * @return 与标准点距离最小的sLine上的点的index
 */
int filedeal::minDistanceIndex(point standPoint,point *sLine,int number,QSet<int> indexSet)
{
    int minIndex=0;
    double min=0;
    double temp=0;
    bool exist=true;
    //下标遍历器
    QSet<int>::iterator it;
    min=distance(standPoint,sLine[0]);
    for(int i=0;i<number;i++)
    {
        temp=distance(standPoint,sLine[i]);
        if(temp<min)
        {
            it=indexSet.find(i);
            if(it!=indexSet.end())
            {
                //还存在
                exist=true;
            }
            else
            {
                //不存在
                exist=false;
            }
            if(exist)
            {
                minIndex=i;
                min=temp;
            }
        }
    }
    return minIndex;
}
/**
 * @brief filedeal::calculatePoint 计算新坐标
 * @param fPoint
 * @param sPoint
 * @param ratio
 * @return 返回计算出的新海岸线的坐标
 */
point filedeal::calculatePoint(point fPoint,point sPoint,double ratio)
{
    double l=distance(sPoint,fPoint)*ratio;
    //    double l=8.0;
    //(standPoint-sLinePoint)/(sLinePoint-fLinePoint)*(sPoint-fPoint);
    //    qDebug()<<"distance(sPoint,fPoint)"<<distance(sPoint,fPoint);
    thisCount++;

    double k,b;

    point newPoint;
    int x1,x2,y1,y2;

    if(fPoint.x-sPoint.x!=0)
    {
        //斜率存在时
        k=(fPoint.y-sPoint.y)/(fPoint.x-sPoint.x);
        b=fPoint.y-k*fPoint.x;
        //(1+x2)xn2+2(kb-x0-ky0)xn+(x02+y02-2by0+b2-l2)=0
        x1=(2*(k*sPoint.y+sPoint.x-k*b)+sqrt(pow((2*(k*b-sPoint.x-sPoint.y*k)),2)-4*(1+k*k)*(sPoint.x*sPoint.x+b*b+sPoint.y*sPoint.y-2*sPoint.y*b-l*l)))/(2*(1+k*k));//一元二次方程求根公式
        x2=(2*(k*sPoint.y+sPoint.x-k*b)-sqrt(pow((2*(k*b-sPoint.x-sPoint.y*k)),2)-4*(1+k*k)*(sPoint.x*sPoint.x+b*b+sPoint.y*sPoint.y-2*sPoint.y*b-l*l)))/(2*(1+k*k));//一元二次方程求根公式
        y1=k*x1+b;
        y2=k*x2+b;
        if((sPoint.x-fPoint.x)*(x1-sPoint.x)>=0||(sPoint.y-fPoint.y)*(y1-sPoint.y)>=0)
        {
            //方向相同,x1是要找的根
            newPoint.x=x1;
            newPoint.y=y1;
        }
        else
        {
            //方向不同x2才是要找的根
            newPoint.x=x2;
            newPoint.y=y2;
        }

    }
    else
    {
        //斜率不存在时
        x1=fPoint.x;
        y1=sPoint.y-l;
        x2=fPoint.x;
        y2=fPoint.y+l;
        if((sPoint.y-fPoint.y)*(y1-sPoint.y)>=0)
        {
            //方向相同是要求的点
            newPoint.x=x1;
            newPoint.y=y1;
        }
        else
        {
            //方向相反
            newPoint.x=x2;
            newPoint.y=y2;
        }

    }
    //    if(l>20)
    //    {
    //        qDebug()<<thisCount<<"fpoint"<<"  x:"<<fPoint.x<<"  y"<<fPoint.y;
    //        qDebug()<<thisCount<<"spoint"<<"  x:"<<sPoint.x<<"  y"<<sPoint.y;
    //        qDebug()<<"ratio"<<ratio;
    //        qDebug()<<"l"<<l;
    //        qDebug()<<"newPoint:  x"<<newPoint.x<<"  y"<<newPoint.y<<"\n";
    //        ERRORCount++;
    //    }

    //    if(newPoint.x>2729||newPoint.x<0||newPoint.y>1302||newPoint.y<0)
    //    {
    //        qDebug()<<"ERROR new point value out of range canvas";
    //    }
    return newPoint;
}

/**
 * @brief filedeal::ratio 求比值
 * @param fLine
 * @param sLine
 * @param number
 * @param standPoint
 * @param k斜率
 * @return 返回 旧（standPoint-sLinePoint）/(sLinePoint-fLinePoint)
 */
double filedeal::oldRatio(point *fLine,point *sLine,int number,point standPoint,double k)
{

    int fIndex=0;
    int sIndex=0;
    double ratio=0;
    int findex[100]={0};
    int sindex[100]={0};
    int fcount=0;
    int scount=0;
    if(k!=2.2222222)
    {
        //斜率存在时
        for(int i=0;i<number;i++)
        {

            ///找过标准点的直线与两条海岸线的交点


            //点斜式：y-y0=k*(x-x0),满足条件则是交点
            if(fLine[i].y-standPoint.y==qCeil(k*(fLine[i].x-standPoint.x))||fLine[i].y-standPoint.y==qFloor(k*(fLine[i].x-standPoint.x)))
            {
                fIndex=i;
                findex[fcount]=i;
                scount++;
            }
            if(sLine[i].y-standPoint.y==qCeil(k*(sLine[i].x-standPoint.x))||sLine[i].y-standPoint.y==qFloor(k*(sLine[i].x-standPoint.x)))
            {
                sIndex=i;
                sindex[scount]=i;
                scount++;
            }
        }
    }
    else
    {
        //斜率不存在时直线方程y=standPoint.x，如果第一条和第二条海岸线上的点的x与直线方程相同即为相交点
        for(int i=0;i<number;i++)
        {
            if(fLine[i].x==standPoint.x)
            {
                fIndex=i;
                findex[fcount]=i;
                scount++;
            }
            if(sLine[i].x==standPoint.x)
            {
                sIndex=i;
                sindex[scount]=i;
                scount++;
            }
        }
    }
    double minDistance=0,tempDistance;
    //这里就是要这样写为了找最终最小的
    minDistance=(distance(fLine[findex[0]],sLine[sindex[0]])+distance(standPoint,sLine[sindex[0]]));
    for(int i=0;i<fcount;i++)
    {
        for(int j=0;j<scount;j++)
        {
            tempDistance=(distance(fLine[findex[i]],sLine[sindex[j]])+distance(standPoint,sLine[sindex[j]]));
            if(tempDistance<minDistance)
            {
                minDistance=tempDistance;
                fIndex=findex[i];
                sIndex=sindex[j];
            }
        }
    }
    if(distance(sLine[sIndex],fLine[fIndex])!=0.0&&distance(standPoint,sLine[sIndex])!=0.0)
    {
        ratio=distance(standPoint,sLine[sIndex])/distance(sLine[sIndex],fLine[fIndex]);
        //qDebug()<<"standPoint.x"<<standPoint.x<<"standPoint.y"<<standPoint.y;
        //qDebug()<<"fLine.x"<<fLine[fIndex].x<<"fLine.y"<<fLine[fIndex].y;
        //qDebug()<<"sLine.x"<<sLine[sIndex].x<<"sLine.y"<<sLine[sIndex].y;
        //qDebug()<<"distance(standPoint,sLine[sIndex])"<<distance(standPoint,sLine[sIndex])<<" distance(sLine[sIndex],fLine[fIndex])"<<distance(sLine[sIndex],fLine[fIndex]);
        //qDebug()<<"ratio"<<ratio;
    }
    else
    {
        ratio=1;
    }

    return ratio;
}
/**
 * @brief filedeal::oldMove 计算出所有移动后的点
 * @param lineSet
 * @param standPoint
 * @param number
 */
void filedeal::oldMove(point**lineSet,point standPoint,int number)
{
    /*
     * 1、第一条海岸线上的点对应的第二条海岸线上的最近的点确定一条直线，此时求出斜率k（斜率存在）
     * 2、根据点斜式求过出标准点斜率为k的一条直线，并求出此直线与海岸线的交点两个，通过标准点，第一条海岸线上的点，第二条海岸线上的点，求出相邻两点的距离，并求出（standPoint-sLinePoint）/(sLinePoint-fLinePoint)比值
     * 3、根据比值计算出第一步直线上的一点（此点即是第三条海岸线上的点）（舍去一个无关点）
     */
    point *fLineP=lineSet[0];
    point *sLineP=lineSet[1];

    sLineP=reorderSLine(fLineP,sLineP,number);//将第二条海岸线上的点数组重排序

    point *tLine=new point[number];
    int index=oldMinDistanceIndex(standPoint,sLineP,number);
    for(int i=index;i<number;i++)
    {
        double k=slope(fLineP[i],sLineP[i]);
        double rat=oldRatio(fLineP,sLineP,number,standPoint,k);
        tLine[i]=calculatePoint(fLineP[i],sLineP[i],rat);
        if((tLine[i].x-sLineP[i].x)*(sLineP[i].x-fLineP[i].x)<0)
        {
            tLine[i]=sLineP[i];
        }
        standPoint=tLine[i];
    }
    for(int i=index;i>0;i--)
    {
        double k=slope(fLineP[i],sLineP[i]);
        double rat=oldRatio(fLineP,sLineP,number,standPoint,k);
        tLine[i]=calculatePoint(fLineP[i],sLineP[i],rat);
        if((tLine[i].x-sLineP[i].x)*(sLineP[i].x-fLineP[i].x)<0)
        {
            tLine[i]=sLineP[i];
        }
        standPoint=tLine[i];
    }

    //    for(int i=0;i<number;i++)
    //    {
    //        double k=slope(fLineP[i],sLineP[i]);
    //        double rat=oldRatio(fLineP,sLineP,number,standPoint,k);
    //        tLine[i]=calculatePoint(fLineP[i],sLineP[i],rat);
    //        if((tLine[i].x-sLineP[i].x)*(sLineP[i].x-fLineP[i].x)<0)
    //        {
    //            tLine[i]=sLineP[i];
    //        }
    //        standPoint=tLine[i];
    //    }
    QImage image(Samples, Lines, QImage::Format_RGB32);
    QRgb valueF;
    QRgb valueS;
    QRgb valueT;
    image.fill(Qt::white);//将图片背景填充为白色
    valueF=qRgb(0,0,255);
    valueS=qRgb(255,0,0);
    valueT=qRgb(0,0,0);
    for(int i=0;i<number;i++)
    {
        image.setPixel(fLineP[i].x,fLineP[i].y,valueF);
        image.setPixel(sLineP[i].x,sLineP[i].y,valueS);
        image.setPixel(tLine[i].x,tLine[i].y,valueT);
        //        qDebug()<<"tLine"<<i<<"    "<<tLine[i].x<<"   "<<tLine[i].y;
    }
    rawImage=image;
    partImage=image;
    visiualdraw(visiualDrawP,currentHeight,currentWidth,image);
    //释放申请的内存
    //    delete[]fLineX;
    //    delete[]fLineY;
    //    delete[]sLineY;
    //    delete[]sLineY;
    //    delete[]fLineP;
    //    delete[]sLineP;
    //    fLineX=NULL,fLineY=NULL,sLineX=NULL,sLineY=NULL,fLineP=NULL,sLineP=NULL;
}
/*
 * @brief filedeal::averageLine 先对第三条海岸线按照坐标重排序，再对相邻的n个点求平均值
 * @param tLine
 * @param number
 */
void filedeal::averageLine(QImage image, int width, int height)
{
    int cut=9;
    QImage newImage(width, height, QImage::Format_RGB32);
    newImage.fill(Qt::white);//将图片背景填充为白色
    QRgb valueF;
    valueF=qRgb(255,0,0);
    int count=0;
    point *tempPoi=new point[100];
    for(int h=0;h<height/cut-1;h++)
    {
        for(int w=0;w<width/cut-1;w++)
        {

            count=0;
            //w,w+1,w+2
            //h,h+1,h+2
            //获取点
            for(int i=0;i<cut;i++)
            {
                for(int j=0;j<cut;j++)
                {

                    if(QColor(image.pixel(w*cut+i,h*cut+j)).red()==0&&
                            QColor(image.pixel(w*cut+i,h*cut+j)).green()==0&&
                            QColor(image.pixel(w*cut+i,h*cut+j)).blue()==0)
                    {
                        tempPoi[count].x=w*cut+i;
                        tempPoi[count].y=h*cut+j;
                        count++;
                    }
                }
            }
            if(count>0)
            {
                //求平均值
                int sumX=0,sumY=0,averageX=0,averageY=0;
                for(int i=0;i<count;i++)
                {
                    sumX+=tempPoi[i].x;
                    sumY+=tempPoi[i].y;
                }
                averageX=sumX/count;
                averageY=sumY/count;
                newImage.setPixel(averageX,averageY,valueF);
            }
        }
    }

    rawImage=newImage;
    partImage=newImage;
    visiualdraw(visiualDrawP,currentHeight,currentWidth,newImage);
    linkPoint(newImage,Samples,Lines);
}

void filedeal::linkPoint(QImage image,int width,int height)
{
    int count=0;
    QRgb valueF;
    valueF=qRgb(0,0,0);
    point *tempPoint=new point[Samples*10];
    for(int h=0;h<height;h++)
    {
        for(int w=0;w<width;w++)
        {
            if(QColor(image.pixel(w,h)).red()==255&&
                    QColor(image.pixel(w,h)).green()==0&&
                    QColor(image.pixel(w,h)).blue()==0)
            {
                //                image.setPixel(w,h,valueF);
                tempPoint[count].x=w;
                tempPoint[count].y=h;
                count++;
            }
        }
    }
    qDebug()<<"found point numebr is "<<count;
    //    printPointSet(count,tempPoint);
    //判断点是否在线段上
    //或者过定点的曲线
    //先找最小范围

    //////////////////////////////////////找图中最左边的点////////////////////////////////////////////////////
    //先找最左边的点
    point mostLeft;
    mostLeft.x=tempPoint[0].x;
    int mostLeftIndex=0;
    for(int i=0;i<count;i++)
    {
        if(tempPoint[i].x<mostLeft.x)
        {
            mostLeft.x=tempPoint[i].x;
            mostLeft.y=tempPoint[i].y;
            mostLeftIndex=i;
        }
    }

    //第二个坐标
    int currentIndex=mostLeftIndex;

    /////////////////////////////////////////////////////////////////////


    //tempPoint[i],tempPoint[i+1]最小x,最大x
    QSet<int> indexSet;
    point *index=new point[count];

    for(int i=0;i<count;i++)
    {
        indexSet.insert(i);
    }

    //    count=3;////////测试专用要删掉
    count--;
    for(int i=0;i<count;i++)
    {
        //        int index1=i;
        //        int index2=ptpMinDisIndex(i,tempPoint,count,indexSet);//最近点下标
        //        int index2=ptpMinDisIndex(i,tempPoint,count,index);//最近点下标

        index[i].x=currentIndex;
        indexSet.remove(currentIndex);//去重复点
        currentIndex=ptpMinDisIndex(currentIndex,tempPoint,count,indexSet);//最近点下标
        index[i].y=currentIndex;
        indexSet.remove(currentIndex);//去重复点
        if(distance(tempPoint[index[i].x],tempPoint[index[i].y])<100)
        {
//            image=linkLine(image,tempPoint[index[i].x],tempPoint[index[i].y]);
            image=midPointLink(image,tempPoint[index[i].x],tempPoint[index[i].y]);
        }

        partImage=image;
        rawImage=image;
        visiualdraw(visiualDrawP,currentHeight,currentWidth,image);
        //        qDebug()<<index[i].x;
        //        qDebug()<<index[i].y;
        //        qDebug()<<tempPoint[index[i].x].x<<tempPoint[index[i].x].y;
        //        qDebug()<<tempPoint[index[i].y].x<<tempPoint[index[i].y].y;
        //        qDebug()<<"\n";
    }
    QImage image1(Samples, Lines, QImage::Format_RGB32);
    image1.fill(Qt::white);
    QRgb valueP;
    valueP=qRgb(0,0,0);

    for(int i=0;i<count;i++)
    {
        image1.setPixel(tempPoint[index[i].x].x,tempPoint[index[i].x].y,valueP);
    }
    image1.save("D:\\averageline.tif");

    FILE *fp;
    fp=fopen("D:\\binaryLine","wb");
    if(fp==NULL)
    {
        qDebug()<<"ERROR";
        return;
    }

    for(int i=0;i<count;i++)
    {
        fwrite(&tempPoint[index[i].x].x,2,1,fp);
        fwrite(&tempPoint[index[i].x].y,2,1,fp);
    }
    fclose(fp);

    delete[] tempPoint;
    tempPoint=NULL;
    delete[] index;
    index=NULL;
    partImage=image;
    rawImage=image;
    visiualdraw(visiualDrawP,currentHeight,currentWidth,image);
}
/**
 * @brief filedeal::judgeExist 判断下标组是否被用过
 * @param number
 * @param index1
 * @param index2
 * @param indexSet
 * @return
 */
////////暂时不用
bool filedeal::judgeExist(int number,int index1,int index2,point *indexSet)
{
    //查找是否用到过该组下标
    for(int i=0;i<number;i++)
    {
        //该组下标已经用过
        if(index1==indexSet[i].x&&index2==indexSet[i].y||index2==indexSet[i].x&&index1==indexSet[i].y)
        {
            return true;
        }

    }
    //该组下标还未用过
    return false;
}
/**
 * @brief filedeal::ptpMinDisIndex 返回距离最小的点
 * @param index
 * @param p
 * @param pNumber
 * @param indexSet
 * @return
 */
////////暂时不用
int filedeal::ptpMinDisIndex(int count,int index,point*p,int pNumber,point *indexSet)
{
    int *dis=new int[pNumber];
    for(int i=0;i<pNumber;i++)
    {
        dis[i]=distance(p[index],p[i]);
    }
    //自己跟自己的距离设置成最大
    dis[index]=65535;
    int  minDistance=65535;

    int tempMin=0;
    int minIndex=0;
    for(int i=0;i<pNumber;i++)
    {
        tempMin=dis[i];
        if(tempMin<minDistance)
        {
            //如果该组下标没被用过，说明两点未连成线
            //若该组下标已经被用过，则跳过该最小距离并不动原来最小的下标
            if(!judgeExist(count,index,i,indexSet))
            {
                minDistance=tempMin;
                minIndex=i;
            }

        }
    }
    delete[] dis;
    dis=NULL;
    return minIndex;
}

/**
 * @brief filedeal::ptpMinDisIndex获取与某个点距离最近的点
 * @param index 此点index
 * @param p     点集合
 * @param pNumber 集合中点的数量
 * @return 最近点index
 */
int filedeal::ptpMinDisIndex(int index,point*p,int pNumber,QSet<int> indexSet)
{
    //    bool exist=false;
    QSet<int>::iterator it;
    int *dis=new int[pNumber];
    for(int i=0;i<pNumber;i++)
    {
        it=indexSet.find(i);
        if(it!=indexSet.end())
        {
            if(i!=index)
            {
                dis[i]=distance(p[index],p[i]);
            }
            else
            {
                dis[i]=65535;
            }
        }
        else
        {
            dis[i]=65535;
        }

    }

    int  minDistance=65535;
    int tempMin=0;
    int minIndex=0;
    for(int i=0;i<pNumber;i++)
    {
        tempMin=dis[i];
        //        exist=false;
        if(tempMin<minDistance)
        {
            //            it=indexSet.find(i);
            //            if(it!=indexSet.end())
            //            {
            //                //还存在
            //                exist=true;
            //            }
            //            else
            //            {
            //                //不存在
            //                exist=false;
            //            }
            //            if(exist)
            //            {
            minDistance=tempMin;
            minIndex=i;
            //                qDebug()<<"mindistance is "<<distance(p[index],p[minIndex]);
            //            }

        }
    }
    //    qDebug()<<"mindistance is "<<distance(p[index],p[minIndex])<<"\n";


    delete[] dis;
    dis=NULL;
    return minIndex;
}
/**
 * @brief filedeal::judgePoint//向量法：判断中间一点 是否在直线上
 * @param p1
 * @param p2
 * @param testPoint 判断点是否在线段上
 * @return
 */
bool filedeal::judgePoint(point p1,point p2,point testPoint)
{
    double s1,s2,t1,t2;
    s1=testPoint.x-p1.x;
    t1=testPoint.y-p1.y;
    s2=p1.x-p2.x;
    t2=p1.y-p2.y;
    if(-5<=(s1*t2-t1*s2)&&(s1*t2-t1*s2)<=5)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//给定一点Q(a,b),和线段M的首尾两个端点P1(X1,Y1),P2(X2,Y2),要求判断点Q否在线段M上；
//(为了方便理解,这里我们就认为X1>X2,Y1>Y2)
//看到这个题，我们说先会想到的肯定是判断该点是否在线段的范围内，如果不在，肯定在线段上。
//所以我们首先应该保证：X2<=a<=X1 &&  Y2<=b<=Y1
//这样点Q就在以P1，P2为首位的矩形内，我们就排除了Q在M的延长线上的可能。
//然后判断点是否在线段上了，今天我看到了两种方法，各具特色
//向量法：
//其实思路很简单，就是判断
//Q-P1 == k * (P1-P2)  ?Yes : No;

//#include<stdio.h>
//int main()
//{
//    double a,b,x1,x2,y1,y2;
//    scanf("%lf%lf",&a,&b);          //Q(a,b)
//    scanf("%lf%lf",&x1,&y1);        //P1(x1,y1)
//    scanf("%lf%lf",&x2,&y2);        //P2(x2,y2)
//    double s1=a-x1, t1=b-y1;        //Q-P1(s1,t1)
//    double s2=x1-x2,t2=y1-y2;       //P1-P2(s2,t2)
//    puts((s1*t2-t1*s2)==0?"Yes":"No");        //二维点的×乘x1*y2-x2*y1
//    return 0;
//}
QImage filedeal::ddaLinkLine(QImage image,point p1,point p2){
    QRgb valueF;
    valueF=qRgb(0,0,0);
    float x,y;
    float dx,dy,k;
    dx=(float)(p2.x-p1.x);
    dy=(float)(p2.y-p1.y);
    k=dy/dx;//斜率
    x=p1.x;
    y=p1.y;

    if (abs(k)<1)//斜率绝对值小于1时，以x步进
    {
        for (;x<=p2.x;++x)
        {
            image.setPixel(x,int(y+0.5),valueF);
            y+=k;
        }
    }
    if (abs(k)>=1)//斜率绝对值大于等于1时，以y步进

    {
        for (;y<p2.y;++y)
        {
            image.setPixel(int(x+0.5),y,valueF);
            x+=1/k;
        }
    }
    return image;
}
//Bresenham  算法
QImage filedeal::OnBresenhamline(QImage image,point p1,point p2)
{
    QRgb valueF;
    valueF=qRgb(0,0,0);
    int x1=p1.x, y1=p1.y, x2=p2.x, y2=p2.y;
    int i,s1,s2,interchange;
    float x,y,deltax,deltay,f,temp;
    x=x1;
    y=y1;
    deltax=abs(x2-x1);
    deltay=abs(y2-y1);
    if(x2-x1>=0) s1=1; else s1=-1;
    if(y2-y1>=0) s2=1; else s2=-1;

    if(deltay>deltax)
    {
        temp=deltax;
        deltax=deltay;
        deltay=temp;
        interchange=1;
    }
    else interchange=0;
    f=2*deltay-deltax;
    image.setPixel(x,y,valueF);

    for(i=1;i<=deltax;i++)
    {

        if(f>=0)
        {
            if(interchange==1) x+=s1;

            else y+=s2;
            image.setPixel(x,y,valueF);
            f=f-2*deltax;
        }
        else
        {
            if(interchange==1) y+=s2;
            else x+=s1;
            f=f+2*deltay;
        }
    }
    return image;
}
/**
 * @brief filedeal::Line_Midpoint 生成直线的中点画法
 * @param image
 * @param p1
 * @param p2
 * @return
 */
QImage filedeal:: midPointLink(QImage image,point p1,point p2)
{
    QRgb valueF;
    valueF=qRgb(0,0,0);

    int x = p1.x, y = p1.y;
    int a = p1.y - p2.y, b = p2.x - p1.x;
    int cx = (b >= 0 ? 1 : (b = -b, -1));
    int cy = (a <= 0 ? 1 : (a = -a, -1));

    image.setPixel(x, y, valueF);

    int d, d1, d2;
    if (-a <= b)     // 斜率绝对值 <= 1
    {
        d = 2 * a + b;
        d1 = 2 * a;
        d2 = 2 * (a + b);
        while(x != p2.x)
        {
            if (d < 0)
            {
                y += cy, d += d2;
            }
            else
            {
                d += d1;
            }
            x += cx;
            image.setPixel(x, y, valueF);
        }
    }
    else                // 斜率绝对值 > 1
    {
        d = 2 * b + a;
        d1 = 2 * b;
        d2 = 2 * (a + b);
        while(y != p2.y)
        {
            if(d < 0)
            {
                d += d1;
            }
            else
            {
                x += cx, d += d2;
            }
            y += cy;
           image.setPixel(x, y, valueF);
        }
    }
    return image;
}
