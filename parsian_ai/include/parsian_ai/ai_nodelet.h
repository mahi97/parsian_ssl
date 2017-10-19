#ifndef AINODELET_H
#define AINODELET_H

#include <ros/ros.h>
#include <pluginlib/class_list_macros.h>
#include <nodelet/nodelet.h>

#include <parsian_msgs/parsian_skill_gotoPoint.h>
#include <parsian_msgs/parsian_skill_gotoPointAvoid.h>
#include <parsian_msgs/parsian_skill_receivePass.h>
#include <parsian_msgs/parsian_skill_kick.h>

#include <parsian_msgs/parsian_debugs.h>
#include <parsian_msgs/parsian_draw.h>
#include <parsian_ai/ai.h>

#include <dynamic_reconfigure/server.h>
#include "parsian_ai/aiConfig.h"


namespace parsian_ai {
    class AINodelet : public nodelet::Nodelet {

    private:
        AI ai;
        ros::Subscriber worldModelSub;
        ros::Subscriber robotStatusSub;
        ros::Subscriber refereeSub;
        ros::Publisher drawPub;
        ros::Publisher debugPub;

        ros::Publisher robTask[_MAX_NUM_PLAYERS];

        ros::Timer timer_;

        //config server setup
        boost::shared_ptr<dynamic_reconfigure::Server<ai_config::aiConfig>> server;
        void ConfigServerCallBack(const ai_config::aiConfig &config, uint32_t level) ;

        void onInit();

        void worldModelCallBack(const parsian_msgs::parsian_world_modelConstPtr &_wm);
        void refereeCallBack(const parsian_msgs::ssl_refree_wrapperConstPtr & _ref);
        void robotStatusCallBack(const parsian_msgs::parsian_robotConstPtr & _rs);

        void timerCb(const ros::TimerEvent &event);
    };
}
#endif //AINODELET_H
