#include "operate.h"
#include "ui_operate.h"
#include <QListView>
double scale=1;//缩放比例
int RGBselectCount=0;//已经选择波段的数目
int currentSmall=0;//当前略缩图的index
int currentBig=0;//当前细节图的index
int selectR=0,selectG=0,selectB=0;//要显示的三个波段
int count_for_select_area=-1;//已经选择的地物种类
int count_for_add_point=0;//抠图选点数目
bool selectEnable=false;//样本选择功能是否开启
bool create_label=false;//是否允许创建新label
int labelIndex=0;
QString ruleName;
Points Point=(point*)malloc(100*sizeof(point));//找到数据的矩形区域
Points p=(point*)malloc(2*sizeof(point));
QList <MyLabel*> labelList;//label的列表
SamplesDetails details={0};//传入地物区分功能的样本信息

operate::operate(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::operate)
{
    ui->setupUi(this);
    this->setMaximumSize(320,520);
    this->setMinimumSize(320,520);
    this->setWindowTitle(QStringLiteral("遥感系统"));
    empty_thread=new emptychild;//空线程
    fileDeal=new filedeal;//类
    fileDeal->moveToThread(empty_thread);//处理数据线程移入空线程
    empty_thread->start();//线程启动

    emptySortThread=new emptychild;//空线程
    mySort=new classify;
    mySort->moveToThread(emptySortThread);
    emptySortThread->start();

    choiceSample=new MyDialog;
    initShowBandWidget();//初始化要显示波段的面板
    initCombox();//初始化combox控件
    choiceSample->setWindowFlags(Qt::WindowStaysOnTopHint);

    //    imageFile *file=new imageFile;

    seaLine=new sealine;

    Sqlsever=new database;//数据库文件浏览

    ui->startSortBtn->setEnabled(false);


    ///////////////////////////////////////////////测试

    //    empty_thread1=new emptychild;//空线程
    //    zone=new zonedeal;//类
    //    zone->moveToThread(empty_thread);//处理数据线程移入空线程F
    //    empty_thread1->start();//线程启动




    ////////////////////////////////////////////









    qRegisterMetaType<Points>("Points");//注册自定义类型的槽信号
    qRegisterMetaType<SamplesDetails>("SamplesDetails");//注册自定义类型的槽信号
    qRegisterMetaType<simples>("simples");//注册自定义类型的槽信号
    connect(this,SIGNAL(getDataImage(Points)),fileDeal,SLOT(getData_image(Points)));//样本选择时图片变化
    connect(this,SIGNAL(sendFileName(QString)),fileDeal,SLOT(openPathFile(QString)));//向处理线程发送文件名及路径
    connect(this,SIGNAL(showImage(int,int,int)),fileDeal,SLOT(getRGB(int,int,int)));//显示图片按钮与子线程通信
    connect(this,SIGNAL(startSelect()),fileDeal,SLOT(startSelectDate()));//开始选择样本初始化
    connect(this,SIGNAL(getData(Points,int)),fileDeal,SLOT(findEdge(Points,int)));//开始选择样本初始化
    connect(this,SIGNAL(deleteCurrent(int)),fileDeal,SLOT(deleteDataFun(int)));//删除样本时信号
    connect(this,SIGNAL(newCurrent(int)),fileDeal,SLOT(currentFunction(int)));//新建样本时信号
    connect(this,SIGNAL(sendDetails(SamplesDetails)),mySort,SLOT(getDetails(SamplesDetails)));//向决策树地物区分线程发送样本地物波段等信息
    connect(this,SIGNAL(sendSvmDetails(SamplesDetails)),fileDeal,SLOT(dataInfoGet(SamplesDetails)));
    connect(this,SIGNAL(sortAlready(QString)),fileDeal,SLOT(newParser(QString)));
    connect(this,SIGNAL(lowPoiSignal()),fileDeal,SLOT(lowPointsStart()));
    connect(this,SIGNAL(mCombine(QString,QString)),fileDeal,SLOT(combineLine(QString,QString)));
    connect(this,SIGNAL(sendSimpleInfo(QString,int*)),fileDeal,SLOT(simpleInfo(QString,int*)));//读取样本并发送信息去文件处理
    connect(this,SIGNAL(sendSvmModel(QString)),fileDeal,SLOT(loadsvmModel(QString)));

    connect(choiceSample,SIGNAL(select_enable()),this,SLOT(getselect_enable()));//样本选择开关
    connect(choiceSample,SIGNAL(stop_select()),this,SLOT(get_stop_select()));//暂停选择样本
    connect(choiceSample,SIGNAL(remove_area(int)),this,SLOT(get_remove_area(int)));//删除选择的区域
    connect(choiceSample,SIGNAL(sendSimpleName(QString)),fileDeal,SLOT(saveSample(QString)));//保存样本信号
    connect(fileDeal,SIGNAL(bandToUi(int)),this,SLOT(uiGetBand(int)));//显示波段的面板
    connect(fileDeal,SIGNAL(sendImageToUi(QImage,int)),this,SLOT(getImage(QImage,int)));//子线程与UI线程通信
    connect(fileDeal,SIGNAL(sendVisiualP(int,int)),this,SLOT(updateVisiual(int,int)));//更新显示框
    connect(fileDeal,SIGNAL(dataDetails(int)),this,SLOT(getDataDetails(int)));//更新显示框
    connect(fileDeal,SIGNAL(sendData(simples,int)),mySort,SLOT(getdata_from_ui(simples,int)));
    connect(fileDeal,SIGNAL(sendFeature(QStringList)),this,SLOT(getFeature(QStringList)));//获取决策树特征
    connect(fileDeal,SIGNAL(sendSize(int,int)),this,SLOT(getSize(int,int)));//有新图片要显示时获取新图片的size
    connect(fileDeal,SIGNAL(fileDataDetails(int,int)),this,SLOT(getDataFileDetails(int,int)));
    connect(fileDeal,SIGNAL(messageInfo(QString,int)),this,SLOT(showMessage(QString,int)));
    connect(fileDeal,SIGNAL(initProgressbar()),this,SLOT(initProBar()));//初始化progressbar


    connect(mySort,SIGNAL(sortComplete(QString)),fileDeal,SLOT(newParser(QString)));
    connect(seaLine,SIGNAL(getSealine(int*,int*)),fileDeal,SLOT(slotSealine(int*,int*)));//海岸线提取

    connect(Sqlsever,SIGNAL(sendImageName(QString)),this,SLOT(getImageName(QString)));//数据库发来的文件
    connect(Sqlsever,SIGNAL(sendRule(QString)),this,SLOT(getRuleOrLineName(QString)));//数据库发来的规则
    connect(Sqlsever,SIGNAL(sendSeaLine(QString)),this,SLOT(openSeaLine(QString)));//数据库发来的规则

    //    connect(this,SIGNAL(click()),zone,SLOT(click()));
    //    connect(zone,SIGNAL(sendImageToUi(QImage,int)),this,SLOT(getImage(QImage,int)));//子线程与UI线程通信

}








