#include "zonedeal.h"
zonedeal::zonedeal()
{
    //    click();
}
void zonedeal::click()
{
    //    输入一个图片地址
    //    C:\\Users\\25235\\Desktop\\file6(2).tif
    QImage image;
    image.load("C:\\Users\\25235\\Desktop\\file");
    if(!image.isNull())
    {
        main(image);
    }
    else
    {
        qDebug()<<QStringLiteral("文件为空");
    }
    //    unsigned short int testImage[32]={
    //        1,1,2,1,2,1,2,1,
    //        1,1,2,1,2,1,2,1,
    //        1,1,2,1,2,1,2,1,
    //        1,1,2,1,2,1,2,1,
    //    };
    //    unsigned short int testImage[32]={
    //        1,1,2,1,2,1,2,1,
    //        1,1,2,1,4,4,4,1,
    //        3,3,3,1,2,1,2,1,
    //        1,1,2,1,2,1,2,1,
    //    };
    //    ImageArray imageArray;
    //    imageArray.colorTh=testImage;
    //    //imageArray.id=new unsigned short int [32];
    //    test(imageArray,8,4);
}
void zonedeal::main(QImage image)
{
    int R[7]={ 255,  255,    0,   0,   139,0};
    int G[7]={   0,  255,    0,   255, 0 ,0};
    int B[7]={   0  ,  0,  255,   0,   139   ,0};
    //红 黄 蓝 绿 紫 黑
    //s_color *landColor=new s_color[6];
    int Samples=image.width();
    int Lines=image.height();
    qDebug()<<QStringLiteral("图像宽度：")<<Samples;
    qDebug()<<QStringLiteral("图像高度：")<<Lines;
    bool flag=false;

    unsigned short *testImage=new unsigned short[Samples*Lines];

    //将图片转化为数组
    for(int h=0;h<Lines;h++)
    {
        for(int w=0;w<Samples;w++)
        {
            flag=false;
            for(int i=0;i<6;i++)
            {
                if(QColor(image.pixel(w,h)).red()==R[i]&&
                        QColor(image.pixel(w,h)).green()==G[i]&&
                        QColor(image.pixel(w,h)).blue()==B[i]
                        )
                {
                    testImage[h*Samples+w]=i;
                    flag=true;
                }
                if(!flag)
                {
                    testImage[h*Samples+w]=0;
                }
            }

        }
    }
    ImageArray imageArray;
    imageArray.colorTh=testImage;
    test(imageArray,Samples,Lines);
    //获得图像地物数组

    delete []testImage;
    testImage=NULL;
}
void zonedeal::test(ImageArray testImage,int Samples,int Lines)
{
    int R[7]={ 255,  255,    0,   0,   139,0};
    int G[7]={   0,  255,    0,   255, 0 ,0};
    int B[7]={   0  ,  0,  255,   0,   139   ,0};
    testImage.id=new unsigned short[Samples*Lines];
    int NodeNumber=0;//节点数量
    int deleteThresould=5000;//最小区域点数量阈值
    int distanceThresould=70;//距离阈值
    float adjIntensity=0.3;//邻接强度阈值
    int geoTh=2;//要找成排的地物
    bool colorChangeFlag=false;//颜色更改flag


    AreaNodeInfo *everyNum=countEveryNumber(testImage.colorTh,Samples,Lines);

    qDebug()<<QStringLiteral("第一次统计完成");

    QImage image(Samples, Lines, QImage::Format_RGB32);
    //    点数量小于区域大小阈值的认为是噪点，将其变成相邻区域颜色
    for(int i=0;everyNum[i].number>0;i++)
    {
        if(everyNum[i].number<deleteThresould)
        {
            unsigned short color=adjancentColor(testImage,Samples,Lines,everyNum[i]);
            testImage.colorTh=newChangeColor(testImage.colorTh,Samples,Lines,everyNum[i],color,deleteThresould);
            colorChangeFlag=true;
        }
    }


    delete everyNum;
    everyNum=NULL;

    if(colorChangeFlag)
    {
        for(int h=0;h<Lines;h++)
        {
            for(int w=0;w<Samples;w++)
            {
                QRgb value=qRgb(R[testImage.colorTh[h*Samples+w]],G[testImage.colorTh[h*Samples+w]],B[testImage.colorTh[h*Samples+w]]);
                image.setPixel(w,h,value);
            }
        }
        //sendImageToUi(image,2);
        image.save("C:\\Users\\25235\\Desktop\\new.tif");
        //image.smoothScaled(2,2);

        //对变色后的区域重新统计每个区域的信息
        everyNum=countEveryNumber(testImage.colorTh,Samples,Lines);
    }


    //给所有区域分配不同id
    for(int i=0;everyNum[i].number!=0;i++)
    {
        NodeNumber++;
        //qDebug()<<QStringLiteral("入口坐标")<<everyNum[i].startX<<everyNum[i].startY<<<<everyNum[i].number;
        everyNum[i].ID=i;//给当前节点分配ID
        Area a=pointIterator(testImage.colorTh,Samples,Lines,everyNum[i]);
        everyNum[i].cycle=calAreaCycle(testImage.colorTh,Samples,Lines,a,everyNum[i]);
        qDebug()<<QStringLiteral("区域")<<i<<QStringLiteral("的周长是")<<everyNum[i].cycle;
        for(int j=0;j<a.number;j++)
        {
            testImage.id[a.p[j].y*Samples+a.p[j].x]=i;//给所有地物标一个唯一id，给图像中的当前区域赋值相同ID
        }
        delete[] a.p;
        a.p=NULL;
    }
    for(int i=0;everyNum[i].number!=0;i++)//计算所有区域的邻接地物以及邻接数量
    {
        Area a=pointIterator(testImage.colorTh,Samples,Lines,everyNum[i]);
        everyNum[i]=rightOrDownLink(testImage,Samples,Lines,everyNum[i],a);//经过这一步，右邻接区域，以及邻接数量全部求出
        //        everyNum[i].rAdjIntensity=new float[everyNum[i].rLinkNum];
        //        everyNum[i].dAdjIntensity=new float[everyNum[i].dLinkNum];
        //        for(int j=0;j<everyNum[i].rLinkNum;j++)//计算右邻接强度
        //        {
        //            everyNum[i].rAdjIntensity[j]=everyNum[i].rAdjacentNum[j]*1.0/everyNum[i].cycle;
        //            qDebug()<<QStringLiteral("右邻接强度是")<<everyNum[i].rAdjIntensity[j];
        //        }
        //        for(int j=0;j<everyNum[i].dLinkNum;j++)//计算下邻接强度
        //        {
        //            everyNum[i].dAdjIntensity[j]=everyNum[i].dAdjacentNum[j]*1.0/everyNum[i].cycle;
        //            qDebug()<<QStringLiteral("下邻接强度是")<<everyNum[i].dAdjIntensity[j];
        //        }
        delete[] a.p;
        a.p=NULL;
    }
    ////////////////////////////////////////////////////////////////////////
    //    //求最短距离
    //    //距离表
    //    Graph distanceGraph;
    //    distanceGraph.headChain=new HeadNode[NodeNumber];
    //    distanceGraph.vexNum=NodeNumber;
    //    //距离邻接表初始化
    //    for(int i=0;i<NodeNumber;i++)
    //    {
    //        distanceGraph.headChain[i].ID=i;//节点id
    //        distanceGraph.headChain[i].colorTh=everyNum[i].colorTh;//节点颜色
    //        distanceGraph.headChain[i].isVisited=false;
    //        distanceGraph.headChain[i].firstNode=NULL;
    //    }

    //    LinkNetNode *dNode,*dTail=NULL;
    //    for(int i=0;i<NodeNumber;i++)
    //    {
    //        for(int j=0;j<NodeNumber;j++)
    //        {
    //            if(i!=j)
    //                if(distanceGraph.headChain[i].colorTh==distanceGraph.headChain[j].colorTh)//两节点是同一地物
    //                {
    //                    point p1,p2;
    //                    p1.x=everyNum[i].startX;
    //                    p1.y=everyNum[i].startY;
    //                    p2.x=everyNum[j].startX;
    //                    p2.y=everyNum[j].startY;
    //                    if(distance(p1,p2)<=maxDistance)
    //                    {
    //                        qDebug()<<distance(p1,p2);
    //                        dNode=new LinkNetNode;
    //                        dNode->ID=distanceGraph.headChain[j].ID;
    //                        dNode->next=NULL;
    //                        if(distanceGraph.headChain[i].firstNode==NULL)
    //                        {
    //                            distanceGraph.headChain[i].firstNode=dNode;
    //                            dTail=dNode;
    //                        }
    //                        else
    //                        {
    //                            dTail->next=dNode;
    //                            dTail=dNode;
    //                        }
    //                    }

    //                }
    //        }
    //    }



    /////////////////////////////////////////////////////////////////////////////////////////

    //邻接表存储
    Graph adjancentGraph;
    adjancentGraph.headChain=new HeadNode[NodeNumber];
    adjancentGraph.vexNum=NodeNumber;
    //邻接表初始化
    for(int i=0;i<NodeNumber;i++)
    {
        adjancentGraph.headChain[i].ID=i;//节点id
        adjancentGraph.headChain[i].isVisited=false;
        adjancentGraph.headChain[i].firstNode=NULL;
    }
    LinkNetNode *node,*tail=NULL;
    for(int i=0;i<NodeNumber;i++)
    {
        for(int j=0;j<everyNum[i].rLinkNum;j++)
        {
            node=new LinkNetNode;
            node->ID=everyNum[i].rID[j];
            node->nextDirection=1;
            node->next=NULL;

            qDebug()<<QStringLiteral("当前节点")<<adjancentGraph.headChain[i].ID<<QStringLiteral("的右邻接是")<<node->ID;

            if(adjancentGraph.headChain[i].firstNode==NULL)
            {
                adjancentGraph.headChain[i].firstNode=node;
                tail=node;
            }
            else
            {
                tail->next=node;
                tail=node;
            }

        }
        for(int j=0;j<everyNum[i].dLinkNum;j++)
        {
            node=new LinkNetNode;
            node->ID=everyNum[i].dID[j];
            node->nextDirection=2;
            node->next=NULL;

            qDebug()<<QStringLiteral("当前节点")<<adjancentGraph.headChain[i].ID<<QStringLiteral("的下邻接是")<<node->ID;
            if(adjancentGraph.headChain[i].firstNode==NULL)
            {
                adjancentGraph.headChain[i].firstNode=node;
                tail=node;
            }
            else
            {
                tail->next=node;
                tail=node;
            }

        }

    }
    //////////////////////////////////////////////////////////////

    //    //将右邻接和下邻接两个网关系变成一个网关系
    //    LinkNetNode* net=new LinkNetNode[NodeNumber];
    //    for(int i=0;everyNum[i].number!=0;i++)
    //    {
    //        net[i].id=everyNum[i].ID;
    //        net[i].colorTh=everyNum[i].colorTh;
    //        net[i].adjancentNum=everyNum[i].rLinkNum+everyNum[i].dLinkNum;
    //        net[i].adjancentID=new unsigned int[net[i].adjancentNum];
    //        net[i].direction=new unsigned short[net[i].adjancentNum];;
    //        net[i].adjancentColorTh=new unsigned short[net[i].adjancentNum];
    //        int temp=0;
    //        for(int j=0;j<everyNum[i].rLinkNum;j++,temp++)//求出右邻接地物的id为everyNum[i].rID[j]
    //        {
    //            net[i].adjancentID[j]=everyNum[i].rID[j];
    //            net[i].direction[j]=1;
    //            net[i].adjancentColorTh[j]=everyNum[i].rightColorTh[j];
    //        }
    //        for(int j=0;j<everyNum[i].dLinkNum;j++,temp++)
    //        {
    //            net[i].adjancentID[temp]=everyNum[i].dID[j];
    //            net[i].direction[temp]=2;
    //            net[i].adjancentColorTh[temp]=everyNum[i].downColorTh[j];
    //        }

    //    }
    //    for(int i=0;i<NodeNumber;i++)
    //    {
    //        for(int j=0;j<net[i].adjancentNum;j++)
    //        {
    //            qDebug()<<QStringLiteral("当前ID")<<net[i].id<<QStringLiteral("的邻接ID是")<<net[i].adjancentID[j];
    //        }
    //    }
    //    //找出所有两步可达的地物相同的区域ToDo:将同一区域去除
    //    int *rNet=new  int[Samples*Lines];
    //    int rNetCount=0;
    //    int *dNet=new  int[Samples*Lines];
    //    int dNetCount=0;

    //    for(int i=0;everyNum[i].number!=0;i++)
    //    {
    //        for(int j=0;j<everyNum[i].rLinkNum;j++)//求出右邻接地物的id为everyNum[i].rID[j]
    //        {

    //            for(int k=0;k<everyNum[everyNum[i].rID[j]].rLinkNum;k++)//在第everyNum[i].rID[j]个区域的右邻接区域中
    //            {
    //                if(everyNum[everyNum[i].rID[j]].rightColorTh[k]==everyNum[i].colorTh)//如果二邻接区域的地物与当前区域i的地物相同，则认为在一排
    //                {
    //                    if(distancesConfine(everyNum[i].startX,everyNum[i].startY,
    //                                        everyNum[everyNum[i].rID[j]].startX,
    //                                        everyNum[everyNum[i].rID[j]].startY)&&
    //                            everyNum[i].ID!=everyNum[everyNum[i].rID[j]].rID[k])
    //                    {
    //                        qDebug()<<QStringLiteral("区域")<<everyNum[i].ID<<QStringLiteral("与区域")
    //                               <<everyNum[everyNum[i].rID[j]].rID[k]<<QStringLiteral("是二阶左右邻居");

    //                        rNet[rNetCount]=everyNum[i].ID;
    //                        rNetCount++;
    //                        rNet[rNetCount]=everyNum[everyNum[i].rID[j]].rID[k];
    //                        rNetCount++;
    //                    }
    //                }
    //            }
    //        }
    //        for(int j=0;j<everyNum[i].dLinkNum;j++)//求出右邻接地物的id为everyNum[i].rID[j]
    //        {
    //            for(int k=0;k<everyNum[everyNum[i].dID[j]].dLinkNum;k++)//在第everyNum[i].rID[j]个区域的右邻接区域中
    //            {
    //                if(everyNum[everyNum[i].dID[j]].downColorTh[k]==everyNum[i].colorTh)//如果二邻接区域的地物与当前区域i的地物相同，则认为在一排
    //                {
    //                    if(distancesConfine(everyNum[i].startX,everyNum[i].startY,
    //                                        everyNum[everyNum[i].rID[j]].startX,
    //                                        everyNum[everyNum[i].rID[j]].startY)&&
    //                            everyNum[i].ID!=everyNum[everyNum[i].dID[j]].dID[k])
    //                    {
    //                        qDebug()<<QStringLiteral("区域")<<everyNum[i].ID<<QStringLiteral("与区域")
    //                               <<everyNum[everyNum[i].dID[j]].dID[k]<<QStringLiteral("是二阶上下邻居");
    //                        dNet[dNetCount]=everyNum[i].ID;
    //                        dNetCount++;
    //                        dNet[dNetCount]=everyNum[everyNum[i].dID[j]].dID[k];
    //                        dNetCount++;
    //                    }
    //                }
    //            }
    //        }
    //    }

    //    //    ToDo:下一步找四步可达
    //    for(int i=0;i<rNetCount;i++)
    //    {
    //        if(i%2==0)
    //        {
    //            //qDebug()<<rNet[i]<<rNet[i+1]<<"\n";
    //            for(int j=0;j<rNetCount;j++)
    //            {
    //                if(j%2==0&&i!=j)
    //                    if(rNet[i+1]==rNet[j])
    //                    {
    //                        qDebug()<<QStringLiteral("区域")<<rNet[i]<<QStringLiteral("与区域")<<rNet[j+1]
    //                               <<QStringLiteral("是四阶上下邻居");
    //                    }
    //            }
    //        }
    //    }
    //    for(int i=0;i<dNetCount;i++)
    //    {
    //        if(i%2==0)
    //        {
    //            //qDebug()<<dNet[i]<<dNet[i+1]<<"\n";
    //            for(int j=0;j<dNetCount;j++)
    //            {
    //                if(j%2==0&&i!=j)
    //                    if(dNet[i+1]==dNet[j])
    //                    {
    //                        qDebug()<<QStringLiteral("区域")<<dNet[i]<<QStringLiteral("与区域")<<dNet[j+1]
    //                               <<QStringLiteral("是四阶上下邻居");
    //                    }

    //            }

    //        }
    //    }
    //    deepIterater(&myGraph,Samples,Lines);
    //    findPatch(&myGraph,Samples,Lines,5,4);
    //    deep(&myGraph);
    //    findEveryPatch(&myGraph,5,4);
    //    deepIterater(&distanceGraph);

    //    unsigned short *direction=new unsigned short[NodeNumber];
    //    direction[0]=1;
    //    direction[1]=1;
    //    direction[2]=1;
    //    direction[3]=1;
    //    getRoad(&adjancentGraph,0,9,direction);


    AdjancentChanin result1=sameColorProcess(&adjancentGraph,testImage,Samples
                                             ,Lines,geoTh,everyNum,distanceThresould,adjIntensity);
    qDebug()<<"1complete";

    AdjancentChanin result2=processResult(everyNum,NodeNumber,geoTh);
    qDebug()<<"2complete";
    for(int i=0;i<result1.number;i++)
    {
        for(int j=0;j<result2.number;j++)
        {
            if(result1.data[i].ID==result2.data[j].ID&&
                    result1.data[i].linkID==result2.data[j].linkID)
            {
                //                qDebug()<<QStringLiteral("成排区域")<<result1.data[i].ID<<result1.data[i].linkID;
                testImage.colorTh=newChangeColor(testImage.colorTh,Samples,Lines,everyNum[result1.data[i].ID],3,10000000);
                testImage.colorTh=newChangeColor(testImage.colorTh,Samples,Lines,everyNum[result1.data[i].linkID],3,10000000);
            }
        }
    }

    for(int h=0;h<Lines;h++)
    {
        for(int w=0;w<Samples;w++)
        {
            QRgb value=qRgb(R[testImage.colorTh[h*Samples+w]],G[testImage.colorTh[h*Samples+w]],B[testImage.colorTh[h*Samples+w]]);
            image.setPixel(w,h,value);
        }
    }
    image.save("C:\\Users\\25235\\Desktop\\new.tif");
    //    delete[] rNet;
    //    rNet=NULL;
    //    delete[] dNet;
    //    dNet=NULL;
    qDebug()<<"3complete";

    for(int i=0;i<NodeNumber;i++)
    {
        delete[] everyNum[i].rID;
        delete[] everyNum[i].dID;
        delete[] everyNum[i].rAdjacentNum;
        delete[] everyNum[i].dAdjacentNum;
        delete[] everyNum[i].downColorTh;
        delete[] everyNum[i].rightColorTh;

        everyNum[i].rID=NULL;
        everyNum[i].dID=NULL;
        everyNum[i].rAdjacentNum=NULL;
        everyNum[i].dAdjacentNum=NULL;
        everyNum[i].downColorTh=NULL;
        everyNum[i].rightColorTh=NULL;
//        delete[] everyNum[i].rAdjIntensity;
//        delete[] everyNum[i].dAdjIntensity;
//        everyNum[i].rAdjIntensity=NULL;
//        everyNum[i].dAdjIntensity=NULL;
    }
    qDebug()<<"4complete";

    delete[] everyNum;
    everyNum=NULL;

    for(int i=0;i<NodeNumber;i++)
    {
        delete[] adjancentGraph.headChain[i].firstNode;
        adjancentGraph.headChain[i].firstNode=NULL;
    }
    delete[] adjancentGraph.headChain;
    adjancentGraph.headChain=NULL;

    delete[] result1.data;
    result1.data=NULL;

    delete[] result2.data;
    result2.data=NULL;

    delete[] testImage.id;
    testImage.id=NULL;


    qDebug()<<"5complete";
}

