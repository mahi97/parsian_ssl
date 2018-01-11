//
// Created by ali on 1/10/18.
//

#ifndef PARSIAN_TOOLS_LATENCY_NODELET_H
#define PARSIAN_TOOLS_LATENCY_NODELET_H


#include <nodelet/nodelet.h>
#include <ros/ros.h>
#include <pluginlib/class_list_macros.h>
#include <latency/latency_info.h>



namespace parsian_tools {
    using namespace parsian_msgs;
    class Latency : public nodelet::Nodelet {
    private:
        void onInit() override;

        LatencyInfo<parsian_world_model  > *world_model  ;
        LatencyInfo<parsian_robot_command> *robot_command;
        LatencyInfo<ssl_vision_detection > *vision       ;
        LatencyInfo<parsian_get_plan     > *plan         ;
        LatencyInfo<parsian_robot_task   > *robot_task   ;
        LatencyInfo<parsian_path         > *path         ;
        LatencyInfo<parsian_packets      > *packet       ;

        ros::NodeHandle nh;
    };
}

#endif //PARSIAN_TOOLS_LATENCY_NODELET_H
