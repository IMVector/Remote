#ifndef SPLITJOINT_H
#define SPLITJOINT_H

#include <QObject>
#include <QtCore>
#include <QImage>
#include <qDebug>

class SplitJoint : public QObject
{
    Q_OBJECT

public:
    explicit SplitJoint(QObject *parent = 0);
    QImage joint(QImage Original, QImage part, int startX, int startY);
    QImage changeColor(QImage image, QRgb changeColor, QRgb changedColor);
    QImage combine(QImage image1, QImage image2);
signals:

public slots:
};

#endif // SPLITJOINT_H
