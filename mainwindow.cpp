#include <QtWidgets>
#include <QDebug>

#include "mainwindow.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <thread>
#include <fstream>
#include <iostream>

// Global variables for thread
static std::mutex FrameMutex;
static std::mutex FinishFlagMutex;
static cv::Mat frame;
static cv::Mat enhencedFrame;
static bool isFinished;
static bool isFinishShowFrame;
static int frameIndex;
static const int buffMaxStorageCount(10);
//! [1]
MainWindow::MainWindow(QApplication* app)
{
    // set  qt awesome
    this->awesome = new QtAwesome(app);
    this->awesome->initFontAwesome();

    // set center window
    this->centerFrameBoard = new FrameWindow(this);
    setCentralWidget(this->centerFrameBoard);

    // default video path
    this->VideoFilePath = tr("");

    createActions();
    createStatusBar();
    createDockWindows();

    // set windows title
    setWindowTitle(tr("ShowCase Demo"));
    setUnifiedTitleAndToolBarOnMac(true);

    // Disable Tools
    DisableZoomActions(true);

    // set maximize window by default
    this->setWindowState(Qt::WindowMaximized);

    // Custom SLOT and SIGNAL
    connect(this, SIGNAL(ShowFinishMessageSignal()), this, SLOT(ShowFinishedMessageSlot()));   // ShowFinish Message
    connect(this, SIGNAL(ShowFinishStatusSignal()), this, SLOT(ShowFinishedStatusSlot()));

    connect(this->controlBoard, &ControlBoard::enableDrawBoxSingal, this, &MainWindow::enableDrawBox);
    connect(this->controlBoard, &ControlBoard::disableDrawBoxSignal, this, &MainWindow::disableDrawBox);

    connect(this->centerFrameBoard, &FrameWindow::sendBoundingBox, this, &MainWindow::GetBox);
}
//! [1]

//! [2]
void MainWindow::openVideoFile()
{
    this->VideoFilePath = QFileDialog::getOpenFileName(this,
                                                       tr("Open Video"), "",
                                                       tr("Video Files (*.avi *.mp4)"));
    if(this->VideoFilePath.isEmpty())
    {
        return;
    }

    // set first frame
    this->capture.open(this->VideoFilePath.toStdString());
    if(!this->capture.isOpened())
    {
        QMessageBox::warning(this, tr("Info"), tr("Unable to open video file!"));
        return;
    }
    cv::Mat firstFrame;
    this->capture >>firstFrame;
    this->centerFrameBoard->SetFrame(firstFrame);

    // set default finish flag
    isFinished = false;

    // Enable Tools
    DisableZoomActions(false);
}
//! [2]

//! [3]
void MainWindow::DetectionFinished(MainWindow *mainWindow)
{
    FinishFlagMutex.lock();
    while (!isFinished)
    {
        FinishFlagMutex.unlock();
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }

    // if finished ...
    mainWindow->DisableZoomActions(false);
    mainWindow->DisableFileActions(false);
    emit mainWindow->ShowFinishedStatusSlot();
    emit mainWindow->ShowFinishMessageSignal();

    mainWindow->VideoFilePath.clear();
    mainWindow->capture.release();
}
//! [3]

//! [4]
void MainWindow::runProcess()
{
    // Check
    if(this->VideoFilePath.isEmpty() || !this->capture.isOpened())
    {
       QMessageBox::warning(this, tr("Warning"),tr("Place open one video file first!"));
       return;
    }

    // Disable some actions
    DisableZoomActions(true);
    DisableFileActions(true);

    // Update status
    statusBar()->showMessage(tr("Processing..."));

    // Luanch threads read, show and finished
    this->pReadFrameThread = new std::thread(DetectionReadFrame, &(this->capture));
    this->pShowFrameThread = new std::thread(DetectionShowFrame, centerFrameBoard);
    this->pFinishedThread = new std::thread(DetectionFinished, this);
}
//! [4]

