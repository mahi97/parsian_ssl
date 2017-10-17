//
// Created by ali on 10/15/17.
//

#include <AgentNodelet.h>


PLUGINLIB_EXPORT_CLASS(parsian_agent::AgentNodelet, nodelet::Nodelet);

using namespace parsian_agent;
void AgentNodelet::onInit(){


    debugger = new Debugger;
    drawer   = new Drawer;

    ros::NodeHandle& nh = getNodeHandle();
    ros::NodeHandle& private_nh = getPrivateNodeHandle();
    timer_ = nh.createTimer(ros::Duration(1.0), boost::bind(& AgentNodelet::timerCb, this, _1));

    world_model_sub = nh.subscribe("wm", 10, &AgentNodelet::wmCb, this);

    debug_pub = nh.advertise<parsian_msgs::parsian_debugs>("debugs", 10);
    draw_pub  = nh.advertise<parsian_msgs::parsian_draw>("draws", 10);

    parsian_robot_command_pub = private_nh.advertise<parsian_msgs::parsian_robot_command>("robot_command", 10);
    grsim_robot_command_pub=private_nh.advertise<parsian_msgs::grsim_robot_command>("GrsimBotCmd0", 10);

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
    //////////////////////////////////////////  real world

    parsian_msgs::parsian_robot_command parsian_robot_command_msg;
    parsian_robot_command_msg.robot_id= static_cast<unsigned char>(agent->id());
    parsian_robot_command_msg.chip= static_cast<unsigned char>(agent->chip);
    parsian_robot_command_msg.packet_id= static_cast<unsigned char>(counter++);
    parsian_robot_command_msg.roller_speed= static_cast<unsigned char>(agent->roller);
    parsian_robot_command_msg.forceKick= static_cast<unsigned char>(agent->forceKick);
    parsian_robot_command_msg.kickSpeed= static_cast<unsigned short>(agent->kickSpeed);
    parsian_robot_command_msg.vel_x=agent->vel().x;
    parsian_robot_command_msg.vel_y=agent->vel().y;
    parsian_robot_command_msg.vel_w=agent->angularVel();
    parsian_robot_command_msg.release= static_cast<unsigned char>(agent->onOffState);
    parsian_robot_command_pub.publish(parsian_robot_command_msg);


    //////////////////////////////////////////  grsim


    parsian_msgs::grsim_robot_command  grsim_robot_command_msg;
    grsim_robot_command_msg.id= static_cast<unsigned char>(agent->id());

    double w1 = agent->v1*gain;
    double w2 = agent->v2*gain;
    double w3 = agent->v3*gain;
    double w4 = agent->v4*gain;

    agent->jacobian(agent->vforward,agent->vnormal,agent->vangular*_DEG2RAD,w1,w2,w3,w4);

    grsim_robot_command_msg.wheelsspeed=1;//true
    grsim_robot_command_msg.wheel1= static_cast<float>(w1);
    grsim_robot_command_msg.wheel2= static_cast<float>(w2);
    grsim_robot_command_msg.wheel3= static_cast<float>(w3);
    grsim_robot_command_msg.wheel4= static_cast<float>(w4);

    grsim_robot_command_msg.velangular=0;
    grsim_robot_command_msg.velnormal=0;
    grsim_robot_command_msg.veltangent=0;
    grsim_robot_command_msg.kickspeedx= static_cast<float>(agent->kickSpeed);
    if (agent->chip){
        grsim_robot_command_msg.kickspeedz= static_cast<float>(agent->kickSpeed);
    }
    else
        grsim_robot_command_msg.kickspeedz=0;
    grsim_robot_command_msg.spinner= static_cast<unsigned char>(false);
    grsim_robot_command_pub.publish(grsim_robot_command_msg);
}
