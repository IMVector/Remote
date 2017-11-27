#include <classify.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <QTime>

int N=4;
int EN=N+1;
int GeoNumber=1;
int max_number=0;//样本数量的最大值
int decide_number=0;
int average_number=0;
int sumSample=0;//所有地物的样本总数量
int sample_1=0;//每个地物中的样本数量
int sample_2=0;
int sample_3=0;
int sample_4=0;
int sample_5=0;
int sample_6=0;
int resultCount = 0;
int queeInCount=0;
bool Extend=true;
int divideCount=0;
QString ruleOwnName;
Tree root= (Tree)malloc(sizeof(TreeNode));
Quee quee=(Quee)malloc(100*sizeof(QueeNode));
Node *Data=NULL;

classify::classify()
{

}
////data:样本区域点
/// number:样本区域点数量
void classify::getdata_from_ui(simples data, int number)
{
    //    int i,j;
    //    for(i=0;i<number;i++)
    //    {
    //        for(j=0;j<N;j++)
    //        {
    //            Data[i].bands[j]=data[i].bands[j];
    //        }

    //    }
    //    delete[] data;
    Data=data;
    emit setProgressValue(2);
    if(Extend==false)
    {
        thismain();
    }
    else
    {
        extendMain();
    }

}

void classify::getDetails(SamplesDetails fd)
{
    emit setProgressRange(0,100);
    emit setProgressValue(1);

    N=fd.bandNumber;
    EN=N+1;
    sumSample=fd.sumNumber;
    GeoNumber=fd.geoNumber;
    ruleOwnName=fd.fileName;
    Extend=fd.extend;
    sample_1=fd.eachnumber[0];
    sample_2=fd.eachnumber[1];
    sample_3=fd.eachnumber[2];
    sample_4=fd.eachnumber[3];
    sample_5=fd.eachnumber[4];
    sample_6=fd.eachnumber[5];
    average_number=(sample_1+
                    sample_2+
                    sample_3+
                    sample_4+
                    sample_5+
                    sample_6)/GeoNumber;
    decide_number=(average_number+0.2*fd.eachmax);
    max_number=fd.eachmax;
    qInfo()<<QStringLiteral("波段数量：")<<N;
    qInfo()<<QStringLiteral("地物数量：")<<GeoNumber;
    qInfo()<<QStringLiteral("第1种地物样本数量：")<<sample_1;
    qInfo()<<QStringLiteral("第2种地物样本数量：")<<sample_2;
    qInfo()<<QStringLiteral("第3种地物样本数量：")<<sample_3;
    qInfo()<<QStringLiteral("第4种地物样本数量：")<<sample_4;
    qInfo()<<QStringLiteral("第5种地物样本数量：")<<sample_5;
    qInfo()<<QStringLiteral("第6种地物样本数量：")<<sample_6;
    qInfo()<<QStringLiteral("所有样本总数量：")<<sumSample;
    qInfo()<<QStringLiteral("阈值数量：")<<decide_number;
    qInfo()<<QStringLiteral("单个样本数量最大值：")<<max_number;
    qInfo()<<QStringLiteral("样本数量平均值：")<<average_number;
    qInfo()<<QStringLiteral("是否启用超级扩展：")<<Extend;
}

