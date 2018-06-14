
#include <QtWidgets>
#include <QDebug>

#include "mainwindow.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

//! [1]
MainWindow::MainWindow(QApplication* app)
{
    // set  qt awesome
    this->awesome = new QtAwesome(app);
    this->awesome->initFontAwesome();

    // set center window
    this->centerFrameBoard = new FrameWindow();
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
                                                       tr("Open Video"), "/home/",
                                                       tr("Video Files (*.avi *.mp4)"));
}
//! [2]

//! [3]

//! [3]

//! [4]
void MainWindow::runProcess()
{
    QMimeDatabase mimeDatabase;
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Choose a file name"), ".",
                                                    mimeDatabase.mimeTypeForName("text/html").filterString());
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Dock Widgets"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << textEdit->toHtml();
    QApplication::restoreOverrideCursor();

    statusBar()->showMessage(tr("Saved '%1'").arg(fileName), 2000);
}
//! [4]

//! [5]
void MainWindow::setting()
{
    QTextDocument *document = textEdit->document();
    document->undo();
}
//! [5]

//! [6]
void MainWindow::insertCustomer(const QString &customer)
{
    if (customer.isEmpty())
        return;
    QStringList customerList = customer.split(", ");
    QTextDocument *document = textEdit->document();
    QTextCursor cursor = document->find("NAME");
    if (!cursor.isNull()) {
        cursor.beginEditBlock();
        cursor.insertText(customerList.at(0));
        QTextCursor oldcursor = cursor;
        cursor = document->find("ADDRESS");
        if (!cursor.isNull()) {
            for (int i = 1; i < customerList.size(); ++i) {
                cursor.insertBlock();
                cursor.insertText(customerList.at(i));
            }
            cursor.endEditBlock();
        }
        else
            oldcursor.endEditBlock();
    }
}
//! [6]

//! [7]
void MainWindow::addParagraph(const QString &paragraph)
{
    if (paragraph.isEmpty())
        return;
    QTextDocument *document = textEdit->document();
    QTextCursor cursor = document->find(tr("Yours sincerely,"));
    if (cursor.isNull())
        return;
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor, 2);
    cursor.insertBlock();
    cursor.insertText(paragraph);
    cursor.insertBlock();
    cursor.endEditBlock();

}
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
    QAction *quitAct = fileMenu->addAction(this->awesome->icon(fa::timescircle),tr("&Quit"), this, &QWidget::close);
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
    QDockWidget *textWidgetDock = new QDockWidget(tr("Text"), this);
    textWidgetDock->setAllowedAreas(Qt::BottomDockWidgetArea |
                                    Qt::TopDockWidgetArea |
                                    Qt::RightDockWidgetArea |
                                    Qt::LeftDockWidgetArea);
    this->textEdit = new QTextEdit(textWidgetDock);
    this->textEdit->setMinimumHeight(300);
    textWidgetDock->setWidget(this->textEdit);
    addDockWidget(Qt::BottomDockWidgetArea, textWidgetDock);

    QDockWidget *enhencedFrameDock = new QDockWidget(tr("EnhencedFrame"), this);
    enhencedFrameDock->setAllowedAreas(Qt::BottomDockWidgetArea |
                                       Qt::TopDockWidgetArea |
                                       Qt::RightDockWidgetArea |
                                       Qt::LeftDockWidgetArea);
    this->enhencedFrameBorad = new FrameWindow(enhencedFrameDock);
    this->enhencedFrameBorad->setMinimumHeight(300);
    enhencedFrameDock->setWidget(this->enhencedFrameBorad);
    addDockWidget(Qt::BottomDockWidgetArea, enhencedFrameDock);

    QDockWidget *imageBoradDock = new QDockWidget(tr("ImageBoard"), this);
    imageBoradDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    this->originalFrameBoard = new FrameWindow(imageBoradDock);
    this->originalFrameBoard->setMinimumWidth(200);
    imageBoradDock->setWidget(this->originalFrameBoard);
    addDockWidget(Qt::LeftDockWidgetArea, imageBoradDock);

    // TO-DO test
    cv::Mat img = cv::imread("/home/runisys/Desktop/data/OpenCVImageData/data/lena.jpg");
    if(!img.empty())
    {
        this->enhencedFrameBorad->SetFrame(img);
        this->originalFrameBoard->SetFrame(img);
    }

    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner,Qt::LeftDockWidgetArea);

    QDockWidget *customersDock = new QDockWidget(tr("Customers"), this);
    customersDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    customerList = new QListWidget(customersDock);
    customerList->addItems(QStringList()
                           << "John Doe, Harmony Enterprises, 12 Lakeside, Ambleton"
                           << "Jane Doe, Memorabilia, 23 Watersedge, Beaton"
                           << "Tammy Shea, Tiblanka, 38 Sea Views, Carlton"
                           << "Tim Sheen, Caraba Gifts, 48 Ocean Way, Deal"
                           << "Sol Harvey, Chicos Coffee, 53 New Springs, Eccleston"
                           << "Sally Hobart, Tiroli Tea, 67 Long River, Fedula");
    customersDock->setWidget(customerList);
    addDockWidget(Qt::RightDockWidgetArea, customersDock);
    viewMenu->addAction(customersDock->toggleViewAction());

    QDockWidget *paragraphsDock = new QDockWidget(tr("Paragraphs"), this);
    paragraphsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    paragraphsList = new QListWidget(paragraphsDock);
    paragraphsList->addItems(QStringList()
                             << "Thank you for your payment which we have received today."
                             << "Your order has been dispatched and should be with you "
                                "within 28 days."
                             << "We have dispatched those items that were in stock. The "
                                "rest of your order will be dispatched once all the "
                                "remaining items have arrived at our warehouse. No "
                                "additional shipping charges will be made."
                             << "You made a small overpayment (less than $5) which we "
                                "will keep on account for you, or return at your request."
                             << "You made a small underpayment (less than $1), but we have "
                                "sent your order anyway. We'll add this underpayment to "
                                "your next bill."
                             << "Unfortunately you did not send enough money. Please remit "
                                "an additional $. Your order will be dispatched as soon as "
                                "the complete amount has been received."
                             << "You made an overpayment (more than $5). Do you wish to "
                                "buy more items, or should we return the excess to you?");
    paragraphsDock->setWidget(paragraphsList);
    addDockWidget(Qt::RightDockWidgetArea, paragraphsDock);
    viewMenu->addAction(paragraphsDock->toggleViewAction());

    connect(customerList, &QListWidget::currentTextChanged,
            this, &MainWindow::insertCustomer);
    connect(paragraphsList, &QListWidget::currentTextChanged,
            this, &MainWindow::addParagraph);
}
//! [9]
