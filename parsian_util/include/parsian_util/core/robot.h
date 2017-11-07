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

    static constexpr double robot_radius_old = 0.090;
//static const double CRobot::robot_radius_new = 0.080;
    static constexpr double robot_radius_new = 0.0890;
    static constexpr double center_from_kicker_old = 0.075;
//static const double CRobot::center_from_kicker_new = 0.070;
    static constexpr double center_from_kicker_new = 0.06;
    static constexpr double kicker_width_old = 0.074;
//static const double CRobot::kicker_width_new = 0.080;
    static constexpr double kicker_width_new = 0.082;
    static constexpr double wheel_rad_old = 0.035;
//static const double CRobot::wheel_rad_new = 0.03;
    static constexpr double wheel_rad_new = 0.027;
    CRobot(int id);


    CRobot(const parsian_msgs::parsian_robot& _robot);
    bool isActive();
    void setActive(bool _active);
    int id;


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



private:

    bool newRobot;
    bool active;

};



#endif //PARSIAN_UTIL_ROBOT_H
