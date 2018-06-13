#include "framewindow.h"

FrameWindow::FrameWindow(QWidget *parent) : QWidget(parent)
{
    // new frame board use qlabel and set object name
    this->pFrameBoard = new QLabel(this);
    this->pFrameBoard->setObjectName(QString::fromUtf8("FrameBorad"));

    // new main layout and set object name
    this->pMainLayout = new QGridLayout(this);
    this->pMainLayout->setObjectName(QString::fromUtf8("MainLayout"));

    // set layout for this widget
    this->setLayout(this->pMainLayout);
}

FrameWindow::~FrameWindow()
{
    delete this->pMainLayout;
    delete this->pFrameBoard;
}
