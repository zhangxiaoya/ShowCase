#include "imagelabel.h"

ImageLabel::ImageLabel()
{

}

void ImageLabel::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event); //绘制背景的图片

    QPainter painter(this);

    painter.setPen(QPen(Qt::red, 2));
    painter.drawRect(QRect(begX, begY, endX - begX, endY - begY));
}

void ImageLabel::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        endX = event->pos().x(); //鼠标相对于所在控件的位置
        endY = event->pos().y();
        update();
    }
}

void ImageLabel::mousePressEvent(QMouseEvent *event)
{
    begX = event->pos().x();
    begY = event->pos().y();
    QCursor cursor;
    cursor.setShape(Qt::ClosedHandCursor);
    QApplication::setOverrideCursor(cursor);
}

void ImageLabel::mouseReleaseEvent(QMouseEvent *event)
{
    endX = event->pos().x(); //鼠标相对于所在控件的位置
    endY = event->pos().y();
    update();
    QApplication::restoreOverrideCursor();

}