/**
 * @brief zonedeal::processResult 处理地物信息结果
 * @param areaInfo
 */
AdjancentChanin zonedeal::processResult(AreaNodeInfo *areaInfo,int nodeNum,unsigned int colorTh)
{

    qDebug()<<QStringLiteral("节点数量是：")<<nodeNum;
    AdjancentNode *relation1=new AdjancentNode[nodeNum*10];
    int count1=0;
    for(int i=0;i<nodeNum;i++)
    {

        for(int j=0;j<areaInfo[i].rLinkNum;j++)
        {
            relation1[count1].ID=areaInfo[i].ID;
            relation1[count1].linkID=areaInfo[i].rID[j];
            count1++;
        }
        for(int j=0;j<areaInfo[i].dLinkNum;j++)
        {
            relation1[count1].ID=areaInfo[i].ID;
            relation1[count1].linkID=areaInfo[i].dID[j];
            count1++;
        }

    }
    //    for(int i=0;i<count1;i++)
    //    {
    //        qDebug()<<relation1[i].ID<<relation1[i].linkID;
    //    }
    qDebug()<<"\n";

    AdjancentChanin realation2;
    realation2.data=new AdjancentNode[count1*count1/5];
    //两步可达路径R^2
    //    AdjancentNode *relation2=new AdjancentNode[count1*count1];
    int count2=0;
    for(int i=0;i<count1;i++)
    {
        for(int j=0;j<count1;j++)
        {
            if(relation1[j].ID==relation1[i].linkID)//关系复合 如果<0,1><1,2>则<0,2>
            {
                //选择满足条件的地物
                if(areaInfo[relation1[i].ID].colorTh==areaInfo[relation1[j].linkID].colorTh&&
                        areaInfo[relation1[j].linkID].colorTh==colorTh)
                {
                    realation2.data[count2].ID=relation1[i].ID;
                    realation2.data[count2].linkID=relation1[j].linkID;
                    //realation2.data[count2].path.append(QString::number(relation1[i].ID));
                    //realation2.data[count2].path.append("->");
                    //realation2.data[count2].path.append(QString::number(relation1[i].linkID));
                    //realation2.data[count2].path.append("->");
                    //realation2.data[count2].path.append(QString::number(relation1[j].linkID));
                    //qDebug()<< realation2.data[count2].path;
                    count2++;
                }
            }
        }
    }
    realation2.number=count2;
    return realation2;

    //    //n 步可达路径
    //    AdjancentNode *relation3=new AdjancentNode[count1*count1*count1];
    //    int fCount=count1,sCount=count2;
    //    bool flag;
    //    do
    //    {

    //        flag=false;
    //        int tCount=0;
    //        for(int i=0;i<sCount;i++)
    //        {
    //            for(int j=0;j<fCount;j++)
    //            {
    //                if(relation1[j].ID==relation2[i].linkID)
    //                {
    //                    relation3[tCount].ID=relation2[i].ID;
    //                    relation3[tCount].linkID=relation1[j].linkID;
    //                    relation3[tCount].path=relation2[i].path;
    //                    relation3[tCount].path.append("->");
    //                    relation3[tCount].path.append(QString::number(relation1[j].linkID));
    //                    qDebug()<<relation3[tCount].path;
    //                    tCount++;
    //                    flag=true;
    //                }
    //            }
    //        }
    //        qDebug()<<"\n";
    //        relation2=relation3;
    //        sCount=tCount;
    //    }while(flag);


    //    qDebug()<<"\n";
    //    //三步可达路径R^3=R^2*R
    //    AdjancentNode *relation3=new AdjancentNode[count1*count2];
    //    int count3=0;
    //    for(int j=0;j<count2;j++)
    //    {   for(int i=0;i<count1;i++)
    //        {
    //            if(relation1[i].ID==relation2[j].linkID)
    //            {
    //                relation3[count3].ID=relation2[j].ID;
    //                relation3[count3].linkID=relation1[i].linkID;
    //                relation3[count3].path.append(relation2[j].path);
    //                relation3[count3].path.append("->");
    //                relation3[count3].path.append(QString::number(relation1[i].linkID));
    //                qDebug()<<relation3[count3].path;
    //                count3++;

    //            }
    //        }
    //    }
    //    qDebug()<<"\n";


    //    AdjancentNode *relation4=new AdjancentNode[count3*count1];

    //    //N步可达路径R^n=R^(n-1)*R=R^3
    //    int count4=0;
    //    for(int j=0;j<count3;j++)
    //    {   for(int i=0;i<count1;i++)
    //        {
    //            if(relation1[i].ID==relation3[j].linkID)
    //            {
    //                relation4[count4].ID=relation3[j].ID;
    //                relation4[count4].linkID=relation1[i].linkID;
    //                relation4[count4].path.append(relation3[j].path);
    //                relation4[count4].path.append("->");
    //                relation4[count4].path.append(QString::number(relation1[i].linkID));
    //                qDebug()<<relation4[count4].path;
    //                count4++;

    //            }
    //        }
    //    }


    //    delete[] relation1;
    //    relation1=NULL;
    //    delete[] relation2;
    //    relation2=NULL;
    //    delete[] relation3;
    //    relation3=NULL;
    //    delete[] relation4;
    //    relation4=NULL;

}





