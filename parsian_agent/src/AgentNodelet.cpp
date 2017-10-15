//
// Created by ali on 10/15/17.
//

#include <AgentNodelet.h>


void AgentNodelet::onInit(){
    ros::NodeHandle& nh = getNodeHandle();
    ros::NodeHandle& private_nh = getPrivateNodeHandle();
    timer_ = nh.createTimer(ros::Duration(1.0), boost::bind(& AgentNodelet::timerCb, this, _1));
   // sub_ = nh.subscribe("incoming_chatter", 10, boost::bind(& AgentNodelet::messageCb, this, _1));
   // pub_ = private_nh.advertise<std_msgs::String>("outgoing_chatter", 10);

}

void timerCb(const ros::TimerEvent& event){

}
