// auto-generated don't edit !

#ifndef OnetouchAction_HEADER_
#define OnetouchAction_HEADER_



#include <parsian_util/action/action.h>
#include <parsian_util/geom/geom.h>
#include <parsian_msgs/parsian_skill_oneTouch.h>

class OnetouchAction : public Action {

public:

    void setMessage(void* _msg);

    void* getMessage();

    SkillProperty(OnetouchAction, quint8, Robot_Id, robot_id);
    SkillProperty(OnetouchAction, bool, Chip, chip);
    SkillProperty(OnetouchAction, qint32, Kickspeed, kickSpeed);
    SkillProperty(OnetouchAction, double, Disttoballline, distToBallLine);
    SkillProperty(OnetouchAction, double, Velocitytoballline, velocityToBallLine);
    SkillProperty(OnetouchAction, bool, Receivechip, receiveChip);
    SkillProperty(OnetouchAction, bool, Avoidpenaltyarea, avoidPenaltyArea);
    SkillProperty(OnetouchAction, bool, Movetowardtheball, moveTowardTheBall);
    SkillProperty(OnetouchAction, bool, Shottoemptyspot, shotToEmptySpot);
    SkillProperty(OnetouchAction, Vector2D, Waitpos, waitPos);
    SkillProperty(OnetouchAction, Vector2D, Target, target);

};

#endif // OnetouchAction_HEADER_