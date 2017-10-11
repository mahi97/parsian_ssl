// auto-generated don't edit !

#ifndef Receivepass_HEADER_
#define Receivepass_HEADER_


#include <parsian_util/actions/action.h>
#include <parsian_util/geom/geom.h>


class ReceivepassAction : public Action {

public:
    Receivepass();
    ~Receivepass();

    void setMessage(const parsian_msgs:: _msg) {
            robot_id = _msg.robot_id;
            target = _msg.target;
            avoidOurPenaltyArea = _msg.avoidOurPenaltyArea;
            avoidOppPenaltyArea = _msg.avoidOppPenaltyArea;
            slow = _msg.slow;
            receiveRadius = _msg.receiveRadius;
            ignoreAngle = _msg.ignoreAngle;
            IATargetDir = _msg.IATargetDir;
    }

    parsian_msgs::Receivepass getMessage() {
        parsian_msgs::Receivepass _msg;
        _msg.robot_id = robot_id;
        _msg.target = target;
        _msg.avoidOurPenaltyArea = avoidOurPenaltyArea;
        _msg.avoidOppPenaltyArea = avoidOppPenaltyArea;
        _msg.slow = slow;
        _msg.receiveRadius = receiveRadius;
        _msg.ignoreAngle = ignoreAngle;
        _msg.IATargetDir = IATargetDir;

    }

    SkillProperty(Receivepass, quint8, Robot_Id, robot_id)
    SkillProperty(Receivepass, Vector2D, Target, target)
    SkillProperty(Receivepass, bool, Avoidourpenaltyarea, avoidOurPenaltyArea)
    SkillProperty(Receivepass, bool, Avoidopppenaltyarea, avoidOppPenaltyArea)
    SkillProperty(Receivepass, bool, Slow, slow)
    SkillProperty(Receivepass, double, Receiveradius, receiveRadius)
    SkillProperty(Receivepass, bool, Ignoreangle, ignoreAngle)
    SkillProperty(Receivepass, Vector2D, Iatargetdir, IATargetDir)
}

#endif // Receivepass_HEADER_