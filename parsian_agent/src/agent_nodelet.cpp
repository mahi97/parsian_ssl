#include <agent_nodelet.h>

PLUGINLIB_EXPORT_CLASS(parsian_agent::AgentNodelet, nodelet::Nodelet);

using namespace parsian_agent;
void AgentNodelet::onInit(){
    ROS_INFO("oninit");

    debugger = new Debugger;
    drawer   = new Drawer;
    agent.reset(new Agent(1));
    wm = new CWorldModel;

    ros::NodeHandle& nh = getNodeHandle();
    ros::NodeHandle& private_nh = getPrivateNodeHandle();

    gotoPoint = new CSkillGotoPoint(agent.get());
    gotoPointAvoid = new CSkillGotoPointAvoid(agent.get());
    skillKick = new CSkillKick(agent.get());
    oneTouch = new CSkillKickOneTouch(agent.get());
    receivePass = new CSkillReceivePass(agent.get());

    world_model_sub = nh.subscribe("world_model", 10000, &AgentNodelet::wmCb, this);
    robot_task_sub  = nh.subscribe("robot_task_0", 10000, &AgentNodelet::rtCb, this);

    debug_pub = nh.advertise<parsian_msgs::parsian_debugs>("debugs", 1000);
    draw_pub  = nh.advertise<parsian_msgs::parsian_draw>("draws", 1000);

    parsian_robot_command_pub = nh.advertise<parsian_msgs::parsian_robot_command>("robot_command0", 1000);



    server.reset(new dynamic_reconfigure::Server<agent_config::agentConfig>(private_nh));
    dynamic_reconfigure::Server<agent_config::agentConfig>::CallbackType f;
    f = boost::bind(&AgentNodelet::ConfigServerCallBack,this, _1, _2);
    server->setCallback(f);

    timer_ = nh.createTimer(ros::Duration(0.01), &AgentNodelet::timerCb, this);



}

void AgentNodelet::wmCb(const parsian_msgs::parsian_world_modelConstPtr& _wm) {
   // ROS_INFO("agent nodelet::updated");
    wm->update(_wm);
    if (agent->skill != nullptr) {
       // ROS_INFO_STREAM("active size::  "<<wm->our.activeAgentsCount());
        agent->execute();
        parsian_robot_command_pub.publish(agent->getCommand());

    }
//    ROS_INFO_STREAM("ADDA : " << _wm);
//
//    NODELET_INFO_STREAM("lag : " << ros::Time::now() - _wm->Header.stamp);
// ROS_INFO(QString::number(wm->our.active(0)).toStdString().data());
}

void AgentNodelet::timerCb(const ros::TimerEvent& event){
   if (debugger != nullptr) debug_pub.publish(debugger->debugs);
    if (drawer   != nullptr) {
        ROS_INFO_STREAM("agent drawer"<<drawer);
        drawer->draws.texts.clear();
       draw_pub.publish(drawer->draws);

        drawer->draws.circles.clear();
        drawer->draws.segments.clear();
        drawer->draws.vectors.clear();
        drawer->draws.rects.clear();
    }
     //ROS_INFO("draawwwerrr");
}

void AgentNodelet::rtCb(const parsian_msgs::parsian_robot_taskConstPtr& _robot_task){

  //  ROS_INFO("callBack called");
    agent->skill = getSkill(_robot_task);
}

CSkill* AgentNodelet::getSkill(const parsian_msgs::parsian_robot_taskConstPtr &_task) {
    CSkill *skill = nullptr;
    switch (_task->select){
        case parsian_msgs::parsian_robot_task::GOTOPOINT: {
            gotoPoint->setMessage(&_task->gotoPointTask);
            skill = gotoPoint;
            ROS_INFO("GOTOPOINT executed!");
        }
            break;
        case parsian_msgs::parsian_robot_task::GOTOPOINTAVOID: {
            gotoPointAvoid->setMessage(&_task->gotoPointAvoidTask);
            skill = gotoPointAvoid;
            ROS_INFO_STREAM("GOTOPOINTAVOID executed!" << gotoPointAvoid->getTargetpos().y);
        }
            break;
        case parsian_msgs::parsian_robot_task::KICK: {
            skillKick->setMessage(&_task->kickTask);
            skill = skillKick;
            ROS_INFO("KICK executed!");
            break;
        }
        case parsian_msgs::parsian_robot_task::ONETOUCH: {
            oneTouch->setMessage(&_task->oneTouchTask);
            skill = oneTouch;
            ROS_INFO("ONETOUCH executed!");
        }
            break;
        case parsian_msgs::parsian_robot_task::RECIVEPASS: {
            receivePass->setMessage(&_task->receivePassTask);
            skill = receivePass;
            ROS_INFO("RECIVEPASS executed!");
        }
            break;


        default:break;
    }
    return skill;
}

void AgentNodelet::ConfigServerCallBack(const agent_config::agentConfig &config, uint32_t level)
{
    conf = config;
    ROS_INFO_STREAM(conf.AccMaxForward);
}
