/*
  Copyright 2016 Lucas Walter
*/
#ifndef RQT_PARSIAN_GUI_MONITOR_H
#define RQT_PARSIAN_GUI_MONITOR_H

#include <rqt_gui_cpp/plugin.h>
#include <QWidget>
#include <QPushButton>
#include <rqt_parsian_gui/monitorWidget.h>
#include <pluginlib/class_list_macros.h>
#include <parsian_msgs/parsian_world_model.h>
#include <parsian_util/core/worldmodel.h>



namespace rqt_parsian_gui
{

    class Monitor
            : public rqt_gui_cpp::Plugin
    {
    Q_OBJECT
    public:

        Monitor();
        virtual void initPlugin(qt_gui_cpp::PluginContext& context);
        virtual void shutdownPlugin();
        virtual void saveSettings(qt_gui_cpp::Settings& plugin_settings,
                                  qt_gui_cpp::Settings& instance_settings) const;
        virtual void restoreSettings(const qt_gui_cpp::Settings& plugin_settings,
                                     const qt_gui_cpp::Settings& instance_settings);

        ros::NodeHandle n;
        ros::NodeHandle n_private;

        ros::Subscriber wm_sub;
        ros::Subscriber draw_sub;

        ros::Timer timer;


        double radius = 0.0215;


        void wmCb(const parsian_msgs::parsian_world_modelConstPtr& _wm);
        void drawCb(const parsian_msgs::parsian_drawConstPtr& _draw);
        void timerCb(const ros::TimerEvent& _timer);

        // Comment in to signal that the plugin has a way to configure it
        // bool hasConfiguration() const;
        // void triggerConfiguration();
    private:
        QWidget* widget_;

        parsian_msgs::parsian_world_modelConstPtr mywm;
        MonitorWidget* fieldWidget;
    };
}  // namespace rqt_example_cpp
#endif  // RQT_EXAMPLE_CPP_MY_PLUGIN_H
