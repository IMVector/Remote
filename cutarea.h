#ifndef CUTAREA_H
#define CUTAREA_H

#include <QObject>
#include <QImage>
#include <QtCore>

class CutArea : public QObject
{
    Q_OBJECT

public:
    explicit CutArea(QObject *parent = 0);
    void mCutArea(QImage iamge_, int ridus, int firstMixColor, int secondMixColor);
private:
    QImage image;
    void cut(QImage image, int ridus, int firstMixColor, int secondMixColor);
    void expansion(QImage image, int firstMixColor, int secondMixColor);
signals:

public slots:
};

#endif // CUTAREA_H