operate::~operate()
{
    delete ui;
}
/**
 * @brief operate::findSeaLine 找海岸线UI
 */
void operate::findSeaLine()
{
    seaLine->show();
}
/**
 * @brief operate::tempCombineLine 合并海岸线信号发送
 */
void operate::tempCombineLine()
{
    emit mCombine(imageStrF,imageStrS);
}
/**
 * @brief operate::initProBar初始化进度条
 */
void operate::initProBar()
{
    proBar=new progressBar;
    connect(mySort,SIGNAL(setProgressRange(int,int)),proBar,SLOT(setTotalRange(int,int)));
    connect(mySort,SIGNAL(setProgressValue(int)),proBar,SLOT(changeValue(int)));
    connect(mySort,SIGNAL(complete()),proBar,SLOT(close()));
    connect(fileDeal,SIGNAL(complete()),proBar,SLOT(close()));
    connect(fileDeal,SIGNAL(setProgressRange(int,int)),proBar,SLOT(setTotalRange(int,int)));
    connect(fileDeal,SIGNAL(setProgressValue(int)),proBar,SLOT(changeValue(int)));
    connect(fileDeal,SIGNAL(complete(QString)),this,SLOT(lowPointsComplete(QString)));

}
/**
 * @brief operate::lowPointsComplete降噪完成弹出消息框
 * @param message
 */
