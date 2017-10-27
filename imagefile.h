#ifndef IMAGEFILE_H
#define IMAGEFILE_H

#include <QObject>
#include <QtCore>
#include <gdal_priv.h>

class imageFile
{
public:
    imageFile();
private:
    GDALDataset *gdalData;    // 图像gdal数据集
    unsigned int **openFile(QString fileName);
    QImage openFile(int r, int g, int b,QString fileName);
    unsigned char *picSketch(unsigned int *buffer, GDALRasterBand *currentBand, int bandSize, double noValue);

};

#endif // IMAGEFILE_H
