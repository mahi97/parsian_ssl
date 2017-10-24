#include "parsian_protobuf_wrapper/grsim/grsim_nodelet.h"
#include <pluginlib/class_list_macros.h>


PLUGINLIB_EXPORT_CLASS(GrsimNodelet, nodelet::Nodelet);

/*----------filing grSim_Robot_Command with ROS datas for each robots----------*/
void GrsimNodelet::robotCommandCb(const parsian_msgs::grsim_robot_commandConstPtr &msg){

    ROS_INFO("message recived");
    auto* GrsimRobotCommand = grsimCommand->add_robot_commands();
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


/*----------filing grsim_robot_replacement with ROS datas for each robots ##service##----------*/
bool GrsimNodelet::robotReplaceCb(parsian_msgs::grsim_robot_replacementRequest &req ,
                                  parsian_msgs::grsim_robot_replacementResponse &res){

    auto* grsimRReplacement = grsimReplacement->add_robots();
    grsimRReplacement->set_x(req.x);
    grsimRReplacement->set_y(req.y);
    grsimRReplacement->set_dir(req.dir);
    grsimRReplacement->set_id(req.id);
    grsimRReplacement->set_yellowteam(req.yellowteam);
    return true;
}


bool GrsimNodelet::ballReplaceCb(parsian_msgs::grsim_ball_replacementRequest &req ,
                                 parsian_msgs::grsim_ball_replacementResponse &res) {

    auto * GrsimBReplacement = grsimReplacement->mutable_ball();
    GrsimBReplacement->set_x(req.x);
    GrsimBReplacement->set_y(req.y);
    GrsimBReplacement->set_vx(req.vx);
    GrsimBReplacement->set_vy(req.vy);
    return true;
}

/*----------creating a full grSim_Packet protocol buffer and sending it----------*/
void GrsimNodelet::send()
{

    std::string color;
    ros::param::get("team_color", color);
    bool col = ! (color == "yellow");          //check if it is true!
    grsimCommand->set_isteamyellow(col);
    grsimCommand->set_timestamp(0.0);                       //should fix this
    std::string buffer;
    packet->set_allocated_commands(grsimCommand);
   // packet->set_allocated_replacement(grsimReplacement);
    if (packet->commands().robot_commands_size()) {
        ROS_INFO_STREAM("send");
        packet->SerializeToString(&buffer);
        udp->send(buffer);
    }

    packet->clear_commands();
    packet->clear_replacement();
}

void GrsimNodelet::onInit()
{
    port = 34567;//{this->port};
    ip = "127.0.0.1";//{this->ip};
    NODELET_INFO("grsim_nodelet onInit");
    n = getNodeHandle();
    sub1 = n.subscribe("GrsimBotCmd1", 1000, &GrsimNodelet::robotCommandCb, this);
    sub2 = n.subscribe("GrsimBotCmd2", 1000, &GrsimNodelet::robotCommandCb, this);
    sub0 = n.subscribe("GrsimBotCmd0", 1000, &GrsimNodelet::robotCommandCb, this);
    sub3 = n.subscribe("GrsimBotCmd3", 1000, &GrsimNodelet::robotCommandCb, this);
    sub4 = n.subscribe("GrsimBotCmd4", 1000, &GrsimNodelet::robotCommandCb, this);
    sub5 = n.subscribe("GrsimBotCmd5", 1000, &GrsimNodelet::robotCommandCb, this);
    service0 = n.advertiseService("grsim_robot_replace_server", &GrsimNodelet::robotReplaceCb, this);
    service1 = n.advertiseService("grsim_ball_replace_server", &GrsimNodelet::ballReplaceCb, this);
    timer_ = n.createTimer(ros::Duration(0.016), boost::bind(& GrsimNodelet::timerCb, this, _1));
    packet =new grSim_Packet();
    grsimCommand = packet->mutable_commands();
    grsimReplacement = packet->mutable_replacement();

    //server.reset(new dynamic_reconfigure::Server<protobuf_wrapper_config::grsimConfig>);
    //dynamic_reconfigure::Server<protobuf_wrapper_config::grsimConfig>::CallbackType f;
    //f = boost::bind(&GrsimNodelet::UpdatePortIp,this, _1, _2);
    //server->setCallback(f);
    udp = new UDPSend(ip, port);



}

void GrsimNodelet::timerCb(const ros::TimerEvent& event){
    // Using timers is the preferred 'ROS way' to manual threading
    send();
}

void GrsimNodelet::UpdatePortIp(const protobuf_wrapper_config::grsimConfig &config, uint32_t level)
{
//    ROS_INFO_STREAM("the new port is:" << config.grsim_send_port << " and the new ip is" << config.grsim_send_ip);
//    port = config.grsim_send_port;
//    ip = config.grsim_send_ip;
}

