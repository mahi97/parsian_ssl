#include "parsian_protobuf_wrapper/grsim/grsimnodelet.h"
#include <pluginlib/class_list_macros.h>


grSim_Commands* GrsimCommand = new grSim_Commands;
grSim_Replacement* GrsimReplacement = new grSim_Replacement;


/*----------filing grSim_Robot_Command with ROS datas for each robots----------*/
void GrsimBotCmd(const parsian_msgs::grsim_robot_command::ConstPtr& msg)
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
void GrsimRobotReplace(const parsian_msgs::grsim_robot_replacement::ConstPtr& msg)
{
    grSim_RobotReplacement* grsimRReplacement = GrsimReplacement->add_robots();
    grsimRReplacement->set_x(msg->x);
    grsimRReplacement->set_y(msg->y);
    grsimRReplacement->set_dir(msg->dir);
    grsimRReplacement->set_id(msg->id);
    grsimRReplacement->set_yellowteam(msg->yellowteam);
}

/*----------filing grsim_robot_replacement with ROS datas for each robots ##service##----------*/
bool GrsimRobotReplacesrv(parsian_msgs::grsim_robot_replacement_srv::Request& req,
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
void GrsimBallReplace(const parsian_msgs::grsim_ball_replacement::ConstPtr& msg)
{
    grSim_BallReplacement* GrsimBReplacement = new grSim_BallReplacement;
    GrsimBReplacement->set_x(msg->x);
    GrsimBReplacement->set_y(msg->y);
    GrsimBReplacement->set_vx(msg->vx);
    GrsimBReplacement->set_vy(msg->vy);
    GrsimReplacement->set_allocated_ball(GrsimBReplacement);
}

/*----------filing grsim_ball_replacement with ROS datas ##service##----------*/
bool GrsimBallReplacesrv(parsian_msgs::grsim_ball_replacement_srv::Request& req,
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
void send()
{
    int port{12340};
    std::string ip{"127.0.0.1"};
    grSim_Packet packet;
    std::string color;
    ros::param::get("team_color", color);
    bool col = ! (color == "yellow");          //check if it is true!
    GrsimCommand->set_isteamyellow(col);
    GrsimCommand->set_timestamp(0.0);                       //should fix this
    packet.set_allocated_commands(GrsimCommand);
    packet.set_allocated_replacement(GrsimReplacement);
    std::string buffer;
    packet.SerializeToString(&buffer);
    UDPSend udp(ip, port);
    udp.send(buffer);
    GrsimCommand = new grSim_Commands;
    GrsimReplacement = new grSim_Replacement;

}

void GrsimNodelet::timerCb(const ros::TimerEvent& event){
  // Using timers is the preferred 'ROS way' to manual threading
 send();
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
    n = getNodeHandle();
    sub0 = n.subscribe("GrsimBotCmd0", 1000, GrsimBotCmd);
    sub1 = n.subscribe("GrsimBotCmd1", 1000, GrsimBotCmd);
    sub2 = n.subscribe("GrsimBotCmd2", 1000, GrsimBotCmd);
    sub3 = n.subscribe("GrsimBotCmd3", 1000, GrsimBotCmd);
    sub4 = n.subscribe("GrsimBotCmd4", 1000, GrsimBotCmd);
    sub5 = n.subscribe("GrsimBotCmd5", 1000, GrsimBotCmd);
    sub_0 = n.subscribe("GrsimRobotReplace0", 1000, GrsimRobotReplace);
    _sub = n.subscribe("GrsimBallReplace", 1000, GrsimBallReplace);
    service0 = n.advertiseService("GrsimRobotReplacesrv", GrsimRobotReplacesrv);
    service1 = n.advertiseService("GrsimBallReplacesrv", GrsimBallReplacesrv);
    timer_ = n.createTimer(ros::Duration(1.0), boost::bind(& GrsimNodelet::timerCb, this, _1));
}

PLUGINLIB_DECLARE_CLASS(parsian_protobuf_wrapper, GrsimNodelet, GrsimNodelet, nodelet::Nodelet);
//PLUGINLIB_EXPORT_CLASS(Server, nodelet::Nodelet)
