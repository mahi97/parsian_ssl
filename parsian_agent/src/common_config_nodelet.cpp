#include <common_config_nodelet.h>

PLUGINLIB_DECLARE_CLASS(parsian_agent, CommonConfig, CommonConfig, nodelet::Nodelet);

void CommonConfig::onInit(){
    ROS_INFO("CommonConfig onInit");

    nh = getNodeHandle();
    private_nh = getPrivateNodeHandle();

    server.reset(new dynamic_reconfigure::Server<agent_common_config::commonconfigConfig>(private_nh));
    dynamic_reconfigure::Server<agent_common_config::commonconfigConfig>::CallbackType f;

}