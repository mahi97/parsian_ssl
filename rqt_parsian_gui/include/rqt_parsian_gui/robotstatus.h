/*
  Copyright 2016 Lucas Walter
*/
#ifndef RQT_PARSIAN_GUI_RobotStatus_H
#define RQT_PARSIAN_GUI_RobotStatus_H

#include <rqt_gui_cpp/plugin.h>
#include <QWidget>

namespace rqt_parsian_gui
{

class RobotStatus
  : public rqt_gui_cpp::Plugin
{
  Q_OBJECT
public:
  RobotStatus();
  virtual void initPlugin(qt_gui_cpp::PluginContext& context);
  virtual void shutdownPlugin();
  virtual void saveSettings(qt_gui_cpp::Settings& plugin_settings,
      qt_gui_cpp::Settings& instance_settings) const;
  virtual void restoreSettings(const qt_gui_cpp::Settings& plugin_settings,
      const qt_gui_cpp::Settings& instance_settings);

  // Comment in to signal that the plugin has a way to configure it
  // bool hasConfiguration() const;
  // void triggerConfiguration();
private:
  QWidget* widget_;
};
}  // namespace rqt_example_cpp
#endif  // RQT_EXAMPLE_CPP_MY_PLUGIN_H
