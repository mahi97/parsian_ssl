// auto-generated don't edit !

#include <parsian_util/action/autogenerate/gotopointaction.h>

void GotopointAction::setMessage(const void* _msg) {
    parsian_msgs::parsian_skill_gotoPoint msg = *((parsian_msgs::parsian_skill_gotoPoint*)_msg);
        robot_id = msg.robot_id;
        dynamicStart = msg.dynamicStart;
        maxAcceleration = msg.maxAcceleration;
        maxDeceleration = msg.maxDeceleration;
        maxVelocity = msg.maxVelocity;
        maxAccelerationNormal = msg.maxAccelerationNormal;
        maxDecelerationNormal = msg.maxDecelerationNormal;
        maxVelocityNormal = msg.maxVelocityNormal;
        constantVelocity = msg.constantVelocity;
        turningDist = msg.turningDist;
        fasterEnd = msg.fasterEnd;
        notEnd = msg.notEnd;
        fastW = msg.fastW;
        recordProfile = msg.recordProfile;
        ballMode = msg.ballMode;
        slowShot = msg.slowShot;
        moveLookingTarget = msg.moveLookingTarget;
        interceptMode = msg.interceptMode;
        lookForward = msg.lookForward;
        oneTouchMode = msg.oneTouchMode;
        slowMode = msg.slowMode;
        noPID = msg.noPID;
        penaltyKick = msg.penaltyKick;
        goalieMode = msg.goalieMode;
        forwardBias = msg.forwardBias;
        diveMode = msg.diveMode;
        verySlow = msg.verySlow;
        smooth = msg.smooth;
        targetPos = msg.targetPos;
        targetDir = msg.targetDir;
        targetVel = msg.targetVel;
        lookAt = msg.lookAt;
        motionDir = msg.motionDir;

}

void* GotopointAction::getMessage() {
    parsian_msgs::parsian_skill_gotoPoint* _msg = new parsian_msgs::parsian_skill_gotoPoint;
    _msg->robot_id = robot_id;
    _msg->dynamicStart = dynamicStart;
    _msg->maxAcceleration = maxAcceleration;
    _msg->maxDeceleration = maxDeceleration;
    _msg->maxVelocity = maxVelocity;
    _msg->maxAccelerationNormal = maxAccelerationNormal;
    _msg->maxDecelerationNormal = maxDecelerationNormal;
    _msg->maxVelocityNormal = maxVelocityNormal;
    _msg->constantVelocity = constantVelocity;
    _msg->turningDist = turningDist;
    _msg->fasterEnd = fasterEnd;
    _msg->notEnd = notEnd;
    _msg->fastW = fastW;
    _msg->recordProfile = recordProfile;
    _msg->ballMode = ballMode;
    _msg->slowShot = slowShot;
    _msg->moveLookingTarget = moveLookingTarget;
    _msg->interceptMode = interceptMode;
    _msg->lookForward = lookForward;
    _msg->oneTouchMode = oneTouchMode;
    _msg->slowMode = slowMode;
    _msg->noPID = noPID;
    _msg->penaltyKick = penaltyKick;
    _msg->goalieMode = goalieMode;
    _msg->forwardBias = forwardBias;
    _msg->diveMode = diveMode;
    _msg->verySlow = verySlow;
    _msg->smooth = smooth;
    _msg->targetPos = targetPos.toParsianMessage();
    _msg->targetDir = targetDir.toParsianMessage();
    _msg->targetVel = targetVel.toParsianMessage();
    _msg->lookAt = lookAt.toParsianMessage();
    _msg->motionDir = motionDir.toParsianMessage();
    return _msg;

}


QString GotopointAction::getActionName(){
    static QString name("GotopointAction");
    return name;
}
