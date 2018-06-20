#include "framewindow.h"
#include <opencv2/imgproc.hpp>
#include <QDebug>

FrameWindow::FrameWindow(QWidget *parent) : QWidget(parent)
{
    // new frame board use qlabel and set object name
    this->pFrameBoard = new ImageLabel();
    this->pFrameBoard->setObjectName(QString::fromUtf8("FrameBorad"));
    this->pFrameBoard->setBackgroundRole(QPalette::Base);
    this->pFrameBoard->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
    this->pFrameBoard->setScaledContents(true);

    // Scroll Area
    this->scrollArea = new QScrollArea(this);
    this->scrollArea->setBackgroundRole(QPalette::Dark);
    this->scrollArea->setWidget(this->pFrameBoard);
    this->scrollArea->setVisible(true);
    this->scrollArea->setAlignment(Qt::AlignCenter);

    // new main layout and set object name
    this->pMainLayout = new QGridLayout(this);
    this->pMainLayout->setObjectName(QString::fromUtf8("MainLayout"));
    this->pMainLayout->addWidget(this->scrollArea, 0,0,1,1);

    // set layout for this widget
    this->setLayout(this->pMainLayout);

    // set default scale
    this->scaleFactor = 1.0;

}

FrameWindow::~FrameWindow()
{
    delete this->pMainLayout;
    delete this->pFrameBoard;
}

void FrameWindow::SetFrame(cv::Mat &frame)
{
    switch (frame.type()) {
    case CV_8UC1:
        cv::cvtColor(frame,this->currentFrame, CV_GRAY2RGB);
        break;
    case CV_8UC3:
        cv::cvtColor(frame, this->currentFrame, CV_BGR2RGB);
        break;
    default:
        break;
    }
    UpdateFrame();
}

void FrameWindow::UpdateFrame()
{
    assert(this->currentFrame.isContinuous());
    this->currentImage = QImage(this->currentFrame.data, this->currentFrame.cols,this->currentFrame.rows,this->currentFrame.cols*3,QImage::Format_RGB888);
    this->pFrameBoard->setPixmap(QPixmap::fromImage(this->currentImage));
    this->pFrameBoard->resize(QSize(this->currentFrame.cols, this->currentFrame.rows));
}

void FrameWindow::zoomIn()
{
    scaleImage(1.25);
}

void FrameWindow::zoomOut()
{
    scaleImage(0.8);
}

void FrameWindow::normalSize()
{
    double basicWidth =  static_cast<double>(this->currentFrame.cols);
    double currentWidth = static_cast<double>(this->pFrameBoard->size().width());
    double rescaleFactor = basicWidth / currentWidth;

    this->pFrameBoard->adjustSize();
    this->pFrameBoard->resizeBoundingBox(rescaleFactor);
    scaleFactor = 1.0;
}

void FrameWindow::scaleImage(double factor)
{
    Q_ASSERT(this->pFrameBoard->pixmap());

    scaleFactor *= factor;
    this->pFrameBoard->resize(this->scaleFactor * this->pFrameBoard->pixmap()->size());
    this->pFrameBoard->resizeBoundingBox(factor);

    adjustScrollBar(this->scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(this->scrollArea->verticalScrollBar(), factor);
}
void FrameWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}
