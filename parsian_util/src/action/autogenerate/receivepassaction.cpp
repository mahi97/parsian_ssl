// auto-generated don't edit !

#include <parsian_util/action/autogenerate/receivepassaction.h>

void ReceivepassAction::setMessage(void* _msg) {
    parsian_msgs::parsian_skill_receivePass msg = *((parsian_msgs::parsian_skill_receivePass*)_msg);
        robot_id = msg.robot_id;
        avoidOurPenaltyArea = msg.avoidOurPenaltyArea;
        avoidOppPenaltyArea = msg.avoidOppPenaltyArea;
        slow = msg.slow;
        receiveRadius = msg.receiveRadius;
        ignoreAngle = msg.ignoreAngle;
        target = msg.target;
        IATargetDir = msg.IATargetDir;

}

void* ReceivepassAction::getMessage() {
    parsian_msgs::parsian_skill_receivePass* _msg = new parsian_msgs::parsian_skill_receivePass;
    _msg->robot_id = robot_id;
    _msg->avoidOurPenaltyArea = avoidOurPenaltyArea;
    _msg->avoidOppPenaltyArea = avoidOppPenaltyArea;
    _msg->slow = slow;
    _msg->receiveRadius = receiveRadius;
    _msg->ignoreAngle = ignoreAngle;
    _msg->target = target.toParsianMessage();
    _msg->IATargetDir = IATargetDir.toParsianMessage();
    return _msg;

}