void operate::lowPointsComplete(QString message)
{
    messageDialog=new showInfo;
    messageDialog->setMessage(message,0);
    messageDialog->show();
    proBar->close();
}
/**
 * @brief operate::showMassage 消息提示框
 * @param message
 * @param type
 */
void operate::showMessage(QString message,int type)
{
    messageDialog=new showInfo;
    messageDialog->setMessage(message,type);
    messageDialog->show();
}
/**
 * @brief operate::initChoice初始化样本选择框
 */
void operate::initChoice()
{
    count_for_select_area=-1;
    count_for_add_point=0;
    for(int i=0;i<6;i++)
    {
        details.eachnumberFlag[i]=0;
        details.eachnumber[i]=0;//删除当前的样本值
    }
}
////降噪函数
void operate::on_lowPointsBtn_clicked()
{
    initProBar();
    emit lowPoiSignal();
}
////从数据库打开分类规则
void operate::getRuleOrLineName(QString imageName)
{
    ruleName=imageName;
    ui->tabWidget->setCurrentWidget(ui->tabOutTree);
}
////打开从数据库查询的图像
void operate::getImageName(QString imageName)//数据库发来的文件打开
{
    details.fileName=imageName;
    emit sendFileName(imageName);
    ui->tabWidget->setCurrentWidget(ui->tabImage);
}
////查看规则后的图片
void operate::on_showRule_clicked()
{
    emit sortAlready(ruleName);
    ui->tabWidget->setCurrentWidget(ui->tabOutTree);
}
/**
 * @brief operate::on_ruleOpenBtn_clicked 打开区分规则
 */
void operate::on_ruleOpenBtn_clicked()
{
//    emit click();


}
////数据库查询打开海岸线
void operate::openSeaLine(QString imageName)
{
    emit sendFileName(imageName);
    emit showImage(0,1,2);
}
////显示数据库查询UI
void operate::showSql()
{
    Sqlsever->show();
    //Sqlsever->move(QApplication::desktop()->width()-Sqlsever->width(),50);
}
/**
 * @brief operate::Setting 数据库设置
 */
void operate::Setting()
{
    setting=new dbsetting;
    setting->connect(setting,SIGNAL(sendDBSetting(QString,QString,QString,QString,QString)),this,SLOT(getSetString(QString,QString,QString,QString,QString)));
    setting->show();
}
/**
 * @brief operate::getSetString 数据库设置从UI获取参数值
 * @param database
 * @param url
 * @param databseName
 * @param userName
 * @param password
 */
