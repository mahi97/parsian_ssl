//
// Created by noOne on 11/14/2017.
//
#include <rqt_parsian_gui/modeChooserWidget.h>

namespace rqt_parsian_gui
{

ModeChooserWidget::ModeChooserWidget():QWidget()
{
    color =yellow;
    side =left;
    mode =simulation;



    colorStr[yellow]="YELLOW";
    colorStr[!yellow]="bLUE";

    sideStr[left]="LEFT";
    sideStr[!left]="RIGHT";

    modeStr[simulation]="SIMULATION";
    modeStr[!simulation]="REAL";

    mainLayout=new QGridLayout;

    colorPB =new QPushButton(colorStr[color]);
    modePB =new QPushButton(modeStr[mode]);
    sidePB =new QPushButton(sideStr[side]);


    mainLayout->addWidget(colorPB);
    mainLayout->addWidget(sidePB);
    mainLayout->addWidget(modePB);

    this->setLayout(mainLayout);

    connect(colorPB,SIGNAL(clicked(bool)),this,SLOT(toggleColor()));
    connect(sidePB,SIGNAL(clicked(bool)),this,SLOT(toggleSide()));
    connect(modePB,SIGNAL(clicked(bool)),this,SLOT(toggleMode()));


}

void ModeChooserWidget::toggleMode()
{
    mode=!mode;
    modePB->setText(modeStr[mode]);

}

void ModeChooserWidget::toggleColor()
{
    color=!color;
    colorPB->setText(colorStr[color]);
}

void ModeChooserWidget::toggleSide()
{
    side=!side;
    sidePB->setText(sideStr[side]);
}
}


