#include "newproject.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //////////////////////////////////////////主题///////////////////////////////
//    QString path = QDir::currentPath();//获取当前运行路径
//    path.replace("/","\\");
//    path.append("\\image\\blue.css");
    QString path=":/image/blue.css";
    QFile file(path);
    file.open(QFile::ReadOnly);
    QString qss = QLatin1String(file.readAll());
    qApp->setStyleSheet(qss);
    qApp->setPalette(QPalette(QColor("#F0F0F0")));

    /////////////////////////////////////////////////////////////////////////////////

    newProject w;
    w.show();

    return a.exec();
}
