// auto-generated don't edit !

#include <parsian_util/action/autogenerate/gotopointavoidaction.h>

void GotopointavoidAction::setMessage(const void* _msg) {
    parsian_msgs::parsian_skill_gotoPointAvoid msg = *((parsian_msgs::parsian_skill_gotoPointAvoid*)_msg);
        avoidPenaltyArea = msg.avoidPenaltyArea;
        noAvoid = msg.noAvoid;
        avoidCenterCircle = msg.avoidCenterCircle;
        ballObstacleRadius = msg.ballObstacleRadius;
        drawPath = msg.drawPath;
        diveMode = msg.diveMode;
        addVel = msg.addVel;

        GotopointAction::setMessage(&msg.base);
}

void* GotopointavoidAction::getMessage() {
    parsian_msgs::parsian_skill_gotoPointAvoid* _msg = new parsian_msgs::parsian_skill_gotoPointAvoid;
    _msg->base = *((parsian_msgs::parsian_skill_gotoPoint*) GotopointAction::getMessage());
    _msg->avoidPenaltyArea = avoidPenaltyArea;
    _msg->noAvoid = noAvoid;
    _msg->avoidCenterCircle = avoidCenterCircle;
    _msg->ballObstacleRadius = ballObstacleRadius;
    _msg->drawPath = drawPath;
    _msg->diveMode = diveMode;
    _msg->addVel = addVel.toParsianMessage();
    return _msg;

}


QString GotopointavoidAction::getActionName(){
    static QString name("GotopointavoidAction");
    return name;
}