void operate::getSetString(QString database,QString url,QString databseName,QString userName,QString password)
{
    Sqlsever->Setting(database,url,databseName,userName,password);
}
////打开文件路径
void operate::getFilePath(QString filePath)
{
    details.fileName=filePath;
    emit sendFileName(filePath);
    ui->tabWidget->setCurrentWidget(ui->tabImage);
}
////文件刚打开时要显示的波段
void operate::uiGetBand(int band)
{
    if(band==0)
    {
        QMessageBox::information(this, "Warning", "ERROR Try again", QMessageBox::Ok);
    }
    else{
        details.bandNumber=band;
        ui->fileBandWidget->setRowCount(band);
        ui->fileBandWidget->setColumnCount(2);
        ui->fileBandWidget->setColumnWidth(0,120);
        ui->fileBandWidget->setColumnWidth(1,135);
        QStringList header;
        header<< "ID" <<QStringLiteral("波段");
        ui->fileBandWidget->setHorizontalHeaderLabels(header);//显示标题
        ui->fileBandWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不允许修改
        ui->fileBandWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//整行选中
        ui->fileBandWidget->verticalHeader()->setVisible(false);   //隐藏列表头
        for(int i=0;i<band;i++)
        {
            QString strID = QString::number(i+1);
            ui->fileBandWidget->setItem(i,0,new QTableWidgetItem(strID));
            ui->fileBandWidget->setItem(i,1,new QTableWidgetItem(QStringLiteral("波段")+strID));
        }
    }
}
////初始化波段面板
void operate::initShowBandWidget()
{
    ui->showBandWidget->setRowCount(3);
    ui->showBandWidget->setColumnCount(2);
    ui->showBandWidget->setColumnWidth(0,120);
    ui->showBandWidget->setColumnWidth(1,135);
    QStringList header;
    header<<"ID"<<QStringLiteral("要显示的波段");
    ui->showBandWidget->setItem(0,0,new QTableWidgetItem(QStringLiteral("波段1")));
    ui->showBandWidget->setItem(1,0,new QTableWidgetItem(QStringLiteral("波段2")));
    ui->showBandWidget->setItem(2,0,new QTableWidgetItem(QStringLiteral("波段3")));
    ui->showBandWidget->setHorizontalHeaderLabels(header);//显示标题
    ui->showBandWidget->verticalHeader()->setVisible(false);//隐藏列表头
    ui->showBandWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不允许修改
}
////上面的所有波段的面板响应事件
void operate::on_fileBandWidget_clicked(const QModelIndex &index)
{
    QString strNumber;
    int i=RGBselectCount%3;
    switch (i) {
    case 0:
        selectR=index.row();
        RGBselectCount++;
        strNumber = QString::number(selectR+1);//输入样本数量
        ui->showBandWidget->setItem(i,1,new QTableWidgetItem(QStringLiteral("波段")+strNumber));
        break;
    case 1:
        selectG=index.row();
        RGBselectCount++;
        strNumber = QString::number(selectG+1);//输入样本数量
        ui->showBandWidget->setItem(i,1,new QTableWidgetItem(QStringLiteral("波段")+strNumber));
        break;
    case 2:
        selectB=index.row();
        RGBselectCount++;
        strNumber = QString::number(selectB+1);//输入样本数量
        ui->showBandWidget->setItem(i,1,new QTableWidgetItem(QStringLiteral("波段")+strNumber));
        break;
    }

}
////显示图片按钮响应事件
void operate::on_loadImage_clicked()
{
    emit showImage(selectR,selectG,selectB);
}
////新建LabelcomBox
void operate::on_comboBox_3_activated(int index)
{
    if(index==0)
    {
        create_label=false;
        QString strNumber = QString::number(labelList.size()/2+1);//输入样本数量
        ui->comboBox_3->addItem(QStringLiteral("显示区")+strNumber);
    }
    else
    {
        labelIndex=2*index-2;
        //        qDebug()  <<labelIndex<<"activated";
    }
    qDebug()<<QStringLiteral("当前图片显示器的数量")<<labelList.size();
}
void operate::initCombox()
{
    ui->comboBox_3->setItemText(0,QStringLiteral("新建显示区"));
    ui->comboBox->setStyleSheet("QComboBox QAbstractItemView::item{height:20px;}");
    ui->comboBox->setView(new QListView());
    ui->comboBox_3->setStyleSheet("QComboBox QAbstractItemView::item{height:20px;}");
    ui->comboBox_3->setView(new QListView());

}

