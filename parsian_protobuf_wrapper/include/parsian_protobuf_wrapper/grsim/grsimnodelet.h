
#ifndef GRSIMNODELET_H_
#define GRSIMNODELET_H_
#include <nodelet/nodelet.h>
#include "ros/ros.h"
#include <vector>
#include <string>
#include "parsian_msgs/grsim_robot_command.h"
#include "parsian_msgs/grsim_replacement.h"
#include "parsian_protobuf_wrapper/grSim_Packet.pb.h"
#include "parsian_protobuf_wrapper/grSim_Commands.pb.h"
#include "parsian_protobuf_wrapper/grSim_Replacement.pb.h"
#include "parsian_protobuf_wrapper/common/net/udpsend.h"
#include "parsian_msgs/grsim_robot_replacement_srv.h"
#include "parsian_msgs/grsim_ball_replacement_srv.h"

class GrsimNodelet : public nodelet::Nodelet
{
public:
    GrsimNodelet();
    ~GrsimNodelet();

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
    ros::Subscriber sub_0;
    ros::Subscriber _sub;
    ros::ServiceServer service0;
    ros::ServiceServer service1;
};

#endif /* GRSIMNODELET_H_ */