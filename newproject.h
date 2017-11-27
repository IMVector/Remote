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
    void closeEvent(QCloseEvent *event);
private slots:

    void on_openFileImage_triggered();
    void on_openDatabaseImage_triggered();

    void on_findSeaLine_triggered();

    void on_firstLine_triggered();

    void on_secondLine_triggered();

    void on_combine_triggered();

    void on_action_triggered();

    void on_actiontest_triggered();



signals:



};

#endif // NEWPROJECT_H