void operate::getSize(int width, int height)
{
    if(labelList.size()==0)
    {

    }
}
////显示图片到label
void operate::getImage(QImage image, int status)
{
    //status==1时是显示小图（略缩图）
    if(status==1)
    {
        int scaleHeight;
        int scaleWidth;
        if(image.width()>image.height())
        {
            scaleHeight=QApplication::desktop()->height()/4;
            scale=(double)scaleHeight/image.height();
            scaleWidth= image.width()*scale;
        }
        else
        {
            scaleWidth=QApplication::desktop()->width()/12;
            scale=(double)scaleWidth/image.width();
            scaleHeight=image.height()*scale;
        }
        if(create_label==false)
        {
            label=new MyLabel;
            labelList.append(label);
            //            qDebug()<<"labelIndex"<<labelIndex;
            labelList.at(labelIndex+1)->connect(label,SIGNAL(sendlocation(int,int)),fileDeal,SLOT(getMouse(int,int)));
            labelList.at(labelIndex+1)->move(2,QApplication::desktop()->height()-scaleHeight-100);
            create_label=true;
        }
        //显示略缩图
        //        QImage smallimage = image.scaled(scaleWidth,scaleHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//平滑缩放保留细节
        QPixmap mp;
        mp=mp.fromImage(image);
        labelList.at(labelIndex+1)->setPixmap(mp);
        labelList.at(labelIndex+1)->resize(QSize(image.width(),image.height()));
        labelList.at(labelIndex+1)->setScaledContents(true);
        labelList.at(labelIndex+1)->show();

    }
    else if(status==2)
    {
        //大图（细节图）
        if(create_label==false)
        {
            label=new MyLabel;
            labelList.append(label);
            if(image.width()>=image.height())
            {
                labelList.at(labelIndex)->move(2,50);

            }else{
                labelList.at(labelIndex)->move(QApplication::desktop()->width()/10,50);
            }
            //            qDebug()<<"labelIndex"<<labelIndex;
            labelList.at(labelIndex)->connect(label,SIGNAL(currentOver()),this,SLOT(SampleOver()));//鼠标右键点击当前样本结束，从文件获取data
            labelList.at(labelIndex)->connect(label,SIGNAL(sendlocation(int,int)),this,SLOT(addPoint(int,int)));//鼠标左键获取坐标，label显示当前选择区域图像变化
            labelList.at(labelIndex)->connect(label,SIGNAL(sendlocation(int,int)),fileDeal,SLOT(lineMouse(int,int)));//鼠标左键获取坐标，label显示当前选择区域图像变化
            labelList.at(labelIndex)->connect(label,SIGNAL(sendLabelSize(int,int)),fileDeal,SLOT(sizeChange(int,int)));//视野label的Resize事件触重绘
        }
        QPixmap mp;
        mp=mp.fromImage(image);
        labelList.at(labelIndex)->setPixmap(mp);
        labelList.at(labelIndex)->resize(QSize(image.width(),image.height()));
        labelList.at(labelIndex)->setScaledContents(true);
        labelList.at(labelIndex)->show();
    }
}
////更新视区框起始坐标
void operate::updateVisiual(int x, int y)
{
    visiualX=x;
    visiualY=y;
}

////从面板上获取详细信息
bool operate::getDetails(bool flag)
{
    int i;
    bool FLAG=true;

    int geoNumber;//地物个数
    details.extend=ui->checkBox->checkState();
    switch (ui->comboBox->currentIndex())//选择要区分的地物种类数
    {
    case 0:
        FLAG=false;
        geoNumber=1;//1
        break;
    case 1:
        geoNumber=2;//2
        break;
    case 2:
        geoNumber=3;//3
        break;
    case 3:
        geoNumber=4;//4
        break;
    case 4:
        geoNumber=5;//5
        break;
    case 5:
        geoNumber=6;//6
        break;
    }

    details.geoNumber=geoNumber;//地物数

    if(flag=true)
    {    int temp=details.eachnumber[0];
        for(i=0;i<=geoNumber;i++)
        {
            if(details.eachnumber[i]>temp)
            {
                temp=details.eachnumber[i];
            }
        }
        details.eachmax=temp;
        for(i=0;i<=geoNumber;i++)
        {
            if(details.eachnumber[i]<temp)
            {
                temp=details.eachnumber[i];
            }
        }
        details.eachmin=temp;
        for(i=0;i<=geoNumber;i++)
        {
            details.sumNumber+=details.eachnumber[i];
        }
    }
    return FLAG;
}

