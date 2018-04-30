#include "filedeal.h"
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
QImage scaleImage;
int R[7]={255,   255,  255,    0,   0,   139,0};
int G[7]={255,     0,  255,    0,   255, 0 ,0};
int B[7]={255,     0  ,  0,  255,   0,   139   ,0};


filedeal::filedeal()
{
    visiualDrawP.x=0;
    visiualDrawP.y=0;
}
filedeal::~filedeal()
{
    delete[]tempPoint;
    delete[]areaPoint;
}

///获取图像波段和文件信息
void filedeal::openPathFile(QString fileName)
{
    //    qInfo()<<QStringLiteral("文件名称：")<<fileName;
    filePathName=fileName;
    GDALAllRegister();//注册gdal驱动
    CPLSetConfigOption( "GDAL_FILENAME_IS_UTF8", "YES" );//中文路径中文名称支持
    gdalData = ( GDALDataset* )GDALOpen(fileName.toStdString().c_str(), GA_ReadOnly );//打开文件
    if ( gdalData == NULL )
    {
        qWarning()<<QStringLiteral("数据集为空");
        return ;
    }
    Band=0;
    Samples=0;
    Lines=0;
    Band=gdalData->GetRasterCount();
    Samples=gdalData->GetRasterXSize();
    Lines=gdalData->GetRasterYSize();
    qInfo()<<QStringLiteral("波段数量：")<<Band;
    qInfo()<<QStringLiteral("图像宽度：")<<Samples;
    qInfo()<<QStringLiteral("图像高度：")<<Lines;
    double *maxValue=new double[Band];
    for(int i=1;i<=Band;i++)
    {
        double minmax[2];
        double min,max;
        gdalData->GetRasterBand(i)->ComputeRasterMinMax(1,minmax);
        min = minmax[0];//最小值
        max = minmax[1];//最大值
        maxValue[i-1]=max;
    }
    double temp=maxValue[0];
    for(int i=0;i<Band;i++)
    {
        if(maxValue[i]>temp)
        {
            temp=maxValue[i];
        }
    }
    maxDataValue=temp;
    delete[] maxValue;
    maxValue=NULL;
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
    QString sqlInsertStr=QString("insert into RemoteSensingImage(name)values('%1')").arg(fileName);
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
        visiualdraw(p,Lines,500,rawImage);
    }
    else if(Samples<500&&Lines>500)
    {
        currentWidth=Samples;
        visiualdraw(p,500,Samples,rawImage);
    }
    else
    {
        currentWidth=Samples;
        currentHeight=Lines;
        visiualdraw(p,Lines,Samples,rawImage);
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
        qWarning()<<QStringLiteral("数据集为空");
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
        qWarning()<<QStringLiteral("数据集为空");
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
    //    int readHeight=100;
    //根据图片大小调整每次读的行数
    //如果图片很大，每次读50行
    //    if(Samples>7000)
    //    {
    //        readHeight=50;
    //    }
    //    else if(4000<=Samples&&Samples<=7000)
    //    {
    //        readHeight=100;
    //    }
    //    else if(Samples<4000)
    //    {
    //        readHeight=200;
    //    }
    readHeight=Lines;
    float *tempRaw_band1=new float [readHeight*picWidth];
    float *tempRaw_band2=new float [readHeight*picWidth];
    float *tempRaw_band3=new float [readHeight*picWidth];

    QImage image(Samples, Lines, QImage::Format_RGB32);
    QRgb value;
    image.fill(Qt::white);//将图片背景填充为白色
    //以下r，g，b均不是形参的rgb意义
    int colorR,colorG,colorB;

    int startY=0,imageY=0;
    for(int i=0;i<picHeight/readHeight;i++)
    {
        //读取数据
        bandList.at( r )->RasterIO( GF_Read, 0, startY, picWidth, readHeight, tempRaw_band1, picWidth, readHeight, GDT_Float32, 0, 0 );
        bandList.at( g )->RasterIO( GF_Read, 0, startY, picWidth, readHeight, tempRaw_band2, picWidth, readHeight, GDT_Float32, 0, 0 );
        bandList.at( b )->RasterIO( GF_Read, 0, startY, picWidth, readHeight, tempRaw_band3, picWidth, readHeight, GDT_Float32, 0, 0 );
        processData_band1= picSketch( tempRaw_band1,gdalData->GetRasterBand(r+1), picWidth * readHeight, bandList.at( r )->GetNoDataValue());
        processData_band2= picSketch( tempRaw_band2,gdalData->GetRasterBand(g+1), picWidth * readHeight, bandList.at( g )->GetNoDataValue());
        processData_band3= picSketch( tempRaw_band3,gdalData->GetRasterBand(b+1), picWidth * readHeight, bandList.at( b )->GetNoDataValue());

        for( int y=0;y<readHeight;y++)
        {
            for(int x=0;x<Samples;x++)
            {
                colorR=processData_band1[y * Samples + x];
                colorG=processData_band2[y * Samples + x];
                colorB=processData_band3[y * Samples + x];
                value = qRgb(colorR, colorG, colorB);
                image.setPixel(x,imageY,value);
            }
            imageY++;
        }
        delete[] processData_band1;
        delete[] processData_band2;
        delete[] processData_band3;

        //更改读取区域
        startY+=readHeight;
    }
    int endOffset=picHeight%readHeight;

    //读取数据
    bandList.at( r )->RasterIO( GF_Read, 0, startY, picWidth,endOffset , tempRaw_band1, picWidth, endOffset, GDT_Float32, 0, 0 );
    bandList.at( g )->RasterIO( GF_Read, 0, startY, picWidth,endOffset, tempRaw_band2, picWidth, endOffset, GDT_Float32, 0, 0 );
    bandList.at( b )->RasterIO( GF_Read, 0, startY, picWidth,endOffset, tempRaw_band3, picWidth, endOffset, GDT_Float32, 0, 0 );
    processData_band1= picSketch( tempRaw_band1,gdalData->GetRasterBand(r+1), picWidth * endOffset, bandList.at( r )->GetNoDataValue() );
    processData_band2= picSketch( tempRaw_band2,gdalData->GetRasterBand(g+1), picWidth * endOffset, bandList.at( g )->GetNoDataValue() );
    processData_band3= picSketch( tempRaw_band3,gdalData->GetRasterBand(b+1), picWidth * endOffset, bandList.at( b )->GetNoDataValue() );

    for( int y=0;y<endOffset;y++)
    {
        for(int x=0;x<Samples;x++)
        {
            colorR=processData_band1[y * Samples + x];
            colorG=processData_band2[y * Samples + x];
            colorB=processData_band3[y * Samples + x];
            value = qRgb(colorR, colorG, colorB);
            image.setPixel(x,imageY,value);
        }
        imageY++;
    }

    GDALClose(gdalData);
    delete[] tempRaw_band1;
    delete[] tempRaw_band2;
    delete[] tempRaw_band3;
    tempRaw_band1=NULL;
    tempRaw_band2=NULL;
    tempRaw_band3=NULL;
    delete[] processData_band1;
    delete[] processData_band2;
    delete[] processData_band3;
    processData_band1=NULL;
    processData_band2=NULL;
    processData_band3=NULL;

    rawImage=image;
    partImage=image;
}
///返回原始数据
/**
 * @brief filedeal::openFile 获取文件的一小块区域（矩形区域）
 * @param rectangle 传入矩形的左上顶点和右下顶点
 * @return 矩形区域的所有数据（以二维数组形式返回，第一维度是矩形区域的第i个点，第二维度是该点处的所有波段值）
 */
