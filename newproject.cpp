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

    empty_thread = new emptychild();//空线程
    zone = new zonedeal();//类
    zone->moveToThread(empty_thread);//处理数据线程移入空线程
    empty_thread->start();//线程启动

    connect(zoneform,SIGNAL(fileSender(QString)),zone,SLOT(loadImage_slot(QString)));
    qRegisterMetaType<loadInfo>("loadInfo");//注册自定义类型的槽信号
    connect(zoneform,SIGNAL(zoneInfoSender(loadInfo)),zone,SLOT(loadInfo_slot(loadInfo)));


    //    empty_thread = new emptychild();//空线程
    //    newZoneform = new NewZoneForm();//类
    //    newZoneform->moveToThread(empty_thread);//处理数据线程移入空线程
    //    empty_thread->start();//线程启动

    //    connect(zoneform,SIGNAL(fileSender(QString)),newZoneform,SLOT(loadImage_slot(QString)));
    //    qRegisterMetaType<loadInfo>("loadInfo");//注册自定义类型的槽信号
    //    connect(zoneform,SIGNAL(zoneInfoSender(loadInfo)),newZoneform,SLOT(loadInfo_slot(loadInfo)));
    ////    connect(newZoneform,SIGNAL(sendImageToUi(QImage,int)),operateUI,SLOT(getImage(QImage,int)));//子线程与UI线程通信



    splitJoint=new SplitJoint();
    ead=new ErodingAndDilating();
    myClassfication=new BlockClassification();

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


void newProject::on_action_2_triggered()
{
    QImage image;
    image.load("D://resources.tif");
    cutarea=new CutArea;
    empty_thread = new emptychild();//空线程
    cutarea->moveToThread(empty_thread);
    //first是要保留的颜色
    //second是结合的颜色，保留区域中间的
    cutarea->mCutArea(image,300,4,2);

}
/**
 * @brief newProject::on_actionEroding_triggered 图像膨胀
 */
void newProject::on_actionEroding_triggered()
{

    qDebug()<<"erosion Start";
    QRgb changeColor=qRgb(0,0,0);
    for(int i=0;i<1;i++)
    {
        QImage image=ead->expend("D://erosion//erosionComplete.tif",changeColor);
        image.save("D://erosion//erosionComplete.tif");
    }

    qDebug()<<"erosion complete";
}
/**
 * @brief newProject::on_actionCorroding_triggered 图像腐蚀
 */
void newProject::on_actionCorroding_triggered()
{
     qDebug()<<"corrode Start";
    QRgb changeColor=qRgb(0,0,0);
    for(int i=0;i<1;i++)
    {
        QImage image=ead->corrode("D://erosion//erosionComplete.tif",changeColor);
        image.save("D://erosion//erosionComplete.tif");
    }
      qDebug()<<"corrode complete";

}
/**
 * @brief newProject::on_actionchangeColor_triggered 更改颜色
 */
void newProject::on_actionchangeColor_triggered()
{
    qDebug()<<"change Color running";
    QImage image;
    image.load("D:\\fishArea\\im1.tif");
    if(image.isNull())
    {
        qDebug()<<"ERROR";
        return;
    }
    QRgb changeRgb=qRgb(255,0,0);
    QRgb changedRgb=qRgb(0,0,0);
    QImage newImage =splitJoint->changeColor(image,changeRgb,changedRgb);
    newImage.save("D:\\fishArea\\left.tif");
}
/**
 * @brief newProject::on_actionSplitJoint_triggered 图像合成
 */
void newProject::on_actionSplitJoint_triggered()
{
    QImage orignal;
    orignal.load("D:\\fishArea\\newImage.tif");
    QImage part;
    part.load("D:\\fishArea\\left.tif");

    int startX=1024;
    int startY=2483;
    QImage newImage =splitJoint->joint(orignal,part,startX,startY);
    newImage.save("D:\\fishArea\\newnewimage.tif");
}


void newProject::classfication()
{
    file = QFileDialog::getOpenFileName(
                this,
                "Open Document",
                QDir::currentPath(),
                "All files(*.*)");
    if (!file.isNull())
    { //用户选择了文件
        myClassfication->start(file);
    }
    else
    {
        QMessageBox::information(this, "Warning",QStringLiteral("       你未选中任何文件        "), QMessageBox::Ok);
    }


}
/**
 * @brief newProject::on_actiontest_2_triggered 边缘检测
 */
void newProject::on_actiontest_2_triggered()
{
    classfication();
}
/**
 * @brief newProject::on_actioncombine_triggered 图像融合
 */
void newProject::on_actioncombine_triggered()
{

    QImage orignal;
    orignal.load("D:\\test\\im1.tif");
    QImage orignal2;
    orignal2.load("D:\\test\\im2.tif");
    QImage image=splitJoint->combine(orignal,orignal2);
    image.save("D:\\test\\aaaaaa.tif");
}
