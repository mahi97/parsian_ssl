//
// Created by ali on 10/15/17.
//

#ifndef PARSIAN_AGENT_AGENTNODELET_H
#define PARSIAN_AGENT_AGENTNODELET_H

#include <nodelet/nodelet.h>
#include <ros/ros.h>

#include <parsian_util/tools/debuger.h>
#include <parsian_util/tools/drawer.h>
#include <parsian_msgs/parsian_robot_task.h>
#include <parsian_msgs/parsian_world_model.h>
#include <parsian_msgs/parsian_robot_command.h>
#include <parsian_msgs/parsian_ai_status.h>
#include <parsian_msgs/parsian_robot_status.h>
#include <parsian_msgs/parsian_debugs.h>
#include <parsian_msgs/parsian_draw.h>
#include <parsian_agent/agent.h>

class AgentNodelet : public nodelet::Nodelet {
private:
    virtual void onInit();

    void timerCb(const ros::TimerEvent& event);

    // must use a ConstPtr callback to use zero-copy transport
    //void messageCb(const std_msgs::StringConstPtr message);

    ros::Subscriber world_model_sub;
    ros::Subscriber ai_sub;
    ros::Subscriber robot_task_sub;

    ros::Publisher debug_pub;
    ros::Publisher draw_pub;
    ros::Publisher robot_command_pub;
    ros::Publisher robot_status_pub;

    ros::Timer timer_;

    void wmCb(const parsian_msgs::parsian_world_modelConstPtr&);
    void rtCb(const parsian_msgs::parsian_robot_taskConstPtr&);
    void aiCb(const parsian_msgs::parsian_ai_statusConstPtr&);

    boost::shared_ptr<Agent> agent;


};


#endif //PARSIAN_AGENT_AGENTNODELET_H
