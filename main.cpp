#include "newproject.h"
#include <QApplication>
void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

    QString text;
    switch(type)
    {
    case QtDebugMsg:
        text = QString("Debug:");
        break;

    case QtWarningMsg:
        text = QString("Warning:");
        break;

    case QtCriticalMsg:
        text = QString("Critical:");
        break;

    case QtFatalMsg:
        text = QString("Fatal:");
    }

    QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("(%1)").arg(current_date_time);
    QString message = QString("%1 %2 %3 %4").arg(text).arg(context_info).arg(msg).arg(current_date);

    QFile file("log.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //解决数据库乱码
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
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

    //////////////////////////////////////主题结束///////////////////////////////////////////


#ifdef QT_NO_DEBUG
    //注册MessageHandler
    //qDebug() << "release mode";
    qInstallMessageHandler(outputMessage);
#else
    qDebug() << "debug mode";
#endif
    //打印日志到文件中
    //    qDebug("This is a debug message");
    //    qWarning("This is a warning message");
    //    qCritical("This is a critical message");
    //    qFatal("This is a fatal message");


    newProject w;
    w.show();

    return a.exec();
}
