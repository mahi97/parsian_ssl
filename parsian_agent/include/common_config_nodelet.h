#ifndef COMMONCONFIGNODELET_H
#define COMMONCONFIGNODELET_H

#include <nodelet/nodelet.h>
#include <ros/ros.h>
#include <pluginlib/class_list_macros.h>
#include <dynamic_reconfigure/server.h>
#include <parsian_agent/config.h>
#include <parsian_agent/commonconfigConfig.h>
#include <parsian_msgs/parsian_robot_common_config.h>

    class CommonConfig : public nodelet::Nodelet {
    private:

        virtual void onInit();

        ros::NodeHandle nh;
        ros::NodeHandle private_nh;

        boost::shared_ptr<dynamic_reconfigure::Server<agent_common_config::commonconfigConfig>> server;
        void ConfigServerCallBack(const agent_common_config::commonconfigConfig &config, uint32_t level) ;
        parsian_msgs::parsian_robot_common_configPtr getMsg(const agent_common_config::commonconfigConfig &config);

        ros::Publisher common_config_pub;


//        void timerCb(const ros::TimerEvent &event);


    };

#endif //COMMONCONFIGNODELET_H
