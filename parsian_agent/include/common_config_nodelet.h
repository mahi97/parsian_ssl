#ifndef COMMONCONFIGNODELET_H
#define COMMONCONFIGNODELET_H

#include <nodelet/nodelet.h>
#include <ros/ros.h>
#include <pluginlib/class_list_macros.h>
#include <dynamic_reconfigure/server.h>
<<<<<<< HEAD
#include <parsian_agent/config.h>
#include <parsian_agent/commonconfigConfig.h>
#include <parsian_msgs/parsian_robot_common_config.h>
=======
#include <parsian_agent/agentConfig.h>
>>>>>>> 5af32b0f31cbffeb42da4f8be34496f49cee0755

    class CommonConfig : public nodelet::Nodelet {
    private:

        virtual void onInit();

<<<<<<< HEAD
        ros::NodeHandle nh;
        ros::NodeHandle private_nh;

        boost::shared_ptr<dynamic_reconfigure::Server<agent_common_config::commonconfigConfig>> server;
        void ConfigServerCallBack(const agent_common_config::commonconfigConfig &config, uint32_t level) ;

        ros::Publisher common_config_pub;


//        void timerCb(const ros::TimerEvent &event);
=======
        ros::NodeHandle private_nh;

        boost::shared_ptr<dynamic_reconfigure::Server<agent_common::agentConfig>> server;

>>>>>>> 5af32b0f31cbffeb42da4f8be34496f49cee0755


    };

#endif //COMMONCONFIGNODELET_H
