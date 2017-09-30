#include "database.h"
#include "ui_database.h"
#include <QSqlRecord>
QSqlDatabase SqlSever;
int selectIndex=0;
int queryFlag=0;
int qualityIndex=0;
QString thisDatabase="QODBC";
QString thisUrl="127.0.0.1";
QString thisDBName="RemoteSensing";
QString thisUName="sa";
QString thisPassword="123456";
database::database(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::database)
{
    ui->setupUi(this);
    this->setWindowTitle(QString::fromLocal8Bit("遥感数据库"));
    OpenDatabase(thisDatabase,thisUrl,thisDBName,thisUName,thisPassword);
    ui->comboBox->setStyleSheet("QComboBox QAbstractItemView::item{height:20px;}");
    ui->comboBox->setView(new QListView());
}

database::~database()
{
    delete ui;
}
/**
 * @brief database::Setting
 * @param url
 * @param databseName
 * @param userName
 * @param password
 * 数据库连接设置
 */
void database::Setting(QString database,QString url,QString databseName,QString userName,QString password)
{
    thisDatabase=database;
    thisUrl=url;
    thisDBName=databseName;
    thisUName=userName;
    thisPassword=password;
    SqlSever.close();
    OpenDatabase(database,url,databseName,userName,password);
}
/**
 * @brief database::OpenDatabase
 * @param url
 * @param databaseName
 * @param userName
 * @param password
 * 连接数据库
 */
void database::OpenDatabase(QString database, QString url,QString databaseName,QString userName,QString password)
{
    QString thisdatabase="QODBC";
    QString thisUrl="127.0.0.1";
    QString thisDBName="RemoteSensing";
    QString thisUName="sa";
    QString thisPassword="123456";
    thisdatabase=database;
    thisUrl=url;
    thisDBName=databaseName;
    thisUName=userName;
    thisPassword=password;
    SqlSever=QSqlDatabase::addDatabase(thisdatabase);
    SqlSever.setDatabaseName(QString("DRIVER={SQL SERVER};"
                                     "SERVER=%1;" //服务器名称
                                     "DATABASE=%2;"//数据库名
                                     "UID=%3;"           //登录名
                                     "PWD=%4;"        //密码
                                     ).arg(thisUrl)
                             .arg(thisDBName)
                             .arg(thisUName)
                             .arg(thisPassword)
                             );
    if (!SqlSever.open())
    {
        //数据库打开失败
        QMessageBox::warning(0, qApp->tr("      Cannot open database          "),
                             SqlSever.lastError().databaseText(), QMessageBox::Ok);

    }
    else
    {
        //数据库打开成功
        QMessageBox::information(0, qApp->tr("info"),
                             "      database has been opened!        ", QMessageBox::Ok);
    }
}
/**
 * @brief database::on_pushButton_clicked
 * 查询图像按钮响应
 */
void database::on_pushButton_clicked()//查询数据库图像
{
    queryFlag=1;
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setColumnWidth(0,30);
    ui->tableWidget->setColumnWidth(1,500);
    QStringList header;
    header<< "ID" <<QString::fromLocal8Bit("文件名称");;
    ui->tableWidget->setHorizontalHeaderLabels(header);//显示标题
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不允许修改
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//整行选中
    ui->tableWidget->verticalHeader()->setVisible(false);   //隐藏列表头

    QSqlQuery query;
    query.exec("select * from RemoteSensingImage");
    int i=0;
    while(query.next())
    {
        i++;
        QString strID = QString::number(i);
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,new QTableWidgetItem(strID));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,new QTableWidgetItem(query.value("Name").toString()));

    }

}
/**
 * @brief database::on_pushButton_2_clicked
 * 查询区分规则按钮响应
 */

void database::on_pushButton_2_clicked()//查询区分规则
{
    queryFlag=2;
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setColumnWidth(0,30);
    ui->tableWidget->setColumnWidth(1,200);
    ui->tableWidget->setColumnWidth(2,200);
    QStringList header;
    header<< "ID" <<QString::fromLocal8Bit("规则")<<QString::fromLocal8Bit("规则所属图像")<<QString::fromLocal8Bit("规则质量");
    ui->tableWidget->setHorizontalHeaderLabels(header);//显示标题
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不允许修改
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//整行选中
    ui->tableWidget->verticalHeader()->setVisible(false);   //隐藏列表头

    QSqlQuery query;
    query.exec("select * from RemoteSensingRule");
    int i=0;
    while(query.next())
    {
        i++;
        QString strID = QString::number(i);
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,new QTableWidgetItem(strID));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,new QTableWidgetItem(query.value("Name").toString()));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,2,new QTableWidgetItem(query.value("OwnName").toString()));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,3,new QTableWidgetItem(query.value("Quality").toString()));

    }
}
/**
 * @brief database::on_pushButton_3_clicked
 * 已提取海岸线按钮响应
 */