float ** filedeal::openFile(Points rectangle)
{
    int startX=rectangle[0].x,startY=rectangle[0].y,
            xOffset=rectangle[1].x-rectangle[0].x,
            yOffset=rectangle[1].y-rectangle[0].y;
    //qDebug()<<QStringLiteral("打开文件起始坐标")<<startX<<startY;
    //qDebug()<<QStringLiteral("打开文件结束坐标")<<rectangle[1].x<<rectangle[1].y;
    GDALAllRegister();//注册gdal驱动
    CPLSetConfigOption( "GDAL_FILENAME_IS_UTF8", "YES" );//中文路径中文名称支持
    gdalData = ( GDALDataset* )GDALOpen(filePathName.toStdString().c_str(), GA_ReadOnly );//打开文件
    if ( gdalData == NULL )
    {
        qWarning()<<QStringLiteral("数据集为空");
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
        qWarning()<<QStringLiteral("数据集为空");
        return NULL;
    }

    //    int picWidth = bandList.at( 0 )->GetXSize();
    //    int picHeight = bandList.at( 0 )->GetYSize();

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
    int scaleHeight;
    int scaleWidth;
    if(visiualImage.width()>visiualImage.height())
    {
        scaleHeight=QApplication::desktop()->height()/4;
        scaleWidth= visiualImage.width()*scale;
    }
    else
    {
        scaleWidth=QApplication::desktop()->width()/12;
        scaleHeight=visiualImage.height()*scale;
    }
    //    qDebug()<<QStringLiteral("略缩图宽度：")<<scaleWidth;
    //    qDebug()<<QStringLiteral("略缩图高度：")<<scaleHeight;


    int r=255, g=0, b=0;//用来接收颜色
    QRgb value=qRgb(r, g, b);
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
    ///非海岸线版本
    QImage image=visiualImage.scaled(scaleWidth,scaleHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//平滑缩放保留细节;
    int x=startX*scale;
    int y=startY*scale;
    int w=0;
    int viewWidth=visiualWidth*scale;
    int viewHeight=visiualHeight*scale;

    if(x+viewWidth==scaleWidth)
    {
        //x开始点减小1
        x--;
    }
    if(y+viewHeight==scaleHeight)
    {
        //y开始点减小1
        y--;
    }

    while(w<viewWidth)
    {
        image.setPixel(x, y, value);
        image.setPixel(x, y+viewHeight, value);
        x++;
        w++;
    }
    w=0;
    x=startX*scale;
    y=startY*scale;
    if(x+viewWidth==scaleWidth)
    {
        //x开始点减小1
        x--;
    }
    if(y+viewHeight==scaleHeight)
    {
        //y开始点减小1
        y--;
    }
    while(w<viewHeight)
    {
        image.setPixel(x, y, value);
        image.setPixel(x+viewWidth, y, value);
        y++;
        w++;
    }

    ///海岸线版本
    //    QImage image=visiualImage;
    //        int x=startX;
    //        int y=startY;
    //        int w=0;
    //        while(w<visiualWidth)
    //        {
    //            for(int i=0;i<1/scale;i++)//将红框加粗
    //            {
    //                image.setPixel(x, y+i, value);
    //            }
    //            for(int i=1/scale;i>0;i--)//将红框加粗
    //            {
    //                image.setPixel(x, y+visiualHeight-i, value);
    //            }
    //            x++;
    //            w++;
    //        }
    //        w=0;
    //        x=startX;
    //        y=startY;
    //        while(w<visiualHeight)
    //        {

    //            for(int i=0;i<1/scale;i++)//将红框加粗
    //            {
    //                image.setPixel(x+i, y, value);
    //            }
    //            for(int i=1/scale;i>0;i--)//将红框加粗
    //            {
    //                image.setPixel(x+visiualWidth-i, y, value);
    //            }
    //            y++;
    //            w++;
    //        }

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
    //qInfo()<<  MainImage.pixelColor(startX,startY);
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
    QRgb value=qRgb(R[current+1],G[current+1],B[current+1]);
    partImage=midPointLink(partImage,points[0],points[1],value);
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
        //        qInfo()<<selectPoint[i].x;
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
    //    getAimData(tempPoint,openFile());
    newGetAimData(tempPoint,openFile(tempPoint));
}
////从文件获取数据
void filedeal::getAimData(Points mPoints,float **data)
{
    QTime time;
    time.start();
    QRgb value = qRgb(R[current+1], G[current+1], B[current+1]);
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
    //    qInfo()<<time.elapsed()<<"ms";

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
void filedeal::newGetAimData(Points rectangle,float **data)
{
    int startX=rectangle[0].x,startY=rectangle[0].y,
            xOffset=rectangle[1].x-rectangle[0].x,
            yOffset=rectangle[1].y-rectangle[0].y;
    //    qDebug()<<QStringLiteral("使用样本起始坐标")<<startX<<startY;
    //    qDebug()<<QStringLiteral("使用样本结束坐标")<<rectangle[1].x<<rectangle[1].y;
    //    QTime time;
    //    time.start();
    QRgb value = qRgb(R[current+1], G[current+1], B[current+1]);
    //传入待测试的点
    for(int h=rectangle[0].y;h<rectangle[1].y;h++)
    {
        for(int w=rectangle[0].x;w<rectangle[1].x;w++)
        {
            if(judgeArea(samplePointNum,areaPoint,w,h))//判断点是否在区域中
            {
                //样本在区域中
                partImage.setPixel(w, h, value);
                for(int i=0;i<Band;i++)
                {
                    dataCopyArea[current][pointCount[current]].bands[i]=data[i][(h-startY)*xOffset+(w-startX)];
                    //qDebug()<<QStringLiteral("当前运行坐标")<<w<<h;
                    //qDebug()<<QStringLiteral("数据运行坐标")<<w-startX<<h-startY;
                    //qDebug()<<QStringLiteral("数据运行值")<<(h-startY)*xOffset+(w-startX);
                }
                pointCount[current]++;
            }
        }
    }
    //    qInfo()<<time.elapsed()<<"ms";

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
    //    qInfo()<<"data size is"<<dataList.size();
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
    //    delete[] dataCopyArea;
    //    dataCopyArea=NULL;
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
    //    qInfo()<<"currentIndex:"<<currentIndex;
}
////解析分类结果，将图片变色
void filedeal::parser(QString ruleName)
{
    sorted=true;
    float **rawData=NULL;
    rawData=openFile();
    //    if(!rawImage.isNull())
    //    {
    //        partImage=rawImage;
    //    }
    //    else
    //    {
    //        openFile(1,2,3);
    //        partImage=rawImage;
    //    }
    QImage image(Samples, Lines, QImage::Format_RGB32);
    image.fill(Qt::white);//将图片背景填充为白色
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
        //        qInfo()<<str[i];
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
                qInfo()<<QStringLiteral("波段i")<< bandI[q-1];
                qInfo()<<QStringLiteral("波段j")<< bandJ[q-1];
                qInfo()<<QStringLiteral("波段k")<< bandK[q-1];
                qInfo()<<QStringLiteral("比较符")<<compare[q-1];
                qInfo()<<QStringLiteral("阈值")<<  Threshold[q-1];
            }
            //            qInfo()<<strList[i].at(j);
        }
        if(j!=0)
        {
            QString geoStr=strList[i].at(j+6);
            geoName[geoNameCount]=geoStr.toInt();
            qInfo()<<QStringLiteral("第")<<geoName[geoNameCount]<<QStringLiteral("种地物");
            qInfo()<<"--------------------------------";
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
                    image.setPixel(k%Samples,k/Samples,value);
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
    rawImage=image;
    partImage=image;
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
void filedeal::newParser(QString ruleName)
{
    sorted=true;

    QImage image(Samples, Lines, QImage::Format_RGB32);
    image.fill(Qt::white);//将图片背景填充为白色
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
                qInfo()<<QStringLiteral("波段i")<< bandI[q-1];
                qInfo()<<QStringLiteral("波段j")<< bandJ[q-1];
                qInfo()<<QStringLiteral("波段k")<< bandK[q-1];
                qInfo()<<QStringLiteral("比较符")<<compare[q-1];
                qInfo()<<QStringLiteral("阈值")<<  Threshold[q-1];
            }
        }
        if(j!=0)
        {
            QString geoStr=strList[i].at(j+6);
            geoName[geoNameCount]=geoStr.toInt();
            //qInfo()<<QStringLiteral("第")<<geoName[geoNameCount]<<QStringLiteral("种地物");
            //qInfo()<<"--------------------------------";

            ////////////////////////////////////////////////////// int times=Lines/readHeight;
            //            int readHeight=100;//每次读取文件的数量100行
            int startHeight=0;//游标的初始位置
            int endHeight=startHeight+readHeight;//当前游标的结束位置

            point *rectangle=new point[2];
            //开始坐标
            rectangle[0].x=0;
            rectangle[0].y=startHeight;
            //结束坐标
            rectangle[1].x=Samples;
            rectangle[1].y=endHeight;
            //二维坐标一维化
            int startNumber=rectangle[0].y*Samples+rectangle[0].x;
            //真实的坐标（一维）
            int realLocationNum=0;
            //整除100剩余的余数处理
            int restHeight=Lines%readHeight;

            /////////////////////////////////////////////////////////////////////
            for(int m=0;m<Lines/readHeight;m++)
            {
                float **rawData=NULL;
                rawData=openFile(rectangle);
                //k=0;k<readHeight;k++;//当前虚拟坐标（一维的）
                //当前真实坐标（一维的）realLocationNum
                //startNumber=rectangle[0].y*Samples+rectangle[0].x;
                startNumber=rectangle[0].y*Samples+rectangle[0].x;

                for(int k=0;k<Samples*readHeight;k++)
                {
                    realLocationNum=startNumber+k;
                    //标志位置0
                    for(int tn=0;tn<30;tn++)
                    {
                        flag[tn]=0;
                    }
                    for(p=0;p<q;p++)
                    {
                        if(bandJ[p]!=-1&&bandK[p]!=-1&&compare[p]==-2)
                        {
                            if((rawData[bandI[p]][k]+rawData[bandK[p]][k])*1.0/2-rawData[bandJ[p]][k]<=Threshold[p])
                            {
                                flag[p]=1;
                            }else
                            {
                                flag[p]=0;
                            }
                        }
                        else if(bandJ[p]!=-1&&bandK[p]!=-1&&compare[p]==2)
                        {
                            if((rawData[bandI[p]][k]+rawData[bandK[p]][k])*1.0/2-rawData[bandJ[p]][k]>=Threshold[p])
                            {
                                flag[p]=1;
                            }else
                            {
                                flag[p]=0;
                            }
                        }
                        else if(bandJ[p]!=-1&&bandK[p]==-1&&compare[p]==-2)
                        {
                            if(rawData[bandI[p]][k]-rawData[bandJ[p]][k]<=Threshold[p])
                            {
                                flag[p]=1;
                            }else
                            {
                                flag[p]=0;
                            }
                        }
                        else if(bandJ[p]!=-1&&bandK[p]==-1&&compare[p]==2)
                        {
                            if(rawData[bandI[p]][k]-rawData[bandJ[p]][k]>=Threshold[p])
                            {
                                flag[p]=1;
                            }else
                            {
                                flag[p]=0;
                            }
                        }
                        else if(bandJ[p]==-1&&compare[p]==-2)
                        {
                            if(rawData[bandI[p]][k]<=Threshold[p])
                            {
                                flag[p]=1;
                            }else
                            {
                                flag[p]=0;
                            }
                        }
                        else if(bandJ[p]==-1&&compare[p]==2)
                        {
                            if(rawData[bandI[p]][k]>=Threshold[p])
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
                        image.setPixel(realLocationNum%Samples,realLocationNum/Samples,value);
                    }
                }
                startHeight+=readHeight;
                endHeight+=readHeight;
                rectangle[0].x=0;
                rectangle[0].y=startHeight;
                rectangle[1].x=Samples;
                rectangle[1].y=endHeight;
                for(int i=0;i<Band;i++)
                {
                    delete []rawData[i];
                    rawData[i]=NULL;
                }
                delete[] rawData;
                rawData=NULL;

            }//样本循环
            //余数   剩余部分处理
            rectangle[1].y=Lines;
            float **rawData=NULL;
            rawData=openFile(rectangle);
            startNumber=rectangle[0].y*Samples+rectangle[0].x;
            for(int k=0;k<Samples*restHeight;k++)
            {
                realLocationNum=startNumber+k;
                //标志位置0
                for(int tn=0;tn<30;tn++)
                {
                    flag[tn]=0;
                }
                for(p=0;p<q;p++)
                {
                    if(bandJ[p]!=-1&&bandK[p]!=-1&&compare[p]==-2)
                    {
                        if((rawData[bandI[p]][k]+rawData[bandK[p]][k])*1.0/2-rawData[bandJ[p]][k]<=Threshold[p])
                        {
                            flag[p]=1;
                        }else
                        {
                            flag[p]=0;
                        }
                    }
                    else if(bandJ[p]!=-1&&bandK[p]!=-1&&compare[p]==2)
                    {
                        if((rawData[bandI[p]][k]+rawData[bandK[p]][k])*1.0/2-rawData[bandJ[p]][k]>=Threshold[p])
                        {
                            flag[p]=1;
                        }else
                        {
                            flag[p]=0;
                        }
                    }
                    else if(bandJ[p]!=-1&&bandK[p]==-1&&compare[p]==-2)
                    {
                        if(rawData[bandI[p]][k]-rawData[bandJ[p]][k]<=Threshold[p])
                        {
                            flag[p]=1;
                        }else
                        {
                            flag[p]=0;
                        }
                    }
                    else if(bandJ[p]!=-1&&bandK[p]==-1&&compare[p]==2)
                    {
                        if(rawData[bandI[p]][k]-rawData[bandJ[p]][k]>=Threshold[p])
                        {
                            flag[p]=1;
                        }else
                        {
                            flag[p]=0;
                        }
                    }
                    else if(bandJ[p]==-1&&compare[p]==-2)
                    {
                        if(rawData[bandI[p]][k]<=Threshold[p])
                        {
                            flag[p]=1;
                        }else
                        {
                            flag[p]=0;
                        }
                    }
                    else if(bandJ[p]==-1&&compare[p]==2)
                    {
                        if(rawData[bandI[p]][k]>=Threshold[p])
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
                    image.setPixel(realLocationNum%Samples,realLocationNum/Samples,value);
                }
            }
            for(int i=0;i<Band;i++)
            {
                delete []rawData[i];
                rawData[i]=NULL;
            }
            delete[] rawData;
            rawData=NULL;
            geoNameCount++;
            delete[] rectangle;
            rectangle=NULL;
        }
    }
    //显示区分后的图像
    rawImage=image;
    partImage=image;
    visiualdraw(visiualDrawP,currentHeight,currentWidth,partImage);

}
/**
 * @brief filedeal::slotSealine 提取海岸线信号
 * @param seaColor
 * @param landColor
 */
void filedeal::slotSealine(int *seaColor,int *landColor)
{
    qDebug()<<partImage.width()<<partImage.height();
    if(partImage.isNull())
    {
        qDebug()<<QStringLiteral("错误");
        return;
    }
    seaLineGet(partImage,seaColor,landColor);
}
/**
 * @brief filedeal::dataInfoGet 信号响应，从其他线程获取样本的信息
 * @param details
 */
void filedeal::dataInfoGet(SamplesDetails details)
{
    emit initProgressbar();
    emit setProgressRange(0,100);
    emit setProgressValue(0);
    empty_thread=new emptychild;//空线程
    classify=new svmclassify;//类
    classify->moveToThread(empty_thread);//处理数据线程移入空线程
    empty_thread->start();//线程启动

    //获取样本
    int count=0;
    simples data=new Node[300000];
    for(int i=0;i<6;i++)
    {
        for(int j=0;j<pointCount[i];j++)
        {
            for(int b=0;b<Band;b++)
                data[count].bands[b]=dataCopyArea[i][j].bands[b]/maxDataValue;
            count++;
        }
    }
    //    delete[] dataCopyArea;
    //    dataCopyArea=NULL;
    selectEnable=false;

    int band=details.bandNumber;
    int geoNumber=details.geoNumber;
    int *eachNumber=details.eachnumber;
    int sum=details.sumNumber;

    qRegisterMetaType<svm_model>("svm_model");//注册自定义类型的槽信号
    connect(classify,SIGNAL(sendModel(svm_model*)),this,SLOT(getSvmModel(svm_model*)));
    connect(this,SIGNAL(invokeTrain(int,simples,int,int*,int)),classify,SLOT(train(int,simples,int,int*,int)));
    emit invokeTrain(band,data,geoNumber,eachNumber,sum);
    qDebug()<<QStringLiteral("样本获取正常");
}
/**
 * @brief filedeal::getSvmModel svmclassif 信号响应，接收svm_model
 * @param model
 */
void filedeal::getSvmModel(svm_model *model)
{
    QDateTime Systemtime = QDateTime::currentDateTime();//获取系统现在的时间
    QString str = Systemtime.toString("yyyy_MM_dd_hh_mm_ss"); //设置显示格式
    QString fileStr="D:\\model"+str;
    int flag=svm_save_model(fileStr.toStdString().c_str(),model);//保存结果集
    if(flag==-1)
    {
        qDebug()<<QStringLiteral("结果集保存失败");
    }
    else if(flag==0)
    {
        qDebug()<<QStringLiteral("结果集保存成功");
    }
    qDebug()<<QStringLiteral("结果集获取正常");
    svm_node *predictNode=new svm_node[Band+1];
    double result;//接收结果
    //    for(b=0;b<Band;b++)
    //    {
    //        predictNode[b].index=b+1;
    //        predictNode[b].value=;
    //    }
    //    predictNode[Band].index=-1;
    //    result=svm_predict(model,predictNode);


    sorted=true;
    QImage image(Samples, Lines, QImage::Format_RGB32);
    image.fill(Qt::white);//将图片背景填充为白色
    QRgb value;

    //int readHeight=100;//每次读取文件的数量100行
    int startHeight=0;//游标的初始位置
    int endHeight=startHeight+readHeight;//当前游标的结束位置

    point *rectangle=new point[2];
    //开始坐标
    rectangle[0].x=0;
    rectangle[0].y=startHeight;
    //结束坐标
    rectangle[1].x=Samples;
    rectangle[1].y=endHeight;
    //二维坐标一维化
    int startNumber=rectangle[0].y*Samples+rectangle[0].x;
    //真实的坐标（一维）
    int realLocationNum=0;
    //整除100剩余的余数处理
    int restHeight=Lines%readHeight;
    int mod=Samples*Lines/100;
    /////////////////////////////////////////////////////////////////////
    for(int m=0;m<Lines/readHeight;m++)
    {
        float **rawData=NULL;
        rawData=openFile(rectangle);
        //k=0;k<readHeight;k++;//当前虚拟坐标（一维的）
        //当前真实坐标（一维的）realLocationNum
        //startNumber=rectangle[0].y*Samples+rectangle[0].x;
        startNumber=rectangle[0].y*Samples+rectangle[0].x;
        //        QTime time;
        //        time.start();
        for(int k=0;k<Samples*readHeight;k++)
        {
            realLocationNum=startNumber+k;
            if(realLocationNum%mod==0)
            {
                emit setProgressValue((realLocationNum*1.0/(Samples*Lines))*100);
                qDebug()<<(realLocationNum*1.0/(Samples*Lines))*100;
                image.save("D://RemoteTemp//classification.tif");
            }

            //样本判断逻辑
            //rawData是二维数组[i][j]i表示第i个波段,j表示第j个点
            for(int b=0;b<Band;b++)
            {
                predictNode[b].index=b+1;
                predictNode[b].value=rawData[b][k]/maxDataValue;
            }
            predictNode[Band].index=-1;

            result=svm_predict(model,predictNode);

            //            if(k%100==0)
            //            {
            //                qDebug()<<time.elapsed()<<"ms";
            //                qDebug()<<result;
            //            }
            int color=result+1;
            //变色
            value = qRgb(R[color], G[color], B[color]);
            image.setPixel(realLocationNum%Samples,realLocationNum/Samples,value);
        }
        startHeight+=readHeight;
        endHeight+=readHeight;
        rectangle[0].x=0;
        rectangle[0].y=startHeight;
        rectangle[1].x=Samples;
        rectangle[1].y=endHeight;
        for(int i=0;i<Band;i++)
        {
            delete []rawData[i];
            rawData[i]=NULL;
        }
        delete[] rawData;
        rawData=NULL;

    }//样本循环
    //余数   剩余部分处理
    rectangle[1].y=Lines;
    float **rawData=NULL;
    rawData=openFile(rectangle);
    startNumber=rectangle[0].y*Samples+rectangle[0].x;
    for(int k=0;k<Samples*restHeight;k++)
    {
        realLocationNum=startNumber+k;
        //样本判断逻辑
        //emit setProgressValue((realLocationNum/Samples*Lines*1.0)*100);
        //rawData是二维数组[i][j]i表示第i个波段,j表示第j个点
        for(int b=0;b<Band;b++)
        {
            predictNode[b].index=b+1;
            predictNode[b].value=rawData[b][k]/maxDataValue;
        }
        predictNode[Band].index=-1;
        result=svm_predict(model,predictNode);

        int color=result+1;
        //变色
        value = qRgb(R[color], G[color], B[color]);
        image.setPixel(realLocationNum%Samples,realLocationNum/Samples,value);
    }

    delete[] rawData;
    rawData=NULL;
    delete[] rectangle;
    rectangle=NULL;

    svm_free_and_destroy_model(&model);
    //显示区分后的图像
    emit complete();
    rawImage=image;
    partImage=image;
    saveTif();

    visiualdraw(visiualDrawP,currentHeight,currentWidth,partImage);
}
/**
 * @brief filedeal::seaLineGet 海岸线提取
 * @param tempImage
 * @param seaColor
 * @param landColor
 */
void filedeal::seaLineGet(QImage tempImage,int *seaColor,int *landColor)
{
    emit initProgressbar();
    emit setProgressRange(0,100);
    emit setProgressValue(0);

    int yRoad[80] = {-4,-4,-4,-4,-4,-4,-4,-4,-4,-3,-3,-3,-3,-3,-3,-3,-3,-3,-2,-2,-2,-2,-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,
                     1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4};
    int xRoad[80] = {-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,1,2,3,4,-4,-3,-2,-1,0,1,
                     2,3,4,-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,0,1,2,3,4};

    QDateTime Systemtime = QDateTime::currentDateTime();//获取系统现在的时间
    QString str = Systemtime.toString("yyyy_MM_dd_hh_mm_ss"); //设置显示格式
    QString fileStr=filePathName;
    fileStr=fileStr+str;

    QImage image(Samples+1, Lines+1, QImage::Format_RGB32);
    image.fill(Qt::white);//将图片背景填充为白色
    QRgb valueA=qRgb(0,0,0);
    //QRgb valueB=qRgb(255,0,0);

    bool gotoFlag=false;
    //线的下标
    int lineCount=0;
    int maxLineIndex=0;
    //点最多线上点的数量
    int maxPointNum=0;
    //当前线上点的数量
    int currentMaxNum=0;

    //坐标栈
    int pointCount=0;
    point *line=NULL;

    if(Samples>Lines)
    {
        line=new point[Samples*100];
    }
    else
    {
        line=new point[Lines*100];
    }

    unsigned short int *footFlag=new unsigned short int[Samples*Lines+1000];
    unsigned short int *orignImage=new unsigned short int[Samples*Lines+1000];
    for(int h=0;h<Lines;h++)
    {
        for(int w=0;w<Samples;w++)
        {
            footFlag[h*Samples+w]=0;
            orignImage[h*Samples+w]=0;
        }
    }

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
        for(int h=0;h<Lines;h++)
        {
            for(int w=0;w<Samples;w++)
            {
                if(QColor(tempImage.pixel(w,h)).red()==areaAR
                        &&QColor(tempImage.pixel(w,h)).green()==areaAG
                        &&QColor(tempImage.pixel(w,h)).blue()==areaAB)
                {
                    //tempImage.setPixel(w,h,valueA);
                    footFlag[h*Samples+w]=1;
                }
                else if(QColor(tempImage.pixel(w,h)).red()==areaBR
                        &&QColor(tempImage.pixel(w,h)).green()==areaBG
                        &&QColor(tempImage.pixel(w,h)).blue()==areaBB)
                {
                    //tempImage.setPixel(w,h,valueB);
                    footFlag[h*Samples+w]=2;
                }

            }
        }
    }
    //所有界限找出
    //进行降噪
    qDebug()<<QStringLiteral("海岸线提取开始.........");
    int flag=42;
    for(int time=0;time<=15;time++)
    {
        for(int h=5;h<Lines-5;h++)
        {
            for(int w=5;w<Samples-5;w++)
            {
                int diw[4]={0,0,0,0},x_,y_;
                for(int f=0 ; f<80 ; f++)
                {
                    x_ = w+xRoad[f];
                    y_ = h+yRoad[f];
                    diw[footFlag[y_*Samples+x_]]++;
                    if(diw[footFlag[y_*Samples+x_]]==flag)
                    {
                        footFlag[h*Samples+w] = footFlag[y_*Samples+x_];
                    }
                }
            }
        }
    }
    for(int h=1;h<Lines-1;h++)
    {
        for(int w=1;w<Samples-1;w++)
        {
            if(footFlag[h*Samples+w]==2&&footFlag[(h+1)*Samples+w]==1||
                    footFlag[h*Samples+w]==2&&footFlag[(h+1)*Samples+(w-1)]==1||
                    footFlag[h*Samples+w]==2&&footFlag[(h+1)*Samples+(w+1)]==1||
                    footFlag[h*Samples+w]==2&&footFlag[(h-1)*Samples+w]==1||
                    footFlag[h*Samples+w]==2&&footFlag[(h-1)*Samples+(w-1)]==1||
                    footFlag[h*Samples+w]==2&&footFlag[(h-1)*Samples+(w+1)]==1||
                    footFlag[h*Samples+w]==2&&footFlag[h*Samples+(w+1)]==1||
                    footFlag[h*Samples+w]==2&&footFlag[h*Samples+(w-1)]==1
                    )

            {
                image.setPixel(w,h,valueA);
                orignImage[h*Samples+w]=1;
            }
            //找一条最长的海岸线
        }
    }
    //    partImage=image;
    //    rawImage=image;
    //    visiualdraw(visiualDrawP,currentHeight,currentWidth,partImage);
    //    qInfo()<<"normal3";
    //将原来的海岸线数组变成标记数组
    for(int h=0;h<Lines;h++)
    {
        for(int w=0;w<Samples;w++)
        {
            footFlag[h*Samples+w]=0;
        }
    }
    //定义一个方向数组
    int next[25][2]={ {0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1},
                      {-2,2},{-1,2},{0,2},{1,2},{2,2},{2,1},{2,0},{2,-1},
                      {2,-2},{1,-2},{0,-2},{-1,-2},{-2,-2},{-2,-1},{-2,0},{-2,1}
                    };
    //    qInfo()<<"normal4";
    int extends=2;
    int footPath=24;
    int nx=0,ny=0;//移动点
    int x=0,y=0;//基点
    try{
        int mod=Samples*Lines/50;
        int sum=Samples*Lines*2;
        for(int h=0;h<Lines;h++)
        {
            for(int w=0;w<Samples;w++)
            {
                if((h*Samples+w)%mod==0)
                {
                    //emit setProgressValue((h*Samples+w)/sum*100);
                    qDebug()<<(double)(h*Samples+w)/sum*100<<"%";
                }

                pointCount=0;
                currentMaxNum=0;

                if(orignImage[h*Samples+w]==1
                        &&footFlag[h*Samples+w]==0)
                {
                    line[currentMaxNum].x=w;
                    line[currentMaxNum].y=h;
                    footFlag[h*Samples+w]=1;
                    currentMaxNum++;
                    //pointCount++;
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
                            if(orignImage[ny*Samples+nx]==1
                                    &&footFlag[ny*Samples+nx]==0)
                            {
                                pointCount++;
                                line[currentMaxNum].x=nx;
                                line[currentMaxNum].y=ny;
                                footFlag[ny*Samples+nx]=1;
                                //pointCount++;
                                currentMaxNum++;
                                gotoFlag=true;
                                break;
                            }
                        }
                        //7个点都不满足返回上一个点
                        if(gotoFlag==false)
                        {
                            //每返回一个点，都要把指针向下移动一格
                            pointCount--;
                            if(pointCount>-1)
                            {
                                //nx,ny坐标回到上一步坐标
                                nx=line[pointCount].x;
                                ny=line[pointCount].y;
                                //允许循环，可以回到上一步
                                gotoFlag=true;
                            }

                        }

                    } while(gotoFlag);

                    if(currentMaxNum>maxPointNum)
                    {
                        maxLineIndex=lineCount;
                        maxPointNum=currentMaxNum;
                        //                        if(maxPointNum>100)
                        //                            qInfo()<<"maxPointNum"<<maxPointNum;
                    }
                    lineCount++;
                    //qInfo()<<"-----------------------------------------";
                }

            }
        }
    }
    catch(QString exception)
    {
        qInfo()<<exception;
    }
    //    partImage=image;
    //    rawImage=image;
    //    visiualdraw(visiualDrawP,currentHeight,currentWidth,partImage);
    qInfo()<<QStringLiteral("最长海岸线的点的数量")<<lineCount;
    qInfo()<<QStringLiteral("最长海岸线的下标")<<maxLineIndex;
    //重置标记
    for(int h=0;h<Lines;h++)
    {
        for(int w=0;w<Samples;w++)
        {
            footFlag[h*Samples+w]=0;
        }
    }
    //线计数置空
    lineCount=0;
    int mod=Samples*Lines/50;
    int sum=Samples*Lines*2;
    for(int h=0;h<Lines;h++)
    {
        for(int w=0;w<Samples;w++)
        {
            if((h*Samples+w)%mod==0)
            {
                //emit setProgressValue(((h*Samples+w)+Samples*Lines)/(Samples*Lines*2)*100);
                qDebug()<<(double)(h*Samples+w)/sum*100<<"%";
            }
            nx=w;ny=h;
            pointCount=0;
            currentMaxNum=0;
            if(orignImage[h*Samples+w]==1
                    &&footFlag[h*Samples+w]==0)
            {
                line[currentMaxNum].x=w;
                line[currentMaxNum].y=h;
                footFlag[h*Samples+w]=1;
                currentMaxNum++;
                //pointCount++;

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
                        if(orignImage[ny*Samples+nx]==1
                                &&footFlag[ny*Samples+nx]==0)
                        {
                            pointCount++;
                            line[currentMaxNum].x=nx;
                            line[currentMaxNum].y=ny;
                            footFlag[ny*Samples+nx]=1;
                            //pointCount++;
                            currentMaxNum++;
                            gotoFlag=true;
                            break;
                        }
                    }
                    //7个点都不满足返回上一个点
                    if(gotoFlag==false)
                    {
                        //每返回一个点，都要把指针向下移动一格
                        pointCount--;
                        //如果没回到起始点，一直往回退
                        if(pointCount>-1)
                        {
                            //nx,ny坐标回到上一步坐标
                            nx=line[pointCount].x;
                            ny=line[pointCount].y;
                            //允许循环，可以回到上一步
                            gotoFlag=true;
                        }

                    }
                } while(gotoFlag);
                //跳出所有循环
                if(lineCount==maxLineIndex)
                {
                    w=Samples;
                    h=Lines;
                    break;
                }
                lineCount++;
            }

        }
    }
    //    qInfo()<<"lineCount"<<lineCount;
    //    qInfo()<<"maxPointNum"<<maxPointNum;
    image.fill(Qt::white);
    for(int i=0;i<maxPointNum;i++)
    {
        image.setPixel(line[i].x,line[i].y,valueA);
    }
    image.setPixel(nx,ny,valueA);
    partImage=image;
    rawImage=image;
    visiualdraw(visiualDrawP,currentHeight,currentWidth,partImage);

    fileStr.append(".tif");
    image.save(fileStr);
    QSqlQuery query;
    QString sqlInsertStr=QString("insert into RemoteSensingSeaLine(name,OwnName) values('%1','%2')").arg(fileStr).arg(filePathName);
    bool querySuccess=query.exec(sqlInsertStr);
    qInfo()<<QStringLiteral("海岸线插入数据库成功")<<querySuccess;
    //    设定当前位置的初值为入口位置；
    //      do{
    //        若当前位置可通，
    //        则{
    //         将当前位置插入栈顶； 　　　　　　// 纳入路径
    //         若该位置是出口位置，则算法结束；
    //          // 此时栈中存放的是一条从入口位置到出口位置的路径
    //         否则切换当前位置的东邻方块为新的当前位置；
    //         }
    //        否则
    //        {
    //        若栈不空且栈顶位置尚有其他方向未被探索，
    //        则设定新的当前位置为: 沿顺时针方向旋转找到的栈顶位置的下一相邻块；
    //        若栈不空但栈顶位置的四周均不可通，
    //        则{ 删去栈顶位置； 　　　　　　　　// 从路径中删去该通道块
    //          若栈不空，则重新测试新的栈顶位置，
    //          直至找到一个可通的相邻块或出栈至栈空；
    //         }
    //       }
    //    } while (栈不空)；

    delete[] line;
    line=NULL;

    delete[] orignImage;
    orignImage=NULL;
    delete[] footFlag;
    footFlag=NULL;
}