//! [5]
void MainWindow::DetectionReadFrame(cv::VideoCapture* pcapture)
{
    while(true)
    {
        FrameMutex.lock();
        pcapture->read(frame);
        if(frame.empty())
        {
            FrameMutex.unlock();
            break;
        }
        frameIndex++;
        FrameMutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
//! [5]

//! [6]
void MainWindow::DetectionShowFrame(FrameWindow* frameWindow)
{
    while(true)
    {
        FrameMutex.lock();
        if(frame.empty())
        {
            FinishFlagMutex.lock();
            isFinished = true;
            FinishFlagMutex.unlock();
            FrameMutex.unlock();
            break;
        }

        int xmin, xmax,ymin,ymax;
        double score;
        std::string label;
        std::string item;

        char resultFileNameBuff[256];
        char resultFileNameFormat[] = "/home/runisys/Desktop/CodeBag/Demo/Data/result/result_%06d.txt";
        std::sprintf(resultFileNameBuff, resultFileNameFormat, frameIndex);
        std::string filename(resultFileNameBuff);
        std::ifstream fin(filename);

        int font_face = cv::FONT_HERSHEY_COMPLEX;
        double font_scale = 1;
        int thickness = 2;
        int baseline;

        while(fin)
        {
            fin >> item;
            if(item.empty())
                break;
            xmin = std::stoi(item);
            fin >> item;
            ymin = std::stoi(item);
            fin >> item;
            xmax = std::stoi(item);
            fin >> item;
            ymax = std::stoi(item);
            fin >> item;
            score = std::stod(item);
            fin >> label;

            cv::rectangle(frame,cv::Rect(cv::Point(xmin, ymin), cv::Point(xmax, ymax)),cv::Scalar(255,255,0),2);
            cv::putText(frame,label,cv::Point(xmin,ymin),font_face,font_scale,cv::Scalar(0,255,255),thickness);
        }

        frameWindow->SetFrame(frame);
        FrameMutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
//! [6]

//! [7]
void MainWindow::setting()
{
}
//! [7]

//! [8]
void MainWindow::ShowFinishedMessageSlot()
{
    QMessageBox::information(this, tr("Information"),tr("All Done!"));
}
//! [8]

//! [9]
void MainWindow::about()
{
    QMessageBox::about(this, tr("About Showcase Demo"),
                       tr("The <b>Showcase Demo</b> demonstrates how to enhence video, "
                          "and how to detect target from video file. "
                          "And this just a showcase for part of algorithm, "
                          "the complete solution place contact me."));
}
//! [9]

//! [10]
void MainWindow::createActions()
{
    /**************************************************/
    /*   File Menu and Toolbar                        */
    /**************************************************/

    // create menu for file
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    // create tool bar for file
    QToolBar *fileToolBar = addToolBar(tr("File"));

    // create open video file action and set props for this action
    openVideoAct = new QAction(this->awesome->icon(fa::filevideoo), tr("&Open Video"), this);
    openVideoAct->setShortcuts(QKeySequence::Open);
    openVideoAct->setStatusTip(tr("Open One Video File"));

    // connect signal of open file action to open video file slot
    connect(openVideoAct, &QAction::triggered, this, &MainWindow::openVideoFile);

    // add openfile action to menu(file)
    fileMenu->addAction(openVideoAct);
    // add openfile action to toolbar(file)
    fileToolBar->addAction(openVideoAct);

    // create play action and set props
    runAct = new QAction(this->awesome->icon(fa::play), tr("&Play"), this);
    runAct->setShortcuts(QKeySequence::Refresh);
    runAct->setStatusTip(tr("Run Process for current Video"));

    // connect signal of run action to runProcess slot
    connect(runAct, &QAction::triggered, this, &MainWindow::runProcess);

    // add run action to file menu
    fileMenu->addAction(runAct);
    // add run action to file toolbar
    fileToolBar->addAction(runAct);

    // file menu add separator
    fileMenu->addSeparator();

    // create quit action and add to file menu
    QAction *quitAct = fileMenu->addAction(this->awesome->icon(fa::poweroff),tr("&Quit"), this, &QWidget::close);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit the Showcase Demo"));

    /**************************************************/
    /*   Setting Menu and Toolbar                     */
    /**************************************************/

    // Create Setting menu and toolbar
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));

    // create setting action and set props
    QAction *settingAct = new QAction(this->awesome->icon(fa::cog), tr("&Setting"), this);
    settingAct->setShortcuts(QKeySequence::Preferences);
    settingAct->setStatusTip(tr("Setting"));

    // connet setting action signal to setting slot
    connect(settingAct, &QAction::triggered, this, &MainWindow::setting);

    // add setting action to edit menu
    editMenu->addAction(settingAct);
    // add setting action to edit toolbar
    editToolBar->addAction(settingAct);

    /**************************************************/
    /*   Tool Menu and Toolbar                        */
    /**************************************************/

    QMenu *toolMenu = menuBar()->addMenu(tr("&Tools"));
    QToolBar* toolToolBar = addToolBar(tr("Tool"));

    zoomInAct = new QAction(this->awesome->icon(fa::searchplus), tr("Zoom&In"), this);
    zoomOutAct = new QAction(this->awesome->icon(fa::searchminus), tr("Zoom&Out"), this);
    normalSizeAct = new QAction(this->awesome->icon(fa::circle), tr("NormalSize"), this);

    zoomInAct->setShortcuts(QKeySequence::ZoomIn);
    zoomOutAct->setShortcuts(QKeySequence::ZoomOut);
    normalSizeAct->setShortcut(tr("Ctrl+o"));
    zoomInAct->setStatusTip(tr("ZoomIn"));
    zoomOutAct->setStatusTip(tr("ZoomOut"));
    normalSizeAct->setStatusTip(tr("NormalSize"));

    connect(zoomInAct, &QAction::triggered, this->centerFrameBoard, &FrameWindow::zoomIn);
    connect(zoomOutAct, &QAction::triggered, this->centerFrameBoard, &FrameWindow::zoomOut);
    connect(normalSizeAct, &QAction::triggered, this->centerFrameBoard, &FrameWindow::normalSize);

    toolMenu->addAction(zoomInAct);
    toolMenu->addAction(zoomOutAct);
    toolMenu->addAction(normalSizeAct);
    toolToolBar->addAction(zoomInAct);
    toolToolBar->addAction(zoomOutAct);
    toolToolBar->addAction(normalSizeAct);

    /**************************************************/
    /*   View Menu and Toolbar                        */
    /**************************************************/

    // create view menu
    viewMenu = menuBar()->addMenu(tr("&View"));

    menuBar()->addSeparator();

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));

    QAction *aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));

    /******/
    QMenu *showCaseMenu = menuBar()->addMenu(tr("&ShowCase"));
    QToolBar* showCaseToolBar = addToolBar(tr("ShowCase"));

    this->detectionAct = new QAction(this->awesome->icon(fa::binoculars), tr("&Detection"), this);
    this->superResolutionAct = new QAction(this->awesome->icon(fa::arrows), tr("&SuperResolution"), this);
    this->defogAct = new QAction(this->awesome->icon(fa::adjust), tr("&Defogging"), this);
    this->removeRainAct = new QAction(this->awesome->icon(fa::remove), tr("&RemoveRain"), this);
    this->detectionAct->setStatusTip(tr("Show case about Object Detection"));
    this->superResolutionAct->setStatusTip(tr("Show case about Super Resolution"));
    this->defogAct->setStatusTip(tr("Defogging"));
    this->removeRainAct->setStatusTip(tr("Remove Rain from video"));

    showCaseMenu->addAction(detectionAct);
    showCaseMenu->addAction(superResolutionAct);
    showCaseMenu->addAction(defogAct);
    showCaseMenu->addAction(removeRainAct);

    showCaseToolBar->addAction(detectionAct);
    showCaseToolBar->addAction(superResolutionAct);
    showCaseToolBar->addAction(defogAct);
    showCaseToolBar->addAction(removeRainAct);

    connect(detectionAct, &QAction::triggered, this, &MainWindow::detection);
    connect(superResolutionAct, &QAction::triggered, this, &MainWindow::superResolution);
    connect(defogAct, &QAction::triggered, this, &MainWindow::defogging);
    connect(removeRainAct, &QAction::triggered, this, &MainWindow::removeRain);

}
//! [10]

