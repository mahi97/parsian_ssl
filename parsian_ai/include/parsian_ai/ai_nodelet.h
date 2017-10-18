#ifndef AINODELET_H
#define AINODELET_H

#include <ros/ros.h>
#include <pluginlib/class_list_macros.h>
#include <nodelet/nodelet.h>
#include <parsian_msgs/parsian_world_model.h>
#include <parsian_msgs/parsian_debugs.h>
#include <parsian_msgs/parsian_draw.h>
#include <parsian_msgs/ssl_refree_wrapper.h>
#include <parsian_ai/ai.h>
namespace parsian_ai {
    class AINodelet : public nodelet::Nodelet {

    private:
        AI ai;
        ros::Subscriber worldModelSub;
        ros::Subscriber robotStatusSub;
        ros::Subscriber refereeSub;
        ros::Publisher drawPub;
        ros::Publisher debugPub;
        ros::Timer timer_;

        void onInit();

        void timerCb(const ros::TimerEvent &event);
    };
}
#endif //AINODELET_H