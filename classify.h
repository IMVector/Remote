#ifndef CLASSIFY_H
#define CLASSIFY_H
#include <QObject>
#include <QtCore>
#include <QPainter>
#include <QMessageBox>
#include <QMetaType>
#include <string.h>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <showinfo.h>
#include <progressbar.h>
typedef struct TreeNode{
    int bandi;//i波段
    int bandj;//j波段
    int bandk;//k波段
    int visited;//是否已经遍历过
    int status;//是大于还是小于
    int geoName;//第几类地物
    int numbers;//此叶子的数量
    double Threshold;//阈值
    struct TreeNode * left;
    struct TreeNode * right;
}TreeNode,*Tree;

typedef struct{
    unsigned short int bands[13];
}Node, *simples;

typedef struct QueeNode{
    int bandi;//i波段
    int bandj;//j波段
    int bandk;//k波段
    int status;//是大于还是小于
    int visited;//是否已经遍历过
    int geoName;//第几类地物
    int numbers;//此叶子的数量
    double Threshold;//阈值
}*Quee;


typedef struct
{
    int bandNumber;//波段数目
    int eachnumber[7];//每个样本的数量
    int eachnumberFlag[7];//每个样本的选择是否完成
    int sumNumber;//样本总数
    int eachmax;//样本最大值
    int eachmin;//样本最小值
    int geoNumber;//地物数量
    int extend;//扩展波段是否开启
    int fileOpen;//是否从文件打开样本
    QString fileName;//文件名称
}fileDetails;


typedef struct array
{
    double element;
    int id;
}array;//定义结构体，element记录光谱，id记录下标
typedef struct Feature
{
    int band[3];//波段数组前三个数是波段
    int Geography[2];//区分的两地物
    double percent;
    double GainRatio;//最优信息增益比
    double Threshold;//最优阈值
}Feature;


class classify : public QObject
{
    Q_OBJECT

private:


public:
    classify();
    void thismain();
    void DeleteTree(Tree T);
    void removeCharacteristics(Feature characteristics[]);
    void sort(array a[], array b[],int bandNb);//排序
    int sort1(array a[], array b[], int reorder[][15],int bandNb);
    int sort2(array a[], array b[], int first_extend[][15][15],int bandNb);
    void sort3(array a[][15], array b[][15], int oneBand[15], int aNumber, int bNumber,int bandNb);
    void display(int reorder[][15],int zero_extend_Sum_Number,Feature characteristics[100],
    int Geographical_1, int Geographical_2, int oneBand[15],int bandNb);

    Feature FeatureSelection(
            double Resort_class_1[][15], Feature characteristics, int sum_Number_of_Geographical_samples,
    int number_of_Geographical_1_samples,
    int number_of_Geographical_2_samples,
    int number_of_Geographical_3_samples,
    int number_of_Geographical_4_samples,
    int number_of_Geographical_5_samples,
    int number_of_Geographical_6_samples);

    Feature Find_MAX_VALUE(Feature GainRatio_two[100], int Sum_Count);//找最大阈值

    void Tree_divide(int sum_Number_of_Geographical_samples,
                     double data[100000][15], Feature decision_makeing_Info,
    double Resort_class_1_1[][15], double Resort_class_1_2[][15],
    int number_of_Geography_1_samples[6],//记录各个地物的数量
    int number_of_Geography_2_0_samples[6],//记录分开的数据
    int number_of_Geography_2_1_samples[6],int bandNb);//记录分开的数据

    void divide(Feature characteristics[], double data[][15], int sumNumber, int eachNumber[],Tree OUT);
    void findAllPath(Tree T, Quee quee, int status,QString fileName);
    void extendMain();

    void extend(Feature characteristics[], double data[][15]);
    void display(int reorder[][15], int first_extend[][15][15], int zero_extend_Sum_Number, int frist_extend_Sum_Number, Feature characteristics[], int Geographical_1, int Geographical_2);
    void display(int reorder[][15], int first_extend[][15][15], int zero_extend_Sum_Number, int frist_extend_Sum_Number, Feature characteristics[], int Geographical_1, int Geographical_2, int oneBand[], int bandNb);
private slots:
    void getDetails(fileDetails fd);
    void getdata_from_ui(simples data,int number);
signals:
    void sortComplete(QString ruleName);
    void setProgressValue(int value);
    void setProgressRange(int startRange,int endRange);
    void complete();
};

#endif // CLASSIFY_H