//! [11]
void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}
//! [11]

//! [12]
void MainWindow::createDockWindows()
{

    QDockWidget *enhencedFrameDock = new QDockWidget(tr("EnhencedFrame"), this);
    enhencedFrameDock->setAllowedAreas(Qt::BottomDockWidgetArea |
                                       Qt::TopDockWidgetArea |
                                       Qt::RightDockWidgetArea |
                                       Qt::LeftDockWidgetArea);
    this->enhencedFrameBorad = new FrameWindow(enhencedFrameDock);
    this->enhencedFrameBorad->setMinimumHeight(300);
    enhencedFrameDock->setWidget(this->enhencedFrameBorad);
    addDockWidget(Qt::BottomDockWidgetArea, enhencedFrameDock);

    QDockWidget *superResolutionFrameDock = new QDockWidget(tr("SuperResolutionFrame"), this);
    superResolutionFrameDock->setAllowedAreas(Qt::BottomDockWidgetArea |
                                       Qt::TopDockWidgetArea |
                                       Qt::RightDockWidgetArea |
                                       Qt::LeftDockWidgetArea);
    this->enhencedFrameBorad = new FrameWindow(enhencedFrameDock);
    this->enhencedFrameBorad->setMinimumHeight(300);
    enhencedFrameDock->setWidget(this->enhencedFrameBorad);
    addDockWidget(Qt::BottomDockWidgetArea, enhencedFrameDock);


    this->superResolutionFrameBorad = new FrameWindow(superResolutionFrameDock);
    this->superResolutionFrameBorad->setMinimumHeight(300);
    this->superResolutionFrameBorad->setMinimumWidth(300);
    superResolutionFrameDock->setWidget(this->superResolutionFrameBorad);
    addDockWidget(Qt::RightDockWidgetArea, superResolutionFrameDock);

    QDockWidget* controlBoradDock = new QDockWidget(tr("ControlBoard"), this);
    controlBoradDock->setAllowedAreas(Qt::RightDockWidgetArea| Qt::LeftDockWidgetArea);
    this->controlBoard = new ControlBoard(awesome, controlBoradDock);
    controlBoradDock->setWidget(this->controlBoard);
    addDockWidget(Qt::RightDockWidgetArea, controlBoradDock);

    QDockWidget* logDock = new QDockWidget(tr("Logs"), this);
    logDock->setAllowedAreas(Qt::BottomDockWidgetArea |
                             Qt::TopDockWidgetArea |
                             Qt::RightDockWidgetArea |
                             Qt::LeftDockWidgetArea);
    this->logList = new QListWidget(logDock);
    this->logList->setMinimumHeight(100);
    logDock->setWidget(this->logList);
    addDockWidget(Qt::RightDockWidgetArea, logDock);

    // set corner to right and left
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner,Qt::LeftDockWidgetArea);

    // viewMenu->addAction(enhencedFrameDock->toggleViewAction());
}

