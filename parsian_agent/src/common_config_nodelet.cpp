#include <common_config_nodelet.h>

PLUGINLIB_DECLARE_CLASS(parsian_agent, CommonConfig, CommonConfig, nodelet::Nodelet);

void CommonConfig::onInit(){
    ROS_INFO("CommonConfig onInit");
<<<<<<< HEAD

    nh = getNodeHandle();
    private_nh = getPrivateNodeHandle();

    common_config_pub = nh.advertise<parsian_msgs::parsian_robot_common_config>("common_config", 10);

    server.reset(new dynamic_reconfigure::Server<agent_common_config::commonconfigConfig>(private_nh));
    dynamic_reconfigure::Server<agent_common_config::commonconfigConfig>::CallbackType f;
    f = boost::bind(&CommonConfig::ConfigServerCallBack,this, _1, _2);
    server->setCallback(f);

}


void CommonConfig::ConfigServerCallBack(const agent_common_config::commonconfigConfig &config, uint32_t level)
{
    parsian_msgs::parsian_robot_common_configPtr msg{new parsian_msgs::parsian_robot_common_config};
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
    msg->Waypoint_Catch_Probablity = config.Waypoint_Catch_Probablity;
    msg->Extend_Step = config.Extend_Step;
    msg->Target_Distance_Threshold = config.Target_Distance_Threshold;
    //SkillsParametersKickOneTouch
    msg->Landa = config.Landa;
    msg->Gamma = config.Gamma;
    msg->Delay = config.Delay;
    msg->TimeFactor = config.TimeFactor;

    common_config_pub.publish(msg);


}
=======
    private_nh = getPrivateNodeHandle();
    server.reset(new dynamic_reconfigure::Server<agent_common::agentConfig>(private_nh));
}
>>>>>>> 5af32b0f31cbffeb42da4f8be34496f49cee0755
