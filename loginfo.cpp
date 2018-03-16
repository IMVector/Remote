#include "loginfo.h"
#include "ui_loginfo.h"

Loginfo::Loginfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Loginfo)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Log日志"));
    connect(ui->clearBtn, &QPushButton::clicked, ui->textEdit,&QTextEdit::clear);
    connect(this,SIGNAL(sendMsg(QString)),this,SLOT(showLog(QString)));
}

Loginfo::~Loginfo()
{
    delete ui;
}
void Loginfo::showLog(QString msg)
{
    ui->textEdit->append(msg);
}
void Loginfo::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton answer = QMessageBox::question(
        this,
        tr("Close Log Browser?"),
        tr("Do you really want to close the log browser?"),
        QMessageBox::Yes | QMessageBox::No);
    if (answer == QMessageBox::Yes)
        event->accept();
    else
        event->ignore();
}
void Loginfo::keyPressEvent(QKeyEvent *event)
{
    event->ignore();
}
