#ifndef FRAMEWINDOW_H
#define FRAMEWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <opencv2/core.hpp>

#include <imagelabel.h>

class QScrollBar;

class FrameWindow : public QWidget
{
    Q_OBJECT
public:
    explicit FrameWindow(QWidget *parent = nullptr);

    ~FrameWindow();

    void SetFrame(cv::Mat& frame);

    void SetDrawFlag(const bool flag);

signals:

public slots:
    void zoomIn();
    void zoomOut();
    void normalSize();

protected:
    void UpdateFrame();

private:
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

private:
    // Label for show mat
    ImageLabel* pFrameBoard;
    // Main layout for this widget
    QGridLayout* pMainLayout;
    // QScroll Area
    QScrollArea* scrollArea;

    // Current frame
    cv::Mat currentFrame;
    QImage currentImage;

    // zoom factor
    double scaleFactor;
};

#endif // FRAMEWINDOW_H
