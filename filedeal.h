#ifndef FILEDEAL_H
#define FILEDEAL_H
#include <QtCore>
#include <QObject>
#include <gdal_priv.h>
#include <QImage>
#include <QApplication>
#include <QtGui>
#include <QDesktopWidget>
#include <classify.h>
#include <stdio.h>
#include <set>
#include <String>
#include <algorithm>
#include <showinfo.h>
typedef struct{
    unsigned short int x;
    unsigned short int y;
}point, *Points;
class filedeal : public QObject
{
    Q_OBJECT
public:
    filedeal();
    void startSort();
    void saveBinary();
    void saveTif();
private:
    double scale;
    GDALDataset *gdalData;    // 图像gdal数据集
    bool selectEnable=false;
    int readHeight=100;
    int visiualX=0;
    int visiualY=0;
    int currentHeight=500;
    int currentWidth=500;
    unsigned char *picSketch(float *buffer, GDALRasterBand *currentBand, int bandSize, double noValue);
    int judgeArea(int point_number, Points mpoints, int testx, int testy);
    float **openFile();
    void openFile(int r, int g, int b);
    void visiualdraw(point p, int visiualHeight, int visiualWidth,QImage visiualImage);
    void getAimData(Points mPoints, float **data);
    void addPoint(float x0, float y0);
    void loadPartImage(int startX, int startY, int visiualHeight, int visiualWidth, QImage visiualImage);
    void lowBadPoints(int w, int h);
    point *reorderSLine(point *fLineP, point *sLineP, int number);
    double distance(point p1, point p2);
    point calculatePoint(point fPoint, point sPoint, double oldRatio);
    double oldRatio(point *fLine, point *sLine, int number, point standPoint, double k);
    double slope(point p1, point p2);
    void oldMove(point **lineSet, point standPoint, int number);
    point **loadLine(QString imageStrF, QString imageStrS);
    void moveLine(int x, int y);
    double ratio(point standPoint, point sLine, point *fLine, int number);
    void move(point **lineSet, point standPoint, int number);
    int minDistanceIndex(point mark, point *sLine, int number, QSet<int> indexSet);
    int oldMinDistanceIndex(point standPoint, point *sLine, int number);
    double newRatio(point standPoint, int sIndex, point sLineP, point *fLine);
    void averageLine(QImage image, int width, int height);
    bool judgePoint(point p1, point p2, point testPoint);
    void linkPoint(QImage image, int width, int height);
    int ptpMinDisIndex(int index, point *p, int pNumber, QSet<int> indexSet);
    bool pExistLine(point p, point *Line, int number);
    QImage unLinkPoint(QImage image, point *Line, int number);
    QImage linkLine(QImage image, point p1, point p2);
    int pToLminIndex(point alonePoint, point *Line, int number);
    int ptpMinDisIndex(int count, int index, point *p, int pNumber, point *indexSet);
    bool judgeExist(int number, int index1, int index2, point *indexSet);
    QImage ddaLinkLine(QImage image, point p1, point p2);
    QImage OnBresenhamline(QImage image, point p1, point p2);
    QImage midPointLink(QImage image, point p1, point p2,QRgb color);
    point *newAveragePoint(point *line, int number);
    void calculateDiffer(point *calLine, point *stdLine, int calNumber, int stdNumber);
    point *averagePoint(point *line, int number, point *minDistanceLine);
    void seaLineGet(QImage partImage, int *seaColor, int *landColor);
    float **openFile(Points rectangle);
    void newGetAimData(Points rectangle, float **data);

private slots:
    //    void seaLineGet(int *seaColor, int *landColor);
    void openPathFile(QString fileName);
    void getRGB(int r,int g,int b);
    void getMouse(int x,int y);
    void startSelectDate();
    void getData_image(Points points);
    void sizeChange(int height,int width);
    void findEdge(Points selectPoint, int number);
    void currentFunction(int currentCount);
    void deleteDataFun(int currentIndex);
    void parser(QString ruleName);
    void newParser(QString ruleName);

    void lowPointsStart();
    void combineLine(QString imageF, QString imageS);
    void saveSample(QString name);
    void simpleInfo(QString name,int *number);
    void lineMouse(int x, int y);

    void slotSealine(int *seaColor, int *landColor);
signals:
    void sendVisiualP(int x,int y);
    void bandToUi(int band);
    void sendImageToUi(QImage image,int status);
    void dataDetails(int number);
    void sendData(simples data,int number);
    void sendFeature(QStringList list);
    void sendSize(int width,int height);
    void fileDataDetails(int number,int geoNumber);
    void setProgressValue(int value);
    void setProgressRange(int startRange,int endRange);
    void complete(QString message);
    void messageInfo(QString message,int type);
};

#endif // FILEDEAL_H
