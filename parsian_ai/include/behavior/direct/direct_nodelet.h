#ifndef DIRECTNODELET_H
#define DIRECTNODELET_H

#include <ros/ros.h>
#include <pluginlib/class_list_macros.h>
#include <nodelet/nodelet.h>

#include <parsian_msgs/parsian_debugs.h>
#include <parsian_msgs/parsian_draw.h>
#include <parsian_msgs/ssl_refree_wrapper.h>
#include <parsian_msgs/parsian_robots_status.h>
#include <parsian_msgs/parsian_behavior.h>
#include <parsian_msgs/parsian_ai_status.h>
#include <behavior/direct/direct.h>

#include <dynamic_reconfigure/server.h>
#include <parsian_ai/directConfig.h>
#include <parsian_ai/config.h>


namespace parsian_ai {
    class DirectNodelet : public nodelet::Nodelet {

    private:
        boost::shared_ptr<BehaviorDirect> direct;
        ros::Subscriber worldModelSub;
        ros::Subscriber refereeSub;
        ros::Subscriber teamConfSub;
	ros::Subscriber robotStatusSub;
	ros::Subscriber aiStatusSub;
        ros::Publisher drawPub;
        ros::Publisher debugPub;
        ros::Publisher behavPub;
        ros::Timer timer_;

        //config server setup
        boost::shared_ptr<dynamic_reconfigure::Server<ai_config::directConfig>> server;
        void ConfigServerCallBack(const ai_config::directConfig &config, uint32_t level) ;

        void onInit() override;
        void robotStatusCallBack(const parsian_msgs::parsian_robots_statusConstPtr& _rs);
        void aiStatusCallBack(const parsian_msgs::parsian_ai_statusConstPtr& _rs);
        void worldModelCallBack(const parsian_msgs::parsian_world_modelConstPtr &_wm);
        void refereeCallBack(const parsian_msgs::ssl_refree_wrapperConstPtr & _ref);
        void timerCb(const ros::TimerEvent &event);
    };
}
#endif //DIRECTNODELET_H
