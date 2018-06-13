#ifndef FRAMEWINDOW_H
#define FRAMEWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>
#include <opencv2/core.hpp>

class FrameWindow : public QWidget
{
    Q_OBJECT
public:
    explicit FrameWindow(QWidget *parent = nullptr);

    ~FrameWindow();

    void SetFrame(cv::Mat& frame);

signals:

public slots:

protected:
    void UpdateFrame();

private:
    // Label for show mat
    QLabel* pFrameBoard;
    // Main layout for this widget
    QGridLayout* pMainLayout;
    QGridLayout* scrollAreaLayout;
    // QScroll Area
    QScrollArea* scrollArea;

    // Current frame
    cv::Mat currentFrame;
    QImage currentImage;
};

#endif // FRAMEWINDOW_H