/**
 * @brief zonedeal::sameColorProcess 处理相同颜色地物的距离问题
 * @param graph
 * @param image
 * @param Samples
 * @param Lines
 * @param colorTh
 * @param areaInfo
 * @param threshold
 */
AdjancentChanin zonedeal::sameColorProcess(Graph *graph,ImageArray image,int Samples,int Lines,unsigned short colorTh,
                                           AreaNodeInfo *areaInfo,unsigned int threshold,float adjIntensity)
{
    //定义矩阵，保存每个区域的边界的点到其他区域边界的点的距离小于距离阈值的数量与该区域边界周长的比值
    float ** distanceGraph=NULL;
    distanceGraph=new float*[graph->vexNum];
    for(int i=0;i<graph->vexNum;i++)
    {
        distanceGraph[i]=new float[graph->vexNum];
    }

    for(int i=0;i<graph->vexNum;i++)
    {
        Area area1;
        area1=pointIterator(image.colorTh,Samples,Lines,areaInfo[i]);
        Area edgePoint1=areaEdge(image.colorTh,Samples,Lines,area1,areaInfo[i]);
        for(int j=0;j<graph->vexNum;j++)
        {
            if(i!=j)
            {
                Area area2=pointIterator(image.colorTh,Samples,Lines,areaInfo[j]);
                Area edgePoint2=areaEdge(image.colorTh,Samples,Lines,area2,areaInfo[j]);
                distanceGraph[i][j]=calAreaEdgedistance(edgePoint1,edgePoint2,threshold);

                if(area1.p)
                {
                    delete[] area1.p;
                    area1.p=NULL;
                }
                if(area2.p)
                {
                    delete[] area2.p;
                    area2.p=NULL;

                }

                delete[] edgePoint2.p;
                edgePoint2.p=NULL;
            }
            else
            {
                distanceGraph[i][j]=0;
            }

        }
        delete[] edgePoint1.p;
        edgePoint1.p=NULL;
    }

    //为同一地物距离邻接图分配内存
    DGraph dGraph;
    dGraph.headChain=new DHeadNode[graph->vexNum];
    for(int i=0;i<graph->vexNum;i++)
    {
        dGraph.headChain[i].ID=i;
        dGraph.headChain[i].isVisited=false;
        dGraph.headChain[i].firstNode=NULL;
    }
    DLinkNode *node,*tail=NULL;
    //输出所有满足距离关系的相同颜色区域的距离邻接关系
    AdjancentChanin result;
    result.data=new AdjancentNode[graph->vexNum*graph->vexNum];
    int count1=0;
    for(int i=0;i<graph->vexNum;i++)
    {
        for(int j=0;j<graph->vexNum;j++)
        {
            //两区域满足输入的颜色
            if(areaInfo[i].colorTh==colorTh&&areaInfo[i].colorTh==areaInfo[j].colorTh)
            {
                //两区域距离满足阈值要求
                if(distanceGraph[i][j]>=adjIntensity&&distanceGraph[j][i]>=adjIntensity)
                {
                    node=new DLinkNode;
                    node->AdjIntensity=distanceGraph[i][j];
                    node->ID=j;
                    node->next=NULL;
                    if(dGraph.headChain[i].firstNode==NULL)
                    {
                        dGraph.headChain[i].firstNode=node;
                        tail=node;
                        //qDebug()<<QStringLiteral("节点")<<i<<QStringLiteral("插入节点")<<j;
                    }
                    else
                    {
                        tail->next=node;
                        tail=node;
                        //qDebug()<<QStringLiteral("节点")<<i<<QStringLiteral("插入节点")<<j;
                    }

                    deleteLater();
                    //i区域与j区域满足距离要求
                    qDebug()<<i<<j<<QStringLiteral("临界指标")<<distanceGraph[i][j];
                    result.data[count1].ID=i;
                    result.data[count1].linkID=j;
                    count1++;
                }
                ////////////////////////////此函数从此往上测试正常//////////////////////////////////////////
                //求出i区域与j区域的可达路径，找出其几步可达
            }
        }
    }

    for(int i=0;i<graph->vexNum;i++)
    {
        delete[] distanceGraph[i];
        distanceGraph[i]=NULL;
    }
    delete[]  distanceGraph;
    distanceGraph=NULL;
    for(int i=0;i<graph->vexNum;i++)
    {
        delete[] dGraph.headChain[i].firstNode;
        dGraph.headChain[i].firstNode=NULL;
    }
    delete[] dGraph.headChain;
    dGraph.headChain=NULL;

    result.number=count1;

    return result;
}
/**
 * @brief zonedeal::calAreaEdgedistance计算一个区域之间的边界的点到另一区域边界的点的距离小于距离阈值的数量与该区域边界周长的比值 已测试
 * @param pointSet1
 * @param pointSet2
 * @param threshold
 */
