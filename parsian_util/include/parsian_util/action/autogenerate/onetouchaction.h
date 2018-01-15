// auto-generated don't edit !

#ifndef OnetouchAction_HEADER_
#define OnetouchAction_HEADER_



#include <parsian_util/action/action.h>
#include <parsian_util/geom/geom.h>
#include <parsian_msgs/parsian_skill_oneTouch.h>
#include <list>

class OnetouchAction : public Action {

public:
    OnetouchAction();
    void setMessage(const void* _msg);

    void* getMessage();

    QString getActionName() override;
    static QString SActionName();


    SkillProperty(OnetouchAction, bool, Chip, chip);
    SkillProperty(OnetouchAction, double, Kickspeed, kickSpeed);
    SkillProperty(OnetouchAction, bool, Shottoemptyspot, shotToEmptySpot);
    SkillProperty(OnetouchAction, Vector2D, Waitpos, waitPos);
    SkillProperty(OnetouchAction, Vector2D, Target, target);


};

#endif // OnetouchAction_HEADER_