//////////////////////////////////海岸线提取///////////////////////////////////////////////////////

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
    int progressValue=0;
    int lowCount=0;
    emit setProgressRange(0,100);
    emit setProgressValue(0);
    for(int time=0;time<2;time++)
    {

        for(int i=18;i>=1;i--)
        {
            lowBadPoints(i*5,i*5);
            lowCount++;
            progressValue=1.38*lowCount;
            emit setProgressValue(progressValue);
        }
        for(int i=1;i<=18;i++)
        {
            lowBadPoints(i*5,i*5);
            lowCount++;
            progressValue=1.38*lowCount;
            emit setProgressValue(progressValue);
        }
        visiualdraw(visiualDrawP,currentHeight,currentWidth,partImage);
    }
    emit complete(QStringLiteral("降噪完成"));

}
////降噪函数
void filedeal::lowBadPoints(int w,int h){
    //    qInfo()<<"lowPoints in";
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
                //                qInfo()<<"there is /0";
                continue;
            }
            //            qInfo()<<"tempNumber/boxNumber:"<<(double)tempNumber/boxNumber<<"tempIndex:"<<tempIndex;
            int centerY=(double)(j*h+(j+1)*h)/2;//圆心y坐标
            int centerX=(double)(i*w+(i+1)*w)/2;//圆心x坐标
            int radius=centerX-i*w;//半径
            if(((double)tempNumber/boxNumber)>=0.97)//直径100时98.7基本不动边界//直径50时96基本不动边界
            {
                //                qInfo()<<tempIndex;
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
                        //                            qInfo()<<location<<"location";
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
                //                qInfo()<<"there is /0";
                continue;
            }
            //            qInfo()<<"tempNumber/boxNumber:"<<(double)tempNumber/boxNumber<<"tempIndex:"<<tempIndex;
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
    //        qInfo()<<i<<"i";
    //        qInfo()<<j<<"j";

}