float zonedeal::calAreaEdgedistance(Area pointSet1,Area pointSet2,unsigned int threshold)
{
    unsigned int pointCount=0;//满足条件的点的数量
    for(int i=0;i<pointSet1.number;i++)
    {
        for(int j=0;j<pointSet2.number;j++)
        {
            //如果一个区域之间的边界的点到另一区域边界的点的距离小于距离阈值，则将其计数
            if(distance(pointSet1.p[i],pointSet2.p[j])<=threshold)
            {
                pointCount++;
                break;          //只要找到距离满足阈值的距离就跳出测试
            }
        }
    }
    return pointCount*1.0/pointSet1.number;
}


/**
 * @brief zonedeal::areaEdge 返回一个区域的边界点 已测试
 * @param imageArray         总图像
 * @param Samples            图像宽度
 * @param Lines              图像高度
 * @param stake              区域点坐标集合
 * @param nodeinfo           区域信息
 * @return
 */
Area zonedeal::areaEdge(unsigned short int *imageArray,int Samples,int Lines,Area stake,AreaNodeInfo nodeinfo)
{
    Area area;
    area.p=new point[stake.number];
    unsigned int count=0;
    for(int i=0;i<stake.number;i++)
    {
        if(stake.p[i].y-1>0)//判断是否是边界点，保证数组不越界
        {
            if(imageArray[((stake.p[i].y-1)*Samples+stake.p[i].x)]!=nodeinfo.colorTh)//满足条件则是边界点
            {
                area.p[count]=stake.p[i];
                count++;
                continue;
            }
        }
        else//是边界点
        {
            area.p[count]=stake.p[i];
            count++;
            continue;
        }
        if(stake.p[i].x-1>0)//判断是否是边界点，保证数组不越界
        {
            if(imageArray[(stake.p[i].y*Samples+stake.p[i].x-1)]!=nodeinfo.colorTh)//满足条件则是边界点
            {
                area.p[count]=stake.p[i];
                count++;
                continue;
            }
        }
        else//是边界点
        {
            area.p[count]=stake.p[i];
            count++;
            continue;
        }
        if(stake.p[i].y+1<Lines)//判断是否是边界点，保证数组不越界
        {   if(imageArray[((stake.p[i].y+1)*Samples+stake.p[i].x)]!=nodeinfo.colorTh)//满足条件则是边界点
            {
                area.p[count]=stake.p[i];
                count++;
                continue;
            }
        }
        else//是边界点
        {
            area.p[count]=stake.p[i];
            count++;
            continue;
        }
        if(stake.p[i].x+1<Samples)//判断是否是边界点，保证数组不越界
        {   if(imageArray[(stake.p[i].y*Samples+stake.p[i].x+1)]!=nodeinfo.colorTh)//满足条件则是边界点
            {
                area.p[count]=stake.p[i];
                count++;
                continue;
            }
        }
        else//是边界点
        {
            area.p[count]=stake.p[i];
            count++;
            continue;
        }
    }
    area.number=count;
    //    for(int i=0;i<count;i++)
    //    {
    //        qDebug()<<area.p[i].x<<area.p[i].y;
    //    }
    //    qDebug()<<"\n";
    return area;
}
/**
 * @brief zonedeal::deep 深度优先遍历邻接表1 已测试
 * @param graph
 */
void zonedeal::deep(Graph *graph)
{
    //将邻接表置为未访问状态
    for(int i=0;i<graph->vexNum;i++)
    {
        graph->headChain[i].isVisited=false;
    }
    QStack<HeadNode*>stack;
    stack.push(&(graph->headChain[0]));//起始点入栈

    int current=0;
    bool isHead=true;
    LinkNetNode *node=NULL;

    do{
        if(isHead==true)
        {
            node=stack.top()->firstNode;//如果是头节点则获取它的邻接节点
            isHead=false;
        }
        else
        {
            node=node->next;//不是头节点，则获取下一邻接节点
        }

        if(node!=NULL)
        {
            if(!(graph->headChain[node->ID]).isVisited)//当前结点未访问过，开始访问
            {
                graph->headChain[node->ID].isVisited=true;//访问标志
                stack.push(&(graph->headChain[node->ID]));//将该节点入栈
                //qDebug()<<stack.top()->ID;
                current=stack.top()->ID;
                isHead=true;
                //（原理：深度优先遍历所有节点）
                //若此节点不是终止节点则以此节点为中心找此节点的邻接节点（回到上面isHead=true）
            }
        }
        else //当此节点没有邻接节点或者所有邻接节点都已经访问
        {
            for(int i=0;i<stack.size();i++)
            {
                qDebug()<<stack.at(i)->ID;
            }
            qDebug()<<"\n";
            //当节点没有邻接节点了，访问标识弹栈
            stack.pop();
            if(!stack.isEmpty())
            {
                //若栈不为空获取栈顶节点
                node=graph->headChain[stack.top()->ID].firstNode;
                while(node->ID!=current)
                {
                    node=node->next;
                }
                current=stack.top()->ID;
                isHead=0;
            }
        }

    }while(!stack.isEmpty());

}
/**
 * @brief zonedeal::deepIterater 深度优先遍历邻接表2 已测试
 * @param graph
 * @param Samples
 * @param Lines
 */
void zonedeal::deepIterater(Graph *graph)
{
    for(int i=0;i<graph->vexNum;i++)
    {
        graph->headChain[i].isVisited=false;
    }

    HeadNode *vnode;
    QStack<HeadNode*> TraverseStack;
    TraverseStack.push(&(graph->headChain[0]));
    while(!TraverseStack.empty())
    {
        vnode=(HeadNode*)TraverseStack.top();
        if(!vnode->isVisited)
        {
            vnode->isVisited=true;
            qDebug()<<vnode->ID;
        }
        TraverseStack.pop();
        LinkNetNode *node=vnode->firstNode;
        while(node!=NULL)
        {
            if(!(graph->headChain[node->ID]).isVisited)
            {
                qDebug()<<QStringLiteral("入栈节点是：")<<graph->headChain[node->ID].ID;
                TraverseStack.push(&(graph->headChain[node->ID]));
            }
            node=node->next;
        }
    }

}
/**
 * @brief zonedeal::getRoad 获取任意两点间的所有路径
 * @param graph
 * @param start
 * @param end
 * @param roadDirection
 */
void zonedeal::getRoad(Graph *graph,int start,int end,unsigned short *roadDirection)
{
    deleteFile("D:\\stack.txt");
    deleteFile("D:\\stackReverse.txt");
    findEveryPatch(graph,start,end,"D:\\stack.txt");
    findEveryPatch(graph,end,start,"D:\\stackReverse.txt");
    //fileStackRead("D:\\stack.txt",roadDirection,graph);
    //fileStackRead("D:\\stackReverse.txt",roadDirection,graph);
    fileStackMinRead("D:\\stack.txt");
}
/**
 * @brief zonedeal::deleteFile 删除给定路径的文件
 * @param path
 */
void zonedeal::deleteFile(QString path)
{
    QFile file(path.toStdString().c_str());
    if(file.exists())
    {
        file.remove();
    }
}
/**
 * @brief zonedeal::roadProcess 对路径进行加工
 * @param road
 * @param direction
 * @param graph
 */
void zonedeal::roadProcess(QVector<unsigned short>road, QVector<unsigned short>direction,Graph *graph)
{
    for(int i=0;i<road.size();i++)
    {
        qDebug()<<QStringLiteral("地物编号：")<<road.at(i);
        if(i<road.size()-1)
        {
            if(direction.at(i)==1)
            {
                qDebug()<<QStringLiteral("的右邻接");
            }
            else
            {
                qDebug()<<QStringLiteral("的下邻接");
            }
        }
    }
}
/**
 * @brief zonedeal::fileStackMinRead 查找两点间的最近的路径
 * @param path                       文件的路径
 * @param graph                      邻接表
 */
void zonedeal::fileStackMinRead(QString path)
{
    QFile file(path.toStdString().c_str());
    QVector<int> RoadLong;
    if(file.open(QFile::ReadOnly|QIODevice::Text))
    {
        QTextStream in(&file);
        QString string;
        for(int i=0;(string=in.readLine())!=NULL;i++)
        {
            //读取路径长度
            if(i%2==1)
            {
                QVector<QString> road;
                qDebug()<<string;
                road=string.split(" ").toVector();
                RoadLong.push_back(road.size());
            }

        }
        int temp=RoadLong.at(0);
        int minIndex=0;
        for(int i=0;i<RoadLong.size();i++)
        {
            if(RoadLong.at(i)<temp)
            {
                temp=RoadLong.at(i);
                minIndex=i;
            }
        }

        qDebug()<<QStringLiteral("最小的下标")<<minIndex;
        in.seek(0);//回到文件开头重新读取
        for(int i=0;(string=in.readLine())!=NULL;i++)
        {
            if(i==minIndex*2+1)
            {
                //读取路径长度
                QVector<QString> road;
                qDebug()<<string;
                road=string.split(" ").toVector();
                road.pop_back();
                for(int j=0;j<road.size();j++)
                {
                    qDebug()<<QStringLiteral("最短路径是：")<<road.at(j);
                }
                break;
            }
        }
    }
}
/**
 * @brief zonedeal::fileStackRead 读出文件栈中路径，并判读是否存在所要找的路径
 * @param path                    文件路径
 * @param roadDirection           方向数组
 */
