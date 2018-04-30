#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include <QObject>
#include <QImage>
#include <filedeal.h>
#include <zonedeal.h>
class PostProcess : public QObject
{
    Q_OBJECT
    unsigned short *changeColor(ImageArray imageArray, int Samples, int Lines, AreaNodeInfo nodeinfo, int color);
    AreaNodeInfo *countEveryNumber(ImageArray imageArray, int Samples, int Lines);

    void erode();
    void deleteArea();
    Area pointIterator(ImageArray imageArray, int Samples, int Lines, AreaNodeInfo nodeinfo);
public:
    explicit PostProcess(QObject *parent = 0);
    void loadImage(QString path);
signals:

public slots:
};

#endif // POSTPROCESS_H
