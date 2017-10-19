// auto-generated don't edit !

#ifndef GotopointAction_HEADER_
#define GotopointAction_HEADER_



#include <parsian_util/action/action.h>
#include <parsian_util/geom/geom.h>
#include <parsian_msgs/parsian_skill_gotoPoint.h>

class GotopointAction : public Action {

public:

    void setMessage(void* _msg);

    void* getMessage();

    static QString getActionName();


    SkillProperty(GotopointAction, quint8, Robot_Id, robot_id);
    SkillProperty(GotopointAction, bool, Dynamicstart, dynamicStart);
    SkillProperty(GotopointAction, float, Maxacceleration, maxAcceleration);
    SkillProperty(GotopointAction, float, Maxdeceleration, maxDeceleration);
    SkillProperty(GotopointAction, float, Maxvelocity, maxVelocity);
    SkillProperty(GotopointAction, float, Maxaccelerationnormal, maxAccelerationNormal);
    SkillProperty(GotopointAction, float, Maxdecelerationnormal, maxDecelerationNormal);
    SkillProperty(GotopointAction, float, Maxvelocitynormal, maxVelocityNormal);
    SkillProperty(GotopointAction, float, Constantvelocity, constantVelocity);
    SkillProperty(GotopointAction, float, Turningdist, turningDist);
    SkillProperty(GotopointAction, bool, Fasterend, fasterEnd);
    SkillProperty(GotopointAction, bool, Notend, notEnd);
    SkillProperty(GotopointAction, bool, Fastw, fastW);
    SkillProperty(GotopointAction, bool, Recordprofile, recordProfile);
    SkillProperty(GotopointAction, bool, Ballmode, ballMode);
    SkillProperty(GotopointAction, bool, Slowshot, slowShot);
    SkillProperty(GotopointAction, bool, Movelookingtarget, moveLookingTarget);
    SkillProperty(GotopointAction, bool, Interceptmode, interceptMode);
    SkillProperty(GotopointAction, bool, Lookforward, lookForward);
    SkillProperty(GotopointAction, bool, Onetouchmode, oneTouchMode);
    SkillProperty(GotopointAction, bool, Slowmode, slowMode);
    SkillProperty(GotopointAction, bool, Nopid, noPID);
    SkillProperty(GotopointAction, bool, Penaltykick, penaltyKick);
    SkillProperty(GotopointAction, bool, Goaliemode, goalieMode);
    SkillProperty(GotopointAction, float, Forwardbias, forwardBias);
    SkillProperty(GotopointAction, bool, Divemode, diveMode);
    SkillProperty(GotopointAction, bool, Veryslow, verySlow);
    SkillProperty(GotopointAction, bool, Smooth, smooth);
    SkillProperty(GotopointAction, Vector2D, Targetpos, targetPos);
    SkillProperty(GotopointAction, Vector2D, Targetdir, targetDir);
    SkillProperty(GotopointAction, Vector2D, Targetvel, targetVel);
    SkillProperty(GotopointAction, Vector2D, Lookat, lookAt);
    SkillProperty(GotopointAction, Vector2D, Motiondir, motionDir);

};

#endif // GotopointAction_HEADER_
