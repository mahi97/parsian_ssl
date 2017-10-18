//
// Created by parsian-ai on 9/22/17.
//

#include <parsian_ai/ai.h>
#include <parsian_ai/gamestate.h>


AI::AI() {
    soccer = new CSoccer();
    wm = new CWorldModel();
    gameState =new GameState();
}

AI::~AI() {

}

void AI::execute() {
    soccer->execute();
}

parsian_msgs::parsian_robot_task AI::getTask(int robotID){
    for(int i=0; i<wm->our.activeAgentsCount(); i++) {
        if (wm->our.activeAgentID(i) == robotID) {
            if (soccer->agents[robotID]->action->getName() == "kick" /*KickAction::getName()*/) {
                parsian_msgs::parsian_skill_kick *task;
                task = reinterpret_cast<parsian_skill_kick *>(soccer->agents[robotID]->action->getMessage());
                robotTask[robotID].kickTask = *task;
                robotTask[robotID].select = robotTask[robotID].KICK;
            } else if (false) {

            }

            return robotTask[robotID];
        }
    }

}

void AI::updateRobotStatus(const parsian_msgs::parsian_robotConstPtr & _rs) {

}

void AI::updateWM(const parsian_msgs::parsian_world_modelConstPtr & _wm) {
    wm->update(*_wm);
}

void AI::updateReferee(const parsian_msgs::ssl_refree_wrapperConstPtr & _ref) {
    gameState->setRefree(_ref);
    if(gameState->ready())
        DEBUG("is ready", D_MAHI);
    if(gameState->isPlayOff())
        DEBUG("is play off", D_MAHI);
    if(gameState->isPlayOn())
        DEBUG("is play on", D_MAHI);
    if(gameState->canMove())
        DEBUG("is not halt", D_MAHI);
    DEBUG("is running", D_MAHI);

}

void AI::publish(std::vector<ros::Publisher*> publishers) {
//        for(CAgent* agent : soccer->agents) {
//            if (agent.)
//            publishers.at(i)->getTopic();
//        }

}