#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include "framewindow.h"
#include "QtAwesome.h"
#include "controlboard.h"
#include "defog.h"
#include "removeraincpu.h"
#include <thread>
#include <mutex>
#include "SuperResolutionBase.h"

QT_BEGIN_NAMESPACE
class QAction;
class QListWidget;
class QMenu;
class QTextEdit;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QApplication* app);

public:
    void enableDrawBox();
    void disableDrawBox();

private slots:
    void openVideoFile();
    void runProcess();
    void setting();
    void about();
    void ShowFinishedMessageSlot();
    void ShowFinishedStatusSlot();
    void GetBox(BoundingBox& box);

    void detection();
    void superResolution();
    void defogging();
    void removeRain();

signals:
    void ShowFinishMessageSignal();
    void ShowFinishStatusSignal();

private:
    void createActions();
    void createStatusBar();
    void createDockWindows();

    void DisableZoomActions(bool flag);
    void DisableFileActions(bool flag);


    // declare thread funcs
    static void DetectionReadFrame(cv::VideoCapture* pcapture);
    static void DetectionShowFrame(FrameWindow* frameWindow);
    static void DetectionFinished(MainWindow* mainWindow);

    static void SuperResolutionReadFrame();
    static void SuperResolutionShowFrame(FrameWindow* frameWindow,FrameWindow *enhenceWindow,SuperResolutionBase* superResolution);
    static void SuperResolutionFinished(MainWindow* mainWindow);

    static void DeFogReadFrame(cv::VideoCapture* pcapture);
    static void DeFogShowFrame(FrameWindow* frameWindow, FrameWindow *enhenceWindow);
    static void DeFogFinished(MainWindow* mainWindow);

    static void RemoveRainReadFrame(cv::VideoCapture* pcapture);
    static void RemoveRainShowFrame(FrameWindow* frameWindow, FrameWindow *enhenceWindow, RemoveRainCpu* pRemoveRain);
    static void RemoveRainFinished(MainWindow* mainWindow, RemoveRainCpu* pRemoveRain);

    //removeRain Class
    RemoveRainCpu* pRemoveRain;

//    //storage buffer
//    ReadRingBuffer* readBuff;
//    ShowRingBuffer* showBuff;


    QListWidget *logList;

    // QtAwesome
    QtAwesome* awesome;

    // Dock Windows
    FrameWindow* enhencedFrameBorad;
    FrameWindow* superResolutionFrameBorad;

    ControlBoard* controlBoard;

    // Center Window
    FrameWindow* centerFrameBoard;

    // Save Video file full path
    QString VideoFilePath;

    // Video Capture
    cv::VideoCapture capture;

    // Read frame thread
    std::thread* pReadFrameThread;
    std::thread* pShowFrameThread;
    std::thread* pFinishedThread;

    // declare menus
    QMenu *viewMenu;

    // declare actions
    QAction* zoomInAct;
    QAction* zoomOutAct;
    QAction* normalSizeAct;
    QAction *openVideoAct;
    QAction *runAct;

    QAction* detectionAct;
    QAction* superResolutionAct;
    QAction* defogAct;
    QAction* removeRainAct;

    // BoundingBox
    BoundingBox box;
};

#endif
