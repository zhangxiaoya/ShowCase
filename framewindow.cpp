#include "framewindow.h"
#include <opencv2/imgproc.hpp>

FrameWindow::FrameWindow(QWidget *parent) : QWidget(parent)
{

    // new frame board use qlabel and set object name
    this->pFrameBoard = new QLabel();
    this->pFrameBoard->setObjectName(QString::fromUtf8("FrameBorad"));
    this->pFrameBoard->setBackgroundRole(QPalette::Base);
    this->pFrameBoard->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
    this->pFrameBoard->setScaledContents(true);

    this->scrollAreaLayout = new QGridLayout();
    this->scrollAreaLayout->setObjectName(QString::fromUtf8("ScrollLayout"));
    this->scrollAreaLayout->addWidget(this->pFrameBoard,0,0,1,1);

    // Scroll Area
    this->scrollArea = new QScrollArea(this);
    this->scrollArea->setBackgroundRole(QPalette::Dark);
    this->scrollArea->setWidget(this->pFrameBoard);
    this->scrollArea->setVisible(true);
    this->scrollArea->setAlignment(Qt::AlignCenter);
    this->scrollArea->setLayout(this->scrollAreaLayout);

    // new main layout and set object name
    this->pMainLayout = new QGridLayout(this);
    this->pMainLayout->setObjectName(QString::fromUtf8("MainLayout"));
    this->pMainLayout->addWidget(this->scrollArea, 0,0,1,1);

    // set layout for this widget
    this->setLayout(this->pMainLayout);

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
    this->pFrameBoard->setFixedSize(this->currentFrame.cols, this->currentFrame.rows);
}
