//
// Created by ali on 10/15/17.
//

#include <AgentNodelet.h>
#include <parsian_agent/gotopoint.h>
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

void AgentNodelet::rtCb(const parsian_msgs::parsian_robot_taskConstPtr& robot_task){
    static int counter=0;
    if(!robot_task->gotoPointTask.size()){
        if(agent->skill != nullptr)
            delete agent->skill;
        CSkillGotoPoint *gotoPoint= new CSkillGotoPoint();
        gotoPoint->setMessage(robot_task->gotoPointTask[0]);
        agent->skill=gotoPoint;
    } else if(!robot_task->gotoPointAvoidTask.size()){

    }else if(!robot_task->kickTask.size()){

    } else if(!robot_task->oneTouchTask.size()){

    }else if(!robot_task->receivePassTask.size()){

    }
    agent->skill->execute();
    parsian_msgs::parsian_robot_command robot_command_msg;
    robot_command_msg.robot_id= static_cast<unsigned char>(agent->id());
    robot_command_msg.chip= static_cast<unsigned char>(agent->chip);
    robot_command_msg.packet_id= static_cast<unsigned char>(counter++);
    robot_command_msg.roller_speed= static_cast<unsigned char>(agent->roller);
    robot_command_msg.forceKick= static_cast<unsigned char>(agent->forceKick);
    robot_command_msg.kickSpeed= static_cast<unsigned short>(agent->kickSpeed);
    robot_command_msg.vel_x=agent->vel().x;
    robot_command_msg.vel_y=agent->vel().y;
    robot_command_msg.vel_w=agent->angularVel();

}
