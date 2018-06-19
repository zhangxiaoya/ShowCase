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
    cursor.setShape(Qt::CrossCursor);
    QApplication::setOverrideCursor(cursor);
}

void ImageLabel::mouseReleaseEvent(QMouseEvent *event)
{
    endX = event->pos().x(); //鼠标相对于所在控件的位置
    endY = event->pos().y();
    update();
    QApplication::restoreOverrideCursor();
}

void ImageLabel::resizeBoundingBox(double factor)
{
    begX = static_cast<int>(static_cast<double>(begX) * factor);
    endX = static_cast<int>(static_cast<double>(endX) * factor);
    begY = static_cast<int>(static_cast<double>(begY) * factor);
    endY = static_cast<int>(static_cast<double>(endY) * factor);
}