//! [13]
void MainWindow::DisableZoomActions(bool flag)
{
    this->zoomInAct->setDisabled(flag);
    this->zoomOutAct->setDisabled(flag);
    this->normalSizeAct->setDisabled(flag);
}
//! [13]

//! [14]
void MainWindow::DisableFileActions(bool flag)
{
    this->openVideoAct->setDisabled(flag);
    this->runAct->setDisabled(flag);
}
//! [14]

//! [15]
void MainWindow::ShowFinishedStatusSlot()
{
    // Crashed !!! TODO
//    statusBar()->showMessage(tr("Done!"));
}
//! [15]

void MainWindow::enableDrawBox()
{
    this->centerFrameBoard->SetDrawFlag(true);
}

void MainWindow::disableDrawBox()
{
    this->centerFrameBoard->SetDrawFlag(false);
}

void MainWindow::GetBox(BoundingBox &box)
{
    this->box = BoundingBox(box.begX(),box.begY(),box.endX(), box.endY());

    this->controlBoard->SetBoundingBox(this->box);
}

void MainWindow::detection()
{
    // open video file
    std::string videofilepath = "/home/runisys/Desktop/CodeBag/Demo/Data/video.avi";
    this->capture.open(videofilepath);
    if(!this->capture.isOpened())
    {
        QMessageBox::warning(this, tr("Info"), tr("Unable to open video file!"));
        return;
    }

    // set default finish flag
    isFinished = false;

    // Disable some actions
    DisableZoomActions(true);
    DisableFileActions(true);

    // Reset Frame Index;
    frameIndex = -1;

    // Update status
    statusBar()->showMessage(tr("Processing..."));

    // Luanch threads read, show and finished
    this->pReadFrameThread = new std::thread(DetectionReadFrame, &(this->capture));
    this->pShowFrameThread = new std::thread(DetectionShowFrame, centerFrameBoard);
    this->pFinishedThread = new std::thread(DetectionFinished, this);
}

void MainWindow::superResolution()
{
    // TO-DO
    QMessageBox::warning(this, tr("Info"), tr("Super Resolution Showcase!"));
}

void MainWindow::defogging()
{
    // TO-DO Zhang Zheng
    // open video file
    std::string videofilepath = "C:\\Users\\zerm1\\Desktop\\fog.avi";
    this->capture.open(videofilepath);
    if(!this->capture.isOpened())
    {
        QMessageBox::warning(this, tr("Info"), tr("Unable to open video file!"));
        return;
    }

    // set default finish flag
    isFinished = false;
    isFinishShowFrame = false;

    // Disable some actions
    DisableZoomActions(true);
    DisableFileActions(true);

    // Reset Frame Index;
    frameIndex = -1;

    // Update status
    statusBar()->showMessage(tr("Processing..."));

    capture >> frame;
    cv::resize(frame,frame,Size(1080,720));
    frame.copyTo(enhencedFrame);

    pReadFrameThread = new std::thread(DeFogReadFrame, &(this->capture));
    pShowFrameThread = new std::thread(DeFogShowFrame, centerFrameBoard, enhencedFrameBorad);
    pFinishedThread = new std::thread(DeFogFinished, this);
}

