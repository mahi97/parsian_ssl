// auto-generated don't edit !

#include <parsian_util/action/autogenerate/kickaction.h>

void KickAction::setMessage(const void* _msg) {
    parsian_msgs::parsian_skill_kick msg = *((parsian_msgs::parsian_skill_kick*)_msg);
        tolerance = msg.tolerance;
        chip = msg.chip;
        kickSpeed = msg.kickSpeed;
        spin = msg.spin;
        slow = msg.slow;
        avoidPenaltyArea = msg.avoidPenaltyArea;
        avoidOppPenaltyArea = msg.avoidOppPenaltyArea;
        interceptMode = msg.interceptMode;
        dontKick = msg.dontKick;
        sagMode = msg.sagMode;
        penaltyKick = msg.penaltyKick;
        shotEmptySpot = msg.shotEmptySpot;
        passProfiler = msg.passProfiler;
        veryFine = msg.veryFine;
        goalieMode = msg.goalieMode;
        kickAngTol = msg.kickAngTol;
        kickWithCenterOfDribbler = msg.kickWithCenterOfDribbler;
        playMakeMode = msg.playMakeMode;
        target = msg.target;

}

void* KickAction::getMessage() {
    parsian_msgs::parsian_skill_kick* _msg = new parsian_msgs::parsian_skill_kick;
    _msg->tolerance = tolerance;
    _msg->chip = chip;
    _msg->kickSpeed = kickSpeed;
    _msg->spin = spin;
    _msg->slow = slow;
    _msg->avoidPenaltyArea = avoidPenaltyArea;
    _msg->avoidOppPenaltyArea = avoidOppPenaltyArea;
    _msg->interceptMode = interceptMode;
    _msg->dontKick = dontKick;
    _msg->sagMode = sagMode;
    _msg->penaltyKick = penaltyKick;
    _msg->shotEmptySpot = shotEmptySpot;
    _msg->passProfiler = passProfiler;
    _msg->veryFine = veryFine;
    _msg->goalieMode = goalieMode;
    _msg->kickAngTol = kickAngTol;
    _msg->kickWithCenterOfDribbler = kickWithCenterOfDribbler;
    _msg->playMakeMode = playMakeMode;
    _msg->target = target.toParsianMessage();
    return _msg;

}


QString KickAction::getActionName(){
    static QString name("KickAction");
    return name;
}

