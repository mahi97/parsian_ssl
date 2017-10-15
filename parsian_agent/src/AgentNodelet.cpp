//
// Created by ali on 10/15/17.
//

#include <AgentNodelet.h>


void AgentNodelet::onInit(){


    debugger = new Debugger;
    drawer   = new Drawer;

    ros::NodeHandle& nh = getNodeHandle();
    ros::NodeHandle& private_nh = getPrivateNodeHandle();
    timer_ = nh.createTimer(ros::Duration(1.0), boost::bind(& AgentNodelet::timerCb, this, _1));

    world_model_sub = nh.subscribe("wm", 10, &AgentNodelet::wmCb, this);

    debug_pub = nh.advertise<parsian_msgs::parsian_debugs>("debugs", 10);
    draw_pub  = nh.advertise<parsian_msgs::parsian_draw>("draws", 10);

    robot_command_pub = private_nh.advertise<parsian_msgs::parsian_robot_command>("robot_command", 10);

    agent.reset(new Agent(1));
    wm = new CWorldModel;
}

void AgentNodelet::wmCb(const parsian_msgs::parsian_world_modelConstPtr& _wm) {
    wm->update(*_wm);
}

void AgentNodelet::timerCb(const ros::TimerEvent& event){
    if (debugger != nullptr) debug_pub.publish(debugger->debugs);
    if (drawer   != nullptr) draw_pub.publish(drawer->draws);
}
