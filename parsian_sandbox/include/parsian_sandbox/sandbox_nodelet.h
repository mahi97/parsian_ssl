#ifndef SANDBOXNODELET_H
#define SANDBOXNODELET_H

#include <nodelet/nodelet.h>
#include <ros/ros.h>
#include <pluginlib/class_list_macros.h>
#include <parsian_util/tools/debuger.h>
#include <parsian_util/tools/drawer.h>
#include <parsian_msgs/parsian_robot_task.h>
#include <parsian_msgs/parsian_world_model.h>
#include <parsian_msgs/parsian_robot_command.h>
#include <parsian_msgs/grsim_robot_command.h>
#include <parsian_msgs/parsian_ai_status.h>
#include <parsian_msgs/parsian_robot_status.h>
#include <parsian_msgs/parsian_debugs.h>
#include <parsian_msgs/parsian_draw.h>
#include <dynamic_reconfigure/server.h>
#include <parsian_sandbox/sandboxes.h>
#include <parsian_util/core/worldmodel.h>
#include <std_msgs/UInt8.h>


namespace parsian_sandbox {
    class SandBoxNodelet : public nodelet::Nodelet {
    private:

        virtual void onInit();

        void timerCb(const ros::TimerEvent &event);

        ros::Subscriber world_model_sub;
        ros::Subscriber ai_sub;
        ros::Subscriber robot_task_sub;

        ros::Publisher debug_pub;
        ros::Publisher draw_pub;
        ros::Publisher parsian_robot_command_pub;
        ros::Publisher robot_status_pub;

        ros::Timer timer_;

//        void wmCb(const parsian_msgs::parsian_world_modelConstPtr &);
//
//        void rtCb(const parsian_msgs::parsian_robot_taskConstPtr &);
//
//        void aiCb(const parsian_msgs::parsian_ai_statusConstPtr &);
        void modeCb(const std_msgs::UInt8ConstPtr &);

        boost::shared_ptr<SandBox> sandBox;

        // Config
//        boost::shared_ptr<dynamic_reconfigure::Server<agent_config::agentConfig>> server;
//        void ConfigServerCallBack(const agent_config::agentConfig &config, uint32_t level) ;

    };
}

#endif //SANDBOXNODELET_H