void classify::thismain()
{

    if(root==NULL)
    {
        root= (Tree)malloc(sizeof(TreeNode));
    }
    resultCount = 0;
    queeInCount=0;
    root->bandi=-1;
    root->bandj=-1;
    root->bandk=-1;
    root->numbers=-1;
    root->Threshold=-1;
    for(int i=0;i<100;i++)
    {
        quee[i].bandi=-1;
        quee[i].bandj=-1;
        quee[i].bandk=-1;
        quee[i].geoName=-1;
        quee[i].numbers=-1;
        quee[i].status=-1;
        quee[i].Threshold=-1;
        quee[i].visited=-1;
    }
    int i = 0, j = 0, k = 0,judge1;
    int Number_of_cycles_a = 0, Number_of_cycles_b = 0;//循环次数
    int zero_extend_Sum_Number = 0;//函数调用计数变量
    array(*a)[15];
    a = (array(*)[15])malloc(20000 * 15 * sizeof(array));
    array(*b)[15];
    b = (array(*)[15])malloc(20000 * 15 * sizeof(array));
    int reorder[15][15] = { 0 };
    memset(reorder,-1,sizeof(reorder));
    int oneBand[15];
    memset(oneBand, -1, sizeof(oneBand));//正常
    int Geographical_1 = -1, Geographical_2 = -1;//存储地物信息
    Feature  characteristics[100];//存储组合特征
    for(int i=0;i<100;i++)
    {
        characteristics[i].band[0]=-1;
        characteristics[i].band[1]=-1;
        characteristics[i].band[2]=-1;
        characteristics[i].GainRatio=-1;
        characteristics[i].Geography[0]=-1;
        characteristics[i].Geography[1]=-1;
        characteristics[i].Threshold=-1;
    }
    double(*data)[15];
    data = (double(*)[15])malloc(100000 *15* sizeof(double));
    int number_item2 = sample_1 + sample_2;
    int number_item3 = number_item2 + sample_3;
    int number_item4 = number_item3 + sample_4;
    int number_item5 = number_item4 + sample_5;
    int number_item6 = number_item5 + sample_6;

    for (i = 0; i < sumSample; i++)
    {
        for (j = 0; j < N; j++)
        {
            data[i][j]=Data[i].bands[j];//读取所有数据
        }
        if (i < sample_1)
        {
            data[i][N] = 1;
        }
        if (i >= sample_1 && i < number_item2)
        {
            data[i][N] = 2;
        }
        if (i >= number_item2 && i < number_item3)
        {
            data[i][N] = 3;
        }
        if (i >= number_item3&& i < number_item4)
        {
            data[i][N] = 4;
        }
        if(i>=number_item4&&i<number_item5)
        {
            data[i][N]=5;
        }
        if(i>=number_item5&&i<number_item6)
        {
            data[i][N]=6;
        }
    }
    qInfo()<<QStringLiteral("区分前准备工作正常");
    //数据输入模块
    for(int count=0;count<GeoNumber*(GeoNumber-1)/2;count++)
    {
        emit setProgressValue(3+((count/GeoNumber)*(GeoNumber-1)/2)*45);
        switch (count) {
        case 0:
            Geographical_1 = 1;
            Geographical_2 = 2;
            for(i=0;i<sample_1;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=sample_1;i<number_item2;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        case 1:
            Geographical_1 = 1;
            Geographical_2 = 3;
            for(i=0;i<sample_1;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item2;i<number_item3;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        case 2:
            Geographical_1 = 2;
            Geographical_2 = 3;
            for(i=sample_1;i<number_item2;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item2;i<number_item3;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        case 3:
            Geographical_1 = 1;
            Geographical_2 = 4;
            for(i=0;i<sample_1;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item3;i<number_item4;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }

            break;
        case 4:
            Geographical_1 = 2;
            Geographical_2 = 4;
            for(i=sample_1;i<number_item2;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item3;i<number_item4;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        case 5:
            Geographical_1 = 3;
            Geographical_2 = 4;
            for(i=number_item2;i<number_item3;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item3;i<number_item4;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }

            break;
        case 6:
            Geographical_1 = 1;
            Geographical_2 = 5;
            for(i=0;i<sample_1;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item4;i<number_item5;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        case 7:
            Geographical_1 = 2;
            Geographical_2 = 5;
            for(i=sample_1;i<number_item2;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item4;i<number_item5;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }

            break;
        case 8:
            Geographical_1 = 3;
            Geographical_2 = 5;
            for(i=number_item2;i<number_item3;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item4;i<number_item5;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        case 9:
            Geographical_1 = 4;
            Geographical_2 = 5;
            for(i=number_item3;i<number_item4;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item4;i<number_item5;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        case 10:
            Geographical_1 = 1;
            Geographical_2 = 6;
            for(i=0;i<sample_1;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item5;i<number_item6;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        case 11:
            Geographical_1 = 2;
            Geographical_2 = 6;
            for(i=sample_1;i<number_item2;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item5;i<number_item6;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }

            break;
        case 12:
            Geographical_1 = 3;
            Geographical_2 = 6;
            for(i=number_item2;i<number_item3;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item5;i<number_item6;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        case 13:
            Geographical_1 = 4;
            Geographical_2 = 6;
            for(i=number_item3;i<number_item4;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item5;i<number_item6;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        case 14:
            Geographical_1 = 5;
            Geographical_2 = 6;
            for(i=number_item4;i<number_item5;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item5;i<number_item6;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        }
        //读取模块
        qInfo()<<QStringLiteral("获取数据正常");
        sort3(a, b, oneBand, Number_of_cycles_a, Number_of_cycles_b,N);
        for (j = 0; j < Number_of_cycles_a; j++)
        {
            for (k = 0; k < Number_of_cycles_b; k++)
            {
                judge1 = sort1(a[j], b[k], reorder,N);
                zero_extend_Sum_Number++;
            }
        }
        display(reorder,zero_extend_Sum_Number,characteristics, Geographical_1, Geographical_2, oneBand,N);
        {
            zero_extend_Sum_Number= 0;
            Number_of_cycles_a = Number_of_cycles_b = 0;
            memset(reorder, -1,sizeof(reorder));
            memset(oneBand, -1,sizeof(oneBand));
            //数组清空函数
        }
    }
    free(a);//将分配的内存释放
    free(b);//将分配的内存释放
    removeCharacteristics(characteristics);

    int Number_of_samples[6] = { 0 };
    Number_of_samples[0] = sample_1;
    Number_of_samples[1] = sample_2;
    Number_of_samples[2] = sample_3;
    Number_of_samples[3] = sample_4;
    Number_of_samples[4] = sample_5;
    Number_of_samples[5] = sample_6;
    qInfo()<<QStringLiteral("开始递归分离");
    divide(characteristics, data, sumSample, Number_of_samples,root);//分离函数(递归)
    qInfo()<<QStringLiteral("递归分离正常");
    QDateTime Systemtime = QDateTime::currentDateTime();//获取系统现在的时间
    QString str = Systemtime.toString("yyyy_MM_dd_hh_mm_ss"); //设置显示格式
    QString fileStr=ruleOwnName;
    fileStr=fileStr+"Rule"+str;
    findAllPath(root,quee,0,fileStr);//将所有规则写入文件
    emit sortComplete(fileStr);//地物分类完成
    emit setProgressValue(100);
    emit complete();

    free(data);//将分配的内存释放
    DeleteTree(root->left);
    DeleteTree(root->right);

    QSqlQuery query;
    QString sqlInsertStr=QString("insert into RemoteSensingRule(name,OwnName,quality) values('%1','%2','%3')").arg(fileStr).arg(ruleOwnName).arg("no");
    query.exec(sqlInsertStr);
    delete[] Data;
    Data=NULL;
    qInfo()<<QStringLiteral("地物分离一切正常");
    return;
}
////排序模块
void classify::sort(array a[], array b[],int bandNb)
{
    int i, j;
    array temp;//交换辅助变量
    for (i = 1; i < bandNb; i++)
        for (j = 0; j < bandNb - i; j++)
        {
            if (a[j].element > a[j + 1].element)
            {
                temp = a[j];
                a[j] = a[j + 1];
                a[j + 1] = temp;
            }
            if (b[j].element > b[j + 1].element)
            {
                temp = b[j];
                b[j] = b[j + 1];
                b[j + 1] = temp;
            }
        }
}//排序

//第一步分离
int classify::sort1(array a[], array b[], int reorder[][15],int bandNb)
{
    int i, j, decide = 0;
    for (i = 0; i < bandNb - 1; i++)
    {
        for (j = i + 1; j < bandNb; j++)
        {
            if ((a[i].element - a[j].element)*(b[i].element - b[j].element) < 0)
            {
                decide = 1;
                reorder[i][j]++;//用数组下标记录可区分的波段i，j，数组值记录出现次数，未排序不需用id记录
            }
        }
    }
    if (decide == 1)
    {
        return 10;	//返回10可以区分，不用调用一次扩展
    }
    else return 11;//返回11调用一次扩展
}

//可区分一次扩展
int classify::sort2(array a[], array b[], int first_extend[][15][15],int bandNb)
{
    //k>j>i
    int i, j, k, decide = 0;
    double x = 0, y = 0;
    for (i = 0; i < (bandNb - 2); i++)
    {
        for (j = i + 1; j < (bandNb - 1); j++)
        {
            for (k = j + 1; k < bandNb; k++)
            {
                x = ((a[j].element - a[i].element) - (a[k].element - a[j].element));
                y = ((b[j].element - b[i].element) - (b[k].element - b[j].element));
                if (x*y < 0)
                {
                    decide = 1;
                    first_extend[a[i].id][a[j].id][a[k].id]++;//用数组下标记录可区分的波段i，j，k ，数组值记录出现次数，由于是排序后要记录下标要用id记录
                }
            }
        }
    }
    if (decide == 1)
    {
        return 20;//返回20可以区分不用调用二次扩展
    }
    else
    {
        return 21;//返回21，调用二次扩展
    }
}

//单波段区分
void classify::sort3(array a[][15], array b[][15], int oneBand[15], int aNumber, int bNumber,int bandNb)//排序
{
    int i, j;
    double sum = 0, avg_a[15] = { 0 }, avg_b[15] = { 0 };
    for (i = 0; i < bandNb; i++)
    {
        for (j = 0; j < aNumber; j++)
        {
            sum += a[j][i].element;
        }
        avg_a[i] = sum / aNumber;
        sum = 0;
        for (j = 0; j < bNumber; j++)
        {
            sum += b[j][i].element;
        }
        avg_b[i] = sum / bNumber;
        sum = 0;
    }
    for (i = 0; i < bandNb; i++)
    {
        if (avg_a[i] / avg_b[i]>1.5 || avg_b[i] / avg_a[i]>1.5)
        {
            oneBand[i]++;
        }
    }
}

//特征赋值模块
void classify::display(int reorder[][15],int zero_extend_Sum_Number,
Feature characteristics[100], int Geographical_1,
int Geographical_2, int oneBand[15],int bandNb)
{
    int i, j;
    //排序阶段输出模块
    for (i = 0; i < bandNb; i++)
        for (j = 0; j < bandNb; j++)
        {
            if (reorder[i][j] != 0 && (reorder[i][j] * 100.0) / zero_extend_Sum_Number >= 51)
            {
                characteristics[resultCount].band[0] = i;
                characteristics[resultCount].band[1] = j;
                characteristics[resultCount].percent = (reorder[i][j] * 100.0) / zero_extend_Sum_Number;
                characteristics[resultCount].Geography[0] = Geographical_1;
                characteristics[resultCount].Geography[1] = Geographical_2;
                resultCount++;
            }
        }
    for (i = 0; i < bandNb; i++)
    {
        if (oneBand[i] != -1)
        {
            characteristics[resultCount].band[0] = i;
            characteristics[resultCount].band[1] = -1;
            characteristics[resultCount].band[2] = -1;
            characteristics[resultCount].Geography[0] = Geographical_1;
            characteristics[resultCount].Geography[1] = Geographical_2;
            resultCount++;
        }
    }
}

//找最大阈值
Feature classify::Find_MAX_VALUE(Feature GainRatio_two[100], int Sum_Count)
{
    int i = 0;
    Feature temp = { 0 };
    temp = GainRatio_two[0];
    for (i = 0; i < Sum_Count; i++)
    {

        if (GainRatio_two[i].GainRatio > temp.GainRatio)/*&&GainRatio_two[i].GainRatio>0&&GainRatio_two[i].GainRatio<=1*/
        {
            temp = GainRatio_two[i];
        }

    }
    return temp;
}
//去除重复特征，提高程序效率
void classify::removeCharacteristics(Feature characteristics[])
{
    int i, j;
    for (i = 0; i < 100; i++)
    {
        for (j = i + 1; j < 100; j++)
            if (characteristics[i].band[0] != -1 && characteristics[i].band[1] == -1&& characteristics[j].band[2]==-1)
            {
                if (characteristics[i].band[0] == characteristics[j].band[0])
                {
                    characteristics[j].band[0] = -1;
                }
            }
    }
}
//特征选择
Feature classify::FeatureSelection(
        double Resort_class_1[][15], Feature characteristics,
int sum_Number_of_Geographical_samples,
int number_of_Geographical_1_samples,
int number_of_Geographical_2_samples,
int number_of_Geographical_3_samples,
int number_of_Geographical_4_samples,
int number_of_Geographical_5_samples,
int number_of_Geographical_6_samples)
{
    int i = 0, j = 0;
    Feature temp = { 0 };
    double GainRatio = 0, EntropyFi = 0, Entropy = 0, Gain = 0,
            Entropy_greater = 0, Entropy_less = 0, SplitInfo = 0;
    int greater_than = 0, less_than = 0,
            greater_thanm1 = 0, greater_thanm2 = 0, greater_thanm3 = 0,
            greater_thanm4 = 0,greater_thanm5 = 0,greater_thanm6 = 0 ,
            less_thanm1 = 0, less_thanm2 = 0, less_thanm3 = 0,
            less_thanm4 = 0,less_thanm5 = 0,less_thanm6 = 0;
    double greater_m1 = 0, greater_m2 = 0, greater_m3 = 0,
            greater_m4 = 0, greater_m5=0,greater_m6=0,
            less_m1 = 0, less_m2 = 0, less_m3 = 0,
            less_m4 = 0,less_m5=0,less_m6=0;
    double Entropy_item_1 = 0, Entropy_item_2 = 0, Entropy_item_3 = 0,
            Entropy_item_4 = 0, Entropy_item_5 = 0, Entropy_item_6 = 0;//Entrop的四项
    double SplitInfo_item_1 = 0, SplitInfo_item_2 = 0;
    double minus[100000] = { 0 };
    Entropy_item_1 = ((number_of_Geographical_1_samples*1.0) / (sum_Number_of_Geographical_samples*1.0)*(log((number_of_Geographical_1_samples*1.0) / (sum_Number_of_Geographical_samples*1.0)) / log(2.0)));
    Entropy_item_2 = ((number_of_Geographical_2_samples*1.0) / (sum_Number_of_Geographical_samples*1.0)*(log((number_of_Geographical_2_samples*1.0) / (sum_Number_of_Geographical_samples*1.0)) / log(2.0)));
    Entropy_item_3 = ((number_of_Geographical_3_samples*1.0) / (sum_Number_of_Geographical_samples*1.0)*(log((number_of_Geographical_3_samples*1.0) / (sum_Number_of_Geographical_samples*1.0)) / log(2.0)));
    Entropy_item_4 = ((number_of_Geographical_4_samples*1.0) / (sum_Number_of_Geographical_samples*1.0)*(log((number_of_Geographical_4_samples*1.0) / (sum_Number_of_Geographical_samples*1.0)) / log(2.0)));
    Entropy_item_5 = ((number_of_Geographical_5_samples*1.0) / (sum_Number_of_Geographical_samples*1.0)*(log((number_of_Geographical_5_samples*1.0) / (sum_Number_of_Geographical_samples*1.0)) / log(2.0)));
    Entropy_item_6 = ((number_of_Geographical_6_samples*1.0) / (sum_Number_of_Geographical_samples*1.0)*(log((number_of_Geographical_6_samples*1.0) / (sum_Number_of_Geographical_samples*1.0)) / log(2.0)));
    if (number_of_Geographical_1_samples == 0) { Entropy_item_1 = 0; }
    if (number_of_Geographical_2_samples == 0) { Entropy_item_2 = 0; }
    if (number_of_Geographical_3_samples == 0) { Entropy_item_3 = 0; }
    if (number_of_Geographical_4_samples == 0) { Entropy_item_4 = 0; }
    if (number_of_Geographical_5_samples == 0) { Entropy_item_5 = 0; }
    if (number_of_Geographical_6_samples == 0) { Entropy_item_6 = 0; }
    Entropy = -(Entropy_item_1 + Entropy_item_2 + Entropy_item_3
                + Entropy_item_4+Entropy_item_5+Entropy_item_6);//求Entropy
    int Geo_number1 = number_of_Geographical_1_samples;//前一种地物的数量
    int Geo_number2 = number_of_Geographical_1_samples + number_of_Geographical_2_samples;//前两种地物的数量
    int Geo_number3 = number_of_Geographical_1_samples + number_of_Geographical_2_samples + number_of_Geographical_3_samples;//前三种地物的数量
    int Geo_number4 = number_of_Geographical_1_samples + number_of_Geographical_2_samples + number_of_Geographical_3_samples + number_of_Geographical_4_samples;//前四种地物的数量
    int Geo_number5=Geo_number4+number_of_Geographical_5_samples;
    int Geo_number6=Geo_number5+number_of_Geographical_6_samples;
    if (characteristics.band[0]!=-1&&characteristics.band[1] == -1 && characteristics.band[2] == -1)
    {
        for (i = 0; i < sum_Number_of_Geographical_samples; i++)
        {
            minus[i] = Resort_class_1[i][characteristics.band[0]];
        }
    }
    else if (characteristics.band[0] != -1&&characteristics.band[1] != -1&&characteristics.band[2] == -1)
    {
        for (i = 0; i < sum_Number_of_Geographical_samples; i++)
        {
            minus[i] = Resort_class_1[i][characteristics.band[0]] - Resort_class_1[i][characteristics.band[1]];//对每一组数据求特征波段差值
        }
    }
    else
    {
        for (i = 0; i < sum_Number_of_Geographical_samples; i++)
        {
            minus[i] = (((Resort_class_1[i][characteristics.band[0]] + Resort_class_1[i][characteristics.band[2]]) / 2)) - Resort_class_1[i][characteristics.band[1]];
        }
    }

    for (j = 0; j < sum_Number_of_Geographical_samples - 1; j++)
    {
        for (i = 0; i < sum_Number_of_Geographical_samples; i++)
        {
            if (i < Geo_number1)
            {
                if (minus[i] >((minus[j] + minus[j + 1]) / 2))//找出地物一中大于（相邻差值平均数）的数量，和小于（相邻差值平均数）的数量
                {
                    greater_thanm1++;
                }
                else
                {
                    less_thanm1++;
                }
            }
            if (i >= Geo_number1 && i < Geo_number2)
            {//找出地物二中大于（相邻差值平均数）的数量，和小于（相邻差值平均数）的数量
                if (minus[i] > ((minus[j] + minus[j + 1]) / 2))
                {
                    greater_thanm2++;
                }
                else
                {
                    less_thanm2++;
                }
            }
            if (i >= Geo_number2&&i < Geo_number3)
            {//找出地物三中大于（相邻差值平均数）的数量，和小于（相邻差值平均数）的数量
                if (minus[i] > ((minus[j] + minus[j + 1]) / 2))
                {
                    greater_thanm3++;
                }
                else
                {
                    less_thanm3++;
                }
            }

            if (i >= Geo_number3&& i < Geo_number4)
            {//找出地物四中大于（相邻差值平均数）的数量，和小于（相邻差值平均数）的数量
                if (minus[i] > ((minus[j] + minus[j + 1]) / 2))
                {
                    greater_thanm4++;
                }
                else
                {
                    less_thanm4++;
                }
            }
            if (i >= Geo_number4&& i < Geo_number5)
            {//找出地物五中大于（相邻差值平均数）的数量，和小于（相邻差值平均数）的数量
                if (minus[i] > ((minus[j] + minus[j + 1]) / 2))
                {
                    greater_thanm5++;
                }
                else
                {
                    less_thanm5++;
                }
            }
            if (i >= Geo_number5&& i < Geo_number6)
            {//找出地物六中大于（相邻差值平均数）的数量，和小于（相邻差值平均数）的数量
                if (minus[i] > ((minus[j] + minus[j + 1]) / 2))
                {
                    greater_thanm6++;
                }
                else
                {
                    less_thanm6++;
                }
            }
        }

        greater_than = greater_thanm1 + greater_thanm2 + greater_thanm3 +
                greater_thanm4+greater_thanm5+greater_thanm6;//大于（相邻差值平均数）的数量
        less_than = less_thanm1 + less_thanm2 + less_thanm3 +
                less_thanm4+less_thanm5+less_thanm6;//小于（相邻差值平均数）的数量
        greater_m1 = ((greater_thanm1*1.0) / (greater_than* 1.0))*(log((greater_thanm1*1.0) / (greater_than * 1.0)) / log(2.0));
        greater_m2 = ((greater_thanm2*1.0) / (greater_than* 1.0))*(log((greater_thanm2*1.0) / (greater_than * 1.0)) / log(2.0));
        greater_m3 = ((greater_thanm3*1.0) / (greater_than* 1.0))*(log((greater_thanm3*1.0) / (greater_than * 1.0)) / log(2.0));
        greater_m4 = ((greater_thanm4*1.0) / (greater_than* 1.0))*(log((greater_thanm4*1.0) / (greater_than * 1.0)) / log(2.0));
        greater_m5 = ((greater_thanm5*1.0) / (greater_than* 1.0))*(log((greater_thanm5*1.0) / (greater_than * 1.0)) / log(2.0));
        greater_m6 = ((greater_thanm6*1.0) / (greater_than* 1.0))*(log((greater_thanm6*1.0) / (greater_than * 1.0)) / log(2.0));
        less_m1 = ((less_thanm1*1.0) / (less_than*1.0))*(log((less_thanm1*1.0) / (less_than*1.0)) / log(2.0));
        less_m2 = ((less_thanm2*1.0) / (less_than*1.0))*(log((less_thanm2*1.0) / (less_than*1.0)) / log(2.0));
        less_m3 = ((less_thanm3*1.0) / (less_than*1.0))*(log((less_thanm3*1.0) / (less_than*1.0)) / log(2.0));
        less_m4 = ((less_thanm4*1.0) / (less_than*1.0))*(log((less_thanm4*1.0) / (less_than*1.0)) / log(2.0));
        less_m5 = ((less_thanm5*1.0) / (less_than*1.0))*(log((less_thanm5*1.0) / (less_than*1.0)) / log(2.0));
        less_m6 = ((less_thanm6*1.0) / (less_than*1.0))*(log((less_thanm6*1.0) / (less_than*1.0)) / log(2.0));
        if (greater_thanm1 == 0) { greater_m1 = 0; }
        if (greater_thanm2 == 0) { greater_m2 = 0; }
        if (greater_thanm3 == 0) { greater_m3 = 0; }
        if (greater_thanm4 == 0) { greater_m4 = 0; }
        if (greater_thanm5 == 0) { greater_m5 = 0; }
        if (greater_thanm6 == 0) { greater_m6 = 0; }
        if (less_thanm1 == 0) { less_m1 = 0; }
        if (less_thanm2 == 0) { less_m2 = 0; }
        if (less_thanm3 == 0) { less_m3 = 0; }
        if (less_thanm4 == 0) { less_m4 = 0; }
        if (less_thanm5 == 0) { less_m5 = 0; }
        if (less_thanm6 == 0) { less_m6 = 0; }
        Entropy_greater = -(greater_m1 + greater_m2 + greater_m3 + greater_m4+ greater_m5+ greater_m6);
        Entropy_less = -(less_m1 + less_m2 + less_m3 + less_m4+ less_m5+ less_m6);
        EntropyFi = ((greater_than * 1.0) / sum_Number_of_Geographical_samples*1.0)*Entropy_greater + ((less_than*1.0) / sum_Number_of_Geographical_samples*1.0)*Entropy_less;
        Gain = Entropy - EntropyFi;
        SplitInfo_item_1 = ((greater_than * 1.0) / sum_Number_of_Geographical_samples*1.0)*(log(((greater_than * 1.0) / sum_Number_of_Geographical_samples*1.0)*1.0) / log(2.0));
        SplitInfo_item_2 = ((less_than*1.0) / sum_Number_of_Geographical_samples*1.0)*(log((less_than*1.0) / sum_Number_of_Geographical_samples*1.0) / log(2.0));
        if (greater_than == 0) { SplitInfo_item_1 = 0; }
        if (less_than == 0) { SplitInfo_item_2 = 0; }
        SplitInfo = -(SplitInfo_item_1 + SplitInfo_item_2);
        GainRatio = Gain / SplitInfo;
        if (GainRatio > temp.GainRatio)
        {
            temp.GainRatio = GainRatio;
            temp.Threshold = ((minus[j] + minus[j + 1])*1.0 / 2.0);
        }

        greater_than = 0, less_than = 0,
                greater_thanm1 = 0, greater_thanm2 = 0, greater_thanm3 = 0,
                greater_thanm4 = 0, greater_thanm5 = 0, greater_thanm6 = 0,
                less_thanm1 = 0, less_thanm2 = 0, less_thanm3 = 0,
                less_thanm4 = 0,less_thanm5 = 0,less_thanm6 = 0;
    }

    qInfo()<<QStringLiteral("波段i")<<characteristics.band[0]<<QStringLiteral("波段j")<<characteristics.band[1]<<"k"<<characteristics.band[2]<<QStringLiteral("增益比")<< temp.GainRatio<<QStringLiteral("阈值")<< temp.Threshold;
    temp.band[0] = characteristics.band[0];
    temp.band[1] = characteristics.band[1];
    temp.band[2] = characteristics.band[2];
    temp.Geography[0] = characteristics.Geography[0];
    temp.Geography[1] = characteristics.Geography[1];
    return temp;
}

void classify::Tree_divide(int sum_Number_of_Geographical_samples,
                           double data[100000][15], Feature decision_makeing_Info,
double Resort_class_1_1[][15], double Resort_class_1_2[][15],
int number_of_Geography_1_samples[6],//记录各个地物的数量
int number_of_Geography_2_0_samples[6],//记录分开的数据
int number_of_Geography_2_1_samples[6],int bandNb)//记录分开的数据
{
    int i, j;
    int cycle_1 = 0, cycle_2 = 0;
    int Geo_number1 = number_of_Geography_1_samples[0];//前一种地物的数量
    int Geo_number2 = number_of_Geography_1_samples[0] + number_of_Geography_1_samples[1];//前两种地物的数量
    int Geo_number3 = number_of_Geography_1_samples[0] + number_of_Geography_1_samples[1] + number_of_Geography_1_samples[2];//前三种地物的数量
    int Geo_number4 = number_of_Geography_1_samples[0] + number_of_Geography_1_samples[1] + number_of_Geography_1_samples[2] + number_of_Geography_1_samples[3];//前四种地物的数量
    int Geo_number5=Geo_number4+number_of_Geography_1_samples[4];
    int Geo_number6=Geo_number5+number_of_Geography_1_samples[5];
    if (decision_makeing_Info.band[0] != -1 && decision_makeing_Info.band[1] == -1 && decision_makeing_Info.band[2] == -1)
    {
        for (i = 0; i < sum_Number_of_Geographical_samples; i++)//将样本分成两类
        {
            if (data[i][decision_makeing_Info.band[0]] > decision_makeing_Info.Threshold)
            {
                for (j = 0; j < bandNb + 1; j++)
                {
                    Resort_class_1_1[cycle_1][j] = data[i][j];
                }
                if (i < Geo_number1)
                {
                    number_of_Geography_2_0_samples[0]++;
                }
                if (i >= Geo_number1 && i < Geo_number2)
                {
                    number_of_Geography_2_0_samples[1]++;
                }
                if (i >= Geo_number2&& i < Geo_number3)
                {
                    number_of_Geography_2_0_samples[2]++;
                }
                if (i >= Geo_number3&& i < Geo_number4)
                {
                    number_of_Geography_2_0_samples[3]++;
                }
                if (i >= Geo_number4&& i < Geo_number5)
                {
                    number_of_Geography_2_0_samples[4]++;
                }
                if (i >= Geo_number5&& i < Geo_number6)
                {
                    number_of_Geography_2_0_samples[5]++;
                }
                cycle_1++;
            }
            else
            {
                for (j = 0; j < bandNb + 1; j++)
                {
                    Resort_class_1_2[cycle_2][j] = data[i][j];
                }
                if (i < Geo_number1)
                {
                    number_of_Geography_2_1_samples[0]++;
                }
                if (i >= Geo_number1 && i < Geo_number2)
                {
                    number_of_Geography_2_1_samples[1]++;
                }
                if (i >= Geo_number2&& i < Geo_number3)
                {
                    number_of_Geography_2_1_samples[2]++;
                }
                if (i >= Geo_number3&& i < Geo_number4)
                {
                    number_of_Geography_2_1_samples[3]++;
                }
                if (i >= Geo_number4&& i < Geo_number5)
                {
                    number_of_Geography_2_1_samples[4]++;
                }
                if (i >= Geo_number5&& i < Geo_number6)
                {
                    number_of_Geography_2_1_samples[5]++;
                }
                cycle_2++;
            }//else
        }
    }
    else if (decision_makeing_Info.band[0] != -1 && decision_makeing_Info.band[1] != -1 && decision_makeing_Info.band[2] == -1)//如果是不是一次扩展得到的特征
    {
        for (i = 0; i < sum_Number_of_Geographical_samples; i++)//将样本分成两类
        {
            if (data[i][decision_makeing_Info.band[0]] - data[i][decision_makeing_Info.band[1]] > decision_makeing_Info.Threshold)
            {
                for (j = 0; j < bandNb + 1; j++)
                {
                    Resort_class_1_1[cycle_1][j] = data[i][j];
                }
                if (i < Geo_number1)
                {
                    number_of_Geography_2_0_samples[0]++;
                }
                if (i >= Geo_number1 && i < Geo_number2)
                {
                    number_of_Geography_2_0_samples[1]++;
                }
                if (i >= Geo_number2&& i < Geo_number3)
                {
                    number_of_Geography_2_0_samples[2]++;
                }
                if (i >= Geo_number3&& i < Geo_number4)
                {
                    number_of_Geography_2_0_samples[3]++;
                }
                if (i >= Geo_number4&& i < Geo_number5)
                {
                    number_of_Geography_2_0_samples[4]++;
                }
                if (i >= Geo_number5&& i < Geo_number6)
                {
                    number_of_Geography_2_0_samples[5]++;
                }
                cycle_1++;
            }
            else
            {
                for (j = 0; j < bandNb + 1; j++)
                {
                    Resort_class_1_2[cycle_2][j] = data[i][j];
                }
                if (i < Geo_number1)
                {
                    number_of_Geography_2_1_samples[0]++;
                }
                if (i >= Geo_number1 && i < Geo_number2)
                {
                    number_of_Geography_2_1_samples[1]++;
                }
                if (i >= Geo_number2&& i < Geo_number3)
                {
                    number_of_Geography_2_1_samples[2]++;
                }
                if (i >= Geo_number3&& i < Geo_number4)
                {
                    number_of_Geography_2_1_samples[3]++;
                }
                if (i >= Geo_number4&& i < Geo_number5)
                {
                    number_of_Geography_2_1_samples[4]++;
                }
                if (i >= Geo_number5&& i < Geo_number6)
                {
                    number_of_Geography_2_1_samples[5]++;
                }
                cycle_2++;
            }//else
        }//for
    }//if
    else
    {
        for (i = 0; i < sum_Number_of_Geographical_samples; i++)//将样本分成两类
        {
            if ((data[i][decision_makeing_Info.band[0]] + data[i][decision_makeing_Info.band[2]]) / 2 - data[i][decision_makeing_Info.band[1]] > decision_makeing_Info.Threshold)
            {
                for (j = 0; j < bandNb + 1; j++)
                {
                    Resort_class_1_1[cycle_1][j] = data[i][j];
                }
                if (i < Geo_number1)
                {
                    number_of_Geography_2_0_samples[0]++;
                }
                if (i >= Geo_number1&& i < Geo_number2)
                {
                    number_of_Geography_2_0_samples[1]++;
                }
                if (i >= Geo_number2 && i < Geo_number3)
                {
                    number_of_Geography_2_0_samples[2]++;
                }
                if (i >= Geo_number3&& i < Geo_number4)
                {
                    number_of_Geography_2_0_samples[3]++;
                }
                if (i >= Geo_number4&& i < Geo_number5)
                {
                    number_of_Geography_2_0_samples[4]++;
                }
                if (i >= Geo_number5&& i < Geo_number6)
                {
                    number_of_Geography_2_0_samples[5]++;
                }
                cycle_1++;
            }
            else
            {
                for (j = 0; j < bandNb + 1; j++)
                {
                    Resort_class_1_2[cycle_2][j] = data[i][j];
                }
                if (i < Geo_number1)
                {
                    number_of_Geography_2_1_samples[0]++;
                }
                if (i >= Geo_number1&& i < Geo_number2)
                {
                    number_of_Geography_2_1_samples[1]++;
                }
                if (i >= Geo_number2 && i < Geo_number3)
                {
                    number_of_Geography_2_1_samples[2]++;
                }
                if (i >= Geo_number3&& i < Geo_number4)
                {
                    number_of_Geography_2_1_samples[3]++;
                }
                if (i >= Geo_number4&& i < Geo_number5)
                {
                    number_of_Geography_2_1_samples[4]++;
                }
                if (i >= Geo_number5&& i < Geo_number6)
                {
                    number_of_Geography_2_1_samples[5]++;
                }
                cycle_2++;
            }//else
        }//for
    }
}
//divide调用tree_divide进行地物分离
void  classify::divide(Feature characteristics[], double data[][15], int sumNumber, int eachNumber[],Tree treeNode)
{
    divideCount++;
    if(divideCount>70)
    {
        qFatal("此样本在地物区分时出现了错误,请点击忽略按钮");
        return;
    }
    emit setProgressValue(50+divideCount*5);
    int geoNameTh=N;
    if(Extend==true)
    {
        geoNameTh=EN;
    }

    treeNode->left=(TreeNode*)malloc(sizeof(TreeNode));
    treeNode->right=(TreeNode*)malloc(sizeof(TreeNode));
    int i;
    int count = 0;
    Feature decision_makeing_Info;
    decision_makeing_Info.band[0]=-1;
    decision_makeing_Info.band[1]=-1;
    decision_makeing_Info.GainRatio=-1;
    decision_makeing_Info.Threshold=-1;
    Feature Temp_GainRatio[150];
    for(int i=0;i<150;i++)//初始化内存
    {
        Temp_GainRatio[i].band[0]=-1;
        Temp_GainRatio[i].band[1]=-1;
        Temp_GainRatio[i].band[2]=-1;
        Temp_GainRatio[i].GainRatio=-1;
        Temp_GainRatio[i].Threshold=-1;
        Temp_GainRatio[i].percent=-1;
    }
    for (i = 0; characteristics[i].band[0] != -1; i++)
    {
        Temp_GainRatio[count] = FeatureSelection(data, characteristics[i],sumNumber,
                                                 eachNumber[0], eachNumber[1],
                eachNumber[2], eachNumber[3],
                eachNumber[4], eachNumber[5]);
        count++;
    }
    decision_makeing_Info = Find_MAX_VALUE(Temp_GainRatio, 150);
    double(*outdata_below)[15];
    outdata_below = (double(*)[15])malloc(100000 * 15 * sizeof(double));
    double(*outdata_above)[15];
    outdata_above = (double(*)[15])malloc(100000 * 15 * sizeof(double));
    int eachNumber_below[6] = { 0 };
    int eachNumber_above[6] = { 0 };
    if(Extend==false)
    {
        Tree_divide(sumNumber, data, decision_makeing_Info, outdata_above, outdata_below, eachNumber, eachNumber_above, eachNumber_below,N);
    }
    else
    {
        Tree_divide(sumNumber, data, decision_makeing_Info, outdata_above, outdata_below, eachNumber, eachNumber_above, eachNumber_below,EN);
    }
    for(int i=0;i<150;i++)//初始化内存
    {
        Temp_GainRatio[i].band[0]=-1;
        Temp_GainRatio[i].band[1]=-1;
        Temp_GainRatio[i].band[2]=-1;
        Temp_GainRatio[i].GainRatio=-1;
        Temp_GainRatio[i].Threshold=-1;
        Temp_GainRatio[i].percent=-1;
    }
    int sumNumber_above = eachNumber_above[0] + eachNumber_above[1] + eachNumber_above[2] + eachNumber_above[3]+eachNumber_above[4]+eachNumber_above[5];
    int sumNumber_below = eachNumber_below[0] + eachNumber_below[1] + eachNumber_below[2] + eachNumber_below[3]+eachNumber_below[4]+eachNumber_below[5];
    if(decision_makeing_Info.band[0]!=-1&&decision_makeing_Info.band[1]==-1&&decision_makeing_Info.band[2]==-1)
    {
        treeNode->bandi=decision_makeing_Info.band[0];
        treeNode->bandj=-1;
        treeNode->bandk=-1;
    }
    else if(decision_makeing_Info.band[0]!=-1&&decision_makeing_Info.band[1]!=-1&&decision_makeing_Info.band[2]==-1)
    {
        treeNode->bandi=decision_makeing_Info.band[0];
        treeNode->bandj=decision_makeing_Info.band[1];
        treeNode->bandk=-1;
    }
    else if(decision_makeing_Info.band[0]!=-1&&decision_makeing_Info.band[1]!=-1&&decision_makeing_Info.band[2]!=-1)
    {
        treeNode->bandi=decision_makeing_Info.band[0];
        treeNode->bandj=decision_makeing_Info.band[1];
        treeNode->bandk=decision_makeing_Info.band[2];
    }
    treeNode->numbers=-1;
    treeNode->geoName=-1;
    treeNode->Threshold=decision_makeing_Info.Threshold;
    if (sumNumber_below < decide_number)
    {
        if (decision_makeing_Info.band[0]!=-1&&decision_makeing_Info.band[1]==-1&&decision_makeing_Info.band[2] == -1)
        {
            treeNode->left->right=NULL;
            treeNode->left->left=NULL;
            treeNode->left->bandi=decision_makeing_Info.band[0];
            treeNode->left->bandj=-1;
            treeNode->left->bandk=-1;
            treeNode->left->numbers=sumNumber_below;
            treeNode->left->Threshold=decision_makeing_Info.Threshold;
            int geo[6]={0};

            for(int i=0;i<sumNumber_below;i++)
            {
                switch ((int)outdata_below[i][geoNameTh]) {
                case 1:
                    geo[0]++;
                    break;
                case 2:
                    geo[1]++;
                    break;
                case 3:
                    geo[2]++;
                    break;
                case 4:
                    geo[3]++;
                    break;
                case 5:
                    geo[4]++;
                    break;
                case 6:
                    geo[5]++;
                    break;
                }
            }
            for(int i=0;i<6;i++)
            {
                if(geo[i]*1.0/sumNumber_below>0.5)
                {
                    treeNode->left->geoName=i+1;
                }
            }
            free(outdata_below);

        }
        else if(decision_makeing_Info.band[0]!=-1&&decision_makeing_Info.band[1]!=-1&&decision_makeing_Info.band[2] == -1)
        {
            treeNode->left->right=NULL;
            treeNode->left->left=NULL;
            treeNode->left->bandi=decision_makeing_Info.band[0];
            treeNode->left->bandj=decision_makeing_Info.band[1];
            treeNode->left->bandk=-1;
            treeNode->left->numbers=sumNumber_below;

            treeNode->left->Threshold=decision_makeing_Info.Threshold;
            int geo[6]={0};

            for(int i=0;i<sumNumber_below;i++)
            {
                switch ((int)outdata_below[i][geoNameTh]) {
                case 1:
                    geo[0]++;
                    break;
                case 2:
                    geo[1]++;
                    break;
                case 3:
                    geo[2]++;
                    break;
                case 4:
                    geo[3]++;
                    break;
                case 5:
                    geo[4]++;
                    break;
                case 6:
                    geo[5]++;
                    break;
                }
            }
            for(int i=0;i<6;i++)
            {
                if(geo[i]*1.0/sumNumber_below>0.5)
                {
                    treeNode->left->geoName=i+1;

                }
            }
            free(outdata_below);

        }
        else
        {
            treeNode->left->right=NULL;
            treeNode->left->left=NULL;
            treeNode->left->bandi=decision_makeing_Info.band[0];
            treeNode->left->bandj=decision_makeing_Info.band[1];
            treeNode->left->bandk=decision_makeing_Info.band[2];
            treeNode->left->numbers=sumNumber_below;

            treeNode->left->Threshold=decision_makeing_Info.Threshold;
            int geo[6]={0};

            for(int i=0;i<sumNumber_below;i++)
            {
                switch ((int)outdata_below[i][geoNameTh]) {
                case 1:
                    geo[0]++;
                    break;
                case 2:
                    geo[1]++;
                    break;
                case 3:
                    geo[2]++;
                    break;
                case 4:
                    geo[3]++;
                    break;
                case 5:
                    geo[4]++;
                    break;
                case 6:
                    geo[5]++;
                    break;
                }
            }
            for(int i=0;i<6;i++)
            {
                if(geo[i]*1.0/sumNumber_below>0.5)
                {
                    treeNode->left->geoName=i+1;

                }
            }
            free(outdata_below);

        }
    }
    else
    {
        if(decision_makeing_Info.band[0]!=-1&&decision_makeing_Info.band[1]==-1&&decision_makeing_Info.band[2]==-1)
        {
            treeNode->left->bandi=decision_makeing_Info.band[0];
            treeNode->left->bandj=-1;
            treeNode->left->bandk=-1;

        }
        else if(decision_makeing_Info.band[0]!=-1&&decision_makeing_Info.band[1]!=-1&&decision_makeing_Info.band[2]==-1)
        {
            treeNode->left->bandi=decision_makeing_Info.band[0];
            treeNode->left->bandj=decision_makeing_Info.band[1];
            treeNode->left->bandk=-1;
        }
        else if(decision_makeing_Info.band[0]!=-1&&decision_makeing_Info.band[1]!=-1&&decision_makeing_Info.band[2]!=-1)
        {
            treeNode->left->bandi=decision_makeing_Info.band[0];
            treeNode->left->bandj=decision_makeing_Info.band[1];
            treeNode->left->bandk=decision_makeing_Info.band[2];
        }
        treeNode->left->Threshold=-1;
        treeNode->left->geoName=-1;
        treeNode->left->numbers=-1;
        divide(characteristics, outdata_below, sumNumber_below, eachNumber_below,treeNode->left);
    }
    if (sumNumber_above < decide_number)
    {
        if (decision_makeing_Info.band[0]!=-1&&decision_makeing_Info.band[1]==-1&&decision_makeing_Info.band[2] == -1)
        {
            treeNode->right->right=NULL;
            treeNode->right->left=NULL;
            treeNode->right->bandi=decision_makeing_Info.band[0];
            treeNode->right->bandj=-1;
            treeNode->right->bandk=-1;
            treeNode->right->numbers=sumNumber_above;

            treeNode->right->Threshold=decision_makeing_Info.Threshold;
            int geo[6]={0};

            for(int i=0;i<sumNumber_above;i++)
            {
                switch ((int)outdata_above[i][geoNameTh]) {
                case 1:
                    geo[0]++;
                    break;
                case 2:
                    geo[1]++;
                    break;
                case 3:
                    geo[2]++;
                    break;
                case 4:
                    geo[3]++;
                    break;
                case 5:
                    geo[4]++;
                    break;
                case 6:
                    geo[5]++;
                    break;
                }
            }
            for(int i=0;i<6;i++)
            {
                if(geo[i]*1.0/sumNumber_above>0.5)
                {
                    treeNode->right->geoName=i+1;
                }
            }
            free(outdata_above);
        }
        else if (decision_makeing_Info.band[0]!=-1&&decision_makeing_Info.band[1]!=-1&&decision_makeing_Info.band[2] == -1)
        {
            treeNode->right->right=NULL;
            treeNode->right->left=NULL;
            treeNode->right->bandi=decision_makeing_Info.band[0];
            treeNode->right->bandj=decision_makeing_Info.band[1];
            treeNode->right->bandk=-1;
            treeNode->right->numbers=sumNumber_above;

            treeNode->right->Threshold=decision_makeing_Info.Threshold;
            int geo[6]={0};
            for(int i=0;i<sumNumber_above;i++)
            {
                switch ((int)outdata_above[i][geoNameTh]) {
                case 1:
                    geo[0]++;
                    break;
                case 2:
                    geo[1]++;
                    break;
                case 3:
                    geo[2]++;
                    break;
                case 4:
                    geo[3]++;
                    break;
                case 5:
                    geo[4]++;
                    break;
                case 6:
                    geo[5]++;
                    break;
                }
            }
            for(int i=0;i<6;i++)
            {
                if(geo[i]*1.0/sumNumber_above>0.5)
                {
                    treeNode->right->geoName=i+1;
                }
            }
            free(outdata_above);
        }
        else
        {
            treeNode->right->right=NULL;
            treeNode->right->left=NULL;
            treeNode->right->bandi=decision_makeing_Info.band[0];
            treeNode->right->bandj=decision_makeing_Info.band[1];
            treeNode->right->bandk=decision_makeing_Info.band[2];
            treeNode->right->numbers=sumNumber_above;

            treeNode->right->Threshold=decision_makeing_Info.Threshold;
            int geo[6]={0};

            for(int i=0;i<sumNumber_above;i++)
            {
                switch ((int)outdata_above[i][geoNameTh]) {
                case 1:
                    geo[0]++;
                    break;
                case 2:
                    geo[1]++;
                    break;
                case 3:
                    geo[2]++;
                    break;
                case 4:
                    geo[3]++;
                    break;
                case 5:
                    geo[4]++;
                    break;
                case 6:
                    geo[5]++;
                    break;
                }
            }
            for(int i=0;i<6;i++)
            {
                if(geo[i]*1.0/sumNumber_above>0.5)
                {
                    treeNode->right->geoName=i+1;
                }
            }
            free(outdata_above);
        }
    }
    else
    {
        if(decision_makeing_Info.band[0]!=-1&&decision_makeing_Info.band[1]==-1&&decision_makeing_Info.band[2]==-1)
        {
            treeNode->right->bandi=decision_makeing_Info.band[0];
            treeNode->right->bandj=-1;
            treeNode->right->bandk=-1;

        }
        else if(decision_makeing_Info.band[0]!=-1&&decision_makeing_Info.band[1]!=-1&&decision_makeing_Info.band[2]==-1)
        {
            treeNode->right->bandi=decision_makeing_Info.band[0];
            treeNode->right->bandj=decision_makeing_Info.band[1];
            treeNode->right->bandk=-1;
        }
        else if(decision_makeing_Info.band[0]!=-1&&decision_makeing_Info.band[1]!=-1&&decision_makeing_Info.band[2]!=-1)
        {
            treeNode->right->bandi=decision_makeing_Info.band[0];
            treeNode->right->bandj=decision_makeing_Info.band[1];
            treeNode->right->bandk=decision_makeing_Info.band[2];
        }
        treeNode->right->Threshold=decision_makeing_Info.Threshold;
        treeNode->right->numbers=-1;
        treeNode->right->geoName=-1;
        divide(characteristics, outdata_above, sumNumber_above, eachNumber_above,treeNode->right);
    }
    return ;
}


//找到所有路径
void classify::findAllPath(Tree T, Quee quee,int status,QString fileName){
    if (T == NULL)
    {
        return;
    }
    //入队
    if(T->bandi!=-1&&T->bandj==-1)
    {
        quee[queeInCount].bandi=T->bandi;
        quee[queeInCount].bandj=-1;
        quee[queeInCount].bandk=-1;
        quee[queeInCount].status=status;
        quee[queeInCount].Threshold=T->Threshold;
        quee[queeInCount].geoName=T->geoName;
        queeInCount++;
    }
    else if(T->bandi!=-1&&T->bandj!=-1&&T->bandk==-1)
    {
        quee[queeInCount].bandi=T->bandi;
        quee[queeInCount].bandj=T->bandj;
        quee[queeInCount].bandk=-1;
        quee[queeInCount].status=status;
        quee[queeInCount].Threshold=T->Threshold;
        quee[queeInCount].geoName=T->geoName;
        queeInCount++;
    }
    else
    {
        quee[queeInCount].bandi=T->bandi;
        quee[queeInCount].bandj=T->bandj;
        quee[queeInCount].bandk=T->bandk;
        quee[queeInCount].status=status;
        quee[queeInCount].Threshold=T->Threshold;
        quee[queeInCount].geoName=T->geoName;
        queeInCount++;
    }
    if (T->left == NULL&&T->right == NULL) //达到了叶子节点
    {
        QString String=NULL;
        QFile write(fileName.toStdString().c_str());
        write.open(QIODevice::WriteOnly|QIODevice::Append);
        QTextStream out(&write);
        int i=0;
        for(i=0;i<queeInCount;i++)
        {
            String.append(QString::number(quee[i].bandi));
            String.append(",");
            String.append(QString::number(quee[i].bandj));
            String.append(",");
            String.append(QString::number(quee[i].bandk));
            String.append(",");
            String.append(QString::number(quee[i].status));
            String.append(",");
            String.append(QString::number(quee[i].Threshold));
            String.append(",#and#,");
        }
        if(-1<quee[i-1].geoName&&quee[i-1].geoName<7)
        {
            String.append(QString::number(quee[i-1].geoName));
        }
        else
        {
            String.append(QString::number(0));
        }

        String.append("#end#");
        out<<String;  //打印路径
    }
    if (T->left!=NULL)//左子树
    {
        findAllPath(T->left, quee,-2,fileName);
    }
    if (T->right!=NULL)//右子树
    {
        findAllPath(T->right, quee,2,fileName);
    }
    queeInCount--;//在返回到父节点之前，在路径上删除当前节点
}
void classify::DeleteTree(Tree T)
{
    if ( T!=NULL )
    {
        DeleteTree(T->left);
        DeleteTree(T->right);
        free(T);
    }
}













///特征赋值模块    含一次扩展
void classify:: display(int reorder[][15], int first_extend[][15][15],
int zero_extend_Sum_Number, int frist_extend_Sum_Number,Feature characteristics[100],
int Geographical_1, int Geographical_2)
{
    int i, j, k;
    //排序阶段输出模块
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
        {
            if (reorder[i][j] != 0 && (reorder[i][j] * 100.0) / zero_extend_Sum_Number >= 75)
            {
                characteristics[resultCount].band[0] = i;
                characteristics[resultCount].band[1] = j;
                characteristics[resultCount].percent = (reorder[i][j] * 100.0) / zero_extend_Sum_Number;
                characteristics[resultCount].Geography[0] = Geographical_1;
                characteristics[resultCount].Geography[1] = Geographical_2;
                resultCount++;
            }
        }
    //一次扩展输出模块
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            for (k = 0; k < N; k++)
            {
                if (first_extend[i][j][k] != 0 && ((first_extend[i][j][k] * 100.0) / frist_extend_Sum_Number) > 75)
                {
                    characteristics[resultCount].band[0] = i;
                    characteristics[resultCount].band[1] = j;
                    characteristics[resultCount].band[2] = k;
                    characteristics[resultCount].percent = (reorder[i][j] * 100.0) / frist_extend_Sum_Number;
                    characteristics[resultCount].Geography[0] = Geographical_1;
                    characteristics[resultCount].Geography[1] = Geographical_2;
                    resultCount++;
                }
            }
}
///特征赋值模块     扩展波段
void classify:: display(int reorder[][15], int first_extend[][15][15],
int zero_extend_Sum_Number, int frist_extend_Sum_Number,Feature characteristics[100],
int Geographical_1, int Geographical_2,int oneBand[],int bandNb)
{
    int i, j, k;
    //排序阶段输出模块
    for (i = 0; i < bandNb; i++)
        for (j = 0; j < bandNb; j++)
        {
            if (reorder[i][j] != 0 && (reorder[i][j] * 100.0) / zero_extend_Sum_Number >= 75)
            {
                characteristics[resultCount].band[0] = i;
                characteristics[resultCount].band[1] = j;
                characteristics[resultCount].band[2] =-1;
                characteristics[resultCount].percent = (reorder[i][j] * 100.0) / zero_extend_Sum_Number;
                characteristics[resultCount].Geography[0] = Geographical_1;
                characteristics[resultCount].Geography[1] = Geographical_2;
                resultCount++;
            }
        }
    //一次扩展输出模块
    for (i = 0; i < bandNb; i++)
        for (j = 0; j < bandNb; j++)
            for (k = 0; k < bandNb; k++)
            {
                if (first_extend[i][j][k] != 0 && ((first_extend[i][j][k] * 100.0) / frist_extend_Sum_Number) > 75)
                {
                    characteristics[resultCount].band[0] = i;
                    characteristics[resultCount].band[1] = j;
                    characteristics[resultCount].band[2] = k;
                    characteristics[resultCount].percent = (reorder[i][j] * 100.0) / frist_extend_Sum_Number;
                    characteristics[resultCount].Geography[0] = Geographical_1;
                    characteristics[resultCount].Geography[1] = Geographical_2;
                    resultCount++;
                }
            }
    for (i = 0; i < bandNb; i++)
    {
        if (oneBand[i] != -1)
        {
            characteristics[resultCount].band[0] = i;
            characteristics[resultCount].band[1] = -1;
            characteristics[resultCount].band[2] = -1;
            characteristics[resultCount].Geography[0] = Geographical_1;
            characteristics[resultCount].Geography[1] = Geographical_2;
            resultCount++;
        }
    }
}
//主函数
void classify:: extendMain(void)
{

    resultCount = 0;
    queeInCount=0;
    root->bandi=-1;
    root->bandj=-1;
    root->bandk=-1;
    root->numbers=-1;
    root->Threshold=-1;
    for(int i=0;i<50;i++)
    {
        quee[i].bandi=-1;
        quee[i].bandj=-1;
        quee[i].bandk=-1;
        quee[i].geoName=-1;
        quee[i].numbers=-1;
        quee[i].status=-1;
        quee[i].Threshold=-1;
        quee[i].visited=-1;
    }
    int i = 0, j = 0, k = 0,judge1;
    int Number_of_cycles_a = 0, Number_of_cycles_b = 0;//循环次数
    int zero_extend_Sum_Number = 0, frist_extend_Sum_Number = 0;//函数调用计数变量
    array(*a)[15];
    a = (array(*)[15])malloc(20000 * 15 * sizeof(array));
    array(*b)[15];
    b = (array(*)[15])malloc(20000 * 15 * sizeof(array));
    array x[15], y[15];//样本副本
    int reorder[15][15] = { 0 };
    memset(reorder,-1,sizeof(reorder));
    int first_extend[15][15][15] = { 0 };
    int oneBand[15];
    memset(oneBand, -1, sizeof(oneBand));//正常
    int Geographical_1 = -1, Geographical_2 = -1;//存储地物信息
    Feature  characteristics[200];//存储组合特征
    for(int i=0;i<200;i++)
    {
        characteristics[i].band[0]=-1;
        characteristics[i].band[1]=-1;
        characteristics[i].band[2]=-1;
        characteristics[i].GainRatio=-1;
        characteristics[i].Geography[0]=-1;
        characteristics[i].Geography[1]=-1;
        characteristics[i].Threshold=-1;
    }
    double(*data)[15];
    data = (double(*)[15])malloc(100000 *15* sizeof(double));
    int number_item2 = sample_1 + sample_2;
    int number_item3 = number_item2 + sample_3;
    int number_item4 = number_item3 + sample_4;
    int number_item5 = number_item4 + sample_5;
    int number_item6 = number_item5 + sample_6;
    for (i = 0; i < sumSample; i++)
    {
        for (j = 0; j < N; j++)
        {
            data[i][j]=Data[i].bands[j];//读取所有数据
        }
        if (i < sample_1)
        {
            data[i][EN] = 1;
        }
        if (i >= sample_1 && i < number_item2)
        {
            data[i][EN] = 2;
        }
        if (i >= number_item2 && i < number_item3)
        {
            data[i][EN] = 3;
        }
        if (i >= number_item3&& i < number_item4)
        {
            data[i][EN] = 4;
        }
        if(i>=number_item4&&i<number_item5)
        {
            data[i][EN]=5;
        }
        if(i>=number_item5&&i<number_item6)
        {
            data[i][EN]=6;
        }
    }
    //数据输入模块

    for(int count=0;count<GeoNumber*(GeoNumber-1)/2;count++)
    {
        switch (count) {
        case 0:
            Geographical_1 = 1;
            Geographical_2 = 2;
            for(i=0;i<sample_1;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=sample_1;i<number_item2;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        case 1:
            Geographical_1 = 1;
            Geographical_2 = 3;
            for(i=0;i<sample_1;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item2;i<number_item3;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        case 2:
            Geographical_1 = 2;
            Geographical_2 = 3;
            for(i=sample_1;i<number_item2;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item2;i<number_item3;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        case 3:
            Geographical_1 = 1;
            Geographical_2 = 4;
            for(i=0;i<sample_1;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item3;i<number_item4;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }

            break;
        case 4:
            Geographical_1 = 2;
            Geographical_2 = 4;
            for(i=sample_1;i<number_item2;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item3;i<number_item4;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        case 5:
            Geographical_1 = 3;
            Geographical_2 = 4;
            for(i=number_item2;i<number_item3;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item3;i<number_item4;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }

            break;
        case 6:
            Geographical_1 = 1;
            Geographical_2 = 5;
            for(i=0;i<sample_1;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item4;i<number_item5;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        case 7:
            Geographical_1 = 2;
            Geographical_2 = 5;
            for(i=sample_1;i<number_item2;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item4;i<number_item5;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }

            break;
        case 8:
            Geographical_1 = 3;
            Geographical_2 = 5;
            for(i=number_item2;i<number_item3;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item4;i<number_item5;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        case 9:
            Geographical_1 = 4;
            Geographical_2 = 5;
            for(i=number_item3;i<number_item4;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item4;i<number_item5;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        case 10:
            Geographical_1 = 1;
            Geographical_2 = 6;
            for(i=0;i<sample_1;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item5;i<number_item6;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        case 11:
            Geographical_1 = 2;
            Geographical_2 = 6;
            for(i=sample_1;i<number_item2;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item5;i<number_item6;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }

            break;
        case 12:
            Geographical_1 = 3;
            Geographical_2 = 6;
            for(i=number_item2;i<number_item3;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item5;i<number_item6;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        case 13:
            Geographical_1 = 4;
            Geographical_2 = 6;
            for(i=number_item3;i<number_item4;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item5;i<number_item6;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        case 14:
            Geographical_1 = 5;
            Geographical_2 = 6;
            for(i=number_item4;i<number_item5;i++)
            {
                for (j = 0; j < N; j++)
                {
                    a[Number_of_cycles_a][j].element=data[i][j];
                    a[Number_of_cycles_a][j].id=j;
                }
                Number_of_cycles_a++;
            }
            for(i=number_item5;i<number_item6;i++)
            {
                for (j = 0; j < N; j++)
                {
                    b[Number_of_cycles_b][j].element=data[i][j];
                    b[Number_of_cycles_b][j].id=j;
                }
                Number_of_cycles_b++;
            }
            break;
        }
        //读取模块
        for (j = 0; j < Number_of_cycles_a; j++)
        {
            for (k = 0; k < Number_of_cycles_b; k++)
            {
                judge1 = sort1(a[j], b[k], reorder,N);
                zero_extend_Sum_Number++;
                if (11 == judge1)//第一步返回结果为11，无法分辨是否为两地事物，开始第二步分辨
                {
                    for (int p = 0; p < N; p++)
                    {
                        x[p] = a[j][p];
                        y[p] = b[k][p];
                    }
                    //11是无法分辨，10是可以分辨
                    sort(x, y,N);//对a地物进行排序和对b地物进行排序
                    sort2(x, y, first_extend,N);//调用第二步分离
                    frist_extend_Sum_Number++;
                }
            }
        }
        display(reorder, first_extend, zero_extend_Sum_Number,frist_extend_Sum_Number,
                characteristics, Geographical_1, Geographical_2);
        {
            //数组清空函数
            zero_extend_Sum_Number = frist_extend_Sum_Number= 0;
            Number_of_cycles_a = Number_of_cycles_b = 0;
            memset(reorder, 0, sizeof(reorder));
            memset(first_extend, 0, sizeof(first_extend));
        }
    }
    extend(characteristics, data);//波段扩展
    for(int i=0;i<200;i++)
    {
        characteristics[i].band[0]=-1;
        characteristics[i].band[1]=-1;
        characteristics[i].band[2]=-1;
        characteristics[i].GainRatio=-1;
        characteristics[i].Geography[0]=-1;
        characteristics[i].Geography[1]=-1;
        characteristics[i].Threshold=-1;
    }
    resultCount = 0;//将characteristics的计数值清零
    Number_of_cycles_a = 0;
    Number_of_cycles_b = 0;
    for (int count = 0; count < GeoNumber*(GeoNumber-1)/2; count++)
    {
        switch (count)
        {
        case 0:
            Geographical_1 = 1;
            Geographical_2 = 2;
            for (j = 0; j < sample_1; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    a[Number_of_cycles_a][i].element = data[j][i];
                    a[Number_of_cycles_a][i].id = i;
                }
                Number_of_cycles_a++;
            }
            for (j = sample_1; j < number_item2; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    b[Number_of_cycles_b][i].element = data[j][i];
                    b[Number_of_cycles_b][i].id = i;
                }
                Number_of_cycles_b++;
            }
            break;
        case 1:
            Geographical_1 = 1;
            Geographical_2 = 3;
            for (j = 0; j < sample_1; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    a[Number_of_cycles_a][i].element = data[j][i];
                    a[Number_of_cycles_a][i].id = i;
                }
                Number_of_cycles_a++;
            }
            for (j = number_item2; j < number_item3; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    b[Number_of_cycles_b][i].element = data[j][i];
                    b[Number_of_cycles_b][i].id = i;
                }
                Number_of_cycles_b++;
            }

            break;
        case 2:
            Geographical_1 = 2;
            Geographical_2 = 3;
            for (j = sample_1; j < number_item2; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    a[Number_of_cycles_a][i].element = data[j][i];
                    a[Number_of_cycles_a][i].id = i;
                }
                Number_of_cycles_a++;
            }
            for (j = number_item2; j < number_item3; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    b[Number_of_cycles_b][i].element = data[j][i];
                    b[Number_of_cycles_b][i].id = i;
                }
                Number_of_cycles_b++;
            }
            break;
        case 3:
            Geographical_1 = 1;
            Geographical_2 = 4;
            for (j = 0; j < sample_1; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    a[Number_of_cycles_a][i].element = data[j][i];
                    a[Number_of_cycles_a][i].id = i;
                }
                Number_of_cycles_a++;
            }
            for (j = number_item3; j < number_item4; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    b[Number_of_cycles_b][i].element = data[j][i];
                    b[Number_of_cycles_b][i].id = i;
                }
                Number_of_cycles_b++;
            }
            break;

        case 4:
            Geographical_1 = 2;
            Geographical_2 = 4;
            for (j = sample_1; j < number_item2; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    a[Number_of_cycles_a][i].element = data[j][i];
                    a[Number_of_cycles_a][i].id = i;
                }
                Number_of_cycles_a++;
            }
            for (j = number_item3; j < number_item4; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    b[Number_of_cycles_b][i].element = data[j][i];
                    b[Number_of_cycles_b][i].id = i;
                }
                Number_of_cycles_b++;
            }
            break;
        case 5:
            Geographical_1 = 3;
            Geographical_2 = 4;
            for (j = number_item2; j < number_item3; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    a[Number_of_cycles_a][i].element = data[j][i];
                    a[Number_of_cycles_a][i].id = i;
                }
                Number_of_cycles_a++;
            }
            for (j = number_item3; j < number_item4; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    b[Number_of_cycles_b][i].element = data[j][i];
                    b[Number_of_cycles_b][i].id = i;
                }
                Number_of_cycles_b++;
            }
            break;
        case 6:
            Geographical_1 = 1;
            Geographical_2 = 5;
            for (j = 0; j < sample_1; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    a[Number_of_cycles_a][i].element = data[j][i];
                    a[Number_of_cycles_a][i].id = i;
                }
                Number_of_cycles_a++;
            }
            for (j = number_item4; j < number_item5; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    b[Number_of_cycles_b][i].element = data[j][i];
                    b[Number_of_cycles_b][i].id = i;
                }
                Number_of_cycles_b++;
            }
            break;
        case 7:
            Geographical_1 = 2;
            Geographical_2 = 5;
            for (j = sample_1; j <number_item2 ; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    a[Number_of_cycles_a][i].element = data[j][i];
                    a[Number_of_cycles_a][i].id = i;
                }
                Number_of_cycles_a++;
            }
            for (j = number_item4; j < number_item5; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    b[Number_of_cycles_b][i].element = data[j][i];
                    b[Number_of_cycles_b][i].id = i;
                }
                Number_of_cycles_b++;
            }
            break;
        case 8:
            Geographical_1 = 3;
            Geographical_2 = 5;
            for (j = number_item2; j <number_item3 ; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    a[Number_of_cycles_a][i].element = data[j][i];
                    a[Number_of_cycles_a][i].id = i;
                }
                Number_of_cycles_a++;
            }
            for (j = number_item4; j < number_item5; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    b[Number_of_cycles_b][i].element = data[j][i];
                    b[Number_of_cycles_b][i].id = i;
                }
                Number_of_cycles_b++;
            }
            break;
        case 9:
            Geographical_1 = 4;
            Geographical_2 = 5;
            for (j = number_item3; j <number_item4 ; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    a[Number_of_cycles_a][i].element = data[j][i];
                    a[Number_of_cycles_a][i].id = i;
                }
                Number_of_cycles_a++;
            }
            for (j = number_item4; j < number_item5; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    b[Number_of_cycles_b][i].element = data[j][i];
                    b[Number_of_cycles_b][i].id = i;
                }
                Number_of_cycles_b++;
            }
            break;
        case 10:
            Geographical_1 = 1;
            Geographical_2 = 6;
            for (j = 0; j <sample_1 ; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    a[Number_of_cycles_a][i].element = data[j][i];
                    a[Number_of_cycles_a][i].id = i;
                }
                Number_of_cycles_a++;
            }
            for (j = number_item5; j < number_item6; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    b[Number_of_cycles_b][i].element = data[j][i];
                    b[Number_of_cycles_b][i].id = i;
                }
                Number_of_cycles_b++;
            }
            break;
        case 11:
            Geographical_1 = 2;
            Geographical_2 = 6;
            for (j = sample_1; j <number_item2 ; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    a[Number_of_cycles_a][i].element = data[j][i];
                    a[Number_of_cycles_a][i].id = i;
                }
                Number_of_cycles_a++;
            }
            for (j = number_item5; j < number_item6; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    b[Number_of_cycles_b][i].element = data[j][i];
                    b[Number_of_cycles_b][i].id = i;
                }
                Number_of_cycles_b++;
            }
            break;
        case 12:
            Geographical_1 = 3;
            Geographical_2 = 6;
            for (j =number_item2; j <number_item3 ; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    a[Number_of_cycles_a][i].element = data[j][i];
                    a[Number_of_cycles_a][i].id = i;
                }
                Number_of_cycles_a++;
            }
            for (j = number_item5; j < number_item6; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    b[Number_of_cycles_b][i].element = data[j][i];
                    b[Number_of_cycles_b][i].id = i;
                }
                Number_of_cycles_b++;
            }
            break;
        case 13:
            Geographical_1 = 4;
            Geographical_2 = 6;
            for (j =number_item3; j <number_item4 ; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    a[Number_of_cycles_a][i].element = data[j][i];
                    a[Number_of_cycles_a][i].id = i;
                }
                Number_of_cycles_a++;
            }
            for (j = number_item5; j < number_item6; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    b[Number_of_cycles_b][i].element = data[j][i];
                    b[Number_of_cycles_b][i].id = i;
                }
                Number_of_cycles_b++;
            }
            break;
        case 14:
            Geographical_1 = 5;
            Geographical_2 = 6;
            for (j =number_item4; j <number_item5 ; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    a[Number_of_cycles_a][i].element = data[j][i];
                    a[Number_of_cycles_a][i].id = i;
                }
                Number_of_cycles_a++;
            }
            for (j = number_item5; j < number_item6; j++)
            {
                for (i = 0; i < EN; i++)
                {
                    b[Number_of_cycles_b][i].element = data[j][i];
                    b[Number_of_cycles_b][i].id = i;
                }
                Number_of_cycles_b++;
            }
            break;
        }
        sort3(a, b, oneBand, Number_of_cycles_a, Number_of_cycles_b,EN);
        for (j = 0; j < Number_of_cycles_a; j++)
        {
            for (k = 0; k < Number_of_cycles_b; k++)
            {
                judge1 = sort1(a[j], b[k], reorder, EN);
                zero_extend_Sum_Number++;
                for (int p = 0; p < EN; p++)
                {
                    x[p] = a[j][p];
                    y[p] = b[k][p];
                }
                //11是无法分辨，10是可以分辨
                sort(x, y, EN);//对a地物进行排序和对b地物进行排序
                sort2(x, y, first_extend, EN);//调用第二步分离
                frist_extend_Sum_Number++;
            }
        }
        display(reorder, first_extend, zero_extend_Sum_Number, frist_extend_Sum_Number
                ,characteristics, Geographical_1, Geographical_2, oneBand,EN);
        {
            //数组清空函数
            Number_of_cycles_a=Number_of_cycles_b=0;
            zero_extend_Sum_Number = frist_extend_Sum_Number= 0;
            memset(reorder, 0, sizeof(reorder));
            memset(first_extend, 0, sizeof(first_extend));
        }
    }

    removeCharacteristics(characteristics);

    int Number_of_samples[6] = { 0 };
    Number_of_samples[0] = sample_1;
    Number_of_samples[1] = sample_2;
    Number_of_samples[2] = sample_3;
    Number_of_samples[3] = sample_4;
    Number_of_samples[4] = sample_5;
    Number_of_samples[5] = sample_6;
    divide(characteristics, data, sumSample, Number_of_samples,root);//分离函数(递归)
    QDateTime Systemtime = QDateTime::currentDateTime();//获取系统现在的时间
    QString str = Systemtime.toString("yyyy_MM_dd_hh_mm_ss"); //设置显示格式
    QString fileStr=ruleOwnName;
    fileStr=fileStr+"Rule"+str;
    findAllPath(root,quee,0,fileStr);//将所有规则写入文件
    emit sortComplete(fileStr);//地物分类完成
    DeleteTree(root->left);
    DeleteTree(root->right);
    QSqlQuery query;
    QString sqlInsertStr=QString("insert into RemoteSensingRule values('%1','%2')").arg(fileStr).arg(ruleOwnName);
    query.exec(sqlInsertStr);
    delete[] Data;
    Data=NULL;
    free(data);
    return;
}
//波段扩展模块
void classify::extend(Feature characteristics[], double data[][15])
{
    bool boolean = false;
    int i = 0, j = 0;
    Feature temp;
    temp = characteristics[0];
    for (i = 0; i < 100; i++)
    {
        if (characteristics[i].band[1] != -1&&characteristics[i].band[0]!=-1)
            if (characteristics[i].percent>temp.percent)
            {
                temp = characteristics[i];
            }
    }
    qInfo()<<"i"<<temp.band[0]<<"j"<<temp.band[1]<<"k"<<temp.band[2];
    if (temp.band[1]!=-1&&temp.band[2] == -1) //两波段的地物扩展
    {
        if (boolean == false)
        {
            for (j = 0; j < sumSample; j++)
            {
                data[j][N] = data[j][temp.band[0]] - data[j][temp.band[1]];
            }
            qInfo()<<"use"<<temp.band[0]<< temp.band[1]<<"extend";
            boolean = true;
        }
    }
    else if(temp.band[0]!=-1&&temp.band[1]!=-1&&temp.band[2]!= -1)    //三波段的地物进行扩展
    {
        if (boolean == false)
        {
            for (j = 0; j < sumSample; j++)
            {
                data[j][N] = (data[j][temp.band[0]] + data[j][temp.band[2]]) / 2 - data[j][temp.band[1]];
            }
            qInfo()<<"use("<<temp.band[0]<<"+"<< temp.band[2]<<")/2-"<<temp.band[1]<<"extend";
            boolean = true;
        }
    }
    if (boolean)
    {
        qInfo()<<"all simples extend a band";
    }
}











