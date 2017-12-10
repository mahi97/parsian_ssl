/*
  Copyright 2016 Lucas Walter
*/

#include "rqt_parsian_gui/robotstatus.h"

class parsian_robot_status;
namespace rqt_parsian_gui
{

RobotStatus::RobotStatus()
  : rqt_gui_cpp::Plugin()
{
  // Constructor is called first before initPlugin function, needless to say.
  // give QObjects reasonable names
  setObjectName("RobotStatus");
}

void RobotStatus::initPlugin(qt_gui_cpp::PluginContext& context)
{
    n = getNodeHandle();
    n_private = getPrivateNodeHandle();

    rs_sub = n_private.subscribe("/robots_status",1000,&RobotStatus::rsCallback,this);
    for (int j = 0; j < max_robot; ++j) {
        QString sub_name = QString("/robot_command")+QString::number(j);
        rc_sub[j] = n_private.subscribe(sub_name.toStdString(),1000,&RobotStatus::rcCallback,this);
    }

  // create QWidget
    scroll_widget = new QWidget;
    scrollArea =new QScrollArea;
    scrollArea_l = new QVBoxLayout;

    scrollArea_l->setSizeConstraint(QLayout::SetMinimumSize);
    scroll_widget->setLayout(scrollArea_l);

    scrollArea->setWidgetResizable(true);
    scrollArea->setWindowTitle("RobotStatus");
    scrollArea->setFixedHeight(700);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setWidget(scroll_widget);

    for(auto &i : statusWidget) {
        i = new RobotStatusWidget(parsian_msgs::parsian_team_config::isYellow);
        scrollArea_l->addWidget(i);
    }



  // extend the widget with all attributes and children from UI file

  // add widget to the user interface

    context.addWidget(scrollArea);
}
    void RobotStatus::rsCallback(const parsian_msgs::parsian_robots_statusConstPtr msg){
        int counter=0;
        for(auto &i : statusWidget) {
            i->setMessage(msg->status[counter++]);
            break;
        }
    }

    void RobotStatus::rcCallback(const parsian_msgs::parsian_robot_commandConstPtr msg){

            statusWidget[msg->robot_id]->setVel(*msg);

    }
}  // namespace rqt_example_cpp
PLUGINLIB_EXPORT_CLASS(rqt_parsian_gui::RobotStatus, rqt_gui_cpp::Plugin)
