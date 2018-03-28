#include "parsian_ai/roles/dynamicrole.h"

CRoleDynamic::CRoleDynamic() {
    shotSkill     = new KickAction;
    receiveSkill  = new ReceivepassAction;
    moveSkill     = new GotopointavoidAction;
    oneTouchSkill = new OnetouchAction;
    positionSkill = PositionSkill ::NoSkill;
    playMakeSkill = PlayMakeSkill ::NoSkill;

    agent = nullptr;
}

CRoleDynamic::CRoleDynamic(const CRoleDynamic &_copy) {
    shotSkill     = new KickAction;
    receiveSkill  = new ReceivepassAction;
    moveSkill     = new GotopointavoidAction;
    oneTouchSkill = new OnetouchAction;
    positionSkill = _copy.positionSkill;
    playMakeSkill = _copy.playMakeSkill;
    agent = _copy.agent;
//    shotSkill->setMessage(_copy.shotSkill->getMessage());
}

CRoleDynamic::CRoleDynamic(CRoleDynamic && _move) noexcept {
    shotSkill     = new KickAction;
    receiveSkill  = new ReceivepassAction;
    moveSkill     = new GotopointavoidAction;
    oneTouchSkill = new OnetouchAction;
    positionSkill = _move.positionSkill;
    playMakeSkill = _move.playMakeSkill;
    agent = _move.agent;
    shotSkill->setMessage(_move.shotSkill->getMessage());
}

CRoleDynamic::~CRoleDynamic() {
    delete shotSkill;
    delete receiveSkill;
    delete moveSkill;
    delete oneTouchSkill;
}

