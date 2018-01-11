//
// Created by ali on 1/11/18.
//

#ifndef PARSIAN_TOOLS_LATENCY_INFO_H
#define PARSIAN_TOOLS_LATENCY_INFO_H

#include <ros/ros.h>
#include <QString>
#include <QQueue>
#include <parsian_msgs/parsian_robot_task.h>
#include <parsian_msgs/parsian_world_model.h>
#include <parsian_msgs/parsian_robot_command.h>
#include <parsian_msgs/parsian_path.h>
#include <parsian_msgs/parsian_packets.h>
#include <parsian_msgs/ssl_vision_detection.h>
#include <parsian_msgs/parsian_get_plan.h>

#define buffSize 600
namespace parsian_tools {
    template<class T>
    class LatencyInfo {
    public:
        LatencyInfo(ros::NodeHandle &nh, const char *name, const char *topic);

        void call_back(boost::shared_ptr<T> &msg);

        std::string info();

        void updateetMinMax(int value);

    private:
        ros::Subscriber subscriber;
        QQueue<int> latency_queue;
        int min, max, sum;
        std::string name;
    };
}
#endif //PARSIAN_TOOLS_LATENCY_INFO_H
