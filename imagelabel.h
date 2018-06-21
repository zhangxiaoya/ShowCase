#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QPainter>
#include <QPen>
#include <QRect>
#include <QCursor>
#include <QApplication>
#include <QPixmap>
#include <QImage>
#include <QLabel>
#include <QMouseEvent>

class ImageLabel : public QLabel
{
public:
    ImageLabel();

    void paintEvent(QPaintEvent* event);

    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

    void resizeBoundingBox(double factor);

    void SetDrawFlag(const bool flag);

private:
    int begX;
    int begY;
    int endX;
    int endY;

    bool drawFlag;
};

#endif // IMAGELABEL_H
