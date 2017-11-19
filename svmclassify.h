#ifndef SVMCLASSIFY_H
#define SVMCLASSIFY_H
#include "svm.h"
#include <QObject>
#include <iostream>
#include <string.h>
#include <QtCore>

class svmclassify : public QObject
{
    Q_OBJECT
    void main();
public:
    explicit svmclassify(QObject *parent = 0);

signals:

public slots:
};

#endif // SVMCLASSIFY_H
