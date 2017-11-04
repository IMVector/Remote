#include "newproject.h"
#include "ui_newproject.h"
QString file;
newProject::newProject(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::newProject)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);//保持窗口最前
    this->setMaximumSize(1000,0);
    this->setMinimumSize(1000,0);
    this->setGeometry(2,30,1000,0);

    this->setWindowTitle(QStringLiteral("遥感系统"));
    this->setWindowIcon(QIcon(":/Remote/img/main.png"));
    operateUI=new operate;

}

newProject::~newProject()
{
    delete ui;

}

void newProject::on_openFileImage_triggered()//文件路径打开文件
{
    file = QFileDialog::getOpenFileName(
                this,
                "Open Document",
                QDir::currentPath(),
                "All files(*.*)");
    if (!file.isNull())
    { //用户选择了文件
       operateUI->getFilePath(file);
       operateUI->show();
       operateUI->move(500,80);
    }
    else
    {
        QMessageBox::information(this, "Warning",QStringLiteral("       你未选中任何文件        "), QMessageBox::Ok);
    }
}

void newProject::on_openDatabaseImage_triggered()//数据库打开文件
{
    operateUI->showSql();
    operateUI->show();
    operateUI->move(500,80);
}



void newProject::on_findSeaLine_triggered()
{
    operateUI->findSeaLine();
    operateUI->show();
    operateUI->move(500,80);
}

void newProject::on_firstLine_triggered()
{
    file = QFileDialog::getOpenFileName(
                this,
                "Open Document",
                QDir::currentPath(),
                "All files(*.*)");
    if (!file.isNull())
    { //用户选择了文件
        operateUI->imageStrF=file;
    }
    else
    {
        // 用户取消选择
        QMessageBox::information(this, "Warning", QStringLiteral("你未选中任何文件"), QMessageBox::Ok);
    }
}

void newProject::on_secondLine_triggered()
{
    file = QFileDialog::getOpenFileName(
                this,
                "Open Document",
                QDir::currentPath(),
                "All files(*.*)");
    if (!file.isNull())
    { //用户选择了文件
        operateUI->imageStrS=file;
    }
    else
    {
        // 用户取消选择
        QMessageBox::information(this, "Warning", QStringLiteral("      你未选中任何文件      "), QMessageBox::Ok);
    }
}

void newProject::on_combine_triggered()
{
    operateUI->tempCombineLine();
}

void newProject::on_action_triggered()
{
    operateUI->Setting();
}
void newProject::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton button;
    button=QMessageBox::warning(this, "Warning", QStringLiteral("             退出系统？                "), QMessageBox::Yes|QMessageBox::Cancel);
    if(button==QMessageBox::Yes)
    {
        qApp->exit();
    }
    else if(button==QMessageBox::Cancel)
    {
        event->ignore();
    }

}
