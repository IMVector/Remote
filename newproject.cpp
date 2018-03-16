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


    operateUI=new operate();
    zoneform=new Zoneform();
//    zoneform->show();

//    empty_thread = new emptychild();//空线程
//    zone = new zonedeal();//类
//    zone->moveToThread(empty_thread);//处理数据线程移入空线程
//    empty_thread->start();//线程启动

//    connect(zoneform,SIGNAL(fileSender(QString)),zone,SLOT(loadImage_slot(QString)));
//    qRegisterMetaType<loadInfo>("loadInfo");//注册自定义类型的槽信号
//    connect(zoneform,SIGNAL(zoneInfoSender(loadInfo)),zone,SLOT(loadInfo_slot(loadInfo)));


    empty_thread = new emptychild();//空线程
    newZoneform = new NewZoneForm();//类
    newZoneform->moveToThread(empty_thread);//处理数据线程移入空线程
    empty_thread->start();//线程启动

    connect(zoneform,SIGNAL(fileSender(QString)),newZoneform,SLOT(loadImage_slot(QString)));
    qRegisterMetaType<loadInfo>("loadInfo");//注册自定义类型的槽信号
    connect(zoneform,SIGNAL(zoneInfoSender(loadInfo)),newZoneform,SLOT(loadInfo_slot(loadInfo)));
//    connect(newZoneform,SIGNAL(sendImageToUi(QImage,int)),operateUI,SLOT(getImage(QImage,int)));//子线程与UI线程通信
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
/**
 * @brief newProject::on_actiontest_triggered 基于空间格局的测试
 */
void newProject::on_actiontest_triggered()
{
    zoneform->show();
//    empty_thread = new emptychild();//空线程
//    zone = new zonedeal();//类
//	zone->moveToThread(empty_thread);//处理数据线程移入空线程
//	empty_thread->start();//线程启动
//    connect(zone,SIGNAL(sendImageToUi(QImage,int)),operateUI,SLOT(getImage(QImage,int)));//子线程与UI线程通信
//	QTimer::singleShot(0, zone, SLOT(startZone()));
//    empty_thread=new emptychild;//空线程
//    zone=new zonedeal;//类
//    zone->moveToThread(empty_thread);//处理数据线程移入空线程
//    empty_thread->start();//线程启动

}

