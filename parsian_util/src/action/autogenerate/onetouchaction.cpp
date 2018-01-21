// auto-generated don't edit !

#include <parsian_util/action/autogenerate/onetouchaction.h>

OnetouchAction::OnetouchAction() {
       chip = false;
       kickSpeed = 0.0;
       shotToEmptySpot = false;
}

void OnetouchAction::setMessage(const void* _msg) {
    parsian_msgs::parsian_skill_oneTouch msg = *((parsian_msgs::parsian_skill_oneTouch*)_msg);
        chip = msg.chip;
        kickSpeed = msg.kickSpeed;
        shotToEmptySpot = msg.shotToEmptySpot;
        waitPos = msg.waitPos;
        target = msg.target;

}

void* OnetouchAction::getMessage() {
    parsian_msgs::parsian_skill_oneTouch* _msg = new parsian_msgs::parsian_skill_oneTouch;
    _msg->chip = chip;
    _msg->kickSpeed = kickSpeed;
    _msg->shotToEmptySpot = shotToEmptySpot;
    _msg->waitPos = waitPos.toParsianMessage();
    _msg->target = target.toParsianMessage();
    return _msg;

}


QString OnetouchAction::getActionName(){
    return SActionName();
}

QString OnetouchAction::SActionName(){
    return QString{"OnetouchAction"};
}

