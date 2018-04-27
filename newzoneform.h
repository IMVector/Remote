#ifndef NEWZONEFORM_H
#define NEWZONEFORM_H

#include <QObject>
#include <filedeal.h>
#include <QtCore>
#include <QStack>
#include <QString>
#include <zonedeal.h>


class NewZoneForm : public QObject
{
    Q_OBJECT
public:
    explicit NewZoneForm(QObject *parent = 0);

private:
    int deleteThresould = 5000;//最小区域点数量阈值
    int distanceThresould = 70;//距离阈值
    float adjIntensity = 0.30;//邻接强度阈值
    int geoTh = 2;//要找成排的地物
    int changedColorTh = 3;//更改后的颜色
    QString fileName;

    void main(QImage image, int deleteThresould, int distanceThresould, float adjIntensity, int geoTh, int changedColorTh);
    AreaNodeInfo *countEveryNumber(ImageArray imageArray, int Samples, int Lines);
    Area pointIterator(ImageArray imageArray, int Samples, int Lines, AreaNodeInfo nodeinfo);
    int calAreaCycle(unsigned short *imageArray, int Samples, int Lines, Area stake, AreaNodeInfo nodeinfo);
    AreaNodeInfo rightOrDownLink(ImageArray imageArray, int Samples, int Lines, AreaNodeInfo areaInfo, Area area);
    double distance(point p1, point p2);
    bool distancesConfine(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2);
    int adjancentColor(ImageArray image, int Samples, int Lines, AreaNodeInfo nodeInfo, int deleteThresould, AreaNodeInfo *allInfo);
    void deepSearchRight(AreaNodeInfo *nodeInfo, int currentNode, int colorTh);
    void deepRightIterater(AreaNodeInfo *nodeInfo, int currentNode);
    void deepIterater(Graph *graph);
    void findEveryPatch(Graph *graph, int start, int end, QString path);
    void deep(Graph *graph);
    void fileStackWrite(QString path, QStack<HeadNode *> *stack);
    void fileStackRead(QString path, unsigned short *roadDirection, Graph *graph);
    void deleteFile(QString path);
    void getRoad(Graph *graph, int start, int end, unsigned short *roadDirection);
    void roadProcess(QVector<unsigned short> road, QVector<unsigned short> direction, Graph *graph);
    Area areaEdge(unsigned short *imageArray, int Samples, int Lines, Area stake, AreaNodeInfo nodeinfo);
    float calAreaEdgedistance(ImageArray image, Area pointSet1, Area pointSet2, unsigned int threshold);
    void fileStackMinRead(QString path);
    AdjancentChanin processResult(AreaNodeInfo *areaInfo, int nodeNum, unsigned int colorTh);
    AdjancentChanin sameColorProcess(Graph *graph, ImageArray image, int Samples, int Lines, unsigned short colorTh, AreaNodeInfo *areaInfo, unsigned int threshold, float adjIntensity);
    unsigned short *newChangeColor(ImageArray imageArray, int Samples, int Lines, AreaNodeInfo nodeinfo, int color, int numThreshold);
    void test(ImageArray testImage, int Samples, int Lines, int deleteThresould, int distanceThresould, float adjIntensity, int geoTh, int changedColorTh);
    void corrosion(ImageArray image, int background, int area);
signals:
    void sendImageToUi(QImage image, int status);
public slots:
    void loadImage_slot(QString name);
    void loadInfo_slot(loadInfo info);
};

#endif // NEWZONEFORM_H
