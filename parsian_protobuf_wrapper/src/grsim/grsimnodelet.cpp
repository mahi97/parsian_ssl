#include "parsian_protobuf_wrapper/grsim/grsimnodelet.h"
#include <pluginlib/class_list_macros.h>



/*----------filing grSim_Robot_Command with ROS datas for each robots----------*/
void GrsimNodelet::GrsimBotCmd(const parsian_msgs::grsim_robot_command::ConstPtr& msg)
{
    grSim_Robot_Command* GrsimRobotCommand = GrsimCommand->add_robot_commands();
    GrsimRobotCommand->set_id(msg->id);
    GrsimRobotCommand->set_kickspeedx(msg->kickspeedx);
    GrsimRobotCommand->set_kickspeedz(msg->kickspeedz);
    GrsimRobotCommand->set_veltangent(msg->veltangent);
    GrsimRobotCommand->set_velnormal(msg->velnormal);
    GrsimRobotCommand->set_velangular(msg->velangular);
    GrsimRobotCommand->set_wheel1(msg->wheel1);
    GrsimRobotCommand->set_wheel2(msg->wheel2);
    GrsimRobotCommand->set_wheel3(msg->wheel3);
    GrsimRobotCommand->set_wheel4(msg->wheel4);
    GrsimRobotCommand->set_spinner(msg->spinner != 0u);
    GrsimRobotCommand->set_wheelsspeed(msg->wheelsspeed != 0u);

}

/*----------filing grsim_robot_replacement with ROS datas for each robots ##message##----------*/
void GrsimNodelet::GrsimRobotReplace(const parsian_msgs::grsim_robot_replacement::ConstPtr& msg)
{
    grSim_RobotReplacement* grsimRReplacement = GrsimReplacement->add_robots();
    grsimRReplacement->set_x(msg->x);
    grsimRReplacement->set_y(msg->y);
    grsimRReplacement->set_dir(msg->dir);
    grsimRReplacement->set_id(msg->id);
    grsimRReplacement->set_yellowteam(msg->yellowteam);
}

/*----------filing grsim_robot_replacement with ROS datas for each robots ##service##----------*/
bool GrsimNodelet::GrsimRobotReplacesrv(parsian_msgs::grsim_robot_replacement_srv::Request& req,
                          parsian_msgs::grsim_robot_replacement_srv::Response& res)
{
    grSim_RobotReplacement* grsimRReplacement = GrsimReplacement->add_robots();
    grsimRReplacement->set_x(req.x);
    grsimRReplacement->set_y(req.y);
    grsimRReplacement->set_dir(req.dir);
    grsimRReplacement->set_id(req.id);
    grsimRReplacement->set_yellowteam(req.yellowteam);
    return true;
}

/*----------filing grsim_ball_replacement with ROS datas ##message##----------*/
void GrsimNodelet::GrsimBallReplace(const parsian_msgs::grsim_ball_replacement::ConstPtr& msg)
{
    grSim_BallReplacement* GrsimBReplacement = new grSim_BallReplacement;
    GrsimBReplacement->set_x(msg->x);
    GrsimBReplacement->set_y(msg->y);
    GrsimBReplacement->set_vx(msg->vx);
    GrsimBReplacement->set_vy(msg->vy);
    GrsimReplacement->set_allocated_ball(GrsimBReplacement);
}

/*----------filing grsim_ball_replacement with ROS datas ##service##----------*/
bool GrsimNodelet::GrsimBallReplacesrv(parsian_msgs::grsim_ball_replacement_srv::Request& req,
                         parsian_msgs::grsim_ball_replacement_srv::Response& res)
{
    grSim_BallReplacement* GrsimBReplacement = new grSim_BallReplacement;
    GrsimBReplacement->set_x(req.x);
    GrsimBReplacement->set_y(req.y);
    GrsimBReplacement->set_vx(req.vx);
    GrsimBReplacement->set_vy(req.vy);
    GrsimReplacement->set_allocated_ball(GrsimBReplacement);
    return true;
}