////保存图像tif文件
void filedeal::saveTif()
{
    QDateTime Systemtime = QDateTime::currentDateTime();//获取系统现在的时间
    QString str = Systemtime.toString("yyyy_MM_dd_hh_mm_ss"); //设置显示格式
    QString fileStr=filePathName;
    fileStr=fileStr+"Rule"+str+".tif";
    if(!partImage.isNull())
    {
        partImage.save(fileStr.toStdString().c_str());
        QString message=QStringLiteral("文件已保存到");
        message=message+fileStr;
        emit messageInfo(message,0);
    }
}

void filedeal::loadsvmModel(QString modelName)
{
    svm_model *model=svm_load_model(modelName.toStdString().c_str());
    getSvmModel(model);
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
        qWarning()<<QStringLiteral("文件为空");
        return;
    }
    int g;
    if(!partImage.isNull())
    {
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
        QString message=QStringLiteral("文件已保存到");
        message=message+fileStr;
        emit messageInfo(message,0);
    }
    if(fp!=NULL)
    {
        fclose(fp);
    }
}
void filedeal::moveLine(int x,int y)
{
    point standPoint;
    //    standPoint.x=x;
    //    standPoint.y=y;
    //    1650 192
    standPoint.x=x;
    standPoint.y=y;
    //    qInfo()<<"location"<<x<<" "<<y;
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
    //    qInfo()<<Samples;
    //    qInfo()<<Lines;
    //emit sendSize(Samples,Lines);
    //fLine是靠近海的那条海岸线
    //sLine是靠近陆地的那条海岸线

    point *fLine=NULL;
    point *sLine=NULL;

    if(Samples>Lines)
    {
        fLine=new point[Samples*50];
        sLine=new point[Samples*50];
    }
    else
    {
        fLine=new point[Lines*50];
        sLine=new point[Lines*50];
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
    //    QRgb valueA=qRgb(255,0,0);
    //    QRgb valueB=qRgb(0,0,255);
    //    QRgb valueC=qRgb(0,0,0);
    //    qInfo()<<Samples;
    //    qInfo()<<Lines;
    if(Samples<500)
    {
        currentWidth=Samples;
    }
    if(Lines<500)
    {
        currentHeight=Lines;
    }
    //emit sendSize(Samples,Lines);
    //fLine是靠近海的那条海岸线
    //sLine是靠近陆地的那条海岸线

    point *fLine=NULL;
    point *sLine=NULL;

    if(Samples>Lines)
    {
        fLine=new point[Samples*100];
        sLine=new point[Samples*100];
        scale=(double)(QApplication::desktop()->height()/4)/Lines;//要想正常显示略缩图必须首先设置缩放率
    }
    else
    {
        fLine=new point[Lines*100];
        sLine=new point[Lines*100];
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
                //                imageF.setPixel(w,h,valueA);
                fLine[x].x=w;
                fLine[x].y=h;
                x++;
            }

            //有线画到总图
            if(QColor(imageS.pixel(w,h)).red()==0&&
                    QColor(imageS.pixel(w,h)).green()==0&&
                    QColor(imageS.pixel(w,h)).blue()==0)
            {
                //                imageF.setPixel(w,h,valueB);
                sLine[y].x=w;
                sLine[y].y=h;
                y++;
            }

        }
    }
    //    imageF.save(imageStrF);
    //    imageS.save(imageStrS);
    --x;
    --y;
    if(1)
    {
        FILE *fp;
        fp=fopen("D://Line1","wb");
        if(fp==NULL)
        {
            return;
        }
        for(int i=0;i<x;i++)
        {
            fprintf(fp,"%d\n",fLine[i].x);
            fprintf(fp,"%d\n",fLine[i].y);
        }
        fclose(fp);

        FILE *fp1;
        fp1=fopen("D://Line2","wb");
        if(fp1==NULL)
        {
            return;
        }
        for(int i=0;i<y;i++)
        {
            fprintf(fp1,"%d\n",sLine[i].x);
            fprintf(fp1,"%d\n",sLine[i].y);
        }
        fclose(fp1);
    }

    calculateDiffer(fLine,sLine,x,y);
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
    //    qInfo()<<"min"<<min;
    //    qInfo()<<"max"<<max;
    //    sLine=reorderSLine(fLine,sLine,min);
    //    for(int i=0;i<min;i++)
    //    {
    //        qInfo()<<"fLine:"<<i<<" x:"<<fLine[i].x<<" y:"<<fLine[i].y;
    //        qInfo()<<"sLine:"<<i<<" x:"<<sLine[i].x<<" y:"<<sLine[i].y<<"\n";
    //    }
    QImage image(Samples, Lines, QImage::Format_RGB32);
    QRgb valueF=qRgb(0,0,255);//从蓝色线网红色线推
    QRgb valueS=qRgb(255,0,0);
    //    QRgb valueT=qRgb(0,255,0);
    image.fill(Qt::white);//将图片背景填充为白色
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
    QRgb valueF=qRgb(0,0,255);
    QRgb valueS=qRgb(255,0,0);
    QRgb valueT=qRgb(0,0,0);
    image.fill(Qt::white);//将图片背景填充为白色
    for(int i=0;i<number;i++)
    {
        image.setPixel(fLineP[i].x,fLineP[i].y,valueF);
        image.setPixel(sLineP[i].x,sLineP[i].y,valueS);
        image.setPixel(tLine[i].x,tLine[i].y,valueT);
        //qInfo()<<"tLine"<<i<<"    "<<tLine[i].x<<"   "<<tLine[i].y;
    }
    image.save("C://abc.tif");
    rawImage=image;
    partImage=image;
    visiualdraw(visiualDrawP,currentHeight,currentWidth,image);
    averageLine(image,Samples,Lines);
    //        unLinkPoint(tLine,number);
    //        rawImage=partImage;
    //        visiualdraw(visiualDrawP,currentHeight,currentWidth,partImage);
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
            }
        }

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
        //qInfo()<<"standPoint.x"<<standPoint.x<<"standPoint.y"<<standPoint.y;
        //qInfo()<<"fLine.x"<<fLine[fIndex].x<<"fLine.y"<<fLine[fIndex].y;
        //qInfo()<<"distance"<<distance(standPoint,sLine)<<" distance(sLine[sIndex],fLine[fIndex])"<<distance(sLine,fLine[fIndex]);
        //qInfo()<<"ratio"<<ratio;
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
        //qInfo()<<"standPoint.x"<<standPoint.x<<"standPoint.y"<<standPoint.y;
        //qInfo()<<"fLine.x"<<fLine[fIndex].x<<"fLine.y"<<fLine[fIndex].y;
        //qInfo()<<"sLine.x"<<sLine[sIndex].x<<"sLine.y"<<sLine[sIndex].y;
        //qInfo()<<"distance(standPoint,sLine[sIndex])"<<distance(standPoint,sLine[sIndex])<<" distance(sLine[sIndex],fLine[fIndex])"<<distance(sLine[sIndex],fLine[fIndex]);
        //qInfo()<<"ratio"<<ratio;
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
    QRgb valueF=qRgb(0,0,255);
    QRgb valueS=qRgb(255,0,0);
    QRgb valueT=qRgb(0,0,0);
    image.fill(Qt::white);//将图片背景填充为白色
    for(int i=0;i<number;i++)
    {
        image.setPixel(fLineP[i].x,fLineP[i].y,valueF);
        image.setPixel(sLineP[i].x,sLineP[i].y,valueS);
        image.setPixel(tLine[i].x,tLine[i].y,valueT);
        //        qInfo()<<"tLine"<<i<<"    "<<tLine[i].x<<"   "<<tLine[i].y;
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
    QRgb valueF=qRgb(255,0,0);
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
            count-=1;
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
    delete[] tempPoi;
    tempPoi=NULL;
}
////废弃不用
point* filedeal::averagePoint(point* line,int number,point* minDistanceLine)
{
    int count=0;
    int radius=11;
    point *dealLine=new point[number];
    point *tempP=new point[20];

    QSet<int> indexSet;
    for(int i=0;i<number;i++)
    {
        indexSet.insert(i);
    }
    QSet<int>::iterator it;
    //如果海岸线是横着的先找最左边的点
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
    //如果海岸线是竖着的先找最上面的点
    point mostTop;
    mostTop.y=tempPoint[0].y;
    int mostTopIndex=0;
    for(int i=0;i<count;i++)
    {
        if(tempPoint[i].y<mostTop.y)
        {
            mostTop.x=tempPoint[i].x;
            mostTop.y=tempPoint[i].y;
            mostTopIndex=i;
        }
    }

    //第二个坐标
    int currentIndex;
    if(mostTop.y<mostLeft.x)
    {
        currentIndex=mostTopIndex;
    }
    else
    {
        currentIndex=mostLeftIndex;
    }
    indexSet.remove(currentIndex);//移除第0个
    dealLine[0].x=minDistanceLine[0].x;
    dealLine[0].y=minDistanceLine[0].y;

    for(int i=1;i<number;i++)
    {
        count=0;
        //找噪点
        for(int j=0;j<number;j++)
        {
            it=indexSet.find(j);
            if(distance(dealLine[i-1],line[j])<=radius&&it!=indexSet.end())
            {
                tempP[count].x=line[j].x;
                tempP[count].y=line[j].y;
                indexSet.remove(j);
                count++;
            }

        }
        int sumX=0,sumY=0,averageX=-1,averageY=-1;
        count-=1;
        //求和
        for(int k=0;k<count;k++)
        {
            sumX+=tempP[k].x;
            sumY+=tempP[k].y;
        }
        //求平均值
        if(count>0)
        {
            averageX=sumX/count;
            averageY=sumY/count;
            dealLine[i].x=averageX;
            dealLine[i].y=averageY;
        }
        else
        {
            //            qInfo()<<"this is a point ............";
            dealLine[i]=minDistanceLine[i];
        }

    }
    delete[]tempP;
    tempP=NULL;
    return dealLine;
}
point* filedeal::newAveragePoint(point* line,int number)
{
    int count=0;
    point *dealLine=new point[number];
    double minDistance=0,maxDistance=0;
    point *tempP=new point[20];
    for(int i=0;i<number-2;i++)
    {
        //距离一
        minDistance=distance(line[i],line[i+1]);
        //距离二
        maxDistance=distance(line[i+1],line[i+2])+minDistance;
        count=0;
        for(int j=0;j<number;j++)
        {
            if(distance(line[i],line[j])>minDistance&&distance(line[i],line[j])<maxDistance)
            {
                //噪点，求平均
                tempP[count]=line[j];
                count++;
            }

        }
        int sumX=0,sumY=0,averageX=-1,averageY=-1;
        tempP[count]=line[i+1];
        //求和
        for(int k=0;k<count;k++)
        {
            sumX+=tempP[k].x;
            sumY+=tempP[k].y;
        }
        //求平均值
        if(count>0)
        {
            averageX=sumX/count;
            averageY=sumY/count;
            //平均点
            dealLine[i].x=averageX;
            dealLine[i].y=averageY;
        }
        else //如果没有平均点
        {
            dealLine[i]=line[i+1];
        }
    }
    delete[]tempP;
    tempP=NULL;
    return dealLine;
}
void filedeal::linkPoint(QImage image,int width,int height)
{
    int count=0;
    int breakValue=0;
    QRgb valueF=qRgb(0,0,0);
    QRgb noVisiusl=qRgb(255,255,255);
    point *tempPoint=new point[Samples*10];
    for(int h=0;h<height;h++)
    {
        for(int w=0;w<width;w++)
        {
            if(QColor(image.pixel(w,h)).red()==255&&
                    QColor(image.pixel(w,h)).green()==0&&
                    QColor(image.pixel(w,h)).blue()==0)
            {
                tempPoint[count].x=w;
                tempPoint[count].y=h;
                count++;
            }
        }
    }
    qInfo()<<"found point numebr is "<<count;
    //    printPointSet(count,tempPoint);
    //判断点是否在线段上
    //或者过定点的曲线
    //先找最小范围


    //////////////////////////////////////找图中最左边的点////////////////////////////////////////////////////
    //如果海岸线是横着的先找最左边的点
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
    //如果海岸线是竖着的先找最上面的点
    point mostTop;
    mostTop.y=tempPoint[0].y;
    int mostTopIndex=0;
    for(int i=0;i<count;i++)
    {
        if(tempPoint[i].y<mostTop.y)
        {
            mostTop.x=tempPoint[i].x;
            mostTop.y=tempPoint[i].y;
            mostTopIndex=i;
        }
    }

    //第二个坐标
    int currentIndex;
    if(mostTop.y<mostLeft.x)
    {
        currentIndex=mostTopIndex;
    }
    else
    {
        currentIndex=mostLeftIndex;
    }
    /////////////////////////////////////////////////////////////////////
    //tempPoint[i],tempPoint[i+1]最小x,最大x
    QSet<int> indexSet;
    point *index=new point[count];

    for(int i=0;i<count;i++)
    {
        indexSet.insert(i);
    }

    count--;
    for(int i=0;i<count;i++)
    {
        index[i].x=currentIndex;
        indexSet.remove(currentIndex);//去重复点
        currentIndex=ptpMinDisIndex(currentIndex,tempPoint,count,indexSet);//最近点下标
        index[i].y=currentIndex;
        indexSet.remove(currentIndex);//去重复点
    }
    point *p=new point[count];
    for(int i=0;i<count;i++)
    {
        p[i]=tempPoint[index[i].x];
    }
    delete[] p;
    p=NULL;

    //连线
    for(int i=0;i<count;i++)
    {
        //如果距离太大自动跳出循环，不再往下连线
        if(distance(tempPoint[index[i].x],tempPoint[index[i].y])>500)
        {
            break;
        }
        breakValue++;
        image=midPointLink(image,tempPoint[index[i].x],tempPoint[index[i].y],valueF);
    }
    //噪点去除
    for(int i=breakValue-1;i<count;i++)
    {
        image.setPixel(tempPoint[index[i].x].x,tempPoint[index[i].x].y,noVisiusl);
    }


    //        point *p=new point[count];
    //        for(int i=0;i<count;i++)
    //        {
    //            p[i]=tempPoint[index[i].x];
    //        }
    //        tempPoint=averagePoint(tempPoint,count,p);
    //        tempPoint=newAveragePoint(p,count);
    //        QImage image1(Samples, Lines, QImage::Format_RGB32);
    //        image1.fill(Qt::white);

    //        delete[] p;
    //        p=NULL;
    //        for(int i=0;i<count-2;i++)
    //        {
    //            if(distance(tempPoint[i],tempPoint[i+1])>100)
    //            {
    //                break;
    //            }
    //            image1=midPointLink(image1,tempPoint[i],tempPoint[i+1],valueF);
    ////            image1.setPixel(tempPoint[i].x,tempPoint[i].y,valueF);
    //        }
    //        partImage=image1;
    //        rawImage=image1;
    //        visiualdraw(visiualDrawP,currentHeight,currentWidth,image1);

    //    FILE *fp;
    //    fp=fopen("D:\\binaryLine","wb");
    //    if(fp==NULL)
    //    {
    //        qWarning()<<QStringLiteral("文件为空");
    //        return;
    //    }
    //    for(int i=0;i<count;i++)
    //    {
    //        fwrite(&tempPoint[index[i].x].x,2,1,fp);
    //        fwrite(&tempPoint[index[i].x].y,2,1,fp);
    //    }
    //    fclose(fp);

    delete[] tempPoint;
    tempPoint=NULL;
    delete[] index;
    index=NULL;
    partImage=image;
    rawImage=image;
    visiualdraw(visiualDrawP,currentHeight,currentWidth,image);
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
        if(tempMin<minDistance)
        {

            minDistance=tempMin;
            minIndex=i;

        }
    }
    //    qInfo()<<"mindistance is "<<distance(p[index],p[minIndex])<<"\n";

    delete[] dis;
    dis=NULL;
    return minIndex;
}

