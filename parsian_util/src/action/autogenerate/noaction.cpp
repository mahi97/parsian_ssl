// auto-generated don't edit !

#include <parsian_util/action/autogenerate/noaction.h>

void NoAction::setMessage(const void* _msg) {
    parsian_msgs::parsian_skill_no msg = *((parsian_msgs::parsian_skill_no*)_msg);
        robot_id = msg.robot_id;
        waithere = msg.waithere;

}

void* NoAction::getMessage() {
    parsian_msgs::parsian_skill_no* _msg = new parsian_msgs::parsian_skill_no;
    _msg->robot_id = robot_id;
    _msg->waithere = waithere;
    return _msg;

}


QString NoAction::getActionName(){
    static QString name("NoAction");
    return name;
}
