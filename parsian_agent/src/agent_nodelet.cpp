#include <agent_nodelet.h>

PLUGINLIB_EXPORT_CLASS(parsian_agent::AgentNodelet, nodelet::Nodelet);

using namespace parsian_agent;
void AgentNodelet::onInit(){


    debugger = new Debugger;
    drawer   = new Drawer;

    ros::NodeHandle& nh = getNodeHandle();
    ros::NodeHandle& private_nh = getPrivateNodeHandle();
    timer_ = nh.createTimer(ros::Duration(1.0), &AgentNodelet::timerCb, this);

    world_model_sub = nh.subscribe("world_model", 10, &AgentNodelet::wmCb, this);
    robot_task_sub  = nh.subscribe("robot_task_0", 10, &AgentNodelet::rtCb, this);

    debug_pub = nh.advertise<parsian_msgs::parsian_debugs>("debugs", 10);
    draw_pub  = nh.advertise<parsian_msgs::parsian_draw>("draws", 10);

    parsian_robot_command_pub = nh.advertise<parsian_msgs::parsian_robot_command>("robot_command0", 10);
    grsim_robot_command_pub   = nh.advertise<parsian_msgs::grsim_robot_command>("GrsimBotCmd0", 10);

    agent.reset(new Agent(0));
    wm = new CWorldModel;

    server.reset(new dynamic_reconfigure::Server<agent_config::agentConfig>);
    dynamic_reconfigure::Server<agent_config::agentConfig>::CallbackType f;
    f = boost::bind(&AgentNodelet::ConfigServerCallBack,this, _1, _2);
    server->setCallback(f);


    gotoPoint = new CSkillGotoPoint(agent.get());
    gotoPointAvoid = new CSkillGotoPointAvoid(agent.get());
    skillKick = new CSkillKick(agent.get());
    oneTouch = new CSkillKickOneTouch(agent.get());
    receivePass = new CSkillReceivePass(agent.get());


}

void AgentNodelet::wmCb(const parsian_msgs::parsian_world_modelConstPtr& _wm) {
    ROS_INFO("agent::wm updated");
    wm->update(*_wm);
}

void AgentNodelet::timerCb(const ros::TimerEvent& event){
  // if (debugger != nullptr) debug_pub.publish(debugger->debugs);
   // if (drawer   != nullptr) draw_pub.publish(drawer->draws);
}

void AgentNodelet::rtCb(const parsian_msgs::parsian_robot_taskConstPtr& _robot_task){

    ROS_INFO("callBack called");
    agent->skill = getSkill(_robot_task);
    agent->execute();
    parsian_robot_command_pub.publish(agent->getCommand());
    grsim_robot_command_pub.publish(agent->getGrSimCommand());

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
            ROS_INFO("GOTOPOINTAVOID executed!");
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
