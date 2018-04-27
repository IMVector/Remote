#ifndef ZONEDEAL_H
#define ZONEDEAL_H

#include <QObject>
#include <filedeal.h>
#include <QtCore>
#include <QStack>
#include <QString>
#include <zoneform.h>

typedef struct
{
	unsigned short R;
	unsigned short G;
	unsigned short B;
}s_color;
typedef struct
{
	unsigned short startX;//入口x
	unsigned short startY;//入口y
	unsigned int ID;//本区域的唯一标识
	unsigned int number;//点的数量
	unsigned int cycle;//一个区域的周长
	unsigned short rLinkNum;//右邻接区域的数量
	unsigned short dLinkNum;//下邻接区域的数量
	unsigned int* rAdjacentNum;//右邻接数量
	unsigned int* dAdjacentNum;//右邻接数量
    float *rAdjIntensity;//右邻接强度
    float *dAdjIntensity;//下邻接强度
	unsigned short* rID;//右邻接区域ID数组
	unsigned short* dID;//下邻接区域ID数组
	unsigned short* downColorTh;//第n种地物
	unsigned short* rightColorTh;//第n种地物
	unsigned short colorTh;//第n种地物
    //	bool isIsolate;//是否是孤立区域
}AreaNodeInfo;


typedef struct
{
	unsigned int ID;
	unsigned int linkID;
    float adjIntensity_a_b;
    float adjIntensity_b_a;

}AdjancentNode;
//    QString path;
typedef struct
{
	AdjancentNode *data;
	int number;

}AdjancentChanin;

//距离邻接节点
typedef struct DLinkNode
{
	unsigned int ID;
	float AdjIntensity;//邻接指标
	struct DLinkNode *next;
}DLinkNode;
//距离邻接头节点
typedef struct
{
	unsigned int ID;//区域ID
	bool isVisited;//访问标识
	DLinkNode *firstNode;//第一个节点
}DHeadNode;

/*图的结构*/
typedef struct {
	DHeadNode *headChain;   //顶点数组
	int vexNum;         //顶点数
	int arcNum;        //弧数
}DGraph;


//链接节点
typedef struct LinkNetNode
{
	unsigned int ID;//该节点指向的的区域的ID
	unsigned short nextDirection;//头节点到下一节点的方向
	struct LinkNetNode *next;//下一条弧
}LinkNetNode;
//头节点
typedef struct
{
	unsigned int ID;//当前头的id
	bool isVisited;//是否已经遍历过
	unsigned short nextDirection;//邻接的反方向
	LinkNetNode *firstNode; //指向该顶点所接的单链表的第一个弧节点
	//    unsigned short colorTh;//当前结点的颜色
	//    unsigned short startX;//区域入口X
	//    unsigned short startY;//区域入口Y
}HeadNode;

/*图的结构*/
typedef struct {
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

typedef struct {
    point *p;//一个区域的点的集合
	int number;//一个区域点的数量
}Area;
typedef struct {
    unsigned short Samples;
    unsigned short Lines;
	unsigned short int *id;
	unsigned short int *colorTh;
}ImageArray;
class zonedeal :public QObject
{
	Q_OBJECT
public:

	zonedeal();
private:
    int deleteThresould = 5000;//最小区域点数量阈值
    int distanceThresould = 70;//距离阈值
    float adjIntensity = 0.30;//邻接强度阈值
    int geoTh = 2;//要找成排的地物
    int changedColorTh = 3;//更改后的颜色
    QString fileName;

    void main(QImage image, int deleteThresould, int distanceThresould, float adjIntensity, int geoTh, int changedColorTh);
    AreaNodeInfo *countEveryNumber(ImageArray imageArray, int Samples, int Lines);
    Area pointIterator(ImageArray imageArray, int Samples, int Lines, AreaNodeInfo nodeinfo);
	void linjie(unsigned short *imageArray, int Samples, int Lines);
    bool getRALLink(ImageArray imageArray, int Samples, int Lines, AreaNodeInfo nodeinfo);
	int calAreaCycle(unsigned short *imageArray, int Samples, int Lines, Area stake, AreaNodeInfo nodeinfo);
	unsigned short *changeColor(unsigned short *imageArray, int Samples, int Lines, AreaNodeInfo nodeinfo, int color);
	AreaNodeInfo rightOrDownLink(ImageArray imageArray, int Samples, int Lines, AreaNodeInfo areaInfo, Area area);
	double distance(point p1, point p2);
	bool distancesConfine(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2);
    int adjancentColor(ImageArray image, int Samples, int Lines, AreaNodeInfo nodeInfo, int deleteThresould, AreaNodeInfo *allInfo);
	void deepSearchRight(AreaNodeInfo *nodeInfo, int currentNode, int colorTh);
	void deepRightIterater(AreaNodeInfo *nodeInfo, int currentNode);
	void deepIterater(Graph *graph);
	void findEveryPatch(Graph *graph, int start, int end, QString path);
	void deep(Graph *graph);
	void fileStackWrite(QString path, QStack<HeadNode *> *stack);
	void fileStackRead(QString path, unsigned short *roadDirection, Graph *graph);
	void deleteFile(QString path);
	void getRoad(Graph *graph, int start, int end, unsigned short *roadDirection);
	void roadProcess(QVector<unsigned short> road, QVector<unsigned short> direction, Graph *graph);
	Area areaEdge(unsigned short *imageArray, int Samples, int Lines, Area stake, AreaNodeInfo nodeinfo);
    float calAreaEdgedistance(ImageArray image, Area pointSet1, Area pointSet2, unsigned int threshold);
	void fileStackMinRead(QString path);
	AdjancentChanin processResult(AreaNodeInfo *areaInfo, int nodeNum, unsigned int colorTh);
	AdjancentChanin sameColorProcess(Graph *graph, ImageArray image, int Samples, int Lines, unsigned short colorTh, AreaNodeInfo *areaInfo, unsigned int threshold, float adjIntensity);
    unsigned short *newChangeColor(ImageArray imageArray, int Samples, int Lines, AreaNodeInfo nodeinfo, int color, int numThreshold);
    void test(ImageArray testImage, int Samples, int Lines, int deleteThresould, int distanceThresould, float adjIntensity, int geoTh, int changedColorTh);
    QImage midPointLink(QImage image, point p1, point p2, int changeColor, int changedColor);
    QImage anyLine(QImage image, ImageArray testImage, AreaNodeInfo *areaInfo, int changeColorTh, int changedColorTh, int id1, int id2);
private slots:
	void startZone();
    void loadImage_slot(QString name);
    void loadInfo_slot(loadInfo info);
signals:
	void sendImageToUi(QImage image, int status);

};

#endif // ZONEDEAL_H