/**
 * @brief filedeal::midPointLink 生成直线的中点画法
 * @param image
 * @param p1
 * @param p2
 * @return
 */
QImage filedeal:: midPointLink(QImage image,point p1,point p2,QRgb color)
{
    int x = p1.x, y = p1.y;
    int a = p1.y - p2.y, b = p2.x - p1.x;
    int cx = (b >= 0 ? 1 : (b = -b, -1));
    int cy = (a <= 0 ? 1 : (a = -a, -1));

    image.setPixel(x, y, color);

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
            image.setPixel(x, y, color);
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
            image.setPixel(x, y, color);
        }
    }
    return image;
}
/**
 * @brief filedeal::calculateDiffer 计算与标准海岸线的误差
 * @param calLine
 * @param stdLine
 * @param calNumber
 * @param stdNumber
 */
void filedeal::calculateDiffer(point*calLine,point *stdLine,int calNumber,int stdNumber)
{
    double calSum=0,stdSum=0;
    double calAverage,stdAverage;
    for(int i=0;i<calNumber;i++)
    {
        int index1=i;
        int index2=oldMinDistanceIndex(calLine[i],stdLine,calNumber);
        calSum+=distance(calLine[index1],stdLine[index2]);
    }
    calAverage=calSum/calNumber;
    for(int i=0;i<stdNumber;i++)
    {
        int index1=i;
        int index2=oldMinDistanceIndex(stdLine[i],calLine,stdNumber);
        stdSum+=distance(stdLine[index1],calLine[index2]);
    }
    stdAverage=stdSum/stdNumber;
    //    qInfo()<<"calAverage"<<calAverage;
    //    qInfo()<<"stdAverage"<<stdAverage;
    //    qInfo()<<calAverage-stdAverage;
}

