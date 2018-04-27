#ifndef BOLCKCLASSIFICATION_H
#define BOLCKCLASSIFICATION_H

#include <QObject>
#include <QtCore>
#include <filedeal.h>
typedef struct
{
    unsigned short area1_Count;
    unsigned short area2_Count;
}typeCount,*typeCountArray;

class BlockClassification : public QObject
{
    Q_OBJECT

    int Band=0;
    int Samples=0;
    int Lines=0;

    float **openFile(QString filePath);
    float **openFile(Points rectangle, QString filePath);
    float **preProcessData(float **rawData, int Samples, int Lines, int Band);
    float *getRound(int x, int y, float **data, int Samples, int Band);
    QImage classfication(float **data, int Samples, int Lines, int Band);

    //    QImage newClassfication(float **data, int Samples, int Lines, int Band);
    QImage blockJudge(QImage image, float averageBand, float stdev, float **data, int Samples, int x, int y, int width, int height);
    float blockAverage(float **data, int Samples, int x, int y, int width, int height);
    QImage relativeClssfication(float **data, int Samples, int Lines, int Band);
    float myStdev(float **data, int Samples, float averageBand, int x, int y, int width, int height);
    QImage newClassification(float **data, int Samples, int Lines, int Band);
    bool isSatisfy(float **data, int Samples, int Lines, int Band, int x, int y);
    bool processImage(float **data, int Samples, int Band, int x, int y, int width, int height);
    QImage judge(float **data, int Samples, int Lines, int Band);
    typeCountArray countRelative(float **data, int Samples, int Lines, int Band);
    typeCountArray blockJudgeCount(typeCountArray numArray, float averageBand, float stdev, float **data, int Samples, int x, int y, int width, int height);
    QImage getLand(float **data, int Samples, int Lines, int Band);
    float **deleteLand(float **data, int Samples, int Lines, int Band);
    QImage landFishAreaJudge(float **data, int Samples, int Lines, int Band);
    typeCountArray blockLandFishAreaJudgeCount(typeCountArray numArray, float averageBand, float stdev, float **data, int Samples, int x, int y, int width, int height);
    typeCountArray countLandFishAreaRelative(float **data, int Samples, int Lines, int Band);
public:
    explicit BlockClassification(QObject *parent = 0);
    void start(QString path);
signals:

public slots:
};

#endif // BOLCKCLASSIFICATION_H
