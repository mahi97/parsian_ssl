//
// Created by parsian-ai on 9/22/17.
//

#include <parsian_ai/ai.h>

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

    for (int j = 0; j < _MAX_NUM_PLAYERS; ++j) {
        if (soccer->agents[j]->action->getName() == "kick" /*KickAction::getName()*/) {
            kick *task;
            task = (parsian_msgs::kick*)(soccer->agents[j]->action->getMessage());
            robotsTask[soccer->agents[j]->id()].kickTask.push_back(*task);
        }
        else if(soccer->agents[j]->action->getName() == "gotoPoint" /*GotopointAction::getName()*/) {
            gotoPoint *task;
            task = (parsian_msgs::gotoPoint*)(soccer->agents[j]->action->getMessage());
            robotsTask[soccer->agents[j]->id()].gotoPointTask.push_back(*task);
        }
        else if(soccer->agents[j]->action->getName() == "gotoPointAvoid"/*GotopointavoidAction::getName()*/) {
            gotoPointAvoid *task;
            task = (parsian_msgs::gotoPointAvoid*)(soccer->agents[j]->action->getMessage());
            robotsTask[soccer->agents[j]->id()].gotoPointAvoidTask.push_back(*task);
        }
        else if(soccer->agents[j]->action->getName() == "receive" /*ReceivepassAction::getName()*/) {
            receivePass *task;
            task = (parsian_msgs::receivePass*)(soccer->agents[j]->action->getMessage());
            robotsTask[soccer->agents[j]->id()].receivePassTask.push_back(*task);
        }
        else if(soccer->agents[j]->action->getName() == "oneTouch" /*OnetouchAction::getName()*/) {
            oneTouch *task;
            task = (parsian_msgs::oneTouch*)(soccer->agents[j]->action->getMessage());
            robotsTask[soccer->agents[j]->id()].oneTouchTask.push_back(*task);
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
