#include "splitjoint.h"

SplitJoint::SplitJoint(QObject *parent) : QObject(parent)
{

}

/**
 * @brief SplitJoint::joint 将一个部分图片嵌入到整体图像中去
 * @param Original 整体图像
 * @param part 部分图像
 * @param startX 部分图像在整体图像中的起始点的X坐标
 * @param startY 部分图像在整体图像中的起始点的Y坐标
 * @return 二者合成后的图片
 */
QImage SplitJoint::joint(QImage Original,QImage part,int startX,int startY)
{
    int width=part.width();
    int height=part.height();
    for(int h=0;h<height;h++)
    {
        for(int w=0;w<width;w++)
        {
            Original.setPixel(w+startX,h+startY,part.pixel(w,h));
        }
    }
    return Original;
}
/**
 * @brief SplitJoint::changeColor 改变图片中某个地物的颜色
 * @param image 图像
 * @param changeColor 要更改的颜色
 * @param changedColor 更改后的颜色
 * @return 颜色更改后的图片
 */
QImage SplitJoint::changeColor(QImage image,QRgb changeColor,QRgb changedColor)
{
    for(int h=0;h<image.height();h++)
    {
        for(int w=0;w<image.width();w++)
        {
            if(QColor(image.pixel(w,h))==QColor(changeColor))
            {
                image.setPixel(w,h,changedColor);
            }
        }
    }
    return image;
}
/**
 * @brief SplitJoint::combine 图像求与运算
 * @param image1 图像一
 * @param image2 图像二
 * @return 合成后的图像
 */
QImage SplitJoint::combine(QImage image1,QImage image2)
{
    int width=image1.width();
    int height=image1.height();
    QImage new_image(image1.width(),image2.height(),QImage::Format_RGB32);

    for(int w=0;w<width;w++)
    {
        for(int h=0;h<height;h++)
        {
            if(QColor(image1.pixel(w,h))==QColor(qRgb(255,255,255))&&QColor(image2.pixel(w,h))==QColor(qRgb(0,0,255)))
            {
                QRgb rgb=qRgb(QColor(image2.pixel(w,h)).red(),QColor(image2.pixel(w,h)).green(),QColor(image2.pixel(w,h)).blue());
                new_image.setPixel(w,h,rgb);
            }
        }
    }
    return new_image;
}