///**
// * @brief filedeal::unLinkPoint 判断点是否连在海岸线上
// * @param Line
// * @param number
// * @return
// */
//QImage filedeal::unLinkPoint(QImage image,point* Line,int number)
//{
//    //(x,y)
//    //(x+1,y)
//    //(x-1,y)
//    //(x,y+1)
//    //(x,y-1)
//    //(x+1,y+1)
//    //(x+1,y-1)
//    //(x-1,y+1)
//    //(x-1,y-1)
//    QRgb value=qRgb(255,255,255);
//    bool flag=false;
//    int count=0;
//    int *index=new int[number];
//    point p;
//    for(int i=0;i<number;i++)
//    {
//        p=Line[i];
//        flag=false;
//        for(int j=-3;j<4;j++)
//        {
//            for(int k=-3;k<4;k++)
//            {
//                p.x=Line[i].x+j;
//                p.y=Line[i].y+k;
//                if(p.x!=Line[i].x&&p.y!=Line[i].y)
//                {
//                    if(pExistLine(p,Line,number))
//                    {
//                        j=4;
//                        k=4;
//                        flag=true;
//                    }
//                }
//            }
//        }
//        if(!flag)
//        {
//            index[count]=i;
//            count++;
//        }
//    }
//    //    qInfo()<<"number"<<number;
//    //    qInfo()<<"不在线上的点的数量是："<<count;
//    for(int i=0;i<count;i++)
//    {
//        image.setPixel(Line[index[i]].x,Line[index[i]].y,value);

//        //        int lineIndex=pToLminIndex(Line[index[i]],Line,number);//返回不在海岸线上的点与海岸线上距离最近的点的index
//        //        partImage=linkLine(partImage,Line[lineIndex],Line[index[i]]);
//    }
//    return image;
//}

///**
// * @brief filedeal::linkLine 将孤立两点连接成线
// * @param image
// * @param p1 点1
// * @param p2 点2
// * @return 返回连接后的图像
// */

//QImage filedeal::linkLine(QImage image,point p1,point p2)
//{

//    point minP,maxP;
//    QRgb valueF;
//    valueF=qRgb(0,0,0);
//    if(p1.x<p2.x)
//    {
//        minP.x=p1.x;
//        maxP.x=p2.x;
//    }
//    else
//    {
//        minP.x=p2.x;
//        maxP.x=p1.x;
//    }

//    if(p1.y<p2.y)
//    {
//        minP.y=p1.y;
//        maxP.y=p2.y;
//    }
//    else
//    {
//        minP.y=p2.y;
//        maxP.y=p1.y;
//    }

//    for(int h=minP.y;h<maxP.y;h++)
//    {
//        for(int w=minP.x;w<maxP.x;w++)
//        {
//            point test;
//            test.x=w;
//            test.y=h;

//            if(judgePoint(p1,p2,test))
//            {
//                image.setPixel(w,h,valueF);
//            }
//        }
//    }
//    return image;
//}
///**
// * @brief filedeal::judgePoint//向量法：判断中间一点 是否在直线上
// * @param p1
// * @param p2
// * @param testPoint 判断点是否在线段上
// * @return
// */
//bool filedeal::judgePoint(point p1,point p2,point testPoint)
//{
//    double s1,s2,t1,t2;
//    s1=testPoint.x-p1.x;
//    t1=testPoint.y-p1.y;
//    s2=p1.x-p2.x;
//    t2=p1.y-p2.y;
//    if(-5<=(s1*t2-t1*s2)&&(s1*t2-t1*s2)<=5)
//    {
//        return true;
//    }
//    else
//    {
//        return false;
//    }
//}

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

