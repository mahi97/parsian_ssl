#ifndef PARSIAN_AGENT_PLANNER_NODELET_H
#define PARSIAN_AGENT_PLANNER_NODELET_H

#include <nodelet/nodelet.h>
#include <ros/ros.h>
#include <pluginlib/class_list_macros.h>
#include <parsian_util/tools/debuger.h>
#include <parsian_util/tools/drawer.h>
#include <parsian_msgs/parsian_world_model.h>
#include <parsian_msgs/parsian_debugs.h>
#include <parsian_msgs/parsian_draw.h>
#include <parsian_agent/agent.h>
#include <parsian_agent/config.h>


namespace parsian_agent {
    class PlannerNodelet : public nodelet::Nodelet {
    private:

        void onInit() override ;

        void timerCb(const ros::TimerEvent &event);

        ros::NodeHandle nh;
        ros::NodeHandle private_nh;

        ros::Subscriber world_model_sub;
        ros::Subscriber ai_sub;
        ros::Subscriber robot_task_sub;
        ros::Subscriber robot_status_sub;
        ros::Subscriber common_config_sub;

        ros::Publisher debug_pub;
        ros::Publisher draw_pub;
        ros::Publisher parsian_robot_command_pub;

        ros::Timer timer_;

        void commonConfigCb(const dynamic_reconfigure::ConfigConstPtr & _cnf);

        void wmCb(const parsian_msgs::parsian_world_modelConstPtr &);


        boost::shared_ptr<Agent> agent;

    };
}


#endif //PARSIAN_AGENT_PLANNER_NODELET_H