void zonedeal::fileStackRead(QString path,unsigned short *roadDirection,Graph *graph)
{
    QFile file(path.toStdString().c_str());
    if(file.open(QFile::ReadOnly|QIODevice::Text))
    {
        QTextStream in(&file);
        QString string;
        bool flag=true;//决定是否输出路径


        for(int i=0;(string=in.readLine())!=NULL;i++)
        {

            if(i%2==0)//方向array
            {
                QVector<QString> direction;
                QVector<unsigned short>realDirection;
                qDebug()<<string;
                direction=string.split(" ").toVector();
                direction.pop_back();//删除结尾处空格
                //将字符串转换为unsign short
                for(int j=0;j<direction.size();j++)
                {
                    realDirection.append(direction.at(j).toUShort());
                }
                flag=true;//true时输出路径，false时不输出路径
                for(int j=0;j<realDirection.size();j++)
                {
                    if(realDirection.at(j)!=roadDirection[j])
                    {
                        flag=false;
                    }
                }
                if(flag)//如果是要找的路径
                {
                    i++;//读取一行  为了保证读取行数与i一致即++
                    string=in.readLine();
                    QVector<QString> road;
                    QVector<unsigned short> realRoad;

                    qDebug()<<string;
                    road=string.split(" ").toVector();
                    road.pop_back();//删除结尾处空格
                    //将字符串转换为unsign short
                    for(int j=0;j<road.size();j++)
                    {
                        realRoad.append(road.at(j).toUShort());
                    }
                    roadProcess(realRoad,realDirection,graph);
                    //找到路径
                    qDebug()<<QStringLiteral("找到路径");
                }
            }

        }
    }
}
/**
 * @brief zonedeal::fileStackWrite 将栈写入文件暂时保存
 * @param path                     文件路径
 * @param stack                    栈
 */
void zonedeal::fileStackWrite(QString path,QStack<HeadNode*> *stack)
{
    QFile file(path.toStdString().c_str());
    if(file.open(QFile::Append | QIODevice::Text)){

        QTextStream out(&file);
        for(int i=1;i<stack->size();i++)
        {
            out<<stack->at(i)->nextDirection<<" ";
        }
        out<<endl;
        for(int i=0;i<stack->size();i++)
        {
            out<<stack->at(i)->ID<<" ";
        }
        out<<endl;

        file.close();
    }
}
/**
 * @brief zonedeal::findEveryPatch 获取邻接表中任意两点间的所有路径
 * @param graph 邻接表
 * @param start 起始节点
 * @param end 结束节点
 * @param path 保存栈文件的路径
 */
void zonedeal::findEveryPatch(Graph *graph,int start,int end,QString path)
{

    //将邻接表置为未访问状态
    for(int i=0;i<graph->vexNum;i++)
    {
        graph->headChain[i].isVisited=false;
    }
    QStack<HeadNode*>stack;
    stack.push(&(graph->headChain[start]));//起始点入栈

    int current=start;
    bool isHead=true;
    LinkNetNode *node=NULL;

    do{
        if(isHead==true)
        {
            node=stack.top()->firstNode;//如果是头节点则获取它的邻接节点
            isHead=false;
        }
        else
        {
            node=node->next;//不是头节点，则获取下一邻接节点
        }

        if(node!=NULL)
        {
            if(!(graph->headChain[node->ID]).isVisited)//当前结点未访问过，开始访问
            {
                graph->headChain[node->ID].isVisited=true;//访问标志
                graph->headChain[node->ID].nextDirection=node->nextDirection;//邻接的方向
                stack.push(&(graph->headChain[node->ID]));//将该节点入栈

                if(node->ID==end)//判断当前结点是否是终止节点
                {
                    //若是终止节点则输出栈中的所有节点组成的路径
                    fileStackWrite(path,&stack);
                    //for(int i=0;i<stack.size();i++)
                    //{
                    //    qDebug()<<stack.at(i)->ID;
                    //    qDebug()<<"("<<stack.at(i)->nextDirection<<")";
                    //}
                    //qDebug()<<"\n";
                    graph->headChain[end].isVisited=false;//将终止节点重新置为未访问状态，以便找其他路径时再次访问

                    stack.pop();//回到终止节点的上一节点
                    current=stack.top()->ID;

                    isHead=false;//继续寻找终止节点的上一节点的邻接节点，看终止节点的上一节点的邻接节点中是否还有能到达终止节点的节点（回到上面isHead=false）
                }
                else
                {
                    current=stack.top()->ID;
                    isHead=true;
                    //（原理：深度优先遍历所有节点）
                    //若此节点不是终止节点则以此节点为中心找此节点的邻接节点（回到上面isHead=true）
                }
            }
        }
        else //当此节点没有邻接节点或者所有邻接节点都已经访问
        {
            //当节点没有邻接节点了，访问标识弹栈
            graph->headChain[stack.top()->ID].isVisited=false;
            stack.pop();

            if(!stack.isEmpty())
            {
                //若栈不为空获取栈顶节点
                node=graph->headChain[stack.top()->ID].firstNode;
                while(node->ID!=current)
                {
                    node=node->next;
                }
                current=stack.top()->ID;
                isHead=0;
            }
        }

    }while(!stack.isEmpty());

}

/**
 * @brief zonedeal::deepRightIterater 右邻接
 * @param nodeInfo
 * @param currentNode
 */
void zonedeal::deepRightIterater(AreaNodeInfo *nodeInfo,int currentNode)
{
    for(int i=0;i<nodeInfo[currentNode].rLinkNum;i++)//右邻接
    {
        qDebug()<<QStringLiteral("当前节点的ID是:")<<nodeInfo[currentNode].ID<<
                  QStringLiteral("当前节点的右邻接节点是：")<<nodeInfo[currentNode].rID[i];
        deepRightIterater(nodeInfo,nodeInfo[currentNode].rID[i]);
    }
}
/**
 * @brief zonedeal::deepSearchRight 二邻接
 * @param nodeInfo
 * @param currentNode
 * @param colorTh
 */
void zonedeal::deepSearchRight(AreaNodeInfo *nodeInfo,int currentNode,int colorTh)
{
    if(nodeInfo[currentNode].rLinkNum>0)
    {
        qDebug()<<QStringLiteral("当前节点的id是：")<<nodeInfo[currentNode].ID;
        for(int i=0;i<nodeInfo[currentNode].rLinkNum;i++)//右邻接
        {
            for(int j=0;j<nodeInfo[nodeInfo[currentNode].rID[i]].rLinkNum;j++)//右邻接的右邻接
            {
                if(nodeInfo[nodeInfo[currentNode].rID[i]].rightColorTh[j]==colorTh)
                {
                    qDebug()<<QStringLiteral("当前结点")<<nodeInfo[currentNode].ID<<QStringLiteral("的二邻接地物是")<<nodeInfo[nodeInfo[currentNode].rID[i]].rID[j];
                    deepSearchRight(nodeInfo,nodeInfo[nodeInfo[currentNode].rID[i]].rID[j],colorTh);
                }
            }
        }
    }
    else
    {
        qDebug()<<QStringLiteral("当前链结束");
    }
}
/**
 * @brief zonedeal::adjancentColor 找邻接区域的颜色 还差一个好的解决方案
 * @param nodeInfo
 * @return
 */
unsigned short zonedeal::adjancentColor(ImageArray image,int Samples,int Lines,AreaNodeInfo nodeInfo)
{
    unsigned int color[6]={0,0,0,0,0,0};
    Area stake=pointIterator(image.colorTh,Samples,Lines,nodeInfo);
    unsigned short *imageArray=image.colorTh;
    for(int i=0;i<stake.number;i++)
    {
        if(stake.p[i].y-1>0)//判断是否是上边界点，保证数组不越界
        {
            if(imageArray[((stake.p[i].y-1)*Samples+stake.p[i].x)]!=nodeInfo.colorTh)//满足条件则是边界点
            {
                //找colorTh点对应的颜色
                color[imageArray[((stake.p[i].y-1)*Samples+stake.p[i].x)]]++;
                continue;
            }
        }
        if(stake.p[i].x-1>0)//判断是否是下边界点，保证数组不越界
        {
            if(imageArray[(stake.p[i].y*Samples+stake.p[i].x-1)]!=nodeInfo.colorTh)//满足条件则是边界点
            {
                color[imageArray[(stake.p[i].y*Samples+stake.p[i].x-1)]]++;
                continue;
            }
        }
        if(stake.p[i].y+1<Lines)//判断是否是边界点，保证数组不越界
        {   if(imageArray[((stake.p[i].y+1)*Samples+stake.p[i].x)]!=nodeInfo.colorTh)//满足条件则是边界点
            {
                color[imageArray[((stake.p[i].y+1)*Samples+stake.p[i].x)]]++;
                continue;
            }
        }
        if(stake.p[i].x+1<Samples)//判断是否是边界点，保证数组不越界
        {   if(imageArray[(stake.p[i].y*Samples+stake.p[i].x+1)]!=nodeInfo.colorTh)//满足条件则是边界点
            {
                color[imageArray[(stake.p[i].y*Samples+stake.p[i].x+1)]]++;
                continue;
            }
        }
    }
    int temp=color[0];
    unsigned short index=0;
    for(int i=0;i<6;i++)
    {
        if(color[i]>temp)
        {
            temp=color[i];
            index=i;
        }

    }
    delete[] stake.p;
    stake.p=NULL;
    return index;
}
/**
 * @brief zonedeal::distance 计算两点之间距离
 * @param p1
 * @param p2
 * @return
 */
