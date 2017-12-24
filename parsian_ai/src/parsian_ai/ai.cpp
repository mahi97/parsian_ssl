#include <parsian_ai/ai.h>

AI::AI() {
    wm = new WorldModel();
    gameState = new GameState();
    soccer = new CSoccer();
}

AI::~AI() {
    delete soccer;
    delete wm;
    delete gameState;
}

void AI::execute() {
    soccer->execute();
}

parsian_msgs::parsian_robot_task AI::getTask(int robotID) {
    if (wm->our.data->activeAgents.contains(robotID)) {
        ROS_INFO_STREAM("HEY" << robotID);
        if (soccer->agents[robotID]->action != nullptr) {
            if (soccer->agents[robotID]->action->getActionName() == KickAction::SActionName()) {
                parsian_msgs::parsian_skill_kick *task;
                task = reinterpret_cast<parsian_skill_kick *>(soccer->agents[robotID]->action->getMessage());
                robotsTask[robotID].kickTask = *task;
                robotsTask[robotID].select = robotsTask[robotID].KICK;

            } else if (soccer->agents[robotID]->action->getActionName() == GotopointavoidAction::SActionName()) {
                parsian_msgs::parsian_skill_gotoPointAvoid *task;
                task = reinterpret_cast<parsian_skill_gotoPointAvoid *>(soccer->agents[robotID]->action->getMessage());
                robotsTask[robotID].gotoPointAvoidTask = *task;
                robotsTask[robotID].select = robotsTask[robotID].GOTOPOINTAVOID;

            } else if (soccer->agents[robotID]->action->getActionName() == GotopointAction::SActionName()) {
                parsian_msgs::parsian_skill_gotoPoint *task;
                task = reinterpret_cast<parsian_skill_gotoPoint *>(soccer->agents[robotID]->action->getMessage());
                robotsTask[robotID].gotoPointTask = *task;
                robotsTask[robotID].select = robotsTask[robotID].GOTOPOINT;

            } else if (soccer->agents[robotID]->action->getActionName() == ReceivepassAction::SActionName()) {
                parsian_msgs::parsian_skill_receivePass *task;
                task = reinterpret_cast<parsian_skill_receivePass *>(soccer->agents[robotID]->action->getMessage());
                robotsTask[robotID].receivePassTask = *task;
                robotsTask[robotID].select = robotsTask[robotID].RECIVEPASS;

            } else if (soccer->agents[robotID]->action->getActionName() == OnetouchAction::SActionName()) {
                parsian_msgs::parsian_skill_oneTouch *task;
                task = reinterpret_cast<parsian_skill_oneTouch *>(soccer->agents[robotID]->action->getMessage());
                robotsTask[robotID].oneTouchTask = *task;
                robotsTask[robotID].select = robotsTask[robotID].ONETOUCH;
            }
        } else {
            // TODO : No Action
        }
//        ROS_INFO_STREAM("MAHI : " << robotID << soccer->agents[robotID]->action->getActionName().toStdString());
    }

    return robotsTask[robotID];
}

void AI::updateRobotStatus(const parsian_msgs::parsian_robotConstPtr & _rs) {

}

void AI::updateWM(const parsian_msgs::parsian_world_modelConstPtr & _wm) {
    wm->update(_wm);

}

void AI::updateReferee(const parsian_msgs::ssl_refree_wrapperConstPtr & _ref) {
    gameState->setRefree(_ref);
    if(gameState->ready()) {
        DEBUG("is ready", D_MAHI);
    }
    if(gameState->isPlayOff()) {
        DEBUG("is play off", D_MAHI);
    }
    if(gameState->isPlayOn()) {
        DEBUG("is play on", D_MAHI);
    }
    if(gameState->canMove()) {
        DEBUG("is not halt", D_MAHI);
    }
    DEBUG("is running", D_MAHI);

}
