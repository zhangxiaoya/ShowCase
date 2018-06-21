#include "boundingbox.h"

BoundingBox::BoundingBox(int bx,int by, int ex , int ey):
    _begX(bx),
    _begY(by),
    _endX(ex),
    _endY(ey)
{

}

int BoundingBox::begX() const
{
    return this->_begX;
}

int BoundingBox::endY() const
{
    return this->_endY;
}

int BoundingBox::endX() const
{
    return this->_endX;
}

int BoundingBox::begY() const
{
    return this->_begY;
}
