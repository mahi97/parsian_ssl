//
// Created by parsian-ai on 9/21/17.
//

#ifndef PARSIAN_UTIL_BALL_H
#define PARSIAN_UTIL_BALL_H

#include <parsian_msgs/parsian_robot.h>
#include "parsian_util/core/movingobject.h"

class CBall : public CMovingObject {
public:
    CBall(const parsian_msgs::parsian_robot& _robot);
    ~CBall();
    double whenBallReachToPoint(double dist) const;
    Vector2D getPosInFuture(double _t) const;
    double getBallAcc() const;


private:


};


#endif //PARSIAN_UTIL_BALL_H
