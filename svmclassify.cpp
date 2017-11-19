#include "svmclassify.h"

svmclassify::svmclassify(QObject *parent) : QObject(parent)
{
    main();
}

svm_parameter param;

void init_param() {
    param.svm_type = C_SVC;//支持向量机类型
    param.kernel_type = RBF;//核函数类型
    param.degree = 4;       //波段个数+1
    param.gamma = 0;
    param.coef0 = 0;

    param.cache_size = 500; //缓存块大小(MB)
    param.eps = 1e-3;// stopping criteria
    param.C = 100;//for C_SVC, EPSILON_SVR and NU_SVR ，惩罚因子越大训练的模型越那个…,当然耗的时间越多
    param.nr_weight = 0;//for C_SVC 权重的数目权重的数目,目前在实例代码中只有两个值，一个是默认0，另外一个是svm_binary_svc_probability函数中使用数值2。
    param.weight_label = NULL; //for C_SVC 权重，元素个数由nr_weight 决定
    param.weight = NULL;//for C_SVC
    param.nu = 0.5;// for NU_SVC, ONE_CLASS, and NU_SVR
    param.p = 0.1; //for EPSILON_SVR
    param.shrinking = 1;//指明训练过程是否使用压缩
    param.probability = 0; //指明是否要做概率估计
}
void svmclassify::main()
{
    int band=3;
    int num=7000;
    double d=0;
    unsigned char **geoTemp=NULL;
    unsigned char **geo=NULL;

    geoTemp=new unsigned char*[8000];
    geo=new unsigned char*[32000];

    for(int i=0;i<8000;i++)
    {
        geoTemp[i]=new unsigned char[band+1];
    }
    for(int i=0;i<32000;i++)
    {
        geo[i]=new unsigned char[band+1];
    }
    FILE *fpra=NULL;
    int count=0;
    for(int i=0;i<4;i++)
    {
        switch (i) {
        case 0:
            fpra = fopen("C:\\Users\\25235\\Desktop\\red.txt", "r");
            break;
        case 1:
            fpra = fopen("C:\\Users\\25235\\Desktop\\yellow.txt", "r");
            break;
        case 2:
            fpra = fopen("C:\\Users\\25235\\Desktop\\blue.txt", "r");
            break;
        case 3:
            fpra = fopen("C:\\Users\\25235\\Desktop\\green.txt", "r");
            break;
        }
        if(!fpra)
        {
            qFatal("Error");
            return;
        }

        for(int h=0;h<7000;h++)
        {
            for(int w=0;w<band;w++)
            {
                fscanf(fpra, "%hhu", &geoTemp[h][w]);
                geo[count][w]=geoTemp[h][w];
            }
            count++;
        }
        fclose(fpra);
    }
    int temp=0;
    temp=geo[0][0];
    for(int i=0;i<4*num;i++)
    {
        for(int j=0;j<band;j++)
        {
            if(temp<geo[i][j])
            {
                temp=geo[i][j];
            }
        }
    }

    init_param();
    svm_problem prob;

    prob.l=num*4;//样本数量

    if (param.gamma == 0) param.gamma = 0.5;
    svm_node *x_space = new svm_node[4*prob.l];//样本特征存储空间
    prob.y=new double[prob.l];//标签
    prob.x = new svm_node *[prob.l]; //每一个X指向一个样本
    for(int i=0;i<num*4;i++)
    {
        x_space[4*i].index=1;
        x_space[4*i].value=geo[i][0];
        x_space[4*i+1].index=2;
        x_space[4*i+1].value=geo[i][1];
        x_space[4*i+2].index=3;
        x_space[4*i+2].value=geo[i][2];
        x_space[4*i+3].index=-1;
        x_space[4*i+3].value=0;
        prob.x[i]=&x_space[4*i];
        if(i<num)
            prob.y[i]=1;
        else if(num<=i&&i<2*num)
            prob.y[i]=2;
        else if(2*num<=i&&i<3*num)
            prob.y[i]=3;
        else if(3*num<=i)
            prob.y[i]=4;
    }

    //    for(int i=0;i<10;i++){
    //        qDebug()<<x_space[i].index;
    //        qDebug()<<x_space[i].value;
    //    }

    svm_model *model = svm_train(&prob, &param);
    svm_node xnode[16];
    xnode[0].index = 1;
    xnode[0].value = 8;
    xnode[1].index = 2;
    xnode[1].value = 10;
    xnode[2].index = 3;
    xnode[2].value = 93;
    xnode[3].index = -1;
    d = svm_predict(model, xnode);
    //red
    //    73	101	125
    //    74	96	96
    //    77	97	108
    qDebug()<<d;
    svm_node xnode1[4];
    xnode1[0].index = 1;
    xnode1[0].value = 73;
    xnode1[1].index = 2;
    xnode1[1].value = 101;
    xnode1[2].index = 3;
    xnode1[2].value = 125;
    xnode1[3].index = -1;
    d = svm_predict(model, xnode1);
    qDebug()<<d;
    //yellow
    //107	126	134
    //106	126	132
    //97	122	136
    //94	120	135
    //95	119	133
    //95	119	135
    //107	125	128
    //102	122	136
    //117	129	119
    //121	126	116
    svm_node xnode2[4];
    xnode2[0].index = 1;
    xnode2[0].value = 107;
    xnode2[1].index = 2;
    xnode2[1].value = 126;
    xnode2[2].index = 3;
    xnode2[2].value = 134;
    xnode2[3].index = -1;
    d = svm_predict(model, xnode2);
    qDebug()<<d;
    //blue
    //8	10	93
    //10	9	90
    //7	4	106
    //6	2	107
    svm_node xnode3[4];
    xnode3[0].index = 1;
    xnode3[0].value = 8;
    xnode3[1].index = 2;
    xnode3[1].value = 10;
    xnode3[2].index = 3;
    xnode3[2].value = 93;
    xnode3[3].index = -1;
    d = svm_predict(model, xnode3);
    qDebug()<<d;
    //green
    //246	251	166
    //245	250	165
    //243	248	165
    //243	248	158
    //244	248	154
    //244	248	156
    //245	248	162
    //243	247	171
    svm_node xnode4[4];
    xnode4[0].index = 1;
    xnode4[0].value = 246;
    xnode4[1].index = 2;
    xnode4[1].value = 251;
    xnode4[2].index = 3;
    xnode4[2].value = 166;
    xnode4[3].index = -1;
    d = svm_predict(model, xnode4);
    qDebug()<<d;


    delete[] x_space;
    delete[] prob.x;
    delete[] prob.y;

    delete[] geoTemp;
    delete[] geo;

}
