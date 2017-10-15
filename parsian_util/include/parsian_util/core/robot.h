//
// Created by parsian-ai on 10/13/17.
//

#ifndef PARSIAN_UTIL_ROBOT_H
#define PARSIAN_UTIL_ROBOT_H

#include "parsian_util/geom/geom.h"
#include "parsian_msgs/parsian_robot.h"
#include "parsian_util/core/movingobject.h"


class CRobot : public CMovingObject {
public:
    CRobot();
    CRobot(const parsian_msgs::parsian_robot& _robot);
    bool isActive();
    void setActive(bool _active);
    int id;

private:
    bool active;

};

#endif //PARSIAN_UTIL_ROBOT_H
