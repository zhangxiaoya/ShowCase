#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H


class BoundingBox
{
public:
    BoundingBox(int bx =0,int by = 0, int ex = 0, int ey =0);

    int begX() const;
    int begY() const;
    int endX() const;
    int endY() const;

private:
    int _begX;
    int _begY;
    int _endX;
    int _endY;
};

#endif // BOUNDINGBOX_H
