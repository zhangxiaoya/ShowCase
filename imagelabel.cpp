#include "imagelabel.h"

ImageLabel::ImageLabel():
    begX(0),
    begY(0),
    endX(0),
    endY(0),
    drawFlag(false)
{

}

void ImageLabel::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event); //绘制背景的图片

    if((begX ==0 && endX == 0) ||(begY == 0 && endY == 0))
        return;

    QPainter painter(this);
    painter.setPen(QPen(Qt::red, 2));
    painter.drawRect(QRect(begX, begY, endX - begX, endY - begY));
}

void ImageLabel::mouseMoveEvent(QMouseEvent *event)
{
    if(!drawFlag)
        return;
    if (event->buttons() & Qt::LeftButton)
    {
        endX = event->pos().x(); //鼠标相对于所在控件的位置
        endY = event->pos().y();
        update();
    }
}

void ImageLabel::mousePressEvent(QMouseEvent *event)
{
    if(!drawFlag)
        return;

    begX = event->pos().x();
    begY = event->pos().y();
}

void ImageLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if(!drawFlag)
        return;
    endX = event->pos().x(); //鼠标相对于所在控件的位置
    endY = event->pos().y();
    update();
}

void ImageLabel::resizeBoundingBox(double factor)
{
    begX = static_cast<int>(static_cast<double>(begX) * factor);
    endX = static_cast<int>(static_cast<double>(endX) * factor);
    begY = static_cast<int>(static_cast<double>(begY) * factor);
    endY = static_cast<int>(static_cast<double>(endY) * factor);
}

void ImageLabel::SetDrawFlag(const bool flag)
{
    this->drawFlag = flag;
    if(flag)
    {
        QCursor cursor;
        cursor.setShape(Qt::CrossCursor);
        QApplication::setOverrideCursor(cursor);
    }
    else
    {
        QApplication::restoreOverrideCursor();
    }
}
