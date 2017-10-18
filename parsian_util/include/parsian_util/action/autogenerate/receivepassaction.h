// auto-generated don't edit !

#ifndef ReceivepassAction_HEADER_
#define ReceivepassAction_HEADER_


#include <parsian_util/action/action.h>
#include <parsian_util/geom/geom.h>
#include <parsian_msgs/parsian_skill_receivePass.h>


class ReceivepassAction : public Action {

public:

    virtual void setMessage(void* _msg);

    virtual void* getMessage();

    SkillProperty(ReceivepassAction, quint8, Robot_Id, robot_id);
    SkillProperty(ReceivepassAction, bool, Avoidourpenaltyarea, avoidOurPenaltyArea);
    SkillProperty(ReceivepassAction, bool, Avoidopppenaltyarea, avoidOppPenaltyArea);
    SkillProperty(ReceivepassAction, bool, Slow, slow);
    SkillProperty(ReceivepassAction, double, Receiveradius, receiveRadius);
    SkillProperty(ReceivepassAction, bool, Ignoreangle, ignoreAngle);
    SkillProperty(ReceivepassAction, Vector2D, Target, target);
    SkillProperty(ReceivepassAction, Vector2D, Iatargetdir, IATargetDir);

};

#endif // ReceivepassAction_HEADER_