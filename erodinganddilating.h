#ifndef ERODINGANDDILATING_H
#define ERODINGANDDILATING_H

#include <QObject>
#include <QtCore>
#include <QImage>

typedef struct
{
    bool flag;
    QRgb value;
}pane;

class ErodingAndDilating : public QObject
{
    Q_OBJECT


    pane *getRoundPixel(QImage image, QRgb changeColor, int x, int y);
public:
    explicit ErodingAndDilating(QObject *parent = 0);
    QImage expend(QString imagePath, QRgb expendColor);
    QImage corrode(QString imagePath, QRgb corrodeColor);
signals:

public slots:
};

#endif // ERODINGANDDILATING_H
