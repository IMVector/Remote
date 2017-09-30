#ifndef COMBINELINE_H
#define COMBINELINE_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
class combineLine :  public QLabel
{
    Q_OBJECT
public:
    explicit combineLine(QWidget *parent = 0);

signals:

public slots:
private slots:
    void mousePressEvent(QMouseEvent *ev);
};

#endif // COMBINELINE_H
