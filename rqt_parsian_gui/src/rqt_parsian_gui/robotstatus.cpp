/*
  Copyright 2016 Lucas Walter
*/

#include "rqt_parsian_gui/robotstatus.h"
#include <pluginlib/class_list_macros.h>
#include <QStringList>

namespace rqt_parsian_gui
{

RobotStatus::RobotStatus()
  : rqt_gui_cpp::Plugin()
  , widget_(0)
{
  // Constructor is called first before initPlugin function, needless to say.

  // give QObjects reasonable names
  setObjectName("RobotStatus");
}

void RobotStatus::initPlugin(qt_gui_cpp::PluginContext& context)
{
  // access standalone command line arguments
  QStringList argv = context.argv();
  // create QWidget
  widget_ = new QWidget();
widget_->setWindowTitle("RobotStatus");

  // extend the widget with all attributes and children from UI file

  // add widget to the user interface
  context.addWidget(widget_);
}

void RobotStatus::shutdownPlugin()
{
  // unregister all publishers here
}

void RobotStatus::saveSettings(qt_gui_cpp::Settings& plugin_settings,
    qt_gui_cpp::Settings& instance_settings) const
{
  // instance_settings.setValue(k, v)
}

void RobotStatus::restoreSettings(const qt_gui_cpp::Settings& plugin_settings,
    const qt_gui_cpp::Settings& instance_settings)
{
  // v = instance_settings.value(k)
}

/*bool hasConfiguration() const
{
  return true;
}

void triggerConfiguration()
{
  // Usually used to open a dialog to offer the user a set of configuration
}*/

}  // namespace rqt_example_cpp
PLUGINLIB_EXPORT_CLASS(rqt_parsian_gui::RobotStatus, rqt_gui_cpp::Plugin)