double zonedeal::distance(point p1,point p2)
{
    return sqrt(((p1.x-p2.x)*(p1.x-p2.x))+((p1.y-p2.y)*(p1.y-p2.y)));
}
/**
 * @brief zonedeal::distancesConfine 判断两个区域之间的距离是否满足条件
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @return 满足返回true 否则false
 */
bool zonedeal::distancesConfine(unsigned short x1,unsigned short y1,unsigned short x2,unsigned short y2)
{
    point p1,p2;
    p1.x=x1,p1.y=y1;
    p2.x=x2,p2.y=y2;
    if(distance(p1,p2)<180)
    {
        return true;
    }
    else
    {
        return false;
    }

}


/**
 * @brief zonedeal::countEveryNumber 统计每一个独立地物块的点的数量以及回溯的入口地址 已经经过测试
 * @param imageArray 图像地物数组
 * @param Samples 图像宽度
 * @param Lines 图像高度
 * @return 每个独立地物块的点的数量以及回溯入口地址
 */
AreaNodeInfo* zonedeal::countEveryNumber(unsigned short int *imageArray,int Samples,int Lines)
{

    //定义一个方向数组
    int next[8][2]={ {0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}};

    point *stake=new point[Samples*Lines];//回溯栈
    AreaNodeInfo * everyNum=new AreaNodeInfo[Samples*Lines/100];//区域信息表
    //初始化，很重要
    for(int i=0;i<Samples*Lines/100;i++)
    {
        everyNum[i].number=0;
    }
    unsigned short int *footFlag=new unsigned short int[Samples*Lines];//是否走过标识
    for(int i=0;i<Samples*Lines;i++)
    {
        footFlag[i]=0;
    }
    int footPath=7;//要遍历的点的数量
    int nx=0,ny=0;//移动点
    int x=0,y=0;//基点
    int pointCount=0;//点数量
    int currentNum=0;//栈指针
    int landCount=0;//独立地物块计数
    bool gotoFlag=false;

    for(int c=0;c<6;c++)
    {

        for(int h=0;h<Lines;h++)
        {
            for(int w=0;w<Samples;w++)
            {
                pointCount=0;
                currentNum=0;//当前指针位置

                if(imageArray[h*Samples+w]==c&&footFlag[h*Samples+w]==0)
                {
                    //qDebug()<<QStringLiteral("当前入口")<<w<<h;
                    //记录当前区域的入口点坐标
                    everyNum[landCount].startX=w;
                    everyNum[landCount].startY=h;
                    //将坐标入栈
                    stake[currentNum].x=w;
                    stake[currentNum].y=h;
                    //                    qDebug()<<"\n\n";
                    //                    qDebug()<<QStringLiteral("当前入栈入口坐标是：")<<w<<h;

                    footFlag[h*Samples+w]=1;

                    currentNum++;
                    pointCount++;
                    nx=w;ny=h;
                    do
                    {
                        x=nx;y=ny;
                        gotoFlag=false;
                        for(int i=0;i<footPath;i++)
                        {
                            nx=x+next[i][0];
                            ny=y+next[i][1];
                            if(nx<0||nx>=Samples||ny<0||ny>=Lines)
                            {
                                continue;
                            }
                            //满足条件跳往下一个移动点
                            if(imageArray[ny*Samples+nx]==c
                                    &&footFlag[ny*Samples+nx]==0)
                            {
                                //坐标入栈
                                stake[currentNum].x=nx;
                                stake[currentNum].y=ny;
                                //                                qDebug()<<QStringLiteral("当前入栈坐标是：")<<nx<<ny;
                                footFlag[ny*Samples+nx]=1;

                                pointCount++;
                                currentNum++;//当前指针位置
                                gotoFlag=true;
                                break;
                            }
                        }
                        //7个点都不满足返回上一个点
                        if(gotoFlag==false)
                        {
                            //每返回一个点，都要把指针向下移动一格
                            currentNum--;
                            if(currentNum>-1)
                            {
                                //nx,ny坐标回到上一步坐标
                                //坐标出栈
                                nx=stake[currentNum].x;
                                ny=stake[currentNum].y;
                                //                                qDebug()<<QStringLiteral("出栈点：")<<nx<<ny;
                                //允许循环，可以回到上一步
                                gotoFlag=true;
                            }

                        }

                    } while(gotoFlag);
                    //记录该区域点的数量以及地物种类
                    everyNum[landCount].number=pointCount;
                    everyNum[landCount].colorTh=c;
                    landCount++;
                }

            }//Samples
        }//Samples*Lines

    }

    delete[] stake;
    stake=NULL;
    delete[] footFlag;
    footFlag=NULL;
    return everyNum;//返回地物信息表
    //        delete[] everyNum;
    //    everyNum=NULL;
}

/**
 * @brief zonedeal::pointIterator 根据入口点信息，获得某一地物的所有的点的坐标 已测试
 * @param imageArray
 * @param Samples
 * @param Lines
 * @param nodeinfo
 * @return
 */
Area zonedeal::pointIterator(unsigned short int *imageArray,int Samples,int Lines,AreaNodeInfo nodeinfo)
{
    //定义一个方向数组
    int next[8][2]={ {0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}};
    Area pointSet;
    pointSet.p=new point[Samples*Lines];//本区域点集合

    point *stake=new point[Samples*Lines];//回溯栈

    unsigned short int *footFlag=new unsigned short int[Samples*Lines];
    for(int i=0;i<Samples*Lines;i++)
    {
        footFlag[i]=0;
    }

    int footPath=7;//要遍历的点的数量
    int currentNum=0;//当前指针位置
    int pointCount=0;
    int nx=nodeinfo.startX,ny=nodeinfo.startY;
    int x=0,y=0;
    bool gotoFlag=false;
    //将入口点入栈
    pointSet.p[pointCount].x=nx;
    pointSet.p[pointCount].y=ny;
    stake[currentNum].x=nx;
    stake[currentNum].y=ny;
    footFlag[ny*Samples+nx]=1;
    //    qDebug()<<"\n\n";
    //    qDebug()<<QStringLiteral("当前入栈入口坐标是：")<<nx<<ny;
    pointCount++;
    currentNum++;

    do
    {
        x=nx;y=ny;
        gotoFlag=false;
        for(int i=0;i<footPath;i++)
        {
            nx=x+next[i][0];
            ny=y+next[i][1];
            if(nx<0||nx>=Samples||ny<0||ny>=Lines)
            {
                continue;
            }
            //满足条件跳往下一个移动点
            if(imageArray[ny*Samples+nx]==nodeinfo.colorTh
                    &&footFlag[ny*Samples+nx]==0)
            {
                //将点加入点集合
                pointSet.p[pointCount].x=nx;
                pointSet.p[pointCount].y=ny;
                //入栈
                stake[currentNum].x=nx;
                stake[currentNum].y=ny;
                //                qDebug()<<QStringLiteral("当前入栈坐标是：")<<nx<<ny;
                footFlag[ny*Samples+nx]=1;
                //imageArray[ny*Samples+nx]=2;变色
                pointCount++;
                currentNum++;
                gotoFlag=true;
                break;
            }
        }
        //7个点都不满足返回上一个点
        if(gotoFlag==false)
        {
            //每返回一个点，都要把指针向下移动一格
            currentNum--;
            if(currentNum>-1)
            {
                //nx,ny坐标回到上一步坐标
                //出栈
                nx=stake[currentNum].x;
                ny=stake[currentNum].y;
                //允许循环，可以回到上一步
                gotoFlag=true;
            }
        }

    } while(gotoFlag);

    pointSet.number=pointCount;
    //    qDebug()<<QStringLiteral("该区域共有点")<<pointCount;
    delete[] footFlag;
    footFlag=NULL;
    delete[] stake;
    stake=NULL;
    //    qDebug()<<QStringLiteral("已经遍历完1所有点");
    return pointSet;//返回该区域点集
    //    delete[] pointSet;
    //    pointSet=NULL;
}
unsigned short *zonedeal::newChangeColor(unsigned short int *imageArray,
                                         int Samples,int Lines,AreaNodeInfo nodeinfo
                                         ,int color,int numThreshold)
{
    Area area=pointIterator(imageArray,Samples,Lines,nodeinfo);
    if(area.number<numThreshold)
    {
        for(int i=0;i<area.number;i++)
        {
            imageArray[area.p[i].y*Samples+area.p[i].x]=color;
        }
    }
    delete[] area.p;
    area.p=NULL;
    return imageArray;
}

/**
 * @brief zonedeal::changeColor 改变颜色 已测试
 * @param imageArray
 * @param Samples
 * @param Lines
 * @param nodeinfo
 * @param color
 * @return
 */
