#ifndef OPERATE_H
#define OPERATE_H
#include <QWidget>
#include <filedeal.h>
#include <QtCore>
#include <QtDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QDialog>
#include <QMessageBox>
#include <emptychild.h>
#include <filedeal.h>
#include <QThread>
#include <MyLabel.h>
#include <MyDialog.h>
#include <QDesktopWidget>
#include <QMetaType>
#include <sealine.h>
#include <database.h>
#include <classify.h>
#include <dbsetting.h>
#include <showinfo.h>
#include <progressbar.h>
#include <showinfo.h>
#include <imagefile.h>
namespace Ui {
class operate;
}

class operate : public QWidget
{
    Q_OBJECT

public:
    explicit operate(QWidget *parent = 0);
    ~operate();
    showInfo *messageDialog;
    QString imageStrF;
    QString imageStrS;
    void getFilePath(QString filePath);
    void findSeaLine();
    void showSql();
    void tempCombineLine();
    void Setting();
public slots:
    void getSetString(QString database,QString url, QString databseName, QString userName, QString password);
private:
    Ui::operate *ui;
    QThread *empty_thread;
    filedeal *fileDeal;
    MyLabel *label;
    MyDialog * choiceSample;
    QThread *emptySortThread;
    classify * mySort;
    sealine *seaLine;
    database  *Sqlsever;
    dbsetting *setting;
    progressBar *proBar;
    showInfo *message;
    int visiualX=0;
    int visiualY=0;



    void initShowBandWidget();//初始化显示波段面板
    bool getDetails(bool flag);
    void initChoice();
    void initCombox();
    //    void showInfo(QString info, QString type);
private slots:
    void uiGetBand(int band);
    void on_fileBandWidget_clicked(const QModelIndex &index);
    void on_loadImage_clicked();
    void on_comboBox_3_activated(int index);
    void getImage(QImage image,int status);
    void on_getSampleBtn_clicked();
    void on_startSortBtn_clicked();
    void getselect_enable();
    void get_stop_select();
    void get_remove_area(int currentIndex);
    void updateVisiual(int x,int y);
    void addPoint(int x0, int y0);
    void SampleOver();
    void getDataDetails(int number);
    void getFeature(QStringList feature);
    void on_showRule_clicked();
    void openSeaLine(QString imageName);
    void getRuleOrLineName(QString imageName);
    void getImageName(QString imageName);
    void on_lowPointsBtn_clicked();
    void on_saveTif_clicked();
    void on_saveBinary_clicked();
    void on_openFromFileBtn_clicked();
    void getSize(int width,int height);
    void getDataFileDetails(int number,int geoNumber);
    void lowPointsComplete(QString message);
    void showMessage(QString message, int type);
    void on_ruleOpenBtn_clicked();
    void on_svmStartBtn_clicked();
    void initProBar();

    void on_loadModelBtn_clicked();

signals:
    void sendFileName(QString fileName);
    void showImage(int r,int g,int b);
    void startSelect();
    void getDataImage(Points p);
    void getData(Points selectPoint,int number);
    void deleteCurrent(int currentIndex);
    void newCurrent(int current);
    void sendDetails(SamplesDetails details);
    void sortAlready(QString ruleName);
    void lowPoiSignal();
    void mCombine(QString imageF,QString imageS);
    void sendSimpleInfo(QString name,int *number);
    void sendSvmDetails(SamplesDetails details);
    void sendSvmModel(QString file);

};

#endif // OPERATE_H