void database::on_pushButton_3_clicked()//查询已提取海岸线
{
    queryFlag=3;
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setColumnWidth(0,30);
    ui->tableWidget->setColumnWidth(1,250);
    ui->tableWidget->setColumnWidth(2,250);
    QStringList header;
    header<< "ID" <<QString::fromLocal8Bit("海岸线")<<QString::fromLocal8Bit("海岸线所属图像");
    ui->tableWidget->setHorizontalHeaderLabels(header);//显示标题
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不允许修改
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//整行选中
    ui->tableWidget->verticalHeader()->setVisible(false);   //隐藏列表头

    QSqlQuery query;
    query.exec("select * from RemoteSensingSeaLine");
    int i=0;
    while(query.next())
    {
        i++;
        QString strID = QString::number(i);
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,new QTableWidgetItem(strID));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,new QTableWidgetItem(query.value("Name").toString()));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,2,new QTableWidgetItem(query.value("OwnName").toString()));

    }
}
/**
 * @brief database::on_pushButton_4_clicked
 * 打开已选中样本响应事件
 */
void database::on_pushButton_4_clicked()//打开所选文件
{
    if(selectIndex==0)
    {
        warningMessage();
        return;
    }
    QSqlQuery query;
    if(queryFlag==1)
    {
        query.exec("select * from RemoteSensingImage");
        for(int i=0;i<selectIndex;i++)
        {
            query.next();
        }
        emit sendImageName(query.value("Name").toString());

    }
    else if(queryFlag==2)
    {
        query.exec("select * from RemoteSensingRule");
        for(int i=0;i<selectIndex;i++)
        {
            query.next();
        }
        emit sendImageName(query.value("OwnName").toString());
        emit sendRule(query.value("Name").toString());

    }
    else if(queryFlag==3)
    {
        query.exec("select * from RemoteSensingSeaLine");
        for(int i=0;i<selectIndex;i++)
        {
            query.next();
        }
        emit sendSeaLine(query.value("Name").toString());
    }
    queryFlag=0;
    selectIndex=0;
    ui->tableWidget->clear();
    this->close();
}
/**
 * @brief database::on_tableWidget_clicked
 * @param index
 * 获取已选中数据的index
 */
void database::on_tableWidget_clicked(const QModelIndex &index)
{
    selectIndex=index.row();//从0开始计数的
    ++selectIndex;//变成从1开始计数的
}

/**
 * @brief database::on_pushButton_5_clicked
 * 删除数据库中的数据
 */
void database::on_pushButton_5_clicked()//删除数据库中的数据
{
    if(selectIndex==0)
    {
        warningMessage();
        return;
    }
    QSqlQuery query;
    if(queryFlag==1)
    {
        query.exec("select * from RemoteSensingImage");
        for(int i=0;i<selectIndex;i++)
        {
            query.next();
        }
        QString str=QString("delete from RemoteSensingImage where Name='%1'").arg(query.value("Name").toString());
        query.exec(str);
        on_pushButton_clicked();
    }
    else if(queryFlag==2)
    {
        query.exec("select * from RemoteSensingRule");
        for(int i=0;i<selectIndex;i++)
        {
            query.next();
        }
        QString str=QString("delete from RemoteSensingRule where Name='%1'").arg(query.value("Name").toString());
        query.exec(str);
        on_pushButton_2_clicked();
    }
    else if(queryFlag==3)
    {
        query.exec("select * from RemoteSensingSeaLine");
        for(int i=0;i<selectIndex;i++)
        {
            query.next();
        }
        QString str=QString("delete from RemoteSensingSeaLine where Name='%1'").arg( query.value("Name").toString());
        query.exec(str);
        on_pushButton_3_clicked();
    }
    selectIndex=0;
}

/**
 * @brief database::on_pushButton_6_clicked
 * 设置选中规则质量
 */
void database::on_pushButton_6_clicked()
{
    if(selectIndex==0)
    {
        warningMessage();
        return;
    }
    QSqlQuery query;
    query.exec("select * from RemoteSensingRule");
    for(int i=0;i<selectIndex;i++)
    {
        query.next();
    }
    QString str ;
    switch (qualityIndex) {
    case 0:
        warningMessage();
        break;
    case 1:
        str=QString::fromLocal8Bit("好");//解决中文乱码
        break;
    case 2:
        str=QString::fromLocal8Bit("中");//解决中文乱码
        break;
    case 3:
        str=QString::fromLocal8Bit("差");//解决中文乱码
        break;

    }
    ui->tableWidget->setItem(selectIndex-1,3,new QTableWidgetItem(str));
    //    数据库中插入数据。。。。。。。。。

    QString sqlUpdateStr=QString("update RemoteSensingRule set Quality ='%1' where Name='%2'").arg(str).arg(query.value("Name").toString());
    query.exec(sqlUpdateStr);
    selectIndex=0;
}

/**
 * @brief database::on_comboBox_currentIndexChanged
 * @param index
 * 获取已选中规则的index
 */
void database::on_comboBox_currentIndexChanged(int index)
{
    qualityIndex=index;
}
/**
 * @brief database::warningMessage
 * 错误，未选中行提醒
 */
void database::warningMessage()
{
    QMessageBox::warning(this, "Warning",QString::fromLocal8Bit("你未选中任何行"), QMessageBox::Ok);
}

