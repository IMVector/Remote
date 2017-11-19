#include "imagefile.h"

#include <QImage>

imageFile::imageFile()
{
}

/**
 * @brief imageFile::openFile 打开原始文件
 * @return
 */
unsigned int ** imageFile::openFile(QString fileName)
{
    GDALAllRegister();//注册gdal驱动
    CPLSetConfigOption( "GDAL_FILENAME_IS_UTF8", "YES" );//中文路径中文名称支持
    gdalData = ( GDALDataset* )GDALOpen(fileName.toStdString().c_str(), GA_ReadOnly );//打开文件
    if ( gdalData == NULL )
    {
        qWarning()<<QStringLiteral("数据集为空");
        return NULL;
    }
    QList<GDALRasterBand*> bandList;
    qDeleteAll(bandList);
    bandList.clear();
    int Band=gdalData->GetRasterCount();
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

    QList<unsigned int*> rawList;
    for(int i=0;i<Band;i++)
    {
        unsigned int *tempRaw=new unsigned [picHeight*picWidth];//GDT_Float32
        bandList.at( i )->RasterIO( GF_Read, 0, 0, picWidth, picHeight, tempRaw, picWidth, picHeight, GDT_UInt16, 0, 0 );
        rawList.append(tempRaw);
    }
    unsigned int **rawData=NULL;
    rawData=new unsigned int *[Band];
    for(int i=0;i<Band;i++)
    {
        rawData[i]=rawList.at(i);
    }
    GDALClose(gdalData);
    return rawData;
}
/**
 * @brief imageFile::openFile 显示经过拉伸后的图像
 * @param r
 * @param g
 * @param b
 * @return
 */
QImage imageFile::openFile(int r, int g, int b, QString fileName)
{
    GDALAllRegister();//注册gdal驱动
    CPLSetConfigOption( "GDAL_FILENAME_IS_UTF8", "YES" );//中文路径中文名称支持
    gdalData = ( GDALDataset* )GDALOpen(fileName.toStdString().c_str(), GA_ReadOnly );//打开文件

    QImage image(gdalData->GetRasterXSize(), gdalData->GetRasterYSize(), QImage::Format_RGB32);
    QRgb value;
    image.fill(Qt::white);//将图片背景填充为白色

    if ( gdalData == NULL )
    {
        qWarning()<<QStringLiteral("数据集为空");
        return image;
    }
    int Band=gdalData->GetRasterCount();
    if(r>Band||g>Band||b>Band)
    {
        r=1,g=2,b=3;
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
    unsigned int *tempRaw_band1=new unsigned int [picHeight*picWidth];
    unsigned int *tempRaw_band2=new unsigned int [picHeight*picWidth];
    unsigned int *tempRaw_band3=new unsigned int [picHeight*picWidth];
    bandList.at( r )->RasterIO( GF_Read, 0, 0, picWidth, picHeight, tempRaw_band1, picWidth, picHeight, GDT_UInt16, 0, 0 );
    bandList.at( g )->RasterIO( GF_Read, 0, 0, picWidth, picHeight, tempRaw_band2, picWidth, picHeight, GDT_UInt16, 0, 0 );
    bandList.at( b )->RasterIO( GF_Read, 0, 0, picWidth, picHeight, tempRaw_band3, picWidth, picHeight, GDT_UInt16, 0, 0 );
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

    //以下r，g，b均不是形参的rgb意义

    for( int y=0;y<picHeight;y++)
    {
        for(int x=0;x<picWidth;x++)
        {
            r=processData_band1[y * picWidth + x];
            g=processData_band2[y * picWidth + x];
            b=processData_band3[y * picWidth + x];
            value = qRgb(r, g, b);
            image.setPixel(x,y,value);
        }
    }
    delete[] processData_band1;
    delete[] processData_band2;
    delete[] processData_band3;
    processData_band1=NULL;
    processData_band2=NULL;
    processData_band3=NULL;
    return image;
}

/**
 * @brief imageFile::picSketch 图像拉伸算法 图像线性拉伸(将rgb转化到0-255内)
 * @param buffer
 * @param currentBand
 * @param bandSize
 * @param noValue
 * @return
 */
unsigned char* imageFile::picSketch( unsigned int* buffer , GDALRasterBand* currentBand, int bandSize, double noValue )
{
    unsigned char* normalBand = new unsigned char[bandSize];
    unsigned int max, min;
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

