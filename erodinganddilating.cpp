#include "erodinganddilating.h"

ErodingAndDilating::ErodingAndDilating(QObject *parent) : QObject(parent)
{

}
/**
 * @brief ErodingAndDilating::getRoundPixel 获取传入点周围8个点的颜色值
 * @param image 图像文件
 * @param changeColor 当前点的颜色
 * @param x 当前点的坐标X
 * @param y 当前点的坐标Y
 * @return 返回传入点周围八个点的QRgb
 */
pane *ErodingAndDilating::getRoundPixel(QImage image,QRgb changeColor,int x, int y) //返回(x,y)周围像素的情况，为黑色，则设置为true
{
    pane pixels[8];
    int num = 0;
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            QRgb rgb=qRgb(QColor(image.pixel(x+i,y+j)).red(),QColor(image.pixel(x+i,y+j)).green(),QColor(image.pixel(x+i,y+j)).blue());
            //qDebug()<<QColor(image.pixel(x+i,y+j)).red()<<QColor(image.pixel(x+i,y+j)).green()<<QColor(image.pixel(x+i,y+j)).blue();
            if (i != 0 || j != 0)
            {
                if(QColor(rgb)==QColor(changeColor))
                {
                    pixels[num].flag = true;
                    pixels[num].value=rgb;
                    //qDebug()<<QColor(rgb).red()<<QColor(rgb).green()<<QColor(rgb).blue();
                    num++;
                }
                else
                {
                    pixels[num].flag = false;
                    pixels[num].value=rgb;
                    //qDebug()<<QColor(rgb).red()<<QColor(rgb).green()<<QColor(rgb).blue();
                    num++;
                }
            }
        }
    }
    return pixels;
}
/**
 * @brief ErodingAndDilating::expend 膨胀处理
 * @param imagePath 图片的路径
 * @param expendColor 要膨胀区域的颜色
 * @return 膨胀后的图片
 */
QImage ErodingAndDilating:: expend(QString imagePath,QRgb expendColor)
{
    QImage image;
    image.load(imagePath);
    int height=image.height();
    int width=image.width();
    QImage image_new=image;
    pane *pixels;
    int length=8;
    for (int i = 1; i < width - 1; i++)
    {
        for (int j = 1; j < height - 1; j++)
        {

            if (QColor(image.pixel(i,j))!=QColor(expendColor))//如果当前点的颜色是白色，则将此点变成黑色
            {
                pixels = getRoundPixel(image,expendColor, i, j);
                for (int k = 0; k < length; k++)
                {
                    if (pixels[k].flag == true)
                    {
                        //set this piexl's color to black
                        image_new.setPixel(i,j,expendColor);
                        break;
                    }
                }
            }
        }
    }
    return image_new;
}

/**
 * @brief ErodingAndDilating::corrode 腐蚀处理
 * @param imagePath 图像路径
 * @param corrodeColor 要腐蚀的区域的颜色
 * @return 腐蚀后的图片
 */
QImage ErodingAndDilating::corrode(QString imagePath,QRgb corrodeColor)
{
    QImage image;
    image.load(imagePath);
    int height=image.height();
    int width=image.width();
    QImage image_new=image;
    pane *pixels;
    int lenght=8;
    for (int i = 1; i < width - 1; i++)
    {
        for (int j = 1; j < height - 1; j++)
        {

            if (QColor(image.pixel(i,j))==QColor(corrodeColor))
            {
                pixels = getRoundPixel(image,corrodeColor,i, j);
                for (int k = 0; k < lenght; k++)
                {
                    if (pixels[k].flag == false)
                    {
                        //qDebug()<<"new";
                        //set this piexl's color to black
                        image_new.setPixel(i,j,pixels[k].value);
                        break;
                    }
                }
            }
        }
    }
    return image_new;
}
