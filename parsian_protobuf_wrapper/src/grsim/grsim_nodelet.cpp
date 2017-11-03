#include "parsian_protobuf_wrapper/grsim/grsim_nodelet.h"
#include <pluginlib/class_list_macros.h>



/*----------filing grSim_Robot_Command with ROS datas for each robots----------*/
void GrsimNodelet::GrsimBotCmd(const parsian_msgs::parsian_robot_command::ConstPtr& msg)
{
    grSim_Robot_Command* GrsimRobotCommand = GrsimCommand->add_robot_commands();
    GrsimRobotCommand->set_id(msg->robot_id);
    GrsimRobotCommand->set_kickspeedx(msg->kickSpeed);
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

//    GrsimRobotCommand->set_id(msg->id);
//    GrsimRobotCommand->set_kickspeedx(msg->kickspeedx);
//    GrsimRobotCommand->set_kickspeedz(msg->kickspeedz);
//    GrsimRobotCommand->set_veltangent(msg->veltangent);
//    GrsimRobotCommand->set_velnormal(msg->velnormal);
//    GrsimRobotCommand->set_velangular(msg->velangular);
//    GrsimRobotCommand->set_wheel1(msg->wheel1);
//    GrsimRobotCommand->set_wheel2(msg->wheel2);
//    GrsimRobotCommand->set_wheel3(msg->wheel3);
//    GrsimRobotCommand->set_wheel4(msg->wheel4);
//    GrsimRobotCommand->set_spinner(msg->spinner != 0u);
//    GrsimRobotCommand->set_wheelsspeed(msg->wheelsspeed != 0u);

}


/*----------filing grsim_robot_replacement with ROS datas for each robots ##service##----------*/
bool GrsimNodelet::GrsimRobotReplacesrv(parsian_msgs::grsim_robot_replacement::Request& req,
                          parsian_msgs::grsim_robot_replacement::Response& res)
{
    grSim_RobotReplacement* grsimRReplacement = GrsimReplacement->add_robots();
    grsimRReplacement->set_x(req.x);
    grsimRReplacement->set_y(req.y);
    grsimRReplacement->set_dir(req.dir);
    grsimRReplacement->set_id(req.id);
    grsimRReplacement->set_yellowteam(req.yellowteam);
    return true;
}

/*----------filing grsim_ball_replacement with ROS datas ##service##----------*/
bool GrsimNodelet::GrsimBallReplacesrv(parsian_msgs::grsim_ball_replacement::Request& req,
                         parsian_msgs::grsim_ball_replacement::Response& res)
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
//    std::string color;
//    ros::param::get("team_color", color);
//    bool col = ! (color == "yellow");          //check if it is true!
    GrsimCommand->set_isteamyellow(color);
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

void GrsimNodelet::visionCB(const parsian_msgs::ssl_vision_detectionConstPtr & msg){
    send();
}

//void GrsimNodelet::timerCb(const ros::TimerEvent& event){
//    // Using timers is the preferred 'ROS way' to manual threading
//    send();
//}

/*----------get ip address and port from configue server----------*/
void GrsimNodelet::conf(protobuf_wrapper_config::grsimConfig &config, uint32_t level)
{
    if(this->ip != config.grsim_send_ip)
    {
        this->ip = config.grsim_send_ip;
        udp->setIP(this->ip);
    }
    if(this->port  != config.grsim_send_port)
    {
        this->port  = config.grsim_send_port;
        udp->setport(this->port);
    }

    NODELET_INFO("grsim_nodelet binded on %s : %d", this->ip.c_str(), this->port);

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
    vision_sub= n.subscribe<parsian_msgs::parsian_world_model>("world_model",1000,boost::bind(& GrsimNodelet::visionCB, this, _1));
    service0 = n.advertiseService<parsian_msgs::grsim_robot_replacement::Request,
            parsian_msgs::grsim_robot_replacement::Response>
            ("GrsimRobotReplacesrv", boost::bind(& GrsimNodelet::GrsimRobotReplacesrv, this, _1, _2));
    service1 = n.advertiseService<parsian_msgs::grsim_ball_replacement::Request,
                                  parsian_msgs::grsim_ball_replacement::Response>
                                  ("GrsimBallReplacesrv", boost::bind(& GrsimNodelet::GrsimBallReplacesrv, this, _1, _2));

    f = boost::bind(& GrsimNodelet::conf, this, _1, _2);
    server.setCallback(f);

    vision_sub= n.subscribe<parsian_msgs::ssl_vision_detection>("vision_detection",1000,boost::bind(& GrsimNodelet::visionCB, this, _1));
    //timer_ = n.createTimer(ros::Duration(1.0), boost::bind(& GrsimNodelet::timerCb, this, _1));

    GrsimCommand = new grSim_Commands;
    GrsimReplacement = new grSim_Replacement;

    std::string col;
    ros::param::get("team_color", color);
    color = ! (col == "yellow");          //check if it is true!

void GrsimNodelet::visionCB(const parsian_msgs::parsian_world_modelConstPtr & msg){
    send();
    ROS_INFO("GRSIM");
}



PLUGINLIB_DECLARE_CLASS(parsian_protobuf_wrapper, GrsimNodelet, GrsimNodelet, nodelet::Nodelet);
//PLUGINLIB_EXPORT_CLASS(Server, nodelet::Nodelet)