unsigned short int * zonedeal::changeColor(unsigned short int *imageArray,int Samples,int Lines,AreaNodeInfo nodeinfo,int color)
{
    //定义一个方向数组
    int next[8][2]={ {0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}};
    Area stake;
    stake.p=new point[Samples*Lines];
    unsigned short int *footFlag=new unsigned short int[Samples*Lines];
    for(int i=0;i<Samples*Lines;i++)
    {
        footFlag[i]=0;
    }
    int footPath=7;//要遍历的点的数量
    int currentNum=0;
    int nx=nodeinfo.startX,ny=nodeinfo.startY;
    int x=0,y=0;
    bool gotoFlag=false;
    //将入口点入栈
    stake.p[currentNum].x=nx;
    stake.p[currentNum].y=ny;
    footFlag[ny*Samples+nx]=1;
    imageArray[ny*Samples+nx]=color;
    currentNum++;

    do
    {
        x=nx;y=ny;
        gotoFlag=false;
        for(int i=0;i<footPath;i++)
        {
            nx=x+next[i][0];
            ny=y+next[i][1];
            if(nx<0||nx>=Samples||ny<0||ny>=Lines)
            {
                continue;
            }
            //            qDebug()<<nx<<ny;
            //满足条件跳往下一个移动点
            if(imageArray[ny*Samples+nx]==nodeinfo.colorTh
                    &&footFlag[ny*Samples+nx]==0)
            {
                stake.p[currentNum].x=nx;
                stake.p[currentNum].y=ny;

                footFlag[ny*Samples+nx]=1;
                imageArray[ny*Samples+nx]=color;
                //qDebug()<<QStringLiteral("颜色发生变化");
                currentNum++;
                gotoFlag=true;
                break;
            }
        }
        //7个点都不满足返回上一个点
        if(gotoFlag==false)
        {
            //每返回一个点，都要把指针向下移动一格
            currentNum--;
            if(currentNum>-1)
            {
                //nx,ny坐标回到上一步坐标
                nx=stake.p[currentNum].x;
                ny=stake.p[currentNum].y;
                //允许循环，可以回到上一步
                gotoFlag=true;
            }
        }

    } while(gotoFlag);

    delete[] footFlag;
    footFlag=NULL;
    delete[] stake.p;
    stake.p=NULL;
    return imageArray;
}
/**
 * @brief zonedeal::calAreaCycle 计算一个区域的周长   已经过测试
 * @param imageArray 图像数组
 * @param Samples 图像width
 * @param Lines 图像height
 * @param stake 区域点集合
 * @param nodeinfo 区域信息
 * @return
 */
int zonedeal::calAreaCycle(unsigned short int *imageArray,int Samples,int Lines,Area stake,AreaNodeInfo nodeinfo)
{
    //计算当前区域的周长
    int cycle=0;
    for(int i=0;i<stake.number;i++)
    {
        if(stake.p[i].y-1>0)//判断是否是边界点，保证数组不越界
        {
            if(imageArray[((stake.p[i].y-1)*Samples+stake.p[i].x)]!=nodeinfo.colorTh)//满足条件则是边界点
            {
                cycle++;
                continue;
            }
        }
        else//是边界点
        {
            cycle++;
            continue;
        }
        if(stake.p[i].x-1>0)//判断是否是边界点，保证数组不越界
        {
            if(imageArray[(stake.p[i].y*Samples+stake.p[i].x-1)]!=nodeinfo.colorTh)//满足条件则是边界点
            {
                cycle++;
                continue;
            }
        }
        else//是边界点
        {
            cycle++;
            continue;
        }
        if(stake.p[i].y+1<Lines)//判断是否是边界点，保证数组不越界
        {   if(imageArray[((stake.p[i].y+1)*Samples+stake.p[i].x)]!=nodeinfo.colorTh)//满足条件则是边界点
            {
                cycle++;
                continue;
            }
        }
        else//是边界点
        {
            cycle++;
            continue;
        }
        if(stake.p[i].x+1<Samples)//判断是否是边界点，保证数组不越界
        {   if(imageArray[(stake.p[i].y*Samples+stake.p[i].x+1)]!=nodeinfo.colorTh)//满足条件则是边界点
            {
                cycle++;
                continue;
            }
        }
        else//是边界点
        {
            cycle++;
            continue;
        }
    }
    //    qDebug()<<QStringLiteral("周长计算完成")<<cycle;
    return cycle;
}
bool zonedeal::getRALLink(unsigned short int *imageArray,int Samples,int Lines,AreaNodeInfo nodeinfo)
{
    Area stake=pointIterator(imageArray,Samples,Lines,nodeinfo);
    qDebug()<<QStringLiteral("开始进入计算  该区域共有点：")<<stake.number;
    int cycle=calAreaCycle(imageArray,Samples,Lines,stake,nodeinfo);

    int dLinkDistance=30;//下邻接距离阈值
    int rLinkDistance=30;//右邻接距离阈值
    int dLinkNumber=0;//满足阈值的下邻接数量
    int rLinkNumber=0;//满足阈值的右邻接数量
    point d;
    point r;
    //ToDo:这里有点问题没解决，不能实现功能
    for(int i=0;i<stake.number;i++)
    {
        //点的坐标是stake.p[i].x   stake.p[i].y

        //下邻接
        if(stake.p[i].y+1<Lines)
            if(imageArray[((stake.p[i].y+1)*Samples+stake.p[i].x)]!=nodeinfo.colorTh)//下边界
            {
                if(stake.p[i].y+dLinkDistance<Lines)//不越界
                    if(imageArray[((stake.p[i].y+dLinkDistance)*Samples+stake.p[i].x)]==nodeinfo.colorTh)
                    {
                        d.x=stake.p[i].x;
                        d.y=stake.p[i].y+dLinkDistance;
                        dLinkNumber++;
                    }
            }
        //右邻接
        if(stake.p[i].x+1<Samples)//保证数组不越界
            if(imageArray[(stake.p[i].y*Samples+stake.p[i].x+1)]!=nodeinfo.colorTh)
            {
                //若下邻接不是相同地物，则计算在一定阈值范围内35m(35个像素)若下邻居的邻居是colorTh地物
                //则认为它和此地地物是在一排上的，将其放入数组
                if(stake.p[i].x+rLinkDistance<Samples)
                    if(imageArray[(stake.p[i].y*Samples+stake.p[i].x+rLinkDistance)]==nodeinfo.colorTh)
                    {
                        r.x=stake.p[i].x+rLinkDistance;
                        r.y=stake.p[i].y;
                        rLinkNumber++;
                    }
            }

    }
    qDebug()<<"dLinkNumber"<<dLinkNumber;
    qDebug()<<"rLinkNumber"<<rLinkNumber;
    bool flag=false;
    double rate=0;
    //下邻接存在
    Area dStake;
    if(dLinkNumber>0&&cycle>0)
    {
        rate=dLinkNumber*1.0/cycle;
        qDebug()<<"rate"<<rate;
        AreaNodeInfo n;
        n.startX=d.x;
        n.startY=d.y;
        n.colorTh=nodeinfo.colorTh;
        qDebug()<<QStringLiteral("入口点坐标")<<n.startX<<"\t"<<n.startY;
        dStake=pointIterator(imageArray,Samples,Lines,n);
        qDebug()<<QStringLiteral("数量是：")<<dStake.number;
        int dCycle=calAreaCycle(imageArray,Samples,Lines,stake,n);

        int uLinkNumber=0;
        for(int i=0;i<dStake.number;i++)
        {
            //上邻接
            if(dStake.p[i].y-1>0)
                if(imageArray[((dStake.p[i].y-1)*Samples+dStake.p[i].x)]!=nodeinfo.colorTh)
                {
                    if(dStake.p[i].y-dLinkDistance>0)
                        if(imageArray[((dStake.p[i].y-dLinkDistance)*Samples+dStake.p[i].x)]==nodeinfo.colorTh)
                        {
                            uLinkNumber++;
                        }
                }
        }
        double uRate=0;
        if(uLinkNumber>0&&dCycle>0)
        {
            uRate=uLinkNumber*1.0/dCycle;
            qDebug()<<"uRate"<<rate;
        }
        //得出rate和uRate如果两个值都很小，则不认为在一排
        if(rate>0.1&&uRate>0.1)
        {
            qDebug()<<QStringLiteral("两个地物块在上下一排");
            flag=true;
        }
        else
        {
            qDebug()<<QStringLiteral("两个地物不在上下一排");
        }

        delete[] dStake.p;
        dStake.p=NULL;
    }

    //右邻接存在
    Area rStake;
    if(rLinkNumber>0)
    {
        rate=dLinkNumber*1.0/cycle;
        qDebug()<<"rate"<<rate;
        AreaNodeInfo n;
        n.startX=r.x;
        n.startY=r.y;
        n.colorTh=nodeinfo.colorTh;
        qDebug()<<QStringLiteral("入口点坐标")<<n.startX<<"\t"<<n.startY;
        rStake=pointIterator(imageArray,Samples,Lines,n);
        qDebug()<<QStringLiteral("数量是：")<<dStake.number;
        int rCycle=calAreaCycle(imageArray,Samples,Lines,stake,n);

        int lLinkNumber=0;
        for(int i=0;i<rStake.number;i++)
        {
            //左邻接
            if(stake.p[i].y-1>0)
                if(imageArray[(rStake.p[i].y*Samples+rStake.p[i].x-1)]!=nodeinfo.colorTh)
                {
                    if(rStake.p[i].x-rLinkDistance>0)
                        if(imageArray[(rStake.p[i].y*Samples+rStake.p[i].x-rLinkDistance)]==nodeinfo.colorTh)
                        {
                            lLinkNumber++;
                        }
                }
        }
        double lRate=0;
        if(lLinkNumber>0&&rCycle>0)
        {
            lRate=lLinkNumber*1.0/rCycle;
            qDebug()<<"lRate"<<lRate;
        }
        //得出rate和uRate如果两个值都很小，则不认为在一排
        if(rate>0.1&&lRate>0.1)
        {
            qDebug()<<QStringLiteral("两个地物块在左右一排");
            flag=true;
        }
        else
        {
            qDebug()<<QStringLiteral("两个地物不在左右一排");
        }

        delete[] rStake.p;
        rStake.p=NULL;
    }
    if(!flag)
    {
        qDebug()<<QStringLiteral("此处应该将该地物变色");
    }
    delete[] stake.p;
    stake.p=NULL;
    return flag;
}

/**
 * @brief zonedeal::rightOrDownLink 已经经过测试
 * @param imageArray
 * @param Samples
 * @param Lines
 * @param areaInfo
 * @param current
 * @param area 当前区域是第current个区域
 * @return
 */
