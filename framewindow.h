#ifndef FRAMEWINDOW_H
#define FRAMEWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>

class FrameWindow : public QWidget
{
    Q_OBJECT
public:
    explicit FrameWindow(QWidget *parent = nullptr);

    ~FrameWindow();

signals:

public slots:

private:
    QLabel* pFrameBoard;
    QGridLayout* pMainLayout;
};

#endif // FRAMEWINDOW_H
