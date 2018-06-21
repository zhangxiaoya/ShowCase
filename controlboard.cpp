#include "controlboard.h"

ControlBoard::ControlBoard(QtAwesome* awesome, QWidget *parent) : QWidget(parent)
{
    this->awesome = awesome;

    this->topGroup = new QGroupBox(this);
    this->topGroup->setObjectName(tr("TopBox Group"));
    this->BoundingBoxButton = new QPushButton(this->awesome->icon(fa::squareo), tr(""),this->topGroup);
    this->BoundingBoxButton->setStatusTip(tr("Draw Bounding Box"));
    this->BoundingBoxButton->setIconSize(QSize(50, 50));
    this->BoundingBoxButton->setMinimumSize(QSize(50, 50));
    this->CancelButton = new QPushButton(this->awesome->icon(fa::times), tr(""), this->topGroup);
    this->CancelButton->setStatusTip(tr("Cancel Draw Bounding Box"));
    this->CancelButton->setIconSize(QSize(50, 50));
    this->CancelButton->setMinimumSize(QSize(50, 50));
    this->BoundingBoxLabel = new QLabel(tr("Draw Box"),this->topGroup);
    this->BoundingBoxLabel->setBuddy(this->BoundingBoxButton);

    this->CancelButton->setEnabled(false);
    this->BoundingBoxButton->setDisabled(false);

    QHBoxLayout *topLayout = new QHBoxLayout(this->topGroup);
    topLayout->addWidget(this->BoundingBoxButton);
    topLayout->addWidget(this->CancelButton);
    topLayout->addWidget(this->BoundingBoxLabel);

    // Bounding Box Group
    this->boundingBoxGroup = new QGroupBox(this);
    this->boundingBoxGroup->setObjectName(tr("BoundingBox Orientation"));

    QVBoxLayout* boundingBoxLayout = new QVBoxLayout(this->boundingBoxGroup);
    // beg X
    this->begXLabel = new QLabel(tr("Top Left X: "), this->boundingBoxGroup);
    this->begXSpinBox = new QSpinBox(this->boundingBoxGroup);
    this->begXLabel->setBuddy(this->begXSpinBox);
    QHBoxLayout * begXLayout = new QHBoxLayout;
    begXLayout->addWidget(this->begXLabel);
    begXLayout->addWidget(this->begXSpinBox);
    boundingBoxLayout->addLayout(begXLayout);

    // beg Y
    this->begYLabel = new QLabel(tr("Top Left Y: "), this->boundingBoxGroup);
    this->begYSpinBox = new QSpinBox(this->boundingBoxGroup);
    this->begYLabel->setBuddy(this->begYSpinBox);
    QHBoxLayout * begYLayout = new QHBoxLayout;
    begYLayout->addWidget(this->begYLabel);
    begYLayout->addWidget(this->begYSpinBox);
    boundingBoxLayout->addLayout(begYLayout);

    // end X
    this->endXLabel = new QLabel(tr("Bottom Right X: "), this->boundingBoxGroup);
    this->endXSpinBox = new QSpinBox(this->boundingBoxGroup);
    this->endXLabel->setBuddy(this->endXSpinBox);
    QHBoxLayout * endXLayout = new QHBoxLayout;
    endXLayout->addWidget(this->endXLabel);
    endXLayout->addWidget(this->endXSpinBox);
    boundingBoxLayout->addLayout(endXLayout);

    // end Y
    this->endYLabel = new QLabel(tr("Bottom Right Y: "), this->boundingBoxGroup);
    this->endYSpinBox = new QSpinBox(this->boundingBoxGroup);
    this->endYLabel->setBuddy(this->endYSpinBox);
    QHBoxLayout * endYLayout = new QHBoxLayout;
    endYLayout->addWidget(this->endYLabel);
    endYLayout->addWidget(this->endYSpinBox);
    boundingBoxLayout->addLayout(endYLayout);


    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(this->topGroup);

    mainLayout->addWidget(this->boundingBoxGroup);

    QSpacerItem *verticalSpacer = new QSpacerItem(20,300);

    mainLayout->addItem(verticalSpacer);

    // Basic Signals and Slots
    connect(this->BoundingBoxButton, &QPushButton::clicked, this, &ControlBoard::DrawBoundingBoxButtonClicked);
    connect(this->CancelButton, &QPushButton::clicked, this, &ControlBoard::CancelDrawBoxButtonClicked);
}

void ControlBoard::DrawBoundingBoxButtonClicked()
{
    this->BoundingBoxButton->setDisabled(true);
    this->CancelButton->setEnabled(true);
    emit enableDrawBoxSingal();
}

void ControlBoard::CancelDrawBoxButtonClicked()
{
    this->CancelButton->setDisabled(true);
    this->BoundingBoxButton->setEnabled(true);
    emit disableDrawBoxSignal();
}
