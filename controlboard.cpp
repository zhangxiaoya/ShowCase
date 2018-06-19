#include "controlboard.h"

ControlBoard::ControlBoard(QtAwesome* awesome, QWidget *parent) : QWidget(parent)
{
    this->awesome = awesome;

    this->BoundingBox = new QPushButton(this->awesome->icon(fa::squareo), tr(""));
    this->Box = new QLabel(tr("BoundingBox"),this);

    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(this->BoundingBox);
    horizontalLayout->addWidget(this->Box);

    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    verticalLayout->addLayout(horizontalLayout);

    QSpacerItem *verticalSpacer = new QSpacerItem(20,300);

    verticalLayout->addItem(verticalSpacer);
}
