/*
  Copyright 2016 Lucas Walter
*/

#include "rqt_parsian_gui/robotstatus.h"

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
  // create QWidget
  statusWidget = new RobotStatusWidget(n);
  statusWidget->setWindowTitle("RobotStatus");

  // extend the widget with all attributes and children from UI file

  // add widget to the user interface
  context.addWidget(statusWidget);
}

}  // namespace rqt_example_cpp
PLUGINLIB_EXPORT_CLASS(rqt_parsian_gui::RobotStatus, rqt_gui_cpp::Plugin)
