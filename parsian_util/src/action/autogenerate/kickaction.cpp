// auto-generated don't edit !

#include <parsian_util/action/autogenerate/kickaction.h>

void KickAction::setMessage(const void* _msg) {
    parsian_msgs::parsian_skill_kick msg = *((parsian_msgs::parsian_skill_kick*)_msg);
        robot_id = msg.robot_id;
        tolerance = msg.tolerance;
        chip = msg.chip;
        kickSpeed = msg.kickSpeed;
        spin = msg.spin;
        slow = msg.slow;
        turn = msg.turn;
        autoChipSpeed = msg.autoChipSpeed;
        throughMode = msg.throughMode;
        parallelMode = msg.parallelMode;
        clear = msg.clear;
        waitFrames = msg.waitFrames;
        avoidPenaltyArea = msg.avoidPenaltyArea;
        avoidOppPenaltyArea = msg.avoidOppPenaltyArea;
        interceptMode = msg.interceptMode;
        dontKick = msg.dontKick;
        sagMode = msg.sagMode;
        dontRecvPass = msg.dontRecvPass;
        recvChip = msg.recvChip;
        penaltyKick = msg.penaltyKick;
        turnLastMomentForPenalty = msg.turnLastMomentForPenalty;
        isGotoPointAvoid = msg.isGotoPointAvoid;
        kkShotEmpySpot = msg.kkShotEmpySpot;
        passProfiler = msg.passProfiler;
        veryFine = msg.veryFine;
        goalieMode = msg.goalieMode;
        alternateMode = msg.alternateMode;
        kickAngTol = msg.kickAngTol;
        kickWithCenterOfDribbler = msg.kickWithCenterOfDribbler;
        playMakeMode = msg.playMakeMode;
        fastIntercept = msg.fastIntercept;
        target = msg.target;

}

void* KickAction::getMessage() {
    parsian_msgs::parsian_skill_kick* _msg = new parsian_msgs::parsian_skill_kick;
    _msg->robot_id = robot_id;
    _msg->tolerance = tolerance;
    _msg->chip = chip;
    _msg->kickSpeed = kickSpeed;
    _msg->spin = spin;
    _msg->slow = slow;
    _msg->turn = turn;
    _msg->autoChipSpeed = autoChipSpeed;
    _msg->throughMode = throughMode;
    _msg->parallelMode = parallelMode;
    _msg->clear = clear;
    _msg->waitFrames = waitFrames;
    _msg->avoidPenaltyArea = avoidPenaltyArea;
    _msg->avoidOppPenaltyArea = avoidOppPenaltyArea;
    _msg->interceptMode = interceptMode;
    _msg->dontKick = dontKick;
    _msg->sagMode = sagMode;
    _msg->dontRecvPass = dontRecvPass;
    _msg->recvChip = recvChip;
    _msg->penaltyKick = penaltyKick;
    _msg->turnLastMomentForPenalty = turnLastMomentForPenalty;
    _msg->isGotoPointAvoid = isGotoPointAvoid;
    _msg->kkShotEmpySpot = kkShotEmpySpot;
    _msg->passProfiler = passProfiler;
    _msg->veryFine = veryFine;
    _msg->goalieMode = goalieMode;
    _msg->alternateMode = alternateMode;
    _msg->kickAngTol = kickAngTol;
    _msg->kickWithCenterOfDribbler = kickWithCenterOfDribbler;
    _msg->playMakeMode = playMakeMode;
    _msg->fastIntercept = fastIntercept;
    _msg->target = target.toParsianMessage();
    return _msg;

}


QString KickAction::getActionName(){
    static QString name("KickAction");
    return name;
}
