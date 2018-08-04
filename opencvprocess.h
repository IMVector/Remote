#ifndef OPENCVPROCESS_H
#define OPENCVPROCESS_H

#include <QObject>
#include <QWidget>

class OpenCVProcess : public QObject
{
    Q_OBJECT
public:
    explicit OpenCVProcess(QObject *parent = 0);

signals:

public slots:
};

#endif // OPENCVPROCESS_H