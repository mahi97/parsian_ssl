//
// Created by parsian-ai on 9/21/17.
//

#include <parsian_util/core/ball.h>

CBall::CBall(const parsian_msgs::parsian_robot &_ball) : CMovingObject(_ball) {
}

CBall::CBall() : CMovingObject() {}

CBall::~CBall() {

}

double CBall::whenBallReachToPoint(double dist) const
{
    double v2 = vel.length()*vel.length();
    double a = getBallAcc();
    double _time = 0;
    if((v2 - 2*a*dist) < 0)
    {
        return -1;
    }
    _time = (vel.length() - sqrt(v2 - 2*a*dist))/a;
    return _time;

}

double CBall::getBallAcc() const {
    // return Gravity*BallFriction();
    return 0.17;
    return this->acc.length();
}

Vector2D CBall::getPosInFuture(double _t) const
{
    return pos + (-0.5*(getBallAcc())*_t*_t + vel.length()*_t)*vel.norm();
}