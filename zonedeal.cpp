#include "zonedeal.h"
zonedeal::zonedeal()
{
    //    //    输入一个图片地址
    //    //    C:\\Users\\25235\\Desktop\\file1.tif
    //    QImage image;
    //    image.load("C:\\Users\\25235\\Desktop\\file.tif");
    //    if(!image.isNull())
    //    {
    //        main(image);
    //    }
    //    else
    //    {
    //        qDebug()<<QStringLiteral("文件为空");
    //    }
    //    unsigned short int testImage[32]={
    //        1,1,2,1,2,1,2,1,
    //        1,1,2,1,2,1,2,1,
    //        1,1,2,1,2,1,2,1,
    //        1,1,2,1,2,1,2,1,
    //    };
    unsigned short int testImage[32]={
        1,1,2,1,2,1,2,1,
        1,1,2,1,4,4,4,1,
        3,3,3,1,2,1,2,1,
        1,1,2,1,2,1,2,1,
    };
    ImageArray imageArray;
    imageArray.colorTh=testImage;
    //imageArray.id=new unsigned short int [32];
    test(imageArray,8,4);

}

void zonedeal::test(ImageArray testImage,int Samples,int Lines)
{
    int R[7]={ 255,  255,    0,   0,   139,0};
    int G[7]={   0,  255,    0,   255, 0 ,0};
    int B[7]={   0  ,  0,  255,   0,   139   ,0};
    testImage.id=new unsigned short int[Samples*Lines];
    int NodeNumber=0;
    AreaNodeInfo *everyNum=countEveryNumber(testImage.colorTh,Samples,Lines);

    //    for(int i=0;everyNum[i].number>0;i++)
    //    {
    //        qDebug()<<everyNum[i].number;
    //    }
    //    点数量太少变色
    //    for(int i=0;everyNum[i].number>0;i++)
    //    {
    //        if(everyNum[i].number<200)
    //        {
    //            //            qDebug()<<everyNum[i].number;
    //            //            qDebug()<<QStringLiteral("未更改的颜色是：")<<everyNum[i].colorTh<<QStringLiteral("更改后的颜色是：")<<adjancentColor(testImage,Samples,Lines,everyNum[i]);
    //            //            unsigned short color=adjancentColor(testImage,Samples,Lines,everyNum[i]);
    //            //            testImage.colorTh=changeColor(testImage.colorTh,Samples,Lines,everyNum[i],4);
    //        }
    //    }
    QImage image(Samples, Lines, QImage::Format_RGB32);;

    for(int h=0;h<Lines;h++)
    {
        for(int w=0;w<Samples;w++)
        {
            QRgb value=qRgb(R[testImage.colorTh[h*Samples+w]],G[testImage.colorTh[h*Samples+w]],B[testImage.colorTh[h*Samples+w]]);
            image.setPixel(w,h,value);
        }
    }
    image.save("C:\\Users\\25235\\Desktop\\bbbbbb.tif");

    everyNum=countEveryNumber(testImage.colorTh,Samples,Lines);
    //给所有区域分配不同id
    for(int i=0;everyNum[i].number!=0;i++)
    {
        NodeNumber++;
        //qDebug()<<QStringLiteral("入口坐标")<<everyNum[i].startX<<everyNum[i].startY;
        //qDebug()<<everyNum[i].number;
        everyNum[i].ID=i;//给当前节点分配ID
        Area a=pointIterator(testImage.colorTh,Samples,Lines,everyNum[i]);
        everyNum[i].cycle=calAreaCycle(testImage.colorTh,Samples,Lines,a,everyNum[i]);
        qDebug()<<QStringLiteral("区域")<<i<<QStringLiteral("的周长是")<<everyNum[i].cycle;
        for(int j=0;j<a.number;j++)
        {
            testImage.id[a.p[j].y*Samples+a.p[j].x]=i;//给所有地物标一个唯一id，给图像中的当前区域赋值相同ID
        }
        delete a.p;
        a.p=NULL;
    }
    for(int i=0;everyNum[i].number!=0;i++)//计算所有区域的邻接地物以及邻接数量
    {
        Area a=pointIterator(testImage.colorTh,Samples,Lines,everyNum[i]);
        everyNum[i]=rightOrDownLink(testImage,Samples,Lines,everyNum[i],a);//经过这一步，右邻接区域，以及邻接数量全部求出
        everyNum[i].rAdjIntensity=new float[everyNum[i].rLinkNum];
        everyNum[i].dAdjIntensity=new float[everyNum[i].dLinkNum];
        for(int j=0;j<everyNum[i].rLinkNum;j++)//计算右邻接强度
        {
            everyNum[i].rAdjIntensity[j]=everyNum[i].rAdjacentNum[j]*1.0/everyNum[i].cycle;
            qDebug()<<QStringLiteral("右邻接强度是")<<everyNum[i].rAdjIntensity[j];
        }
        for(int j=0;j<everyNum[i].dLinkNum;j++)//计算下邻接强度
        {
            everyNum[i].dAdjIntensity[j]=everyNum[i].dAdjacentNum[j]*1.0/everyNum[i].cycle;
            qDebug()<<QStringLiteral("下邻接强度是")<<everyNum[i].dAdjIntensity[j];
        }
        delete a.p;
        a.p=NULL;
    }
    /////////////////////////////////////////////////////////////////////////////////////////
    //邻接表存储
    Graph myGraph;
    myGraph.headChain=new HeadNode[Samples*Lines];
    myGraph.vexNum=NodeNumber;
    for(int i=0;i<NodeNumber;i++)
    {
        myGraph.headChain[i].ID=i;
        myGraph.headChain[i].isVisited=false;
        myGraph.headChain[i].firstNode=NULL;
    }
    LinkNetNode *node,*tail=NULL;
    for(int i=0;i<NodeNumber;i++)
    {
        for(int j=0;j<everyNum[i].rLinkNum;j++)
        {
            node=new LinkNetNode;
            node->ID=everyNum[i].rID[j];
            node->next=NULL;

            qDebug()<<QStringLiteral("当前节点")<<myGraph.headChain[i].ID<<QStringLiteral("的右邻接是")<<node->ID;

            if(myGraph.headChain[i].firstNode==NULL)
            {
                myGraph.headChain[i].firstNode=node;
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
            node->next=NULL;

            qDebug()<<QStringLiteral("当前节点")<<myGraph.headChain[i].ID<<QStringLiteral("的下邻接是")<<node->ID;
            if(myGraph.headChain[i].firstNode==NULL)
            {
                myGraph.headChain[i].firstNode=node;
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
    //找出所有两步可达的地物相同的区域ToDo:将同一区域去除
    int *rNet=new  int[Samples*Lines];
    int rNetCount=0;
    int *dNet=new  int[Samples*Lines];
    int dNetCount=0;

    for(int i=0;everyNum[i].number!=0;i++)
    {
        for(int j=0;j<everyNum[i].rLinkNum;j++)//求出右邻接地物的id为everyNum[i].rID[j]
        {

            for(int k=0;k<everyNum[everyNum[i].rID[j]].rLinkNum;k++)//在第everyNum[i].rID[j]个区域的右邻接区域中
            {
                if(everyNum[everyNum[i].rID[j]].rightColorTh[k]==everyNum[i].colorTh)//如果二邻接区域的地物与当前区域i的地物相同，则认为在一排
                {
                    if(distancesConfine(everyNum[i].startX,everyNum[i].startY,
                                        everyNum[everyNum[i].rID[j]].startX,
                                        everyNum[everyNum[i].rID[j]].startY)&&
                            everyNum[i].ID!=everyNum[everyNum[i].rID[j]].rID[k])
                    {
                        qDebug()<<QStringLiteral("区域")<<everyNum[i].ID<<QStringLiteral("与区域")
                               <<everyNum[everyNum[i].rID[j]].rID[k]<<QStringLiteral("是二阶左右邻居");

                        rNet[rNetCount]=everyNum[i].ID;
                        rNetCount++;
                        rNet[rNetCount]=everyNum[everyNum[i].rID[j]].rID[k];
                        rNetCount++;
                    }
                }
            }
        }
        for(int j=0;j<everyNum[i].dLinkNum;j++)//求出右邻接地物的id为everyNum[i].rID[j]
        {
            for(int k=0;k<everyNum[everyNum[i].dID[j]].dLinkNum;k++)//在第everyNum[i].rID[j]个区域的右邻接区域中
            {
                if(everyNum[everyNum[i].dID[j]].downColorTh[k]==everyNum[i].colorTh)//如果二邻接区域的地物与当前区域i的地物相同，则认为在一排
                {
                    if(distancesConfine(everyNum[i].startX,everyNum[i].startY,
                                        everyNum[everyNum[i].rID[j]].startX,
                                        everyNum[everyNum[i].rID[j]].startY)&&
                            everyNum[i].ID!=everyNum[everyNum[i].dID[j]].dID[k])
                    {
                        qDebug()<<QStringLiteral("区域")<<everyNum[i].ID<<QStringLiteral("与区域")
                               <<everyNum[everyNum[i].dID[j]].dID[k]<<QStringLiteral("是二阶上下邻居");
                        dNet[dNetCount]=everyNum[i].ID;
                        dNetCount++;
                        dNet[dNetCount]=everyNum[everyNum[i].dID[j]].dID[k];
                        dNetCount++;
                    }
                }
            }
        }
    }

    //    ToDo:下一步找四步可达
    for(int i=0;i<rNetCount;i++)
    {
        if(i%2==0)
        {
            //qDebug()<<rNet[i]<<rNet[i+1]<<"\n";
            for(int j=0;j<rNetCount;j++)
            {
                if(j%2==0&&i!=j)
                    if(rNet[i+1]==rNet[j])
                    {
                        qDebug()<<QStringLiteral("区域")<<rNet[i]<<QStringLiteral("与区域")<<rNet[j+1]
                               <<QStringLiteral("是四阶上下邻居");
                    }
            }
        }
    }
    for(int i=0;i<dNetCount;i++)
    {
        if(i%2==0)
        {
            //qDebug()<<dNet[i]<<dNet[i+1]<<"\n";
            for(int j=0;j<dNetCount;j++)
            {
                if(j%2==0&&i!=j)
                    if(dNet[i+1]==dNet[j])
                    {
                        qDebug()<<QStringLiteral("区域")<<dNet[i]<<QStringLiteral("与区域")<<dNet[j+1]
                               <<QStringLiteral("是四阶上下邻居");
                    }

            }

        }
    }
    cycleSearch(&myGraph,Samples,Lines);
    delete[] rNet;
    rNet=NULL;
    delete[] dNet;
    dNet=NULL;
    //    delete[] net;
    //    net=NULL;
    //    for(int i=0;everyNum[i].number>0;i++)
    //    {
    //        delete[] everyNum[i].dAdjacentNum;
    //        everyNum[i].dAdjacentNum=NULL;
    //        delete[] everyNum[i].dAdjIntensity;
    //        everyNum[i].dAdjIntensity=NULL;
    //        delete[] everyNum[i].dID;
    //        everyNum[i].dID=NULL;
    //        delete[] everyNum[i].downColorTh;
    //        everyNum[i].downColorTh=NULL;
    //        delete[] everyNum[i].downX;
    //        everyNum[i].downX=NULL;
    //        delete[] everyNum[i].downY;
    //        everyNum[i].downY=NULL;
    //        delete[] everyNum[i].rightX;
    //        everyNum[i].rightX=NULL;
    //        delete[] everyNum[i].rightY;
    //        everyNum[i].rightY=NULL;
    //        delete[] everyNum[i].rightColorTh;
    //        everyNum[i].rightColorTh=NULL;
    //        delete[] everyNum[i].rAdjacentNum;
    //        everyNum[i].rAdjacentNum=NULL;
    //    }
}


void zonedeal::cycleSearch(Graph *graph,int Samples,int Lines)
{
    for(int i=0;i<Samples*Lines;i++)
    {
        qDebug()<<graph->headChain[i].ID;
        graph->headChain[i].isVisited=false;
    }

    HeadNode *vnode;
    QStack<HeadNode*> TraverseStack;
    TraverseStack.push(&(graph->headChain[0]));
    while(!TraverseStack.empty())
    {
        vnode=(HeadNode*)TraverseStack.top();
        vnode->isVisited=true;
        qDebug()<<vnode->ID;
        TraverseStack.pop();
        LinkNetNode *node=vnode->firstNode;
        while(node!=NULL)
        {
            if(!(graph->headChain[node->ID]).isVisited)
            {
                TraverseStack.push(&(graph->headChain[node->ID]));
            }
            node=node->next;
        }
    }

}


//void zonedeal::deepSearch(LinkNetNode *linkChain,int current,unsigned int *chain,int count)
//{

//    if(linkChain[current].adjancentNum>0)
//    {
//        for(int i=0;i<linkChain[i].adjancentNum;i++)
//        {
//            chain[count]=linkChain[current].adjancentID[i];
//            count++;
//            deepSearch(linkChain,linkChain[current].adjancentID[i],chain,count);
//        }
//    }
//    else
//    {
//        //        for(int i=0;i<count;i++)
//        //        {
//        //            qDebug()<<QStringLiteral("链中所有的id为")<<chain[i];
//        //        }
//        qDebug()<<QStringLiteral("当前链结束");
//    }
//    qDebug()<<QStringLiteral("返回上一层");
//}
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
void zonedeal::main(QImage image)
{
    int R[7]={ 255,  255,    0,   0,   139,0};
    int G[7]={   0,  255,    0,   255, 0 ,0};
    int B[7]={   0  ,  0,  255,   0,   139   ,0};
    //红 黄 蓝 绿 紫 黑
    //s_color *landColor=new s_color[6];
    int Samples=image.width();
    int Lines=image.height();
    qDebug()<<Samples;
    qDebug()<<Lines;
    bool flag=false;

    unsigned short int *testImage=new unsigned short int[Samples*Lines];

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
    //将每一块地物找出来，把每一块合成一个节点，节点此节点到与它相同的节点的距离小于一定距离的（当中隔着一个其他地物）
    //则认为他们是一排，一排的保留，不是一排的将其剔除（换成相近的地物）

    //    everyLandNum *everyNum=countEveryNumber(imageArray,Samples,Lines);
    //    qDebug()<<"normal";

    //    for(int i=0;everyNum[i].number!=0;i++)
    //    {
    //        if(everyNum[i].colorTh==2)
    //        {
    //            if(everyNum[i].number>1000)
    //            {
    //                everyNum[i].isIsolate=getRALLink(imageArray,Samples,Lines,everyNum[i]);
    //            }
    //        }
    ////一个地物块的点的数量很小，可以认为它是误分区域
    //if(everyNum[i].number<(Samples*Lines)/40)
    //{
    //    //利用回溯算法将此地物转换成将其包围的地物
    //    //回溯的入口(everyNum[i].x,everyNum[i].y)
    //}
    //if(everyNum[i].colorTh==0&&everyNum[i].number>20000&&everyNum[i].number<200000)
    //{
    //    qDebug()<<QStringLiteral("入口所在下标")<<i<<QStringLiteral("数量是")<<everyNum[i].number;
    //    point *pointSet=pointIterator(imageArray,Samples,Lines,everyNum[i]);
    //    delete[] pointSet;
    //    pointSet=NULL;
    //}
    //    }
    //    for(int i=0;everyNum[i].number!=0;i++)
    //    {
    //        if(everyNum[i].colorTh==2)
    //        {
    //            if(everyNum[i].number>1000)
    //            {
    //                if(!everyNum[i].isIsolate)
    //                {
    //                    changeColor(imageArray,Samples,Lines,everyNum[i],5);
    //                }
    //            }
    //            else
    //            {
    //                changeColor(imageArray,Samples,Lines,everyNum[i],5);
    //            }
    //        }
    //    }

    //    for(int h=0;h<Lines;h++)
    //    {
    //        for(int w=0;w<Samples;w++)
    //        {
    //            QRgb value=qRgb(R[imageArray.colorTh[h*Samples+w]],G[imageArray.colorTh[h*Samples+w]],B[imageArray.colorTh[h*Samples+w]]);
    //            image.setPixel(w,h,value);
    //        }
    //    }
    //    image.save("C:\\Users\\25235\\Desktop\\b.tif");

    //    linjie(imageArray,Samples,Lines);
    delete[] imageArray.colorTh;
    //    delete[] imageArray.id;
    imageArray.colorTh=NULL;
    //    imageArray.id=NULL;
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
    AreaNodeInfo * everyNum=new AreaNodeInfo[Samples*(Lines/2)];//区域信息表
    //初始化，很重要
    for(int i=0;i<Samples*(Lines/2);i++)
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
    //    delete[] everyNum;
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
    areaInfo.rightColorTh=new unsigned short int [20];
    areaInfo.rID=new unsigned short int [20];
    areaInfo.rAdjacentNum=new unsigned int [20];
    //下邻接地物是
    areaInfo.downColorTh=new unsigned short int [20];
    areaInfo.dID=new unsigned short int [20];
    areaInfo.dAdjacentNum=new unsigned int [20];
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