////从ui选择样本
void operate::on_getSampleBtn_clicked()
{
    if(ui->getSampleBtn->text()==QStringLiteral("新建样本区域"))
    {
        if(getDetails(false)==false)
        {
            QMessageBox::warning(this, "Warning", QStringLiteral("      你没有选择要区分的种类数量     "), QMessageBox::Ok);
        }
        else
        {
            initChoice();
            ui->getSampleBtn->setText(QStringLiteral("关闭样本选择功能"));
            choiceSample->show();
        }
    }
    else
    {
        ui->getSampleBtn->setText(QStringLiteral("新建样本区域"));
        selectEnable=false;//样本选择功能关闭
        choiceSample->on_exitBtn_clicked();
        count_for_select_area=-1;
        count_for_add_point=0;
        choiceSample->close();//隐藏样本选择样本数目框
    }
}
/**
 * @brief operate::on_startSortBtn_clicked 开始地物区分
 */
void operate::on_startSortBtn_clicked()
{
    getDetails(true);//计算详细信息
    emit sendDetails(details);
    count_for_select_area=-1;
    count_for_add_point=0;
    //    fileDeal->startSort();
    QTimer::singleShot(0,fileDeal,SLOT(startSort()));
    initProBar();
    for(int i=0;i<6;i++)
    {
        details.eachnumberFlag[i]=0;
    }
}

/**
 * @brief operate::on_svmStartBtn_clicked svm训练分类，任务交由fileDeal线程处理
 */
void operate::on_svmStartBtn_clicked()
{
    getDetails(true);//计算详细信息
    emit sendSvmDetails(details);
    count_for_select_area=-1;
    count_for_add_point=0;
    for(int i=0;i<6;i++)
    {
        details.eachnumberFlag[i]=0;
    }
}

////决定样本选择是否开启(新建样本信号)
void operate::getselect_enable()
{
    if(selectEnable==false)
    {
        emit startSelect();
    }
    selectEnable=true;//打开样本区域选择
    details.eachnumberFlag[count_for_select_area]=1;//第一个样本封停（count_for_select_area==-1时）
    count_for_select_area++;//新建样本时再加
    emit newCurrent(count_for_select_area);
}
////暂停样本选择
void operate::get_stop_select()
{
    selectEnable=false;//关闭样本区域选择
}
//////移除样本
void operate::get_remove_area(int currentIndex)
{
    details.eachnumberFlag[count_for_select_area]=1;//将最后一个样本区域封停
    count_for_select_area=(currentIndex-1);//将指针提前到删除的样本前一个
    details.eachnumber[currentIndex]=0;//删除当前的样本值
    details.eachnumberFlag[currentIndex]=0;//可修改状态
    emit deleteCurrent(currentIndex);
    selectEnable=false;//样本选择功能关闭
}
////将抠图点加入数组准备处理(右键点击响应)
void operate::addPoint(int x0,int y0)
{
    if(selectEnable==true)
    {
        Point[count_for_add_point].x=x0+visiualX;
        Point[count_for_add_point].y=y0+visiualY;
        if(count_for_add_point>0)
        {
            p[0]=Point[count_for_add_point];
            p[1]=Point[count_for_add_point-1];
            emit getDataImage(p);
        }
        count_for_add_point++;
    }
}
///当前样本区域结束，从文件获取样本数据（data）
/// 鼠标右键响应
void operate::SampleOver()
{
    if(selectEnable==true)
    {
        emit getData(Point,count_for_add_point);
        //发送信号，调用判断点是否在区域中
        count_for_add_point=0;//清空抠图点计数
    }
}


////从子线程获取得到的样本区域信息
void operate::getDataDetails(int number)
{
    while(details.eachnumber[count_for_select_area]!=0&&details.eachnumberFlag[count_for_select_area]==1)
    {
        count_for_select_area++;
    }
    choiceSample->getinfo(number,count_for_select_area);
    if(count_for_select_area<=details.geoNumber-1)
    {
        details.eachnumber[count_for_select_area]=number;
    }
    if(count_for_select_area==details.geoNumber-1)
    {
        ui->startSortBtn->setEnabled(true);
    }
}
/**
 * @brief operate::getDataFileDetails 从文件获取样本
 * @param number
 */
