#ifndef COMMONCONFIGNODELET_H
#define COMMONCONFIGNODELET_H

#include <nodelet/nodelet.h>
#include <ros/ros.h>
#include <pluginlib/class_list_macros.h>
#include <dynamic_reconfigure/server.h>
#include <parsian_agent/commonconfigConfig.h>

    class CommonConfig : public nodelet::Nodelet {
    private:

        virtual void onInit();

        ros::NodeHandle nh;
        ros::NodeHandle private_nh;

        boost::shared_ptr<dynamic_reconfigure::Server<agent_common_config::commonconfigConfig>> server;

//      void timerCb(const ros::TimerEvent &event);


    };

#endif //COMMONCONFIGNODELET_H
