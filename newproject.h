#ifndef NEWPROJECT_H
#define NEWPROJECT_H
#include <QMainWindow>
#include <QtDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QDialog>
#include <QMessageBox>
#include <emptychild.h>
#include <filedeal.h>
#include <QThread>
#include <operate.h>
#include <database.h>
#include <QCloseEvent>
#include <zonedeal.h>
#include <zoneform.h>
#include <newzoneform.h>
#include <cutarea.h>
#include <erodinganddilating.h>
#include <splitjoint.h>
#include <bolckclassification.h>
#include <postprocess.h>

namespace Ui {
class newProject;
}

class newProject : public QMainWindow
{
    Q_OBJECT

public:
    explicit newProject(QWidget *parent = 0);
    ~newProject();
    //设置皮肤样式
    static void SetStyle(const QString &styleName)
    {
        QFile file(QString(":/image/%1.css").arg(styleName));
        file.open(QFile::ReadOnly);
        QString qss = QLatin1String(file.readAll());
        qApp->setStyleSheet(qss);
        qApp->setPalette(QPalette(QColor("#F0F0F0")));
    }
private:
    Ui::newProject *ui;
    operate * operateUI;
    database * Sqlsever;
    zonedeal *zone;
    QThread *empty_thread;
    Zoneform *zoneform;
    NewZoneForm *newZoneform;
    CutArea *cutarea;
    ErodingAndDilating *ead;
    SplitJoint *splitJoint;
    BlockClassification *myClassfication;
    PostProcess *postProcess;

    void closeEvent(QCloseEvent *event);
    void classfication();
private slots:

    void on_openFileImage_triggered();
    void on_openDatabaseImage_triggered();
    void on_findSeaLine_triggered();
    void on_firstLine_triggered();
    void on_secondLine_triggered();
    void on_combine_triggered();
    void on_action_triggered();
    void on_actiontest_triggered();
    void on_action_2_triggered();

    void on_actionEroding_triggered();

    void on_actionchangeColor_triggered();

    void on_actionSplitJoint_triggered();

    void on_actionCorroding_triggered();

    void on_actiontest_2_triggered();

    void on_actioncombine_triggered();

    void on_actiondeleteArea_triggered();

signals:



};

#endif // NEWPROJECT_H
