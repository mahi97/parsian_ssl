#ifndef PARSIAN_AI_NODELET_H
#define PARSIAN_AI_NODELET_H

#include <ros/ros.h>
#include <pluginlib/class_list_macros.h>
#include <nodelet/nodelet.h>
#include <parsian_msgs/gotoPoint.h>
#include <parsian_msgs/gotoPointAvoid.h>
#include <parsian_msgs/receivePass.h>
#include <parsian_msgs/kick.h>
#include <parsian_msgs/parsian_world_model.h>
#include <parsian_msgs/parsian_debugs.h>
#include <parsian_msgs/parsian_draw.h>
#include <parsian_msgs/ssl_refree_wrapper.h>
#include <parsian_ai/ai.h>

    class ai_nodelet : public nodelet::Nodelet {
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

#endif //PARSIAN_AI_NODELET_H