//
// Created by parsian-ai on 9/22/17.
//

#include <parsian_ai/ai.h>


AI::AI() {
    soccer = new CSoccer();
    wm = new CWorldModel();
}

AI::~AI() {

}

void AI::execute() {
    soccer->execute();
}

void AI::updateRobotStatus(const parsian_msgs::parsian_robot & _rs) {

}

void AI::updateWM(const parsian_msgs::parsian_world_model & _wm) {

}

void AI::updateReferee(const parsian_msgs::ssl_refree_wrapper & _ref) {

}

void AI::publish(std::vector<ros::Publisher*> publishers) {
//        for(CAgent* agent : soccer->agents) {
//            if (agent.)
//            publishers.at(i)->getTopic();
//        }

}