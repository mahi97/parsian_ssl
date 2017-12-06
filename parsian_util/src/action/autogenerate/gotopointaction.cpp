// auto-generated don't edit !

#include <parsian_util/action/autogenerate/gotopointaction.h>

void GotopointAction::setMessage(const void* _msg) {
    parsian_msgs::parsian_skill_gotoPoint msg = *((parsian_msgs::parsian_skill_gotoPoint*)_msg);
        robot_id = msg.robot_id;
        dynamicStart = msg.dynamicStart;
        maxAcceleration = msg.maxAcceleration;
        maxDeceleration = msg.maxDeceleration;
        maxVelocity = msg.maxVelocity;
        oneTouchMode = msg.oneTouchMode;
        slowMode = msg.slowMode;
        penaltyKick = msg.penaltyKick;
        diveMode = msg.diveMode;
        smooth = msg.smooth;
        targetPos = msg.targetPos;
        targetDir = msg.targetDir;
        targetVel = msg.targetVel;
        lookAt = msg.lookAt;

}

void* GotopointAction::getMessage() {
    parsian_msgs::parsian_skill_gotoPoint* _msg = new parsian_msgs::parsian_skill_gotoPoint;
    _msg->robot_id = robot_id;
    _msg->dynamicStart = dynamicStart;
    _msg->maxAcceleration = maxAcceleration;
    _msg->maxDeceleration = maxDeceleration;
    _msg->maxVelocity = maxVelocity;
    _msg->oneTouchMode = oneTouchMode;
    _msg->slowMode = slowMode;
    _msg->penaltyKick = penaltyKick;
    _msg->diveMode = diveMode;
    _msg->smooth = smooth;
    _msg->targetPos = targetPos.toParsianMessage();
    _msg->targetDir = targetDir.toParsianMessage();
    _msg->targetVel = targetVel.toParsianMessage();
    _msg->lookAt = lookAt.toParsianMessage();
    return _msg;

}


QString GotopointAction::getActionName(){
    static QString name("GotopointAction");
    return name;
}