void CRoleDynamic::update() {
    updated = false;

    shotSkill->setPlaymakemode(true);
    shotSkill->setKickwithcenterofdribbler(true);

    switch (positionSkill) {
    case PositionSkill ::Ready:
        receiveSkill->setTarget(target);
        receiveSkill->setReceiveradius(receiveRadius);
        break;
//   case DynamicEnums::Dribble:
//       dribbleSkill->setAgent(agent);
//       dribbleSkill->setTarget(target);
//       dribbleSkill->setInitialLook(wm->field->oppGoal());
//       dribbleSkill->setKickTol(tolerance);
//       dribbleSkill->setChip(chip);
//       if(wm->getIsSimulMode())
//           dribbleSkill->setKickSpeed(kickSpeed);
//       else if (chip) {
//               dribbleSkill->setKickSpeed(max(200, kickSpeed));
//       } else {
//               dribbleSkill->setKickSpeed(max(300, kickSpeed));
//       }
//       break;
    case PositionSkill ::Move:
        moveSkill->setTargetpos(target);
        moveSkill->setTargetdir(targetDir);
        moveSkill->setAvoidpenaltyarea(true);
        moveSkill->setSlowmode(false);
        break;
    case PositionSkill ::OneTouch:
        oneTouchSkill->setWaitpos(waitPos);
        oneTouchSkill->setTarget(target);
        break;
    case PositionSkill ::NoSkill:
    default:
        break;
    }
    switch (playMakeSkill) {
//   case DynamicEnums::Dribble:
//       dribbleSkill->setAgent(agent);
//       dribbleSkill->setTarget(target);
//       dribbleSkill->setInitialLook(wm->field->oppGoal());
//       dribbleSkill->setKickTol(tolerance);
//       dribbleSkill->setChip(chip);
//       if(wm->getIsSimulMode())
//           dribbleSkill->setKickSpeed(kickSpeed);
//       else if (chip) {
//               dribbleSkill->setKickSpeed(max(200, kickSpeed));
//       } else {
//               dribbleSkill->setKickSpeed(max(300, kickSpeed));
//       }
//       break;
        case PlayMakeSkill ::Shot:
            shotSkill->setTarget(target);
            shotSkill->setTolerance(tolerance);
            shotSkill->setAvoidpenaltyarea(true);
            shotSkill->setAvoidopppenaltyarea(true);
            shotSkill->setPlaymakemode(true);
            shotSkill->setChip(chip);
            shotSkill->setVeryfine(veryFine);
//       shotSkill->setShotToEmptySpot(emptySpot);
            shotSkill->setDontkick(false);
//       if(wm->getIsSimulMode()) // TODO : WM SIMUL
            shotSkill->setKickspeed(kickSpeed / 50);
//       else
            shotSkill->setKickspeed(std::max(900, kickSpeed));
            break;
        case PlayMakeSkill::Chip:
            shotSkill->setTarget(target);
            shotSkill->setTolerance(tolerance);
            shotSkill->setAvoidpenaltyarea(true);
            shotSkill->setAvoidopppenaltyarea(true);
            shotSkill->setPlaymakemode(true);
            shotSkill->setChip(true);
            shotSkill->setVeryfine(veryFine);
            shotSkill->setDontkick(false);
//       if(wm->getIsSimulMode()) // TODO : WM SIMUL
            shotSkill->setKickspeed(kickSpeed / 100);
//       else
            shotSkill->setKickspeed(std::max(200, kickSpeed));
            break;
        case PlayMakeSkill ::Pass:
            shotSkill->setTarget(target);
            shotSkill->setTolerance(tolerance);
            shotSkill->setAvoidpenaltyarea(true);
            shotSkill->setAvoidopppenaltyarea(true);
            shotSkill->setPlaymakemode(true);
            shotSkill->setChip(chip);
            shotSkill->setDontkick(noKick);
            shotSkill->setVeryfine(veryFine);
//       shotSkill->setShotToEmptySpot(false);
//       if(wm->getIsSimulMode()) // TODO : WM SIMUL
//            shotSkill->setKickspeed(kickSpeed / 100);
//       else if (chip) {
            shotSkill->setKickspeed(std::max(200, kickSpeed));
//       } else {
            shotSkill->setKickspeed(std::max(300, kickSpeed));
//       }
            break;
        case PlayMakeSkill ::CatchBall:
            shotSkill->setTarget(target);
            shotSkill->setTolerance(tolerance);
            shotSkill->setAvoidpenaltyarea(true);
            shotSkill->setChip(chip);
            shotSkill->setVeryfine(false);
//       shotSkill->setShotToEmptySpot(emptySpot);
//       if(wm->getIsSimulMode())
            shotSkill->setKickspeed(kickSpeed / 50);
//       else
            shotSkill->setKickspeed(1023);
            break;
        case PlayMakeSkill ::Keep:
            break;
        case PlayMakeSkill ::NoSkill:
        default:
            break;
    }

}

void CRoleDynamic::execute() {

    /*if (updated) {
        update();
    }*/
    update();
    switch (positionSkill) {
    case PositionSkill ::Ready:
        agent->action = receiveSkill;
        break;
        DBUG(QString("[dynamicRole] kickSpeed : %1").arg(kickSpeed), D_MAHI);
        agent->action = shotSkill;
        break;
    case PositionSkill::Move:
        agent->action = moveSkill;
        break;
    case PositionSkill::OneTouch:
        agent->action = oneTouchSkill;
        break;
    case PositionSkill::NoSkill:
    default:
        agent->action = nullptr;
        break;
    }
    switch (playMakeSkill) {
        case PlayMakeSkill ::Shot:
        case PlayMakeSkill ::Chip:
        case PlayMakeSkill ::Pass:
        case PlayMakeSkill ::CatchBall:
            DBUG(QString("[dynamicRole] kickSpeed : %1").arg(kickSpeed), D_MAHI);
            agent->action = shotSkill;
            break;
        case PlayMakeSkill ::NoSkill:
        default:
            agent->action = nullptr;
            break;
    }
}

CRoleDynamic& CRoleDynamic::operator=(CRoleDynamic &&_move) noexcept {
    return *this;
}

CRoleDynamic& CRoleDynamic::operator=(const CRoleDynamic& _copy) {
    return *this;
}

