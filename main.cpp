#include "newproject.h"
#include <QApplication>
#include <loginfo.h>

//#include <vld.h>

//QPointer<Loginfo> mylog;
Loginfo *mylog;
/**
 * @brief outputMessage 将日志输出到文件
 * @param type
 * @param context
 * @param msg
 */
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

//    if (log_broswer)
//    {
//        log_broswer->outputMessage(type, context, msg);
//    }
    if(mylog)
    {
//        QTimer::singleShot(0, mylog, SLOT(showLog(message)));
        emit mylog->sendMsg(message);
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //解决数据库乱码
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
    //////////////////////////////////////////主题///////////////////////////////

    QString path=":/image/blue.css";
    QFile file(path);
    file.open(QFile::ReadOnly);
    QString qss = QLatin1String(file.readAll());
    qApp->setStyleSheet(qss);
    qApp->setPalette(QPalette(QColor("#F0F0F0")));
    //    QString path = QDir::currentPath();//获取当前运行路径
    //    path.replace("/","\\");
    //    path.append("\\image\\blue.css");
    //////////////////////////////////////主题结束///////////////////////////////////////////



#ifdef QT_NO_DEBUG
    //注册MessageHandler
    qDebug() << "release mode";
//    mylog=new Loginfo();
//    mylog->show();
//    qInstallMessageHandler(outputMessage);


#else
    qDebug() << "debug mode";
//    log_broswer = new LogBrowser();
//    log_broswer->show();

//    mylog=new Loginfo();
//    mylog->show();
//    qInstallMessageHandler(outputMessage);
#endif
    //打印日志到文件中
    //    qDebug("This is a debug message");
    //    qWarning("This is a warning message");
    //    qCritical("This is a critical message");
    //    qFatal("This is a fatal message");


    newProject mainFrame;
    mainFrame.show();

    return a.exec();
}
