//
// Created by parsian-ai on 9/22/17.
//

#include "parsian_util/core/movingobject.h"


CMovingObject::CMovingObject() = default;

Vector2D CMovingObject::predict(double time)
{
    //must be checked if it works precisely or not
    if (time < 0.001)
        return pos;

    if (acc.length() < 0.01 || vel.length() < 0.1)
    {
        return pos;
    }

    if (acc.valid() && (acc*vel<0))
    {
        double vf = (vel.length() - acc.length()*time);
        if (vf < 0) return pos + vel*vel.length() / (2.0*acc.length());
        return pos - 0.5*acc.length()*vel.norm()*time*time + vel*time;
    }
    return pos + vel*time;
}


Vector2D CMovingObject::predictV(double time)
{
    if (acc.valid() && (acc*vel<0))
    {
        double vf = vel.length() - (acc.length()*time);
        if (vf < 0.0) vf = 0.0;
        return vel.norm() * vf;
    }
    return vel;
}

Vector2D CMovingObject::whereIsAtVel(Vector2D V)
{
    double dx=(V.x*V.x - vel.x*vel.x)/(2*acc.x);
    double dy=(V.y*V.y - vel.y*vel.y)/(2*acc.y);
    return Vector2D{pos.x + dx, pos.y + dy};
}

double CMovingObject::whenIsAtVel(double L)
{
    double A=acc.x*acc.x + acc.y*acc.y;
    double B=2*(vel.x*acc.x + vel.y*acc.y);
    double C=vel.x*vel.x+vel.y*vel.y-L*L;

    double delta=B*B - 4*A*C;
    if(delta<0)
        return -1;
    double T1 = (-B  + sqrt(delta))/(2*A);
    double T2 = (-B  - sqrt(delta))/(2*A);

    if(T1>0)
    {
        if(T2>0)
        {
            return min(T1,T2);
        }

        return T1;

    }

    if(T2>0)
    {
        return T2;
    }

    return -2;
}

