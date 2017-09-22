//
// Created by parsian-ai on 9/21/17.
//

#include <parsian_util/core/ball.h>

CBall::CBall(const parsian_msgs::parsian_robot &_ball) : CMovingObject() {

    this->pos = rcsc::Vector2D(_ball.pos.x, _ball.pos.y);
    this->vel = rcsc::Vector2D(_ball.vel.x, _ball.vel.y);
    this->acc = rcsc::Vector2D(_ball.acc.x, _ball.acc.y);
    this->dir = rcsc::Vector2D(_ball.dir.x, _ball.dir.y);

    this->angularVel = _ball.angularVel;
}

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