#ifndef AGENT_H
#define AGENT_H

#include "parsian_util/geom/geom.h"
#include "parsian_msgs/parsian_agent.h"
#include "parsian_msgs/parsian_robot.h"

class CRobot {
public:
    CRobot(const parsian_msgs::parsian_robot& _robot) {
        this->pos = rcsc::Vector2D(_robot.pos.x, _robot.pos.y);
        this->vel = rcsc::Vector2D(_robot.vel.x, _robot.vel.y);
        this->acc = rcsc::Vector2D(_robot.acc.x, _robot.acc.y);
        this->dir = rcsc::Vector2D(_robot.dir.x, _robot.dir.y);
        this->angularVel = _robot.angularVel;
    }

    rcsc::Vector2D pos, vel, acc, dir;
    double angularVel;

};

class CAgent {
public:
    CAgent(const parsian_msgs::parsian_agent& _agent);
    Vector2D pos();
    Vector2D vel();
    Vector2D dir();
    Vector2D acc();



    CRobot self;
};

#endif // AGENT_H
