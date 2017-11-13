
#include <plan_server/plan_server_nodelet.h>

PLUGINLIB_EXPORT_CLASS(plan_server::PlanServerNodelet, nodelet::Nodelet);

using namespace plan_server;


void PlanServerNodelet::onInit() {

    ros::NodeHandle &nh = getNodeHandle();
    ros::NodeHandle &private_nh = getPrivateNodeHandle();
    ROS_INFO("inited");

    drawer = new Drawer();
    debugger = new Debugger();

    planSelector = new CPlanSelector();


    planPub = nh.advertise<parsian_msgs::parsian_plan>("/playoff_plan", 1000);
    planReqSub = nh.subscribe("/ai_plan_request", 1000, &PlanServerNodelet::PlanRequestCallBack, this);

}

void PlanServerNodelet::PlanRequestCallBack(const parsian_msgs::parsian_ai_plan_requestConstPtr &_planReq){
    POMODE mode = (POMODE)_planReq.get()->gameMode;
    QList<int> ourPlayers;
    for(int i=0; i< _planReq.get()->ourPlayers.size(); i++){
        ourPlayers.append((int)_planReq.get()->ourPlayers.at(i));
    }

    planSelector->initStaticPlay(mode, ourPlayers);
}