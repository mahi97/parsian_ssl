#include <agent_nodelet.h>

PLUGINLIB_EXPORT_CLASS(parsian_agent::AgentNodelet, nodelet::Nodelet);

using namespace parsian_agent;
void AgentNodelet::onInit(){


    debugger = new Debugger;
    drawer   = new Drawer;

    ros::NodeHandle& nh = getNodeHandle();
    ros::NodeHandle& private_nh = getPrivateNodeHandle();
    timer_ = nh.createTimer(ros::Duration(1.0), &AgentNodelet::timerCb, this);

    world_model_sub = nh.subscribe("wm", 10, &AgentNodelet::wmCb, this);
    robot_task_sub  = nh.subscribe("robot_task_0", 10, &AgentNodelet::rtCb, this);

    debug_pub = nh.advertise<parsian_msgs::parsian_debugs>("debugs", 10);
    draw_pub  = nh.advertise<parsian_msgs::parsian_draw>("draws", 10);

    parsian_robot_command_pub = private_nh.advertise<parsian_msgs::parsian_robot_command>("robot_command", 10);
    grsim_robot_command_pub   = private_nh.advertise<parsian_msgs::grsim_robot_command>("GrsimBotCmd0", 10);

    agent.reset(new Agent(1));
    wm = new CWorldModel;

    server.reset(new dynamic_reconfigure::Server<agent_config::agentConfig>);
    dynamic_reconfigure::Server<agent_config::agentConfig>::CallbackType f;
    f = boost::bind(&AgentNodelet::ConfigServerCallBack,this, _1, _2);
    server->setCallback(f);
}

void AgentNodelet::wmCb(const parsian_msgs::parsian_world_modelConstPtr& _wm) {
    wm->update(*_wm);
}

void AgentNodelet::timerCb(const ros::TimerEvent& event){
    if (debugger != nullptr) debug_pub.publish(debugger->debugs);
    if (drawer   != nullptr) draw_pub.publish(drawer->draws);
}

void AgentNodelet::rtCb(const parsian_msgs::parsian_robot_taskConstPtr& robot_task){

    agent->setTask(robot_task);
    agent->execute();
    parsian_robot_command_pub.publish(agent->getCommand());
    grsim_robot_command_pub.publish(agent->getGrSimCommand());

}

void ConfigServerCallBack(const agent_config::agentConfig &config, uint32_t level)
{

}
