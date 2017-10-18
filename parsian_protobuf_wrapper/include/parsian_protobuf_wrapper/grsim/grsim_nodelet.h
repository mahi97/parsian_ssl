
#ifndef GRSIMNODELET_H_
#define GRSIMNODELET_H_

#include <nodelet/nodelet.h>
#include "ros/ros.h"
#include <vector>
#include <string>
#include "parsian_msgs/grsim_robot_command.h"
#include "parsian_protobuf_wrapper/grSim_Packet.pb.h"
#include "parsian_protobuf_wrapper/grSim_Commands.pb.h"
#include "parsian_protobuf_wrapper/grSim_Replacement.pb.h"
#include "parsian_protobuf_wrapper/common/net/udpsend.h"
#include "parsian_msgs/grsim_robot_replacement.h"
#include "parsian_msgs/grsim_ball_replacement.h"

#include <dynamic_reconfigure/server.h>
#include "parsian_protobuf_wrapper/grsimConfig.h"



class GrsimNodelet : public nodelet::Nodelet
{
public:

    virtual void onInit();
    void timerCb(const ros::TimerEvent& event);
    ros::NodeHandle n;
    ros::Timer timer_;
    ros::Subscriber sub0;
    ros::Subscriber sub1;
    ros::Subscriber sub2;
    ros::Subscriber sub3;
    ros::Subscriber sub4;
    ros::Subscriber sub5;
    ros::ServiceServer service0;
    ros::ServiceServer service1;

    int port{12340};
    std::string ip{"127.0.0.1"};

    boost::shared_ptr<dynamic_reconfigure::Server<protobuf_wrapper_config::grsimConfig>> server;
    void UpdatePortIp(const protobuf_wrapper_config::grsimConfig &config, uint32_t level) ;
    void send();
    bool ballReplaceCb(parsian_msgs::grsim_ball_replacementRequest& req,
                       parsian_msgs::grsim_ball_replacementResponse& res);
    bool robotReplaceCb(parsian_msgs::grsim_robot_replacementRequest& req,
                        parsian_msgs::grsim_robot_replacementResponse& res);
    void robotCommandCb(const parsian_msgs::grsim_robot_commandConstPtr& msg);

private:
    grSim_Commands* grsimCommand;
    grSim_Replacement* grsimReplacement;
    grSim_Packet packet;


};

#endif /* GRSIMNODELET_H_ */
