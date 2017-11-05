// auto-generated don't edit !

#include <parsian_util/action/autogenerate/onetouchaction.h>

void OnetouchAction::setMessage(const void* _msg) {
    parsian_msgs::parsian_skill_oneTouch msg = *((parsian_msgs::parsian_skill_oneTouch*)_msg);
        robot_id = msg.robot_id;
        chip = msg.chip;
        kickSpeed = msg.kickSpeed;
        distToBallLine = msg.distToBallLine;
        velocityToBallLine = msg.velocityToBallLine;
        receiveChip = msg.receiveChip;
        avoidPenaltyArea = msg.avoidPenaltyArea;
        moveTowardTheBall = msg.moveTowardTheBall;
        shotToEmptySpot = msg.shotToEmptySpot;
        waitPos = msg.waitPos;
        target = msg.target;

}

void* OnetouchAction::getMessage() {
    parsian_msgs::parsian_skill_oneTouch* _msg = new parsian_msgs::parsian_skill_oneTouch;
    _msg->robot_id = robot_id;
    _msg->chip = chip;
    _msg->kickSpeed = kickSpeed;
    _msg->distToBallLine = distToBallLine;
    _msg->velocityToBallLine = velocityToBallLine;
    _msg->receiveChip = receiveChip;
    _msg->avoidPenaltyArea = avoidPenaltyArea;
    _msg->moveTowardTheBall = moveTowardTheBall;
    _msg->shotToEmptySpot = shotToEmptySpot;
    _msg->waitPos = waitPos.toParsianMessage();
    _msg->target = target.toParsianMessage();
    return _msg;

}


QString OnetouchAction::getActionName(){
    static QString name("OnetouchAction");
    return name;
}
