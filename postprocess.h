#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include <QObject>
#include <QImage>
#include <filedeal.h>
class PostProcess : public QObject
{
    Q_OBJECT
public:
    explicit PostProcess(QObject *parent = 0);

signals:

public slots:
};

#endif // POSTPROCESS_H