void operate::getDataFileDetails(int number, int geoNumber)
{

    if(count_for_select_area==-1)
    {
        choiceSample->show();
    }
    count_for_select_area++;
    //    qDebug()<<count_for_select_area;
    choiceSample->fileDataRead();
    choiceSample->getinfo(number,count_for_select_area);

    //    qDebug()<<"details.geoNumber"<<details.geoNumber;
    details.eachnumberFlag[count_for_select_area]=1;
    if(count_for_select_area<=geoNumber-1)
    {
        details.eachnumber[count_for_select_area]=number;
    }
    if(count_for_select_area==geoNumber-1)
    {
        ui->startSortBtn->setEnabled(true);
        ui->comboBox->setCurrentIndex(geoNumber-1);//功能与这一句相同details.geoNumber=geoNumber;
        getDetails(true);//计算详细信息
        emit sendDetails(details);
        initProBar();
    }

}

void operate::getFeature(QStringList feature)//将决策树显示到ui界面
{
    details.sumNumber=0;//样本总数置0；
    ui->tabWidget->setCurrentWidget(ui->tabOutTree);//设置到当前widget
    ui->textEdit->clear();
    QString featureStr=NULL;
    QVector<QString>str(20);
    QVector<QStringList>strList(20);
    for(int i=0;i<feature.size()-1;i++)
    {
        str[i]=feature.at(i);
    }
    for(int i=0;i<feature.size()-1;i++)
    {
        strList[i]=str[i].split(",");
    }
    QString tempStr;
    QString bandI[30],bandJ[30],bandK[30];
    QString compare[30];
    QString Threshold[30];
    int i=0,j=0,p=0,q=0;
    int geoNameCount=0;
    int geoName[30];
    for(i=0;i<strList.size();i++)
    {
        q=0;
        for(j=0;j<strList[i].size()-7;j++)
        {
            tempStr=strList[i].at(j);
            if((j+1)%6==1)
            {
                bandI[q]=tempStr; //bandi
            }
            if((j+1)%6==2)
            {
                bandJ[q]=tempStr;  //bandj
            }
            if((j+1)%6==3)
            {
                bandK[q]=tempStr;//bandk
            }
            if((j+1)%6==4)
            {
                QString str=strList[i].at(j+6);
                compare[q]=str;//>or<   要用j+6来计算
            }
            if((j+1)%6==5)
            {
                Threshold[q]=tempStr; //Threshold
                q++;
            }
        }
        featureStr.clear();
        if(q!=0)
        {
            QString geoStr=strList[i].at(j+6);
            geoName[geoNameCount]=geoStr.toInt();
            for(p=0;p<q;p++)
            {
                if(bandJ[p]!="-1"&&bandK[p]!="-1"&&compare[p]=="-2")
                {
                    featureStr.append("(");
                    featureStr.append("band");
                    featureStr.append(bandI[p]);
                    featureStr.append("+band");
                    featureStr.append(bandK[p]);
                    featureStr.append(")/2-");
                    featureStr.append("band");
                    featureStr.append(bandJ[p]);
                    featureStr.append("<");
                    featureStr.append(Threshold[p]);
                    featureStr.append("&&");
                }
                else if(bandJ[p]!="-1"&&bandK[p]!="-1"&&compare[p]=="2")
                {
                    featureStr.append("(");
                    featureStr.append("band");
                    featureStr.append(bandI[p]);
                    featureStr.append("+band");
                    featureStr.append(bandK[p]);
                    featureStr.append(")/2-");
                    featureStr.append("band");
                    featureStr.append(bandJ[p]);
                    featureStr.append(">");
                    featureStr.append(Threshold[p]);
                    featureStr.append("&&");
                }
                else if(bandJ[p]!="-1"&&bandK[p]=="-1"&&compare[p]=="-2")
                {
                    featureStr.append("band");
                    featureStr.append(bandI[p]);
                    featureStr.append("-band");
                    featureStr.append(bandJ[p]);
                    featureStr.append("<");
                    featureStr.append(Threshold[p]);
                    featureStr.append("&&");

                }
                else if(bandJ[p]!="-1"&&bandK[p]=="-1"&&compare[p]=="2")
                {
                    featureStr.append("band");
                    featureStr.append(bandI[p]);
                    featureStr.append("-band");
                    featureStr.append(bandJ[p]);
                    featureStr.append(">");
                    featureStr.append(Threshold[p]);
                    featureStr.append("&&");
                }
                else if(bandJ[p]=="-1"&&compare[p]=="-2")
                {
                    featureStr.append("band");
                    featureStr.append(bandI[p]);
                    featureStr.append("<");
                    featureStr.append(Threshold[p]);
                    featureStr.append("&&");
                }
                else if(bandJ[p]=="-1"&&compare[p]=="2")
                {
                    featureStr.append("band");
                    featureStr.append(bandI[p]);
                    featureStr.append(">");
                    featureStr.append(Threshold[p]);
                    featureStr.append("&&");
                }
            }
            featureStr.append("end");
            switch (geoName[geoNameCount]) {
            case 1:
                featureStr.append(QStringLiteral("红色地区"));
                break;
            case 2:
                featureStr.append(QStringLiteral("黄色地区"));
                break;
            case 3:
                featureStr.append(QStringLiteral("蓝色地区"));
                break;
            case 4:
                featureStr.append(QStringLiteral("绿色地区"));
                break;
            case 5:
                featureStr.append(QStringLiteral("紫色地区"));
                break;
            case 6:
                featureStr.append(QStringLiteral("黑色地区"));
                break;
            default:
                featureStr.append(QStringLiteral("其他地区"));
            }
            geoNameCount++;
            ui->textEdit->append(featureStr+="\n");
        }
    }
    message=new showInfo;
    message->setMessage(QStringLiteral("地物区分完成"),0);
    message->show();
}

