#ifndef AGENTNODELET_H
#define AGENTNODELET_H

#include <nodelet/nodelet.h>
#include <ros/ros.h>
#include <pluginlib/class_list_macros.h>
#include <parsian_util/tools/debuger.h>
#include <parsian_util/tools/drawer.h>
#include <parsian_msgs/parsian_robot_task.h>
#include <parsian_msgs/parsian_world_model.h>
#include <parsian_msgs/parsian_robot_command.h>
#include <parsian_msgs/grsim_robot_command.h>
#include <parsian_msgs/parsian_ai_status.h>
#include <parsian_msgs/parsian_robot_status.h>
#include <parsian_msgs/parsian_debugs.h>
#include <parsian_msgs/parsian_draw.h>
#include <parsian_agent/agent.h>

#include <dynamic_reconfigure/server.h>
#include "parsian_agent/agentConfig.h"

namespace parsian_agent {
    class AgentNodelet : public nodelet::Nodelet {
    private:

        virtual void onInit();

        void timerCb(const ros::TimerEvent &event);

        ros::Subscriber world_model_sub;
        ros::Subscriber ai_sub;
        ros::Subscriber robot_task_sub;

        ros::Publisher debug_pub;
        ros::Publisher draw_pub;
        ros::Publisher parsian_robot_command_pub;
        ros::Publisher grsim_robot_command_pub;
        ros::Publisher robot_status_pub;

        ros::Timer timer_;

        void wmCb(const parsian_msgs::parsian_world_modelConstPtr &);

        void rtCb(const parsian_msgs::parsian_robot_taskConstPtr &);

        void aiCb(const parsian_msgs::parsian_ai_statusConstPtr &);

        boost::shared_ptr<Agent> agent;

        boost::shared_ptr<dynamic_reconfigure::Server<agent_config::agentConfig>> server;
        void ConfigServerCallBack(const agent_config::agentConfig &config, uint32_t level) ;


    };
}

#endif //PARSIAN_AGENT_AGENTNODELET_H
