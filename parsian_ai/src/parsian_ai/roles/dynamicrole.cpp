#include "parsian_ai/roles/dynamicrole.h"

CRoleDynamic::CRoleDynamic() {
    shotSkill     = new KickAction;
    receiveSkill  = new ReceivepassAction;
    moveSkill     = new GotopointavoidAction;
    oneTouchSkill = new OnetouchAction;
    selectedSkill = DynamicEnums::NoSkill;
    agent = NULL;
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

    switch(selectedSkill) {
        case DynamicEnums::Ready:
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
        case DynamicEnums::Shot:
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
        case DynamicEnums::Chip:
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
        case DynamicEnums::Pass:
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
        case DynamicEnums::CatchBall:
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
        case DynamicEnums::Move:
            moveSkill->setTargetpos(target);
            moveSkill->setTargetdir(targetDir);
            moveSkill->setAvoidpenaltyarea(true);
            moveSkill->setSlowmode(false);
            break;
        case DynamicEnums::OneTouch:
            oneTouchSkill->setWaitpos(waitPos);
            oneTouchSkill->setTarget(target);
            break;
        case DynamicEnums::Keep:
            break;
        case DynamicEnums::NoSkill:
        default:
            break;
    }

}

void CRoleDynamic::execute() {

    /*if (updated) {
        update();
    }*/
    update();

    ROS_INFO("seda mizane");
    switch(selectedSkill) {
        case DynamicEnums::Ready:
            agent->action = receiveSkill;
            break;
        case DynamicEnums::Shot:
        case DynamicEnums::Chip:
        case DynamicEnums::Pass:
        case DynamicEnums::CatchBall:
            DBUG(QString("[dynamicRole] kickSpeed : %1").arg(kickSpeed), D_MAHI);
            agent->action = shotSkill;
            break;
        case DynamicEnums::Move:
            agent->action = moveSkill;
            break;
        case DynamicEnums::OneTouch:
            break;
        case DynamicEnums::NoSkill:
        default:
            break;
    }

}

