/*
  Copyright 2016 Lucas Walter
*/
#ifndef RQT_PARSIAN_GUI_HANDUCONTROLLER_H
#define RQT_PARSIAN_GUI_HANDUCONTROLLER_H

#include <rqt_gui_cpp/plugin.h>
#include <ros/ros.h>
#include <QWidget>
#include <QPushButton>
#include <QKeyEvent>
#include <QString>
#include <pluginlib/class_list_macros.h>
#include <rqt_parsian_gui/ui_handycontroller.h>
#include <parsian_msgs/parsian_robot_command.h>
#include <parsian_util/base.h>
//#include <parsian_util/core/worldmodel.h>


// main TODOs:
//     1)the cmd publishes in /rqt_parsian_gui/robot_command0 not in the /robot_command0
//     2)the cmd is not Ptr and I dont know what to do
//     3)the robots does not act very fluent and smooth
//     4)the right and left actions doesnt work correctly

namespace rqt_parsian_gui
{
    class HandyController
            : public rqt_gui_cpp::Plugin/*, public QWidget*/
    {
    Q_OBJECT
    public:
//        class KeyEvent : public QWidget
//        {
//        public:
//            KeyEvent(QWidget *parent) : QWidget(parent){}
//        protected:
//            void keyPressEvent(QKeyEvent *event)
//            {
//                emit keypressed(event);
//            }
//        signals:
//            void keypressed(QKeyEvent *event);
//        };

        HandyController();
        virtual void initPlugin(qt_gui_cpp::PluginContext& context);
        virtual void shutdownPlugin();
        virtual void saveSettings(qt_gui_cpp::Settings& plugin_settings,
                                  qt_gui_cpp::Settings& instance_settings) const;
        virtual void restoreSettings(const qt_gui_cpp::Settings& plugin_settings,
                                     const qt_gui_cpp::Settings& instance_settings);


        virtual bool eventFilter(QObject *, QEvent *);
        void jacobian(double vx, double vy, double w, double &v1, double &v2, double &v3, double &v4);
        void takeAction(EActionType action);
        void setRobotVel(double _vtan , double _vnorm , double _w );
        void cleanRobotVel();
        void cleanRobotChip();
        void cleanRobotKick();


        ros::Publisher grsim_pub;
        parsian_msgs::parsian_robot_command cmd;


    public slots:
        void leftpressed();
        void uppressed();
        void rightpressed();
        void downpressed();
        void angle1pressed();
        void angle2pressed();
        void kickpressed();
        void chippressed();
        void released();
        void rollerpressed();
        void agentidchanged(QString);
        void speedchanged(int);
        void kickspeedchanged(int);
        void chipkickspeedchanged(int);
        void rollerspeedchanged(int);

        // Comment in to signal that the plugin has a way to configure it
        // bool hasConfiguration() const;
        // void triggerConfiguration();
    private:
        QWidget* widget_;
        Ui::HandyController mUI;
        int agentID, speed, kickspeed, chipkickspeed, rollerspeed;


    };
}       // namespace rqt_example_cpp
#endif  // RQT_PARSIAN_GUI_HANDUCONTROLLER_H
