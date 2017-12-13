#ifndef ZONEDEAL_H
#define ZONEDEAL_H

#include <QObject>
#include <filedeal.h>
#include <QtCore>
#include <QStack>
typedef struct
{
    unsigned short int R;
    unsigned short int G;
    unsigned short int B;
}s_color;
typedef struct
{
    unsigned short int startX;//入口x
    unsigned short int startY;//入口y
    unsigned int ID;//本区域的唯一标识
    int number;//点的数量
    int cycle;//一个区域的周长
    bool isIsolate;//是否是孤立区域
    unsigned short int rLinkNum;//右邻接区域的数量
    unsigned short int dLinkNum;//下邻接区域的数量
    unsigned int* rAdjacentNum;//右邻接数量
    unsigned int* dAdjacentNum;//右邻接数量
    float *rAdjIntensity;
    float *dAdjIntensity;
    unsigned short int* rID;//右邻接区域ID数组
    unsigned short int* dID;//下邻接区域ID数组
    unsigned short int* rightX;//右邻接地物入口
    unsigned short int* rightY;
    unsigned short int* downX;//下邻接地物入口
    unsigned short int* downY;
    unsigned short int* downColorTh;//第n种地物
    unsigned short int* rightColorTh;//第n种地物
    unsigned short int colorTh;//第n种地物
}AreaNodeInfo;
//typedef struct
//{
//    unsigned int id;//当前区域的id
//    unsigned short colorTh;//当前区域的颜色
//    unsigned int *adjancentID;//邻接区域的ID
//    unsigned short *direction;//在哪个方向邻接 1代表右邻接 2代表下邻接
//    unsigned short*adjancentColorTh;//邻接区域的颜色
//    unsigned int adjancentNum;//邻接区域的数量
//}LinkNetNode;
typedef struct LinkNetNode
{
    unsigned int ID;//该节点指向的的区域的ID
    struct LinkNetNode *next;//下一条弧
}LinkNetNode;

typedef struct
{
    unsigned int ID;//当前头的id
    bool isVisited;//是否已经遍历过
    LinkNetNode *firstNode; //指向该顶点所接的单链表的第一个弧节点
}HeadNode;

/*图的结构*/
typedef struct{
    HeadNode *headChain;   //顶点数组
    int vexNum;         //顶点数
    int arcNum;        //弧数
}Graph;

typedef struct
{
    unsigned int id;
    unsigned int rNum;
    unsigned int dNum;
    unsigned int *rID;
    unsigned int *dID;
}twoAdjacent;

typedef struct{
    point *p;//一个区域的点的集合
    int number;//一个区域点的数量
}Area;
typedef struct{
    unsigned short int *id;
    unsigned short int *colorTh;
}ImageArray;
class zonedeal:public QObject
{
    Q_OBJECT
public:
    zonedeal();
private:
    void main(QImage image);
    AreaNodeInfo *countEveryNumber(unsigned short *imageArray, int Samples, int Lines);
    Area pointIterator(unsigned short *imageArray, int Samples, int Lines, AreaNodeInfo nodeinfo);
    void linjie(unsigned short *imageArray, int Samples, int Lines);
    bool getRALLink(unsigned short *imageArray, int Samples, int Lines, AreaNodeInfo nodeinfo);
    int calAreaCycle(unsigned short *imageArray, int Samples, int Lines, Area stake, AreaNodeInfo nodeinfo);
    unsigned short *changeColor(unsigned short *imageArray, int Samples, int Lines, AreaNodeInfo nodeinfo, int color);
    void test(ImageArray testImage, int Samples, int Lines);
    AreaNodeInfo rightOrDownLink(ImageArray imageArray, int Samples, int Lines, AreaNodeInfo areaInfo, Area area);
    double distance(point p1, point p2);
    bool distancesConfine(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2);
    unsigned short adjancentColor(ImageArray image, int Samples, int Lines, AreaNodeInfo nodeInfo);
    void deepSearchRight(AreaNodeInfo *nodeInfo, int currentNode, int colorTh);
    void deepRightIterater(AreaNodeInfo *nodeInfo, int currentNode);
    void deepSearch(LinkNetNode *linkChain, int current, unsigned int *chain, int count);
    void cycleSearch(Graph *graph, int Samples, int Lines);
};

#endif // ZONEDEAL_H
