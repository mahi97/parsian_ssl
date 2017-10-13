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

void AI::updateRobotStatus(const parsian_msgs::parsian_robotConstPtr & _rs) {

}

void AI::updateWM(const parsian_msgs::parsian_world_modelConstPtr & _wm) {

}

void AI::updateReferee(const parsian_msgs::ssl_refree_wrapperConstPtr & _ref) {
    gameState->setRefree(_ref);
    if(gameState->ready())
        ROS_INFO("is ready");
    if(gameState->isPlayOff())
        ROS_INFO("is play off");
    if(gameState->isPlayOn())
        ROS_INFO("is play on");
    if(gameState->canMove())
        ROS_INFO("is not halt");
    ROS_INFO("is running");

}

void AI::publish(std::vector<ros::Publisher*> publishers) {
//        for(CAgent* agent : soccer->agents) {
//            if (agent.)
//            publishers.at(i)->getTopic();
//        }

}