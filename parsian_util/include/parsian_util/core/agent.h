#ifndef AGENT_H
#define AGENT_H

#include "parsian_msgs/parsian_agent.h"
#include <parsian_util/core/robot.h>


class CRobot : public CMovingObject {
private:
    const double robot_radius_old = 0.090;
//const double CRobot::robot_radius_new = 0.080;
    const double robot_radius_new = 0.0890;
    const double center_from_kicker_old = 0.075;
//const double CRobot::center_from_kicker_new = 0.070;
    const double center_from_kicker_new = 0.06;
    const double kicker_width_old = 0.074;
//const double CRobot::kicker_width_new = 0.080;
    const double kicker_width_new = 0.082;
    const double wheel_rad_old = 0.035;
//const double CRobot::wheel_rad_new = 0.03;
    const double wheel_rad_new = 0.027;
    bool newRobot;
    bool active;


public:
    CRobot() {
        newRobot= false;
    };


    void setNewRobot(bool _new)
    {
        newRobot = _new;
    }

    bool isNewRobot()
    {
        return newRobot;
    }

    double robotRadius()
    {
        return (newRobot) ? robot_radius_new : robot_radius_old;
    }

    double kickerWidth()
    {
        return (newRobot) ? kicker_width_new : kicker_width_old;
    }

    double centerFromKicker()
    {
        return (newRobot) ? center_from_kicker_new : center_from_kicker_old;
    }

    double wheelRadius()
    {
        return (newRobot) ? wheel_rad_new : wheel_rad_old;
    }
    CRobot(const parsian_msgs::parsian_robot& _robot) :
            CMovingObject(_robot), id(_robot.id) {};
    bool isActive() { return active ; }
    void setActive(bool _active) { active = _active; }
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
    bool isVisible() {return self.isActive(); }

    CRobot self;
};

#endif // AGENT_H
