#ifndef ZONEFORM_H
#define ZONEFORM_H

#include <QWidget>
#include <QtCore>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
typedef struct{
    int disThresould;
    int deleteThresould ;
    int geoTh;
    int changedTh;
    float adjIntensity;

}loadInfo;

namespace Ui {
class Zoneform;
}

class Zoneform : public QWidget
{
    Q_OBJECT

public:
    explicit Zoneform(QWidget *parent = 0);
    ~Zoneform();

signals:
    void fileSender(QString fileName);
    void zoneInfoSender(loadInfo info);
private slots:
    void openFile();
    void startChange();
private:
    Ui::Zoneform *ui;
};

#endif // ZONEFORM_H
