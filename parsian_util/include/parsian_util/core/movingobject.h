//
// Created by parsian-ai on 9/22/17.
//

#ifndef PARSIAN_UTIL_MOVINGOBJECT_H
#define PARSIAN_UTIL_MOVINGOBJECT_H

#include "parsian_util/geom/geom.h"

class CMovingObject
{
public:
    CMovingObject();

    //Final Specifications
    Vector2D pos;
    Vector2D dir;
    Vector2D vel;
    Vector2D acc;

    double angularVel;

    double inSight;
    double obstacleRadius;
    int cam_id;
    Vector2D predict(double time);
    Vector2D predictV(double time);
    Vector2D whereIsAtVel(Vector2D V);
    double whenIsAtVel(double L);
};

#endif //PARSIAN_UTIL_MOVINGOBJECT_H
