
#include <QtWidgets>
#include <QDebug>

#include "mainwindow.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

//! [1]
MainWindow::MainWindow(QApplication* app)
{
    // set  qt awesome
    this->awesome = new QtAwesome(app);
    this->awesome->initFontAwesome();

    // set center window
    this->centerFrameBoard = new FrameWindow(this);
    setCentralWidget(this->centerFrameBoard);

    // TODO--test
    cv::Mat img = cv::imread("/home/runisys/Desktop/data/OpenCVImageData/data/lena.jpg");
    if(!img.empty())
    {
        this->centerFrameBoard->SetFrame(img);
    }

    // default video path
    this->VideoFilePath = tr("");

    createActions();
    createStatusBar();
    createDockWindows();

    // set windows title
    setWindowTitle(tr("ShowCase Demo"));

    setUnifiedTitleAndToolBarOnMac(true);

    // set maximize window by default
    this->setWindowState(Qt::WindowMaximized);
}
//! [1]

//! [2]
void MainWindow::openVideoFile()
{
    this->VideoFilePath = QFileDialog::getOpenFileName(this,
                                                       tr("Open Video"), "",
                                                       tr("Video Files (*.avi *.mp4)"));

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
}
//! [2]

//! [3]

//! [3]

//! [4]
void MainWindow::runProcess()
{
    if(this->VideoFilePath.isEmpty() || !this->capture.isOpened())
    {
       QMessageBox::warning(this, tr("Warning"),tr("Place open one video file first!"));
       return;
    }

    statusBar()->showMessage(tr("Processing..."));
}
//! [4]

//! [5]
void MainWindow::setting()
{
}
//! [5]

//! [6]
//! [6]

//! [7]

//! [7]

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Showcase Demo"),
                       tr("The <b>Showcase Demo</b> demonstrates how to enhence video, "
                          "and how to detect target from video file. "
                          "And this just a showcase for part of algorithm, "
                          "the complete solution place contact me."));
}

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
    QAction *openVideoAct = new QAction(this->awesome->icon(fa::filevideoo), tr("&Open Video"), this);
    openVideoAct->setShortcuts(QKeySequence::Open);
    openVideoAct->setStatusTip(tr("Open One Video File"));

    // connect signal of open file action to open video file slot
    connect(openVideoAct, &QAction::triggered, this, &MainWindow::openVideoFile);

    // add openfile action to menu(file)
    fileMenu->addAction(openVideoAct);
    // add openfile action to toolbar(file)
    fileToolBar->addAction(openVideoAct);

    // create play action and set props
    QAction *runAct = new QAction(this->awesome->icon(fa::play), tr("&Play"), this);
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

    QAction* zoomInAct = new QAction(this->awesome->icon(fa::searchplus), tr("Zoom&In"), this);
    QAction* zoomOutAct = new QAction(this->awesome->icon(fa::searchminus), tr("Zoom&Out"), this);
    QAction* normalSizeAct = new QAction(this->awesome->icon(fa::circle), tr("NormalSize"), this);
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
}

//! [8]
void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}
//! [8]

//! [9]
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

    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner,Qt::LeftDockWidgetArea);

    viewMenu->addAction(enhencedFrameDock->toggleViewAction());
}
//! [9]