AreaNodeInfo zonedeal::rightOrDownLink(ImageArray imageArray,int Samples,int Lines,
                                       AreaNodeInfo areaInfo,Area area)
{
    //右邻接地物是
    areaInfo.rightColorTh=new unsigned short [1000];
    areaInfo.rID=new unsigned short [1000];
    areaInfo.rAdjacentNum=new unsigned [1000];
    //下邻接地物是
    areaInfo.downColorTh=new unsigned short [1000];
    areaInfo.dID=new unsigned short [1000];
    areaInfo.dAdjacentNum=new unsigned [1000];
    int rLinkNum=0,dLinkNum=0;
    unsigned int rAdjacentNum=0,dAdjacentNum=0;
    bool flag=false;

    for(int i=0;i<area.number;i++)
    {
        if(area.p[i].x+1<Samples)
            if(imageArray.colorTh[area.p[i].y*Samples+area.p[i].x+1]!=areaInfo.colorTh)
            {
                //右邻接地物是第imageArray.colorTh[area.p[i].y*Samples+area.p[i].x+1]种地物
                //右邻接地物的id是imageArray.id[area.p[i].y*Samples+area.p[i].x+1];
                //qDebug()<<QStringLiteral("邻接的坐标")<<area.p[i].x+1<<area.p[i].y;
                //如果一个id不在已经邻接的所有id中，则加入id
                flag=false;
                for(int j=0;j<rLinkNum;j++)
                {
                    if(imageArray.id[area.p[i].y*Samples+area.p[i].x+1]==areaInfo.rID[j])
                    {
                        flag=true;
                        break;//是已经找到的邻接地物，不用加入邻接数组
                    }
                }
                if(!flag)//不是已经找到的邻接地物，加入邻接数组
                {

                    rAdjacentNum=0;//邻接数量置0
                    //qDebug()<<imageArray.id[area.p[i].y*Samples+area.p[i].x+1];
                    areaInfo.rightColorTh[rLinkNum]=imageArray.colorTh[area.p[i].y*Samples+area.p[i].x+1];
                    areaInfo.rID[rLinkNum]=imageArray.id[area.p[i].y*Samples+area.p[i].x+1];
                    //qDebug()<<QStringLiteral("右邻接id:")<<areaInfo.rID[rLinkNum]<<"\n";
                    rLinkNum++;
                }
                rAdjacentNum++;//邻接计数加
                areaInfo.rAdjacentNum[rLinkNum-1]=rAdjacentNum;//邻接计数赋值（由于总是先增加rLinkNum所以要减1）

            }
        if(area.p[i].y+1<Lines)
            if(imageArray.colorTh[(area.p[i].y+1)*Samples+area.p[i].x]!=areaInfo.colorTh)
            {
                //下邻接地物是第imageArray.colorTh[area.p[i].y*Samples+area.p[i].x+1]种地物
                //下邻接地物的id是imageArray.id[area.p[i].y*Samples+area.p[i].x+1];

                //如果一个id不在已经邻接的所有id中，则加入id
                flag=false;
                //                dAdjacentNum++;
                //                if(dLinkNum-1>=0)
                //                    areaInfo.dAdjacentNum[dLinkNum-1]=dAdjacentNum;
                for(int j=0;j<dLinkNum;j++)
                {
                    if(imageArray.id[(area.p[i].y+1)*Samples+area.p[i].x]==areaInfo.dID[j])
                    {
                        flag=true;
                        break;//是已经找到的邻接地物，不用加入邻接数组
                    }
                }
                if(!flag)//不是已经找到的邻接地物，加入邻接数组
                {
                    dAdjacentNum=0;
                    //qDebug()<<imageArray.id[(area.p[i].y+1)*Samples+area.p[i].x];
                    areaInfo.downColorTh[dLinkNum]=imageArray.colorTh[(area.p[i].y+1)*Samples+area.p[i].x];
                    areaInfo.dID[dLinkNum]=imageArray.id[(area.p[i].y+1)*Samples+area.p[i].x];
                    //qDebug()<<QStringLiteral("下邻接id:")<<areaInfo.dID[dLinkNum]<<"\n";
                    dLinkNum++;
                }
                dAdjacentNum++;
                areaInfo.dAdjacentNum[dLinkNum-1]=dAdjacentNum;
            }
    }
    areaInfo.rLinkNum=rLinkNum;
    areaInfo.dLinkNum=dLinkNum;
    //    for(int k=0;k<areaInfo.rLinkNum;k++)
    //    {
    //        qDebug()<<QStringLiteral("右邻接区域的邻接id")<<areaInfo.rID[k]<<QStringLiteral("右邻接区域的邻接数量")<<areaInfo.rAdjacentNum[k]<<"\n";
    //    }
    //    for(int k=0;k<areaInfo.dLinkNum;k++)
    //    {
    //        qDebug()<<QStringLiteral("下邻接区域的邻接id")<<areaInfo.dID[k]<<QStringLiteral("下邻接区域的邻接数量")<<areaInfo.dAdjacentNum[k]<<"\n";
    //    }
    //    qDebug()<<QStringLiteral("右邻接区域的数量")<<areaInfo.rLinkNum;
    //    qDebug()<<QStringLiteral("下邻接区域的数量")<<areaInfo.dLinkNum;
    return areaInfo;
}
void zonedeal::linjie(unsigned short int *imageArray,int Samples,int Lines)
{
    //统计四种地物（0 1）（0 2）（0 3）（1 2）（1 3）（2 3）邻接关系（右邻接和下邻接）
    unsigned short int path[15][2]={{0,1},{0,2},{1,2},{0,3},{1,3},{2,3}};
    int right[15][3];
    int down[15][3];
    int rightRevers[15][3];
    int downRevers[15][3];
    for(int i=0;i<15;i++)
    {
        for(int j=0;j<3;j++)
        {
            right[i][j]=0;
            down[i][j]=0;
            rightRevers[i][j]=0;
            downRevers[i][j]=0;
        }
    }
    int rightCount=0,downCount=0,rightReversCount=0,downReversCount=0;
    for(int i=0;i<6;i++)
    {
        //清空上一轮计数
        rightCount=0;
        downCount=0;
        rightReversCount=0;
        downReversCount=0;

        for(int h=0;h<Lines-1;h++)
        {
            for(int w=0;w<Samples-1;w++)
            {
                //对于第path[i][0]种地物，若他的右边的地物是path[i][1]地物则记录下第i种地物的右邻接地物，并记录右邻接地物的数量
                //声明right[15][3]={{path[i][0],path[i][1],number},.....}
                //声明down[15][3]={{path[i][0],path[i][1],number},......}

                //右邻接path[i][0]到path[i][1]
                if(imageArray[h*Samples+w]==path[i][0]&&imageArray[(h*Samples+w+1)]==path[i][1])
                {
                    right[i][0]=path[i][0];
                    right[i][1]=path[i][1];
                    right[i][2]=rightCount;
                    rightCount++;
                }
                //右邻接path[i][1]到path[i][0]
                if(imageArray[h*Samples+w]==path[i][1]&&imageArray[(h*Samples+w+1)]==path[i][0])
                {
                    rightRevers[i][0]=path[i][1];
                    rightRevers[i][1]=path[i][0];
                    rightRevers[i][2]=rightReversCount;
                    rightReversCount++;
                }
                //下邻接path[i][0]到path[i][1]
                if(imageArray[h*Samples+w]==path[i][0]&&imageArray[(h+1)*Samples+w]==path[i][1])
                {
                    down[i][0]=path[i][0];
                    down[i][1]=path[i][1];
                    down[i][2]=downCount;
                    downCount++;
                }
                //下邻接path[i][1]到path[i][0]
                if(imageArray[h*Samples+w]==path[i][1]&&imageArray[(h+1)*Samples+w]==path[i][0])
                {
                    downRevers[i][0]=path[i][1];
                    downRevers[i][1]=path[i][0];
                    downRevers[i][2]=downReversCount;
                    downReversCount++;
                }
            }
        }
    }

    //返回右邻接和下邻接表
    for(int i=0;i<6;i++)
    {
        qDebug()<<QStringLiteral("地物")<<right[i][0]<<QStringLiteral("地物")<<right[i][1]<<QStringLiteral("右邻接，邻接数量")<<right[i][2];
    }
    qDebug()<<"\n";
    for(int i=0;i<6;i++)
    {
        qDebug()<<QStringLiteral("地物")<<down[i][0]<<QStringLiteral("地物")<<down[i][1]<<QStringLiteral("下邻接，邻接数量")<<down[i][2];
    }
    qDebug()<<"\n";
    for(int i=0;i<6;i++)
    {
        qDebug()<<QStringLiteral("地物")<<rightRevers[i][0]<<QStringLiteral("地物")<<rightRevers[i][1]<<QStringLiteral("右邻接，邻接数量")<<rightRevers[i][2];
    }
    qDebug()<<"\n";
    for(int i=0;i<6;i++)
    {
        qDebug()<<QStringLiteral("地物")<<downRevers[i][0]<<QStringLiteral("地物")<<downRevers[i][1]<<QStringLiteral("下邻接，邻接数量")<<downRevers[i][2];
    }
}



//深度优先遍历表
//    AdjancentNode *temp=&relation[0];
//    QStack<AdjancentNode*> stake;
//    stake.push(&relation[0]);
//    do
//    {
//        bool flag=false;
//        if(!stake.isEmpty())
//        {
//            temp=stake.top();
//        }
//        for(int i=0;i<count;i++)
//        {
//            if(relation[i].ID==temp->linkID&&relation[i].isVisited==false)
//            {
//                flag=true;
//                relation[i].isVisited=true;
//                qDebug()<<temp->ID<<relation[i].linkID;
//                stake.push(&relation[i]);
//                break;
//            }
//        }
//        if(!flag)
//        {
//            stake.pop();
//        }


//    }while(!stake.isEmpty());

