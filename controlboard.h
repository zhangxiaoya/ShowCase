#ifndef CONTROLBOARD_H
#define CONTROLBOARD_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "QtAwesome.h"

#include <QPushButton>
#include <QLabel>

class ControlBoard : public QWidget
{
    Q_OBJECT
public:
    explicit ControlBoard(QtAwesome* awesome, QWidget *parent = nullptr);

signals:

public slots:
private:
    QPushButton* BoundingBox;
    QLabel* Box;

    QtAwesome* awesome;
};

#endif // CONTROLBOARD_H
