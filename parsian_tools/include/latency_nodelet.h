//
// Created by ali on 1/10/18.
//

#ifndef PARSIAN_TOOLS_LATENCY_NODELET_H
#define PARSIAN_TOOLS_LATENCY_NODELET_H


#include <nodelet/nodelet.h>
#include <ros/ros.h>
#include <pluginlib/class_list_macros.h>
#include <parsian_msgs/parsian_robot_task.h>
#include <parsian_msgs/parsian_world_model.h>
#include <parsian_msgs/parsian_robot_command.h>
#include <parsian_msgs/parsian_ai_status.h>
#include <parsian_msgs/parsian_debugs.h>
#include <parsian_msgs/parsian_draw.h>
#include <parsian_msgs/parsian_path.h>
#include <QQueue>

#define buffSize 800

namespace parsian_tools {
    class Latency : public nodelet::Nodelet {
    private:

        void onInit() override;

        ros::NodeHandle nh;
        ros::NodeHandle private_nh;

        ros::Subscriber world_model_sub;
        ros::Subscriber ai_sub;
        ros::Subscriber robot_task_sub;
        ros::Subscriber robot_command_sub;
        ros::Subscriber planner_sub;


        QQueue <int> wm_latency_queue,rc_latency_queue,task_latency_queue;
        int wm_sum,rc_sum,task_sum;

        void wmCb(const parsian_msgs::parsian_world_modelConstPtr &);

        void rtCb(const parsian_msgs::parsian_robot_taskConstPtr &);

        void rcCb(const parsian_msgs::parsian_robot_commandConstPtr &);

        void aiCb(const parsian_msgs::parsian_ai_statusConstPtr &);

        void plannerCb(const parsian_msgs::parsian_pathConstPtr&);

    };
}

#endif //PARSIAN_TOOLS_LATENCY_NODELET_H