void MainWindow::removeRain()
{
    // TO-DO Zhang Zheng
    std::string videofilepath = "C:\\Users\\zerm1\\Desktop\\testSnow.mp4";
    this->capture.open(videofilepath);
    if(!this->capture.isOpened())
    {
        QMessageBox::warning(this, tr("Info"), tr("Unable to open video file!"));
        return;
    }

    capture >> frame;
    cv::resize(frame,frame,Size(1080,720));
    pRemoveRain = new RemoveRainCpu(frame);

    isFinishShowFrame = false;

    pReadFrameThread = new std::thread(RemoveRainReadFrame, &(this->capture));
    pShowFrameThread = new std::thread(RemoveRainShowFrame, centerFrameBoard, enhencedFrameBorad, this->pRemoveRain);
    pFinishedThread = new std::thread(RemoveRainFinished, this, this->pRemoveRain);

}

void MainWindow::SuperResolutionReadFrame(cv::VideoCapture *pcapture)
{
    // TO-DO Zhyn

}

void MainWindow::SuperResolutionShowFrame(FrameWindow *frameWindow)
{
    // TO-DO Zhyn
}

void MainWindow::SuperResolutionFinished(MainWindow *mainWindow)
{
    // TO-DO Zhyn
}

void MainWindow::DeFogReadFrame(cv::VideoCapture *pcapture)
{
    // TO-DO Zhang Zheng
    while(true)
    {
        //read frame from pcapture
        FrameMutex.lock();
        pcapture->read(frame);
        if(frame.empty())
        {
            FrameMutex.unlock();
            break;
        }
        cv::resize(frame,frame,Size(1080,720));
        FrameMutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void MainWindow::DeFogShowFrame(FrameWindow *frameWindow, FrameWindow *enhenceWindow)
{
    // TO-DO Zhang Zheng
    while(true)
    {
        FrameMutex.lock();
        if(frame.empty())
        {
            FinishFlagMutex.lock();
            isFinishShowFrame = true;
            FinishFlagMutex.unlock();
            FrameMutex.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            break;
        }

        Defog::GetDefogImg(frame,enhencedFrame,5,30,0.001);
        frameWindow->SetFrame(frame);
        enhenceWindow->SetFrame(enhencedFrame);
        FrameMutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void MainWindow::DeFogFinished(MainWindow *mainWindow)
{
    // TO-DO Zhang Zheng
    FinishFlagMutex.lock();
    while (!isFinishShowFrame)
    {
        FinishFlagMutex.unlock();
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
    FinishFlagMutex.unlock();
    mainWindow->DisableZoomActions(false);
    mainWindow->DisableFileActions(false);
    emit mainWindow->ShowFinishedStatusSlot();
    emit mainWindow->ShowFinishMessageSignal();

    mainWindow->VideoFilePath.clear();
    mainWindow->capture.release();

}

void MainWindow::RemoveRainReadFrame(cv::VideoCapture *pcapture)
{
    // TO-DO Zhang Zheng
    while(true)
    {
        FrameMutex.lock();
        pcapture->read(frame);
        if(frame.empty())
        {
            FrameMutex.unlock();
            break;
        }
        cv::resize(frame,frame,Size(1080,720));
        FrameMutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void MainWindow::RemoveRainShowFrame(FrameWindow *frameWindow, FrameWindow *enhenceWindow, RemoveRainCpu* pRemoveRain)
{
    // TO-DO Zhang Zheng
    time_t t1, t2;
    while (true)
    {
        FrameMutex.lock();
        if(frame.empty())
        {
            FinishFlagMutex.lock();
            isFinishShowFrame = true;
            FinishFlagMutex.unlock();
            FrameMutex.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            break;
        }
        t1 = clock();
        pRemoveRain->NewGetRemoveRainFrame(frame, enhencedFrame);
        t2 = clock();
        std::cout<<"RemoveRain use time is " << t2 - t1 << std::endl;
        frameWindow->SetFrame(frame);
        enhenceWindow->SetFrame(enhencedFrame);
        FrameMutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void MainWindow::RemoveRainFinished(MainWindow *mainWindow, RemoveRainCpu* pRemoveRain)
{
    // TO-DO Zhang Zheng
    FinishFlagMutex.lock();
    while (!isFinishShowFrame)
    {
        FinishFlagMutex.unlock();
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
    FinishFlagMutex.unlock();
    mainWindow->DisableZoomActions(false);
    mainWindow->DisableFileActions(false);
    emit mainWindow->ShowFinishedStatusSlot();
    emit mainWindow->ShowFinishMessageSignal();

    mainWindow->VideoFilePath.clear();
    mainWindow->capture.release();
    delete pRemoveRain;
}
