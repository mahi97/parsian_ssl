/*
  Copyright 2016 Lucas Walter
*/
#ifndef RQT_PARSIAN_GUI_RobotStatus_H
#define RQT_PARSIAN_GUI_RobotStatus_H

#include <rqt_gui_cpp/plugin.h>
#include <rqt_parsian_gui/robotstatus_widget.h>
#include <pluginlib/class_list_macros.h>
namespace rqt_parsian_gui
{

class RobotStatus : public rqt_gui_cpp::Plugin
{
  Q_OBJECT
public:
  RobotStatus();
  virtual void initPlugin(qt_gui_cpp::PluginContext& context);


private:
    ros::NodeHandle n;
    ros::NodeHandle n_private;
    RobotStatusWidget* statusWidget;
};
}  // namespace rqt_example_cpp
#endif  // RQT_EXAMPLE_CPP_MY_PLUGIN_H