//QImage filedeal::ddaLinkLine(QImage image,point p1,point p2){
//    QRgb valueF;
//    valueF=qRgb(0,0,0);
//    float x,y;
//    float dx,dy,k;
//    dx=(float)(p2.x-p1.x);
//    dy=(float)(p2.y-p1.y);
//    k=dy/dx;//斜率
//    x=p1.x;
//    y=p1.y;

//    if (abs(k)<1)//斜率绝对值小于1时，以x步进
//    {
//        for (;x<=p2.x;++x)
//        {
//            image.setPixel(x,int(y+0.5),valueF);
//            y+=k;
//        }
//    }
//    if (abs(k)>=1)//斜率绝对值大于等于1时，以y步进

//    {
//        for (;y<p2.y;++y)
//        {
//            image.setPixel(int(x+0.5),y,valueF);
//            x+=1/k;
//        }
//    }
//    return image;
//}
////Bresenham  算法
//QImage filedeal::OnBresenhamline(QImage image,point p1,point p2)
//{
//    QRgb valueF;
//    valueF=qRgb(0,0,0);
//    int x1=p1.x, y1=p1.y, x2=p2.x, y2=p2.y;
//    int i,s1,s2,interchange;
//    float x,y,deltax,deltay,f,temp;
//    x=x1;
//    y=y1;
//    deltax=abs(x2-x1);
//    deltay=abs(y2-y1);
//    if(x2-x1>=0) s1=1; else s1=-1;
//    if(y2-y1>=0) s2=1; else s2=-1;

//    if(deltay>deltax)
//    {
//        temp=deltax;
//        deltax=deltay;
//        deltay=temp;
//        interchange=1;
//    }
//    else interchange=0;
//    f=2*deltay-deltax;
//    image.setPixel(x,y,valueF);

//    for(i=1;i<=deltax;i++)
//    {

//        if(f>=0)
//        {
//            if(interchange==1) x+=s1;

//            else y+=s2;
//            image.setPixel(x,y,valueF);
//            f=f-2*deltax;
//        }
//        else
//        {
//            if(interchange==1) y+=s2;
//            else x+=s1;
//            f=f+2*deltay;
//        }
//    }
//    return image;
//}
///**
// * @brief filedeal::ptpMinDisIndex 返回距离最小的点
// * @param index
// * @param p
// * @param pNumber
// * @param indexSet
// * @return
// */
//////////暂时不用
//int filedeal::ptpMinDisIndex(int count,int index,point*p,int pNumber,point *indexSet)
//{
//    int *dis=new int[pNumber];
//    for(int i=0;i<pNumber;i++)
//    {
//        dis[i]=distance(p[index],p[i]);
//    }
//    //自己跟自己的距离设置成最大
//    dis[index]=65535;
//    int  minDistance=65535;

//    int tempMin=0;
//    int minIndex=0;
//    for(int i=0;i<pNumber;i++)
//    {
//        tempMin=dis[i];
//        if(tempMin<minDistance)
//        {
//            //如果该组下标没被用过，说明两点未连成线
//            //若该组下标已经被用过，则跳过该最小距离并不动原来最小的下标
//            if(!judgeExist(count,index,i,indexSet))
//            {
//                minDistance=tempMin;
//                minIndex=i;
//            }

//        }
//    }
//    delete[] dis;
//    dis=NULL;
//    return minIndex;
//}
///**
// * @brief filedeal::judgeExist 判断下标组是否被用过
// * @param number
// * @param index1
// * @param index2
// * @param indexSet
// * @return
// */
//////////暂时不用
//bool filedeal::judgeExist(int number,int index1,int index2,point *indexSet)
//{
//    //查找是否用到过该组下标
//    for(int i=0;i<number;i++)
//    {
//        //该组下标已经用过
//        if(index1==indexSet[i].x&&index2==indexSet[i].y||index2==indexSet[i].x&&index1==indexSet[i].y)
//        {
//            return true;
//        }

//    }
//    //该组下标还未用过
//    return false;
//}



///原海岸线提取
//unsigned short int mark[10000][10000];   //count(max)=561875  有563行
//定义了一个二维数组，将每一个点设成一个坐标。如第一个点为change[0][0] ,以此类推。
//unsigned short int longLineMark[10000][10000] ={0};//记录最长的海岸线
// 去除噪点用的图像
//int flag = 12;
//int x_go[8] = {1,-1,0,0,1,1,-1,-1};
//int y_go[8] = {0,0,1,-1,1,-1,1,-1};

//int x1_go[24] = {-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,0,0,0,0,1,1,1,1,1,2,2,2,2,2};
//int y1_go[24] = {-2,-1,0,1,2,-2,-1,0,1,2,-2,-1,1,2,-2,-1,0,1,2,-2,-1,0,1,2};

//int x2_go[48] = {-3,-3,-3,-3,-3,-3,-3,-2,-2,-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,1,1,1,1,1,1,1,2,2,2,2,2,2,2,3,3,3,3,3,3,3};
//int y2_go[48] = {-3,-2,-1,0,1,2,3,-3,-2,-1,0,1,2,3,-3,-2,-1,0,1,2,3,-3,-2,-1,1,2,3,-3,-2,-1,0,1,2,3,-3,-2,-1,0,1,2,3,-3,-2,-1,0,1,2,3};

//int x3_go[80] = {-4,-4,-4,-4,-4,-4,-4,-4,-4,-3,-3,-3,-3,-3,-3,-3,-3,-3,-2,-2,-2,-2,-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,
//                 1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4};
//int y3_go[80] = {-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,1,2,3,4,-4,-3,-2,-1,0,1,
//                 2,3,4,-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,0,1,2,3,4,-4,-3,-2,-1,0,1,2,3,4};

//定义一个方向数组
//int next[8][2]={{-1,1}, //右上
//                {0,1},//右
//                {1,1}, //右下
//                {1,0},//下
//                {1,-1},//左下
//                {0,-1},//左
//                {-1,-1},//左上
//                {-1,0}};//上

//long int count1,maxLine;
//int *sum=NULL;
//int *regist_x=NULL;
//int *regist_y=NULL;

//void filedeal:: seaLineGet(int *seaColor,int *landColor)
//{
//    sum=new int[Samples*Lines];
//    regist_x=new int[Samples*Lines];
//    regist_y=new int[Samples*Lines];

//    QImage image(Samples, Lines, QImage::Format_RGB32);
//    QRgb value = qRgb(0, 0, 0);
//    image.fill(Qt::white);//将图片背景填充为白色
//    int final=0;
//    count1=0,maxLine=0;
//    memset(sum,0,Samples*Lines*sizeof(int));
//    memset(regist_x ,0,Samples*Lines*sizeof(int));
//    memset(regist_y ,0,Samples*Lines*sizeof(int));
//    memset(mark,0,Samples*Lines*sizeof(unsigned short int));
//    memset(longLineMark,0,Samples*Lines*sizeof(unsigned short int));
//    int i,j;
//    QDateTime Systemtime = QDateTime::currentDateTime();//获取系统现在的时间
//    QString str = Systemtime.toString("yyyy_MM_dd_hh_mm_ss"); //设置显示格式
//    QString fileStr=filePathName;
//    fileStr=fileStr+str;

//    int areaAR=0,areaAG=0,areaAB=0,areaBR=0,areaBG=0,areaBB=0;
//    for(int k=0;k<5;k++)
//    {
//        switch(landColor[k])
//        {
//        case 1:
//            areaAR=255;
//            areaAG=0;
//            areaAB=0;
//            break;
//        case 2:
//            areaAR=255;
//            areaAG=255;
//            areaAB=0;
//            break;
//        case 3:
//            areaAR=0;
//            areaAG=0;
//            areaAB=255;
//            break;
//        case 4:
//            areaAR=0;
//            areaAG=255;
//            areaAB=0;
//            break;
//        case 5:
//            areaAR=139;
//            areaAG=0;
//            areaAB=139;
//            break;
//        case 6:
//            areaAR=0;
//            areaAG=0;
//            areaAB=0;
//            break;
//        default:
//            areaAR=300;
//            areaAG=300;
//            areaAB=300;
//            break;
//        }
//        switch(seaColor[k])
//        {
//        case 1:
//            areaBR=255;
//            areaBG=0;
//            areaBB=0;
//            break;
//        case 2:
//            areaBR=255;
//            areaBG=255;
//            areaBB=0;
//            break;
//        case 3:
//            areaBR=0;
//            areaBG=0;
//            areaBB=255;
//            break;
//        case 4:
//            areaBR=0;
//            areaBG=255;
//            areaBB=0;
//            break;
//        case 5:
//            areaBR=139;
//            areaBG=0;
//            areaBB=139;
//            break;
//        case 6:
//            areaBR=0;
//            areaBG=0;
//            areaBB=0;
//            break;
//            areaBR=300;
//            areaBG=300;
//            areaBB=300;
//        }
//        qInfo()<<areaAR<<"    "<<areaAG<<"   "<<areaAB<<"A";
//        qInfo()<<areaBR<<"    "<<areaBG<<"   "<<areaBB<<"B";
//        for(int h=0;h<Lines;h++)
//        {
//            for(int w=0;w<Samples;w++)
//            {
//                if(QColor(partImage.pixel(w,h)).red()==areaAR
//                        &&QColor(partImage.pixel(w,h)).green()==areaAG
//                        &&QColor(partImage.pixel(w,h)).blue()==areaAB)
//                {
//                    mark[h][w]=1;//长宽对调
//                }
//                else if(QColor(partImage.pixel(w,h)).red()==areaBR
//                        &&QColor(partImage.pixel(w,h)).green()==areaBG
//                        &&QColor(partImage.pixel(w,h)).blue()==areaBB)
//                {
//                    mark[h][w]=2;//长宽对调
//                }
//            }
//        }
//        //        for(i=0;i<Samples*Lines;i++)
//        //        {
//        //            if(data_copy[i].bands[changer]==areaAR&&data_copy[i].bands[changeg]==areaAG&&data_copy[i].bands[changeb]==areaAB)
//        //            {
//        //                mark[i/Samples][(i-1)%Samples]=1;
//        //            }
//        //            else if(data_copy[i].bands[changer]==areaBR&&data_copy[i].bands[changeg]==areaBG&&data_copy[i].bands[changeb]==areaBB)
//        //            {
//        //                mark[i/Samples][(i-1)%Samples]=2;
//        //            }
//        //            else
//        //            {
//        //                mark[i/Samples][(i-1)%Samples]=1;
//        //            }
//        //        }
//    }
//    //淤泥与养殖区的交界线是海岸线或海水与养殖区的交界线是海岸线
//    qInfo()<<"normal in mark";
//    /***************************去噪声点 9宫格*********************************************/
//    /* int time=0;
//     for(time=1;time<=5;time++) {
//      if(time<=2)flag=4;
//      else flag=5;
//    for(i=1123;i>=1;i--)
//        for(j=1994;j>=1;j--) {

