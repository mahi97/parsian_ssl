//
// Created by noOne on 11/14/2017.
//
#include <rqt_parsian_gui/modeChooserWidget.h>

namespace rqt_parsian_gui
{
ModeChooserWidget::ModeChooserWidget(ros::NodeHandle & n)
    :QWidget()
{
    color =yellow;
    side =left;
    mode =simulation;

    team_config_pub=n.advertise<parsian_msgs::parsian_team_config>("team_config",1000);

    colorStr[yellow]="YELLOW";
    colorStr[!yellow]="BLUE";

    sideStr[left]="LEFT";
    sideStr[!left]="RIGHT";

    modeStr[simulation]="SIMULATION";
    modeStr[!simulation]="REAL";

    this->setFixedSize(75,100);

    mainLayout=new QGridLayout;

    colorPB =new QPushButton(colorStr[color]);
    modePB =new QPushButton(modeStr[mode]);
    sidePB =new QPushButton(sideStr[side]);

    colorPB->setFixedSize(70,30);
    modePB->setFixedSize(70,30);
    sidePB->setFixedSize(70,30);

    modeAct =new QAction(this);
    modeAct->setShortcut(*new QKeySequence(tr("Ctrl+S")));

    mainLayout->addWidget(colorPB);
    mainLayout->addWidget(sidePB);
    mainLayout->addWidget(modePB);

    this->setLayout(mainLayout);
    this->addAction(modeAct);

    connect(colorPB,SIGNAL(clicked(bool)),this,SLOT(toggleColor()));
    connect(sidePB,SIGNAL(clicked(bool)),this,SLOT(toggleSide()));
    connect(modePB,SIGNAL(clicked(bool)),this,SLOT(toggleMode()));
    connect(modeAct, SIGNAL(triggered(bool)), this, SLOT(toggleMode()));



}

void ModeChooserWidget::toggleMode()
{
    mode=!mode;
    modePB->setText(modeStr[mode]);

    sendTeamConfig();


}

void ModeChooserWidget::toggleColor()
{
    color=!color;
    colorPB->setText(colorStr[color]);
    sendTeamConfig();
}

void ModeChooserWidget::toggleSide()
{
    side=!side;
    sidePB->setText(sideStr[side]);
    sendTeamConfig();
}


void ModeChooserWidget::sendTeamConfig()
{

    parsian_msgs::parsian_team_configPtr team_config{new parsian_msgs::parsian_team_config};
    team_config->color=color;
    team_config->mode =mode;
    team_config->side =side;
    team_config_pub.publish(team_config);
}
}


