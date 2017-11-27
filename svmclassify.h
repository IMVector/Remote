#ifndef SVMCLASSIFY_H
#define SVMCLASSIFY_H
#include "svm.h"
#include <QObject>
#include <iostream>
#include <string.h>
#include <QtCore>
#include <classify.h>
class svmclassify : public QObject
{
    Q_OBJECT
private:
    void main();
    int *infoProcess(int geoTypeNumber, int *eachNumber);
    unsigned short *labelProcess(int *numberArray, int geoTypeNumber);

public:
    explicit svmclassify(QObject *parent = 0);

signals:
    void sendModel(svm_model* model);

public slots:
    void train(int band, simples date, int geoTypeNumber, int *eachNumber, int sumNumber);

};

#endif // SVMCLASSIFY_H
