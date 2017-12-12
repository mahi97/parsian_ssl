#ifndef WORLDMODEL_H
#define WORLDMODEL_H

#include <parsian_util/core/worldmodel.h>

class WorldModel : public CWorldModel
{
public:

    WorldModel();
    WorldModel(const parsian_msgs::parsian_world_model &_pwm);

    ~WorldModel();

    bool AHZIsInOurPenaltyArea(Vector2D point);
    QList<Vector2D> AHZOurPAreaIntersect(Segment2D segment,QString role);




private:


};

extern WorldModel * wm;

#endif // KNOWLEDGE_H