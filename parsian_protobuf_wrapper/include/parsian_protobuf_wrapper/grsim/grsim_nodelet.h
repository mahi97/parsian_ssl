#ifndef GRSIMNODELET_H_
#define GRSIMNODELET_H_
#include <nodelet/nodelet.h>
#include <dynamic_reconfigure/server.h>
#include <parsian_protobuf_wrapper/visionConfig.h>
#include "ros/ros.h"
#include <vector>
#include <string>
//#include "parsian_msgs/grsim_robot_command.h"
#include "parsian_msgs/parsian_robot_command.h"
//#include "parsian_msgs/grsim_replacement.h"
#include <parsian_msgs/ssl_vision_detection.h>
#include "parsian_protobuf_wrapper/grSim_Packet.pb.h"
#include "parsian_protobuf_wrapper/grSim_Commands.pb.h"
#include "parsian_protobuf_wrapper/grSim_Replacement.pb.h"
#include "parsian_protobuf_wrapper/common/net/udpsend.h"
#include "parsian_msgs/grsim_robot_replacement.h"
#include "parsian_msgs/grsim_ball_replacement.h"


class GrsimNodelet : public nodelet::Nodelet
{
public:
    GrsimNodelet();
    ~GrsimNodelet();

    virtual void onInit();
    void visionCB(const parsian_msgs::ssl_vision_detectionConstPtr & msg);
    //void timerCb(const ros::TimerEvent& event);
    void GrsimBotCmd(const parsian_msgs::parsian_robot_command::ConstPtr& msg);

    bool GrsimBallReplacesrv(parsian_msgs::grsim_ball_replacement::Request& req,
                             parsian_msgs::grsim_ball_replacement::Response& res);
    bool GrsimRobotReplacesrv(parsian_msgs::grsim_robot_replacement::Request& req,
                              parsian_msgs::grsim_robot_replacement::Response& res);

    dynamic_reconfigure::Server<protobuf_wrapper_config::visionConfig> server;
    dynamic_reconfigure::Server<protobuf_wrapper_config::visionConfig>::CallbackType f;
    void conf(protobuf_wrapper_config::visionConfig &config, uint32_t level);



    void send();
    std::string ip;
    int port;

    bool color;

    grSim_Packet packet;
    grSim_Commands* GrsimCommand;
    grSim_Replacement* GrsimReplacement;

    UDPSend* udp;


    ros::NodeHandle n;
    ros::Subscriber vision_sub;
    //ros::Timer timer_;
    ros::Subscriber sub0;
    ros::Subscriber sub1;
    ros::Subscriber sub2;
    ros::Subscriber sub3;
    ros::Subscriber sub4;
    ros::Subscriber sub5;
    ros::Subscriber sub_0;
    ros::Subscriber _sub;
    ros::ServiceServer service0;
    ros::ServiceServer service1;
};

#endif /* GRSIMNODELET_H_ */
