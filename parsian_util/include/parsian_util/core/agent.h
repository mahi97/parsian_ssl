#ifndef AGENT_H
#define AGENT_H

#include "parsian_util/geom/geom.h"
#include "parsian_msgs/parsian_agent.h"
#include "parsian_msgs/parsian_robot.h"
#include "parsian_util/core/movingobject.h"


class CRobot : public CMovingObject {
public:
    CRobot() {};
    CRobot(const parsian_msgs::parsian_robot& _robot) :
            CMovingObject(_robot), id(_robot.id) {};
    bool isActive() { return active ; }
    void setActive(bool _active) { active = _active; }
    int id;

private:
    bool active;

};

class CAgent {
public:
    CAgent(const parsian_msgs::parsian_agent& _agent);
    Vector2D pos();
    Vector2D vel();
    Vector2D dir();
    Vector2D acc();
    int       id();
    bool isVisible() {return self.isActive(); }

    CRobot self;
};

#endif // AGENT_H
