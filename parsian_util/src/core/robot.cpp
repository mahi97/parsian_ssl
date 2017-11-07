//
// Created by parsian-ai on 10/13/17.
//

#include <parsian_util/core/robot.h>


CRobot::CRobot(int id){
    this->id=id;
    setActive(false);
}
CRobot::CRobot(const parsian_msgs::parsian_robot& _robot) : CMovingObject(_robot), id(_robot.id) {
    setActive(false);
}
bool CRobot::isActive() { return active ; }
void CRobot::setActive(bool _active) { active = _active; }