//        int diw[4]={0,0,0,0},x_,y_,f;
//        for(int f=0 ; f<8 ; f++)
//        {
//            x_ = i+x_go[f];
//            y_ = j+y_go[f];
//            diw[change[x_][y_]]++;

//            if(diw[change[x_][y_]]==flag)
//            {
//                change[i][j] = change[x_][y_];

//            }
//        }
//            }
//           }        */
//    /*****************************25宫格**************************************/
//    //    int time=0;
//    //    for(time=1;time<=2;time++)
//    //    {
//    //        flag=13;
//    //        for(i=Lines-2;i>=1;i--)
//    //            for(j=Samples-2;j>=1;j--)
//    //            {
//    //                int diw[4]={0,0,0,0},x_,y_;
//    //                for(int f=0 ; f<24 ; f++)
//    //                {
//    //                    x_ = i+x1_go[f];
//    //                    y_ = j+y1_go[f];
//    //                    diw[mark[x_][y_]]++;
//    //                    if(diw[mark[x_][y_]]==flag)
//    //                    {
//    //                        mark[i][j] = mark[x_][y_];
//    //                    }
//    //                }
//    //            }
//    //    }
//    //    int time=0;
//    //    for(time=1;time<=1;time++)
//    //    {
//    //        flag=42;
//    //        for(i=Lines-5;i>=4;i--)
//    //            for(j=Samples-5;j>=4;j--)
//    //            {
//    //                int diw[4]={0,0,0,0},x_,y_;
//    //                for(int f=0 ; f<80 ; f++)
//    //                {
//    //                    x_ = i+x3_go[f];
//    //                    y_ = j+y3_go[f];
//    //                    diw[mark[x_][y_]]++;

//    //                    if(diw[mark[x_][y_]]==flag)
//    //                    {
//    //                        mark[i][j] = mark[x_][y_];
//    //                    }
//    //                }
//    //            }
//    //    }
//    int flag=42;
//    for(int time=0;time<=1;time++)
//    {
//        for(int i=5;i<Lines-5;i++)
//        {
//            for(int j=5;j<Samples-5;j++)
//            {
//                int diw[4]={0,0,0,0},x_,y_;
//                for(int f=0 ; f<80 ; f++)
//                {
//                    x_ = i+x3_go[f];
//                    y_ = j+y3_go[f];
//                    diw[mark[x_][y_]]++;

//                    if(diw[mark[x_][y_]]==flag)
//                    {
//                        mark[i][j] = mark[x_][y_];
//                    }
//                }
//            }
//        }
//    }


//    qInfo()<<"normal in down point";
//    /******************************************49宫格***************************************/
//    //    int time=0;
//    //    for(time=1;time<=1;time++)
//    //    {
//    //        flag=24;
//    //        for(i=1121;i>=3;i--)
//    //            for(j=1992;j>=3;j--)
//    //            {

//    //                int diw[4]={0,0,0,0},x_,y_;
//    //                for(int f=0 ; f<48 ; f++)
//    //                {
//    //                    x_ = i+x2_go[f];
//    //                    y_ = j+y2_go[f];
//    //                    diw[mark[x_][y_]]++;

//    //                    if(diw[mark[x_][y_]]==flag)
//    //                    {
//    //                        mark[i][j] = mark[x_][y_];
//    //                    }
//    //                }
//    //            }

//    //    }

//    /****************************修改数据（寻找符合规则的线）************************************************/
//    for(i=1;i<=Lines-2;i++)
//    {
//        for(j=1;j<=Samples-2;j++)
//        {
//            if((mark[i][j]==2&&mark[i-1][j-1]==1)||
//                    (mark[i][j]==2&&mark[i-1][j+1]==1)||
//                    (mark[i][j]==2&&mark[i-1][j]==1)||
//                    (mark[i][j]==2&&mark[i+1][j-1]==1)||
//                    (mark[i][j]==2&&mark[i+1][j]==1)||
//                    (mark[i][j]==2&&mark[i+1][j+1]==1)||
//                    (mark[i][j]==2&&mark[i][j-1]==1)||
//                    (mark[i][j]==2&&mark[i][j+1]==1) )
//            {
//                longLineMark[i][j]=1;           //   chang1=1记录的是符合规则的线
//                //                value = qRgb(0, 0, 0);
//                //                image.setPixel(j,i, value);
//            }
//        }
//    }//for
//    //    fileStr.append(".tif");
//    //    image.save(fileStr);

//    //    for(i=2;i<=Lines-2;i++)
//    //    {
//    //        for(j=2;j<=Samples-2;j++)
//    //        {
//    //            if((mark[i][j]==2&&mark[i-1][j-1]==1)||
//    //                    (mark[i][j]==2&&mark[i-1][j+1]==1)||
//    //                    (mark[i][j]==2&&mark[i-1][j]==1)||
//    //                    (mark[i][j]==2&&mark[i+1][j-1]==1)||
//    //                    (mark[i][j]==2&&mark[i+1][j]==1)||
//    //                    (mark[i][j]==2&&mark[i+1][j+1]==1)||
//    //                    (mark[i][j]==2&&mark[i][j-1]==1)||
//    //                    (mark[i][j]==2&&mark[i][j+1]==1)||//以后为新加
//    //                    (mark[i][j]==2&&mark[i-2][j-2]==1)||
//    //                    (mark[i][j]==2&&mark[i-2][j-1]==1)||
//    //                    (mark[i][j]==2&&mark[i-2][j]==1)||
//    //                    (mark[i][j]==2&&mark[i-2][j+1]==1)||
//    //                    (mark[i][j]==2&&mark[i-2][j+1]==1)||
//    //                    (mark[i][j]==2&&mark[i-2][j+2]==1)||
//    //                    (mark[i][j]==2&&mark[i-1][j+2]==1)||
//    //                    (mark[i][j]==2&&mark[i][j+2]==1)||
//    //                    (mark[i][j]==2&&mark[i+1][j+2]==1)||
//    //                    (mark[i][j]==2&&mark[i+2][j+2]==1)||
//    //                    (mark[i][j]==2&&mark[i+2][j+1]==1)||
//    //                    (mark[i][j]==2&&mark[i+2][j]==1)||
//    //                    (mark[i][j]==2&&mark[i+2][j-1]==1)||
//    //                    (mark[i][j]==2&&mark[i+2][j-2]==1)||
//    //                    (mark[i][j]==2&&mark[i+1][j-2]==1)||
//    //                    (mark[i][j]==2&&mark[i][j-2]==1)||
//    //                    (mark[i][j]==2&&mark[i-1][j-2]==1) )
//    //            {
//    //                longLineMark[i][j]=1;           //   chang1=1记录的是符合规则的线
//    //            }
//    //        }
//    //    }
//    qInfo()<<"longLineMark complete";

//    for(i=1;i<=Lines-2;i++)
//    {
//        for(j=1;j<=Samples-2;j++)
//        {
//            mark[i][j]=0;
//        }
//        //此时的change是book
//    }   //寻找最长的线


//    /******************************************修改数据************************************************************************/
//    qInfo()<<"before dfs";

//    for(i=1;i<=Lines-2;i++)
//    {
//        for(j=1;j<=Samples-2;j++)
//        {
//            count1++;//用作计数的
//            regist_x[count1]=i;//记录x坐标
//            regist_y[count1]=j;//记录y坐标
//            mark[i][j]=1;   //此时的change是book
//            dfs(i,j);
//        }
//    }

//    qInfo()<<count1<<"count1";
//    qInfo()<< regist_x[count1]<<"regist_x[count1]";
//    qInfo()<< regist_y[count1]<<"regist_x[count1]";
//    qInfo()<<maxLine<<"max";

//    maxLine=0;
//    for(i=1;i<=count1;i++)
//    {
//        if(maxLine<sum[i])
//        {
//            maxLine=sum[i];
//            final=i;
//        }
//    }

//    qInfo()<<final<<"final";

//    for(i=1;i<=Lines-2;i++)
//    {
//        for(j=1;j<=Samples-2;j++)
//        {
//            mark[i][j]=0;//change重置为0，在画最长的过程中依旧为book
//        }
//    }

//    qInfo()<<regist_x[final]<<"regist_x[final]";
//    qInfo()<<regist_y[final]<<"regist_y[final]";

//    mark[regist_x[final]][regist_y[final]]=1;
//    longLineMark[regist_x[final]][regist_y[final]]=2;     //change1=2记录的是最长的线
//    redfs(regist_x[final],regist_y[final]);

//    for(i=1;i<=Lines-2;i++)
//    {
//        for(j=1;j<=Samples-2;j++)
//        {
//            if(longLineMark[i][j]!=2)
//            {
//                longLineMark[i][j]=0;
//            }
//        }
//    }


//    for(i=1;i<=Lines-2;i++)
//    {
//        for(j=1;j<=Samples-2;j++)
//        {
//            if(longLineMark[i][j]==2 )
//            {
//                image.setPixel(j,i, value);
//            }
//        }
//    }
//    partImage=image;
//    visiualdraw(visiualDrawP,currentHeight,currentWidth,partImage);
//    fileStr.append(".tif");
//    image.save(fileStr);
//    QSqlQuery query;
//    QString sqlInsertStr=QString("insert into RemoteSensingSeaLine values('%1','%2')").arg(fileStr).arg(filePathName);
//    query.exec(sqlInsertStr);
//    delete[] sum;
//    delete[] regist_x;
//    delete[] regist_y;
//}
//void filedeal::dfs(int x,int y)
//{

//    int k,tx,ty;

//    for(k=0;k<=7;k++)
//    {
//        tx=x+next[k][0];
//        ty=y+next[k][1];
//        //判断是否越界
//        if(tx<1||tx>Lines-1||ty<1||ty>Samples-1)
//            continue;
//        if(longLineMark[tx][ty]==1&&mark[tx][ty]==0)   //此时的change是book
//        {
//            sum[count1]++;	//将每根线的线长存到sum数组中
//            mark[tx][ty]=1;
//            dfs(tx,ty);
//        }
//    }
//    return;

//}
//void filedeal:: redfs(int x,int y)
//{
//    int k,tx,ty;

//    for(k=0;k<=7;k++)
//    {
//        tx=x+next[k][0];
//        ty=y+next[k][1];
//        //判断是否越界
//        if(tx<1||tx>Lines-1||ty<1||ty>Samples-1)
//            continue;
//        if(longLineMark[tx][ty]>0&&mark[tx][ty]==0)   //此时的change是book
//        {
//            mark[tx][ty]=1;
//            longLineMark[tx][ty]=2;
//            redfs(tx,ty);
//        }
//    }
//    return;

//}
