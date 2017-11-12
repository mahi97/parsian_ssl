#include <common_status_nodelet.h>

PLUGINLIB_DECLARE_CLASS(parsian_agent, CommonStatus, CommonStatus, nodelet::Nodelet);

void CommonStatus::onInit(){
    ROS_INFO("CommonStatus onInit");

    nh = getNodeHandle();
    private_nh = getPrivateNodeHandle();

    common_status_pub = nh.advertise<parsian_msgs::parsian_robot_common_status>("common_status", 10);

    server.reset(new dynamic_reconfigure::Server<agent_config1::commonstatusConfig>(private_nh));
    dynamic_reconfigure::Server<agent_config1::commonstatusConfig>::CallbackType f;
    f = boost::bind(&CommonStatus::ConfigServerCallBack,this, _1, _2);
    server->setCallback(f);

}


void CommonStatus::ConfigServerCallBack(const agent_config1::commonstatusConfig &config, uint32_t level)
{
    parsian_msgs::parsian_robot_common_statusPtr msg{new parsian_msgs::parsian_robot_common_status};
    //BangBang
    msg->AccMaxForward = config.AccMaxForward;
    msg->AccMaxNormal = config.AccMaxNormal;
    msg->DecMax = config.DecMax;
    msg->VelMax = config.VelMax;
    msg->posKP = config.posKP;
    msg->posKI = config.posKI;
    msg->posKD = config.posKD;
    msg->thKP = config.thKP;
    msg->thKI = config.thKI;
    msg->thKD = config.thKD;
    //ERRT
    msg->Draw_Path = config.Draw_Path;
    msg->Goal_Probablity = config.Goal_Probablity;
    msg->MWaypoint_Catch_Probablityax = config.MWaypoint_Catch_Probablityax;
    msg->Extend_Step = config.Extend_Step;
    msg->Target_Distance_Threshold = config.Target_Distance_Threshold;
    //SkillsParametersKickOneTouch
    msg->Landa = config.Landa;
    msg->Gamma = config.Gamma;
    msg->Delay = config.Delay;
    msg->TimeFactor = config.TimeFactor;

    common_status_pub.publish(msg);


}
