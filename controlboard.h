#ifndef CONTROLBOARD_H
#define CONTROLBOARD_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "QtAwesome.h"

#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QGroupBox>

#include "boundingbox.h"

class ControlBoard : public QWidget
{
    Q_OBJECT
public:
    explicit ControlBoard(QtAwesome* awesome, QWidget *parent = nullptr);
    void SetBoundingBox(const BoundingBox& box);

signals:
    void enableDrawBoxSingal();
    void disableDrawBoxSignal();

private slots:
    void DrawBoundingBoxButtonClicked();
    void CancelDrawBoxButtonClicked();

private:
    QGroupBox* topGroup;

    QPushButton* BoundingBoxButton;
    QPushButton* CancelButton;
    QLabel* BoundingBoxLabel;

    QGroupBox* boundingBoxGroup;

    QLabel* begXLabel;
    QLabel* begYLabel;
    QLabel* endXLabel;
    QLabel* endYLabel;

    QSpinBox* begXSpinBox;
    QSpinBox* begYSpinBox;
    QSpinBox* endXSpinBox;
    QSpinBox* endYSpinBox;

    QtAwesome* awesome;
};

#endif // CONTROLBOARD_H
