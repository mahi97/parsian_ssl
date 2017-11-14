
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
    POMODE mode;
    if(_planReq.get()->gameMode == _planReq.get()->DIRECT) {
        mode = DIRECT;
    } else if(_planReq.get()->gameMode == _planReq.get()->KICKOFF) {
        mode = KICKOFF;
    } else if(_planReq.get()->gameMode == _planReq.get()->INDIRECT) {
        mode = INDIRECT;
    }

    QList<int> ourPlayers;
    for(int i=0; i< _planReq.get()->ourPlayers.size(); i++){
        ourPlayers.append((int)_planReq.get()->ourPlayers.at(i));
    }

    planSelector->initStaticPlay(mode, ourPlayers);
    NGameOff::SPlan* selectedPlan = planSelector->getSelectedPlan();

    parsian_msgs::parsian_plan planMsg;

    for (int k = 0; k < selectedPlan->execution.AgentPlan.size(); ++k) {
        for (int j = 0; j < selectedPlan->execution.AgentPlan.at(k).size(); ++j) {
            for (int i = 0; selectedPlan->execution.AgentPlan.at(k).at(j).skill.size(); i++) {
                planMsg.agents.at(k).positions.at(j).skills.at(i).name =
                        selectedPlan->execution.AgentPlan.at(k).at(j).skill.at(i).name;
                planMsg.agents.at(k).positions.at(j).skills.at(i).index =
                        selectedPlan->execution.AgentPlan.at(k).at(j).skill.at(i).targetIndex;
                planMsg.agents.at(k).positions.at(j).skills.at(i).agent =
                        selectedPlan->execution.AgentPlan.at(k).at(j).skill.at(i).targetAgent;
                planMsg.agents.at(k).positions.at(j).skills.at(i).secondry =
                        selectedPlan->execution.AgentPlan.at(k).at(j).skill.at(i).data[1];
                planMsg.agents.at(k).positions.at(j).skills.at(i).primary =
                        selectedPlan->execution.AgentPlan.at(k).at(j).skill.at(i).data[0];
            }
            planMsg.agents.at(k).positions.at(j).tolerance =
            selectedPlan->execution.AgentPlan.at(k).at(j).tolerance;
            planMsg.agents.at(k).positions.at(j).angle =
                    selectedPlan->execution.AgentPlan.at(k).at(j).angle.radian();
            planMsg.agents.at(k).positions.at(j).pos.x =
                    selectedPlan->execution.AgentPlan.at(k).at(j).pos.x;
            planMsg.agents.at(k).positions.at(j).pos.y =
                    selectedPlan->execution.AgentPlan.at(k).at(j).pos.y;
        }
//        planMsg.agents.at(k).id = selectedPlan->execution.AgentPlan.at(k).
    }
    planMsg.lastDist = selectedPlan->common.lastDist;

    planPub.publish(planMsg);
}