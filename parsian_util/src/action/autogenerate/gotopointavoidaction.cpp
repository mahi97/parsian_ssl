// auto-generated don't edit !

#include <parsian_util/action/autogenerate/gotopointavoidaction.h>

void GotopointavoidAction::setMessage(void* _msg) {
    parsian_msgs::parsian_skill_gotoPointAvoid msg = *((parsian_msgs::parsian_skill_gotoPointAvoid*)_msg);
        avoidPenaltyArea = msg.avoidPenaltyArea;
        keeplooking = msg.keeplooking;
        extendStep = msg.extendStep;
        plan2 = msg.plan2;
        noAvoid = msg.noAvoid;
        avoidCenterCircle = msg.avoidCenterCircle;
        ballObstacleRadius = msg.ballObstacleRadius;
        avoidBall = msg.avoidBall;
        avoidGoalPosts = msg.avoidGoalPosts;
        drawPath = msg.drawPath;
        addVel = msg.addVel;
        nextPos = msg.nextPos;

}

void* GotopointavoidAction::getMessage() {
    parsian_msgs::parsian_skill_gotoPointAvoid* _msg = new parsian_msgs::parsian_skill_gotoPointAvoid;
    _msg->avoidPenaltyArea = avoidPenaltyArea;
    _msg->keeplooking = keeplooking;
    _msg->extendStep = extendStep;
    _msg->plan2 = plan2;
    _msg->noAvoid = noAvoid;
    _msg->avoidCenterCircle = avoidCenterCircle;
    _msg->ballObstacleRadius = ballObstacleRadius;
    _msg->avoidBall = avoidBall;
    _msg->avoidGoalPosts = avoidGoalPosts;
    _msg->drawPath = drawPath;
    _msg->addVel = addVel.toParsianMessage();
    _msg->nextPos = nextPos.toParsianMessage();
    return _msg;

}