/*----------creating a full grSim_Packet protocol buffer and sending it----------*/
void GrsimNodelet::send()
{
    std::string color;
    ros::param::get("team_color", color);
    bool col = ! (color == "yellow");          //check if it is true!
    GrsimCommand->set_isteamyellow(col);
    GrsimCommand->set_timestamp(0.0);                       //should fix this
    packet.set_allocated_commands(GrsimCommand);
    packet.set_allocated_replacement(GrsimReplacement);
    std::string buffer;
    packet.SerializeToString(&buffer);
    udp->send(buffer);
    packet.clear_commands();
    packet.clear_replacement();
    GrsimCommand = new grSim_Commands;
    GrsimReplacement = new grSim_Replacement;
}

void GrsimNodelet::timerCb(const ros::TimerEvent& event){
    // Using timers is the preferred 'ROS way' to manual threading
    send();
}

/*----------get ip address and port from configue server----------*/
void GrsimNodelet::callback(protobuf_wrapper_config::visionConfig &config, uint32_t level)
{
    if(this->ip != config.vision_multicast_ip)
    {
        this->ip = config.vision_multicast_ip;
        udp->setIP(this->ip);
    }
    if(this->port  != config.vision_multicast_port)
    {
        this->port  = config.vision_multicast_port;
        udp->setport(this->port);
    }

}

GrsimNodelet::GrsimNodelet()
{
    ROS_INFO("grsim_nodelet is running");
}

GrsimNodelet::~GrsimNodelet()
{
    //ROS_INFO("Destructor");
}

void GrsimNodelet::onInit()
{
    NODELET_INFO("grsim_nodelet onInit");
    udp = new UDPSend(ip, port);
    n = getNodeHandle();
    sub0 = n.subscribe<parsian_msgs::grsim_robot_command>("GrsimBotCmd0", 1000, boost::bind(& GrsimNodelet::GrsimBotCmd, this, _1));
    sub1 = n.subscribe<parsian_msgs::grsim_robot_command>("GrsimBotCmd1", 1000, boost::bind(& GrsimNodelet::GrsimBotCmd, this, _1));
    sub2 = n.subscribe<parsian_msgs::grsim_robot_command>("GrsimBotCmd2", 1000, boost::bind(& GrsimNodelet::GrsimBotCmd, this, _1));
    sub3 = n.subscribe<parsian_msgs::grsim_robot_command>("GrsimBotCmd3", 1000, boost::bind(& GrsimNodelet::GrsimBotCmd, this, _1));
    sub4 = n.subscribe<parsian_msgs::grsim_robot_command>("GrsimBotCmd4", 1000, boost::bind(& GrsimNodelet::GrsimBotCmd, this, _1));
    sub5 = n.subscribe<parsian_msgs::grsim_robot_command>("GrsimBotCmd5", 1000, boost::bind(& GrsimNodelet::GrsimBotCmd, this, _1));
    sub_0 = n.subscribe<parsian_msgs::grsim_robot_replacement>("GrsimRobotReplace0", 1000, boost::bind(& GrsimNodelet::GrsimRobotReplace, this, _1));
    _sub = n.subscribe<parsian_msgs::grsim_ball_replacement>("GrsimBallReplace", 1000, boost::bind(& GrsimNodelet::GrsimBallReplace, this, _1));

    service0 = n.advertiseService<parsian_msgs::grsim_robot_replacement_srv::Request,
                                  parsian_msgs::grsim_robot_replacement_srv::Response>
                                  ("GrsimRobotReplacesrv", boost::bind(& GrsimNodelet::GrsimRobotReplacesrv, this, _1, _2));
    service1 = n.advertiseService<parsian_msgs::grsim_ball_replacement_srv::Request,
                                  parsian_msgs::grsim_ball_replacement_srv::Response>
                                  ("GrsimBallReplacesrv", boost::bind(& GrsimNodelet::GrsimBallReplacesrv, this, _1, _2));

    f = boost::bind(& GrsimNodelet::callback, this, _1, _2);
    server.setCallback(f);

    timer_ = n.createTimer(ros::Duration(1.0), boost::bind(& GrsimNodelet::timerCb, this, _1));

    GrsimCommand = new grSim_Commands;
    GrsimReplacement = new grSim_Replacement;

}



PLUGINLIB_DECLARE_CLASS(parsian_protobuf_wrapper, GrsimNodelet, GrsimNodelet, nodelet::Nodelet);
//PLUGINLIB_EXPORT_CLASS(Server, nodelet::Nodelet)
