#ifndef PARSIANAIPLANSERVERNODELET_H
#define PARSIANAIPLANSERVERNODELET_H


#include <ros/ros.h>
#include <pluginlib/class_list_macros.h>
#include <nodelet/nodelet.h>

#include <parsian_msgs/parsian_debugs.h>
#include <parsian_msgs/parsian_draw.h>

#include <plan_server/planselector.h>
#include <parsian_msgs/parsian_plan.h>
#include <parsian_msgs/parsian_ai_plan_request.h>

namespace plan_server{

    class PlanServerNodelet : public nodelet::Nodelet{

    private:

        void onInit();

        void PlanRequestCallBack(const parsian_msgs::parsian_ai_plan_requestConstPtr &_planReq);

        ros::Publisher drawPub;
        ros::Publisher debugPub;

        ros::Publisher planPub;
        ros::Subscriber planReqSub;

        CPlanSelector* planSelector;
    };
}
#endif //PARSIANAIPLANSERVERNODELET_H
