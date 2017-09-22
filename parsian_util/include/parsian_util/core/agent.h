#ifndef AGENT_H
#define AGENT_H

#include "parsian_util/geom/geom.h"
#include "parsian_msgs/parsian_agent.h"
#include "parsian_msgs/parsian_robot.h"
#include "parsian_util/core/movingobject.h"


class CRobot : public CMovingObject {
public:
    CRobot(const parsian_msgs::parsian_robot& _robot) {
        this->pos = rcsc::Vector2D(_robot.pos.x, _robot.pos.y);
        this->vel = rcsc::Vector2D(_robot.vel.x, _robot.vel.y);
        this->acc = rcsc::Vector2D(_robot.acc.x, _robot.acc.y);
        this->dir = rcsc::Vector2D(_robot.dir.x, _robot.dir.y);

        this->id  = _robot.id;
        this->angularVel = _robot.angularVel;
    }
    bool isActive() { return true ; }
    int id;
};

class CAgent {
public:
    CAgent(const parsian_msgs::parsian_agent& _agent);
    Vector2D pos();
    Vector2D vel();
    Vector2D dir();
    Vector2D acc();
    int       id();


    CRobot self;
};

#endif // AGENT_H
