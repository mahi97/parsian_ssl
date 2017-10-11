// auto-generated don't edit !

#ifndef Gotopoint_HEADER_
#define Gotopoint_HEADER_


#include <parsian_util/actions/action.h>
#include <parsian_util/geom/geom.h>


class GotopointAction : public Action {

public:
    Gotopoint();
    ~Gotopoint();

    void setMessage(const parsian_msgs:: _msg) {
            robot_id = _msg.robot_id;
            targetPos = _msg.targetPos;
            targetDir = _msg.targetDir;
            targetVel = _msg.targetVel;
            dynamicStart = _msg.dynamicStart;
            maxAcceleration = _msg.maxAcceleration;
            maxDeceleration = _msg.maxDeceleration;
            maxVelocity = _msg.maxVelocity;
            maxAccelerationNormal = _msg.maxAccelerationNormal;
            maxDecelerationNormal = _msg.maxDecelerationNormal;
            maxVelocityNormal = _msg.maxVelocityNormal;
            constantVelocity = _msg.constantVelocity;
            turningDist = _msg.turningDist;
            fasterEnd = _msg.fasterEnd;
            notEnd = _msg.notEnd;
            fastW = _msg.fastW;
            recordProfile = _msg.recordProfile;
            ballMode = _msg.ballMode;
            slowShot = _msg.slowShot;
            moveLookingTarget = _msg.moveLookingTarget;
            interceptMode = _msg.interceptMode;
            lookForward = _msg.lookForward;
            oneTouchMode = _msg.oneTouchMode;
            slowMode = _msg.slowMode;
            noPID = _msg.noPID;
            penaltyKick = _msg.penaltyKick;
            goalieMode = _msg.goalieMode;
            motionDir = _msg.motionDir;
            forwardBias = _msg.forwardBias;
            diveMode = _msg.diveMode;
            verySlow = _msg.verySlow;
            smooth = _msg.smooth;
    }

    parsian_msgs::Gotopoint getMessage() {
        parsian_msgs::Gotopoint _msg;
        _msg.robot_id = robot_id;
        _msg.targetPos = targetPos;
        _msg.targetDir = targetDir;
        _msg.targetVel = targetVel;
        _msg.dynamicStart = dynamicStart;
        _msg.maxAcceleration = maxAcceleration;
        _msg.maxDeceleration = maxDeceleration;
        _msg.maxVelocity = maxVelocity;
        _msg.maxAccelerationNormal = maxAccelerationNormal;
        _msg.maxDecelerationNormal = maxDecelerationNormal;
        _msg.maxVelocityNormal = maxVelocityNormal;
        _msg.constantVelocity = constantVelocity;
        _msg.turningDist = turningDist;
        _msg.fasterEnd = fasterEnd;
        _msg.notEnd = notEnd;
        _msg.fastW = fastW;
        _msg.recordProfile = recordProfile;
        _msg.ballMode = ballMode;
        _msg.slowShot = slowShot;
        _msg.moveLookingTarget = moveLookingTarget;
        _msg.interceptMode = interceptMode;
        _msg.lookForward = lookForward;
        _msg.oneTouchMode = oneTouchMode;
        _msg.slowMode = slowMode;
        _msg.noPID = noPID;
        _msg.penaltyKick = penaltyKick;
        _msg.goalieMode = goalieMode;
        _msg.motionDir = motionDir;
        _msg.forwardBias = forwardBias;
        _msg.diveMode = diveMode;
        _msg.verySlow = verySlow;
        _msg.smooth = smooth;

    }

    SkillProperty(Gotopoint, quint8, Robot_Id, robot_id)
    SkillProperty(Gotopoint, Vector2D, Targetpos, targetPos)
    SkillProperty(Gotopoint, Vector2D, Targetdir, targetDir)
    SkillProperty(Gotopoint, Vector2D, Targetvel, targetVel)
    SkillProperty(Gotopoint, bool, Dynamicstart, dynamicStart)
    SkillProperty(Gotopoint, float, Maxacceleration, maxAcceleration)
    SkillProperty(Gotopoint, float, Maxdeceleration, maxDeceleration)
    SkillProperty(Gotopoint, float, Maxvelocity, maxVelocity)
    SkillProperty(Gotopoint, float, Maxaccelerationnormal, maxAccelerationNormal)
    SkillProperty(Gotopoint, float, Maxdecelerationnormal, maxDecelerationNormal)
    SkillProperty(Gotopoint, float, Maxvelocitynormal, maxVelocityNormal)
    SkillProperty(Gotopoint, float, Constantvelocity, constantVelocity)
    SkillProperty(Gotopoint, float, Turningdist, turningDist)
    SkillProperty(Gotopoint, bool, Fasterend, fasterEnd)
    SkillProperty(Gotopoint, bool, Notend, notEnd)
    SkillProperty(Gotopoint, bool, Fastw, fastW)
    SkillProperty(Gotopoint, bool, Recordprofile, recordProfile)
    SkillProperty(Gotopoint, bool, Ballmode, ballMode)
    SkillProperty(Gotopoint, bool, Slowshot, slowShot)
    SkillProperty(Gotopoint, bool, Movelookingtarget, moveLookingTarget)
    SkillProperty(Gotopoint, bool, Interceptmode, interceptMode)
    SkillProperty(Gotopoint, bool, Lookforward, lookForward)
    SkillProperty(Gotopoint, bool, Onetouchmode, oneTouchMode)
    SkillProperty(Gotopoint, bool, Slowmode, slowMode)
    SkillProperty(Gotopoint, bool, Nopid, noPID)
    SkillProperty(Gotopoint, bool, Penaltykick, penaltyKick)
    SkillProperty(Gotopoint, bool, Goaliemode, goalieMode)
    SkillProperty(Gotopoint, Vector2D, Motiondir, motionDir)
    SkillProperty(Gotopoint, float, Forwardbias, forwardBias)
    SkillProperty(Gotopoint, bool, Divemode, diveMode)
    SkillProperty(Gotopoint, bool, Veryslow, verySlow)
    SkillProperty(Gotopoint, bool, Smooth, smooth)
}

#endif // Gotopoint_HEADER_