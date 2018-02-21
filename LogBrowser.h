#ifndef LOGBROWSER_H
#define LOGBROWSER_H

#include <QCoreApplication>
#include <QTextBrowser>
#include <QPushButton>
#include <QMessageBox>
#include <QCloseEvent>
#include <QWidget>
#include <QLayout>
#include <QDebug>

class LogBrowser:public QWidget
{
    Q_OBJECT

public:
    explicit LogBrowser(QWidget *parent=0);
    ~LogBrowser();
    void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private:
    bool is_finished;
    QTextBrowser *browser;
    QPushButton *start_button;
    QPushButton *clear_button;

    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

};
#endif // LOGBROWSER_H