////保存降噪后tif文件
void operate::on_saveTif_clicked()
{
    //    fileDeal->saveTif();
    QTimer::singleShot(0,fileDeal,SLOT(saveTif()));
}
////保存降噪后二进制图片
void operate::on_saveBinary_clicked()
{
    //    fileDeal->saveBinary();
    QTimer::singleShot(0,fileDeal,SLOT(saveBinary()));
}

void operate::on_openFromFileBtn_clicked()
{
    QString file;
    file = QFileDialog::getOpenFileName(
                this,
                "Open Document",
                QDir::currentPath(),
                "All files(*.*)");
    if (!file.isNull())
    { //用户选择了文件
        QFile read(file.toStdString().c_str());
        read.open(QFile::ReadOnly);
        QTextStream in(&read);
        QString oldString;
        in>>oldString;
        QStringList List=oldString.split(",");
        QString name=List.at(0);
        int number[6]={0};
        QString tempStr;
        for(int i=1;i<7;i++)
        {
            tempStr=List.at(i);
            number[i-1]=tempStr.toInt();
            //            qDebug()<<number[i-1];
        }
        //        qDebug()<<name;
        read.close();
        emit sendSimpleInfo(name,number);
    }
    else
    {
        QMessageBox::information(this, "Warning",QStringLiteral("你未选中任何文件"), QMessageBox::Ok);
    }

}
//void operate::showInfo(QString info,QString type)
//{
//    if(type=="warning")
//    {
//        QMessageBox::warning(this, "Warning",QStringLiteral(info), QMessageBox::Ok);
//    }
//    else if(type=="info")
//    {
//        QMessageBox::information(this, "Warning",QStringLiteral(info), QMessageBox::Ok);
//    }
//}


/**
 * @brief operate::on_loadModelBtn_clicked 打开svm结果集
 */
void operate::on_loadModelBtn_clicked()
{
    QString file = QFileDialog::getOpenFileName(
                this,
                "Open Document",
                QDir::currentPath(),
                "All files(*.*)");
    if (!file.isNull())
    { //用户选择了文件
        emit sendSvmModel(file);
    }
    else
    {
        // 用户取消选择
        QMessageBox::information(this, "Warning", QStringLiteral("      你未选中任何文件      "), QMessageBox::Ok);
    }

}
