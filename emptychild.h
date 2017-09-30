#ifndef EMPTYCHILD_H
#define EMPTYCHILD_H
#include <QThread>
#include <QtCore>

class emptychild: public QThread
{
    Q_OBJECT
public:
    emptychild();
    void run();
};

#endif // EMPTYCHILD_H
