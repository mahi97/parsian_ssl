#ifndef COMMONSTATUSNODELET_H
#define COMMONSTATUSNODELET_H

#include <nodelet/nodelet.h>
#include <ros/ros.h>
#include <pluginlib/class_list_macros.h>
#include <dynamic_reconfigure/server.h>
#include <parsian_agent/config.h>
#include <parsian_agent/commonstatusConfig.h>
#include <parsian_msgs/parsian_robot_common_status.h>

    class CommonStatus : public nodelet::Nodelet {
    private:

        virtual void onInit();

        ros::NodeHandle nh;
        ros::NodeHandle private_nh;

        boost::shared_ptr<dynamic_reconfigure::Server<agent_config1::commonstatusConfig>> server;
        void ConfigServerCallBack(const agent_config1::commonstatusConfig &config, uint32_t level) ;

        ros::Publisher common_status_pub;


//        void timerCb(const ros::TimerEvent &event);


    };

#endif //COMMONSTATUSNODELET_H
