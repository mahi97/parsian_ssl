#include "plays/playoff.h"


CPlayOff::CPlayOff()
{
    blockerState=0;
    blockerID=-1;
    blockerStopStates=Diversion;
    qDebug() << "Bring yourself back online playoff";

    decidePlan = true;
    agentSize = 1;
    for(int i = 0;i < 6;i++) positionAgent[i].stateNumber = 0;
    for(int i = 0;i < 6;i++) {
        roleAgent[i] = new CRolePlayOff();
        newRoleAgent[i] = new CRolePlayOff();

    }
    isBallIn = false;
    tempAgent = new CRolePlayOff();
    doPass = false;
    doAfterlife = false;
    setTimer = true;
    ////////////

    currentPlan = new SPlayOffPlan();
    masterPlan = NULL;
    kickOffPos[0] = Vector2D(wm->ball->pos.x - 0.3, wm->ball->pos.y);
    kickOffPos[1] = Vector2D(-0.3,  2.5);
    kickOffPos[2] = Vector2D(-0.3, -2.5);
    kickOffPos[3] = Vector2D(-2  ,  0);
    kickOffPos[4] = Vector2D(-0.5,  1);
    kickOffPos[5] = Vector2D(-0.5, -1);

    initial    = true;
    playOnFlag = false;
    havePassInPlan = false;

    //Dynamic
    ready = pass = shot = false;
    dynamicStartTime = -1;
    dynamicSelect = NOSELECT;
    firstStepEnums = Stay;
    blockerStep= S0;

    criticalInit = true;
    criticalKick = new CSkillKick(NULL);
}

CPlayOff::~CPlayOff()
{
    qDebug() << "Playoff is gone";
    for(int i = 0;i < 6;i++) {
        delete roleAgent[i];
        delete newRoleAgent[i];
    }
    delete tempAgent;

}

bool CPlayOff:: isBlockDisturbing(){
    if(blockerState == 7)
        return true;
    else if(policy()->PlayOff_UseForcedBlock()){
        blockerID=knowledge->nearestOppToBall;
        return true;
    }
    else
    return false;

}

void CPlayOff::globalExecute() {

    if (masterMode == NGameOff::StaticPlay) {

        debug(QString("lastTime : %1").arg(knowledge->getCurrentTime() - lastTime), D_MAHI);
        if (knowledge->getCurrentTime() - lastTime > 1000 && !initial && lastBallPos.dist(wm->ball->pos) > 0.06) {
            //             TODO : write critical play here
            if (criticalPlay()) {
                playOnFlag = true;
            }
            return;
        }


        Q_ASSERT(masterPlan != NULL);
        if(masterPlan != NULL) {
            debug (QString("Plan Number : %1 ==> ").arg(masterPlan->gui.planFile), D_MAHI);

            if (initial) {
                qDebug() << *masterPlan;
                lastBallPos = wm->ball->pos;
                lastTime = knowledge->getCurrentTime();

                if (isBlockDisturbing() && policy()->PlayOff_UseBlockBlocker()) {
                    masterPlan->common.currentSize -= 1;
                    BlockerStopperID = masterPlan->common.matchedID[masterPlan->common.currentSize];


                }
                staticExecute();

            }
            debug(QString("policy:%1").arg(policy()->PlayOff_UseBlockBlocker()),D_NADIA);
            if(isBlockDisturbing() && policy()->PlayOff_UseBlockBlocker()){
                agentsID.removeOne(BlockerStopperID);
                if(BlockerExecute(BlockerStopperID)){
                    staticExecute();
                }
            }
            else
                staticExecute();


        } else {
            qDebug() << "master is null";
            initial = true;
            return;
        }

    } else {
        mainExecute();
    }
}

void CPlayOff::mainExecute() {
    switch(masterMode) {
    case NGameOff::StaticPlay:
        staticExecute();
        break;
    case NGameOff::DynamicPlay:
        dynamicExecute();
        break;
    case NGameOff::FirstPlay:
        firstExecute();
        break;
    case NGameOff::FastPlay:
        fastExecute();
        break;
    default:
        break;
    }
}

bool CPlayOff::BlockerExecute(int agentID){

    bool result=false;
    Vector2D pos1,pos2;
    Vector2D virtualPos;
    Segment2D blockerLine;
    Circle2D blockerCircle(wm->ball->pos,wm->opp[blockerID]->pos.dist(wm->ball->pos));
    int ID;

    switch(blockerStopStates){
    case Diversion:
        if(newRoleAgent[0]->getRoleUpdate() == false) {
            newRoleAgent[0]->setUpdated(true);
            newRoleAgent[0]->setAgent(knowledge->getAgent(dynamicMatch[0]));
            newRoleAgent[0]->setRoleUpdate(true);
            newRoleAgent[0]->setAvoidBall(true);
            newRoleAgent[0]->setAvoidPenaltyArea(true);
            newRoleAgent[0]->setSelectedSkill(roleSkill::GotopointAvoid);
        }
        newRoleAgent[0]->setTarget(wm->ball->pos+Vector2D(0.3,0));
        newRoleAgent[0]->setTargetDir(wm->field->ourGoal());
        if (newRoleAgent[0]->getTargetDir().angleWith(newRoleAgent[0]->getAgent()->dir()).degree() < 5) {
            blockerStopStates = BlockStop ;
        }
        else
            newRoleAgent[0]->execute();

        break;
    case TurnAndKick:

        ID=masterPlan->common.currentSize;
        newRoleAgent[ID]->setUpdated(true);
        newRoleAgent[ID]->setAgent(knowledge->getAgent(BlockerStopperID));
        newRoleAgent[ID]->setRoleUpdate(true);
        newRoleAgent[ID]->setAvoidBall(true);
        newRoleAgent[ID]->setAvoidPenaltyArea(true);
        newRoleAgent[ID]->setSelectedSkill(roleSkill::GotopointAvoid);

        virtualPos=wm->opp[blockerID]->pos+Vector2D(0.15,0);
        blockerLine=Segment2D(wm->ball->pos,wm->ball->pos+(virtualPos-wm->ball->pos).norm()*3);
        blockerCircle.intersection(blockerLine,&pos1,&pos2);

        // Nadia
        newRoleAgent[ID]->setTarget(pos1);
        newRoleAgent[ID]->setTargetDir(wm->ball->pos-pos1);


        result = true;
        debug("plan ejra sho",D_NADIA);
        break;
    case BlockStop:
        ID=masterPlan->common.currentSize;
        newRoleAgent[ID]->setUpdated(true);
        newRoleAgent[ID]->setAgent(knowledge->getAgent(BlockerStopperID));
        newRoleAgent[ID]->setRoleUpdate(true);
        newRoleAgent[ID]->setAvoidBall(true);
        newRoleAgent[ID]->setAvoidPenaltyArea(true);
        newRoleAgent[ID]->setSelectedSkill(roleSkill::GotopointAvoid);

        virtualPos=wm->opp[blockerID]->pos+Vector2D(0.15,0);
        blockerLine=Segment2D(wm->ball->pos,wm->ball->pos+(virtualPos-wm->ball->pos).norm()*3);
        blockerCircle.intersection(blockerLine,&pos1,&pos2);

        // Nadia
        newRoleAgent[ID]->setTarget(pos1);
        newRoleAgent[ID]->setTargetDir(wm->ball->pos-pos1);

        if (newRoleAgent[ID]->getTarget().dist(newRoleAgent[ID]->getAgent()->pos()) >0.15)
            newRoleAgent[ID]->execute();
        else
            blockerStopStates=TurnAndKick;

        //        debug(QString("The ID is:%1").arg(newRoleAgent[4]->getAgent()->id()),D_NADIA);

        break;

    }

    return result;


}


void CPlayOff::staticExecute() {
    if (initial) {
        assignTasks();

    } else {

        if (knowledge->getGameState() != CKnowledge::OurKickOff) {

            fillRoleProperties();
            posExecute();
            checkEndState();
            debug(QString("ID : %1, ST : %2").arg(1).arg(positionAgent[1].stateNumber), D_MAHI);
            if(masterPlan->common.currentSize > 1 && havePassInPlan) {
                passManager();
            }

            if(isPlanEnd()) {
                playOnFlag = true;
            }

        } else {
            kickOffStopModePlay(masterPlan->common.currentSize);
            for (int i = 0; i < masterPlan->common.currentSize; i++) {
                newRoleAgent[i]->execute();
            }
        }
    }
}


void CPlayOff::dynamicExecute() {


    if (dynamicSelect == CHIP && false) {
        dynamicPlayChipToGoal();
        checkEndChipToGoal();
    } else if (dynamicSelect == KHAFAN || 1) {
        dynamicPlayKhafan();
        checkEndKhafan();
    } else if (dynamicSelect == BLOCKER) {
        dynamicPlayBlocker();
        checkEndBlocker();
    }

    for(int i = 0;i < dynamicAgentSize;i++) {
        roleAgent[i]->execute();
    }
}


void CPlayOff::dynamicAssignID() {
    dynamicAgentSize = 6;
    for (int i = 0;i < 6;i++) {
        if (dynamicMatch[i] != -1) {
            roleAgent[i] -> setAgent(knowledge->getAgent(dynamicMatch[i]));
            roleAgent[i] -> setAgentID(dynamicMatch[i]);
        } else {
            dynamicAgentSize = i;
            break;
        }
    }
}

void CPlayOff::dynamicPlayChipToGoal() {
    if (initial) {
        dynamicAssignID();
        ready = true;

    } else if (ready) {
        roleAgent[0] -> setAvoidCenterCircle(false);
        roleAgent[0] -> setAvoidPenaltyArea(true);
        roleAgent[0] -> setChip(true);
        double speed = knowledge->getProfile(roleAgent[0]->getAgentID(), roleAgent[0]->getAgent()->pos().dist(wm->field->oppGoal()), false, false);
        roleAgent[0] -> setKickSpeed(speed); // Vartypes This
        roleAgent[0] -> setTarget(wm->field->oppGoal());
        roleAgent[0] -> setDoPass(false);
        roleAgent[0] -> setIntercept(false);
        roleAgent[0] -> setLookForward(false);
        roleAgent[0] -> setSelectedSkill(roleSkill::Kick);

        for (int i = 1; i < dynamicAgentSize; i++) {
            if (dynamicMatch[i] != -1) {
                roleAgent[i] -> setAvoidPenaltyArea(true);
                roleAgent[i] -> setAvoidBall(true);
                roleAgent[i] -> setTimeBased(false);
                roleAgent[i] -> setTarget(getDynamicTarget(i + 1));
                roleAgent[i] -> setTargetDir(wm->field->oppGoal() - roleAgent[i]->getAgent()->pos());
                roleAgent[i] -> setEventDist(0.3);
                roleAgent[i] -> setSlow(false);
                roleAgent[i] -> setSelectedSkill(roleSkill::GotopointAvoid);
            }
        }

        ready = false;

    } else if (shot) {
        roleAgent[0] -> setDoPass(true);
        shot = false;
    }
}

void CPlayOff::dynamicPlayBlocker() {
    if (initial) {
        dynamicAssignID();
        ready = true;

    } else if (ready) {
        roleAgent[0] -> setAvoidCenterCircle(false);
        roleAgent[0] -> setAvoidPenaltyArea(true);
        roleAgent[0] -> setChip(false);
        roleAgent[0] -> setKickSpeed(knowledge->getProfile(roleAgent[0]->getAgentID(), 7.8, false, false)); // Vartypes This
        roleAgent[0] -> setTarget(wm->field->oppGoal().rotatedVector((wm->ball->pos.y < 0 ? 90 : -90)));
        roleAgent[0] -> setDoPass(false);
        roleAgent[0] -> setIntercept(false);
        roleAgent[0] -> setLookForward(false);
        roleAgent[0] -> setSelectedSkill(roleSkill::Kick);

        for (int i = 1; i < dynamicAgentSize; i++) {
            if (dynamicMatch[i] != -1) {
                roleAgent[i] -> setAvoidPenaltyArea(true);
                roleAgent[i] -> setAvoidBall(true);
                roleAgent[i] -> setTimeBased(false);
                roleAgent[i] -> setTarget(getDynamicTarget(i + 1));
                roleAgent[i] -> setTargetDir(wm->field->oppGoal() - roleAgent[i]->getAgent()->pos());
                roleAgent[i] -> setEventDist(0.3);
                roleAgent[i] -> setSlow(false);
                roleAgent[i] -> setSelectedSkill(roleSkill::GotopointAvoid);
            }
        }

        ready = false;

    } else if (shot) {

        roleAgent[0] -> setKickSpeed(knowledge->getProfile(roleAgent[0]->getAgentID(), 7.8, false, false)); // Vartypes This
        roleAgent[0] -> setTarget(wm->field->oppGoal());
        roleAgent[0] -> setDoPass(true);
        roleAgent[0] -> setTargetDir(wm->field->oppGoal());
        roleAgent[0] -> setSelectedSkill(roleSkill::Kick);
        shot = false;
    }

}

void CPlayOff::dynamicPlayKhafan() {
    if (initial) {
        dynamicAssignID();
        ready = true;

    } else if (ready) {
        roleAgent[0] -> setAvoidCenterCircle(false);
        roleAgent[0] -> setAvoidPenaltyArea(true);
        roleAgent[0] -> setChip(true);
        roleAgent[0] -> setKickSpeed(700); // Vartypes This
        roleAgent[0] -> setTarget(wm->field->oppGoal());
        roleAgent[0] -> setDoPass(false);
        roleAgent[0] -> setIntercept(false);
        roleAgent[0] -> setTargetDir(wm->field->oppGoal());
        roleAgent[0] -> setSelectedSkill(roleSkill::Kick);

        for (int i = 1; i < dynamicAgentSize; i++) {
            if (dynamicMatch[i] != -1) {
                roleAgent[i] -> setAvoidPenaltyArea(true);
                roleAgent[i] -> setAvoidBall(true);
                roleAgent[i] -> setTimeBased(false);
                roleAgent[i] -> setTarget(getDynamicTarget(i));
                roleAgent[i] -> setTargetDir(wm->field->oppGoal() - roleAgent[i]->getAgent()->pos());
                roleAgent[i] -> setEventDist(0.3);
                roleAgent[i] -> setSlow(false);
                roleAgent[i] -> setSelectedSkill(roleSkill::GotopointAvoid);
            }
        }

        ready = false;

    } else if (pass) {
        roleAgent[0] -> setDoPass(true);
        pass = false;
        debug("DYNAMIC :D ", D_MAHI);

    } else if (shot) {
        roleAgent[1] -> setAvoidCenterCircle(false);
        roleAgent[1] -> setAvoidPenaltyArea(true);
        roleAgent[1] -> setChip(false);
        roleAgent[1] -> setKickSpeed(1023); // Vartypes This
        roleAgent[1] -> setTarget(wm->field->oppGoal());
        roleAgent[1] -> setDoPass(true);
        roleAgent[1] -> setIntercept(false);
        roleAgent[1] -> setTargetDir(wm->field->oppGoal());
        roleAgent[1] -> setSelectedSkill(roleSkill::Kick);
        shot = false;

        roleAgent[0] -> setAvoidPenaltyArea(true);
        roleAgent[0] -> setAvoidBall(true);
        roleAgent[0] -> setTimeBased(false);
        roleAgent[0] -> setTarget(Vector2D(0,-2));
        roleAgent[0] -> setTargetDir(wm->field->oppGoal() - roleAgent[0]->getAgent()->pos());
        roleAgent[0] -> setEventDist(0.3);
        roleAgent[0] -> setSlow(false);
        roleAgent[0] -> setSelectedSkill(roleSkill::GotopointAvoid);


    }

}


void CPlayOff::checkEndKhafan() {
    if (ready) {
        dynamicState = 2;
    } else if (pass) {
        dynamicState = 4;
    } else if (shot) {
        dynamicState = 6;
    }

    if (dynamicState == 2) {
        if (roleAgent[1] -> getAgent() -> pos().dist(roleAgent[1] -> getTarget())
                < roleAgent[1] -> getEventDist()) {
            dynamicState = 4;
            pass = true;
        }
    }

    if (dynamicState == 4) {
        debug(QString("[dastan] : %1").arg(knowledge->getCurrentTime() - dynamicStartTime), D_MAHI);
        if (wm->ball->pos.dist(wm->field->oppGoal()) - 0.5 < roleAgent[1]->getAgent()->pos().dist(wm->field->oppGoal())) {
            pass = false;
            shot = true;
            dynamicState = 6;
        }
        if (!Circle2D(roleAgent[0]->getAgent()->pos(), 0.5).contains(wm->ball->pos) && dynamicStartTime == -1) {
            dynamicStartTime = knowledge->getCurrentTime();
        }

        if (wm->ball->vel.length() < 0.2 && dynamicStartTime != -1) {
            playOnFlag = true;
            dynamicState = 0;
        }

        if ((knowledge->getCurrentTime() - dynamicStartTime) > 300 && dynamicStartTime != -1) {
            playOnFlag = true;
            dynamicState = 0;

        }
    }

    if (dynamicState == 6) {
        // TODO : check this
        playOnFlag = true;
        shot = false;
        if (wm->ball->vel.length() < 0.2) {
            playOnFlag = true;
            dynamicState = 0;
        }
        debug(QString("[dastan] : %1").arg(knowledge->getCurrentTime() - dynamicStartTime), D_MAHI);

        if (knowledge->getCurrentTime() - dynamicStartTime > 200 && dynamicStartTime != -1) {
            playOnFlag = true;
            dynamicState = 0;

        }

    }

}

void CPlayOff::checkEndBlocker() {
    if (ready) {
        dynamicState = 2;
    } else if (shot) {
        dynamicState = 6;
    }



    if (dynamicState == 2) {
        for (int i = 0;i < wm->opp.activeAgentsCount(); i++) {
            if (Circle2D(roleAgent[0] -> getAgent() -> pos() + roleAgent[0]->getAgent()->dir().norm()*0.6, 0.3).contains(wm->opp.active(i)->pos))
                if(roleAgent[0]->getAgent()->dir().norm().dist(roleAgent[0]->getTarget().norm()) < 0.1) {
                    dynamicState = 6;
                    shot = true;
                }
        }

        dynamicStartTime = knowledge->getCurrentTime();

    }

    if (dynamicState == 6) {

        if (!Circle2D(roleAgent[0]->getAgent()->pos(), 0.5).contains(wm->ball->pos)) {
            playOnFlag = true;
            dynamicState = 0;
        }

        if (knowledge->getCurrentTime() - dynamicStartTime > 300 && dynamicStartTime != -1) {
            playOnFlag = true;
            dynamicState = 0;
        }

    }
}

void CPlayOff::checkEndChipToGoal() {
    if (ready) {
        dynamicState = 2;
    } else if (shot) {
        dynamicState = 6;
    }



    if (dynamicState == 2) {
        if (Circle2D(wm->ball->pos, 0.5).contains(roleAgent[0]->getAgent()->pos())) {
            shot = true;
            dynamicState = 6;
            dynamicStartTime = knowledge->getCurrentTime();
        }
    }

    if (dynamicState == 6) {

        if (!Circle2D(roleAgent[0]->getAgent()->pos(), 0.5).contains(wm->ball->pos)) {
            playOnFlag = true;
            dynamicState = 0;
        }

        if (knowledge->getCurrentTime() - dynamicStartTime > 200 && dynamicStartTime != -1) {
            playOnFlag = true;
            dynamicState = 0;
        }

    }
}

Vector2D CPlayOff::getDynamicTarget(int i) {
    Vector2D first = wm->ball->pos+(wm->field->oppGoal() - wm->ball->pos).norm()*3;
    first.y += 0.3;

    switch (i) {
    case 1:
        return first;
    case 2:
        return Vector2D(3.2, 0.3);
    case 3:
        return Vector2D(3.2,  -0.3);
    case 4:
        return Vector2D(0,  0);
    default:
        return Vector2D::INVALIDATED;
        break;
    }
}

bool CPlayOff::isFirstFinished()
{
    return (firstStepEnums == Done);
}

void CPlayOff::resetFirstPlayFinishedFlag() {
    firstStepEnums = Stay;
    blockerStep = S0;
}

int CPlayOff::getShotSpot() {
    return shotSpot;
}
void CPlayOff::firstDegree() {

    int sign;
    sign= wm->ball->pos.y>0 ? 1:-1;

    if(policy()->PlayOff_UseBlockBlocker()){
        newRoleAgent[0]->setTargetDir(wm->field->oppGoal()-wm->ball->pos);
        newRoleAgent[0]->setTarget(wm->ball->pos-newRoleAgent[0]->getTargetDir().norm()*0.3);
        blockersPenaltyArea.clear();

        for(int i=0;i<wm->opp.data->activeAgents.size();i++){
            if(Triangle2D(newRoleAgent[0]->getAgent()->pos()
                          ,wm->field->oppGoal()
                          ,wm->field->oppGoal()/2).contains(wm->opp.active(i)->pos)
                    && Line2D(newRoleAgent[0]->getAgent()->pos(),newRoleAgent[0]->getAgent()->pos()+newRoleAgent[0]->getAgent()->dir()*5).dist(wm->opp.active(i)->pos)<0.3
                    && wm->opp.active(i)->pos.dist(newRoleAgent[0]->getAgent()->pos())<2)
            {
                if(!blockersPenaltyArea.contains(i)){
                    debug(QString("penaltyAreaID:%1").arg(i),D_NADIA);
                    blockersPenaltyArea.append(i);
                }
            }
        }
    }
    else {
        newRoleAgent[0]->setTarget(wm->ball->pos + Vector2D(-0.3,0));
        newRoleAgent[0]->setTargetDir(wm->field->oppGoal());
    }
}

void CPlayOff::secondDegree() {

    int sign;
    sign= wm->ball->pos.y>0 ? 1:-1;

    if(policy()->PlayOff_UseBlockBlocker()){

        newRoleAgent[0]->setTargetDir(wm->field->oppGoal()/2-wm->ball->pos);
        newRoleAgent[0]->setTarget(wm->ball->pos-newRoleAgent[0]->getTargetDir().norm()*0.3);

        blockersCentralRegion.clear();
        for(int i=0;i<wm->opp.data->activeAgents.size();i++){
            if(Triangle2D(newRoleAgent[0]->getAgent()->pos()
                          ,wm->field->oppGoal()/2
                          ,Vector2D(0,0)).contains(wm->opp.active(i)->pos)
                    && Line2D(newRoleAgent[0]->getAgent()->pos()
                              ,newRoleAgent[0]->getAgent()->pos()+newRoleAgent[0]->getAgent()->dir()*5).dist(wm->opp.active(i)->pos)<0.3)
            {
                if(!blockersCentralRegion.contains(i)){
                    debug(QString("centralID:%1").arg(i),D_NADIA);
                    blockersCentralRegion.append(i);
                }
            }
        }
    }

    else {
        newRoleAgent[0]->setTarget(wm->ball->pos + Vector2D(-0.3,0));
        newRoleAgent[0]->setTargetDir(wm->field->oppGoal());
    }

}

void CPlayOff::thirdDegree() {


    if(policy()->PlayOff_UseBlockBlocker()){

        newRoleAgent[0]->setTargetDir(wm->field->ourGoal()-wm->ball->pos);
        newRoleAgent[0]->setTarget(wm->ball->pos-newRoleAgent[0]->getTargetDir().norm()*0.3);

        blockersRoundRegion.clear();
        for(int i=0;i<wm->opp.data->activeAgents.size();i++){
            if(Triangle2D(newRoleAgent[0]->getAgent()->pos()
                          ,Vector2D(0,0)
                          ,wm->field->ourGoal()).contains(wm->opp.active(i)->pos)
                    && Line2D(newRoleAgent[0]->getAgent()->pos(),newRoleAgent[0]->getAgent()->pos()+newRoleAgent[0]->getAgent()->dir()*5).dist(wm->opp.active(i)->pos)<0.3)
            {
                if(!blockersRoundRegion.contains(i)){
                    debug(QString("RoundId:%1").arg(i),D_NADIA);
                    blockersRoundRegion.append(i);
                }
            }
        }
    }
    else {
        newRoleAgent[0]->setTarget(wm->ball->pos + Vector2D(-0.3,0));
        newRoleAgent[0]->setTargetDir(wm->field->oppGoal());
    }





    blockerState=0;

    debug(QString("blocker state1:%1").arg(blockerState),D_NADIA);
    for(int i=0;i<blockersPenaltyArea.size();i++){
        debug(QString("penaltyArea:%1").arg(blockersPenaltyArea.at(i)),D_NADIA);
        //        if(blockersCentralRegion.contains(blockersPenaltyArea.at(i))){
        blockerState += penaltyAreaBlock;
        //        }
        if(blockersCentralRegion.contains(blockersPenaltyArea.at(i))){
            blockerID = blockersPenaltyArea.at(i);
        }
    }

    for(int i=0;i<blockersCentralRegion.size();i++){
        debug(QString("central:%1").arg(blockersCentralRegion.at(i)),D_NADIA);
        //        if(blockersRoundRegion.contains(blockersCentralRegion.at(i)) && i==blockerID)
        if(blockersCentralRegion.at(i)==blockerID)
            blockerState += centralRegionBlock;
    }


    for(int i=0;i<blockersRoundRegion.size();i++){
        debug(QString("Rounds:%1").arg(blockersRoundRegion.at(i)),D_NADIA);
        if(blockersRoundRegion.at(i)==blockerID)
            blockerState += RoundRegionBlock;
    }


    debug(QString("blocker state2:%1").arg(blockerState),D_NADIA);

}

void CPlayOff::doneDegree() {

    if(policy()->PlayOff_UseBlockBlocker()){
        newRoleAgent[0]->setTarget(wm->ball->pos + Vector2D(0.3,0));
        newRoleAgent[0]->setTargetDir(wm->field->ourGoal()-wm->ball->pos);

    }
    else {
        newRoleAgent[0]->setTarget(wm->ball->pos + Vector2D(-0.3,0));
        newRoleAgent[0]->setTargetDir(wm->field->oppGoal());
    }
}

void CPlayOff::stayPoistioning() {

    double x=wm->ball->pos.x;
    int m;
    if(x >_FIELD_WIDTH/3)
        m=0;
    else if( x > _FIELD_WIDTH /6)
        m=1;
    else
        m=-1;



    newRoleAgent[1]->setTarget(Vector2D(1, .5));
    newRoleAgent[1]->setTargetDir(wm->field->oppGoal());
    newRoleAgent[2]->setTarget(Vector2D(1-m, -1.5));
    newRoleAgent[2]->setTargetDir(wm->field->oppGoal());
    newRoleAgent[3]->setTarget(Vector2D(1+m, 1.5));
    newRoleAgent[3]->setTargetDir(wm->field->oppGoal());
    newRoleAgent[4]->setTarget(Vector2D(1-2*m, -2.5));
    newRoleAgent[4]->setTargetDir(wm->field->oppGoal());
    newRoleAgent[5]->setTarget(Vector2D(1+2*m, 2.5));
    newRoleAgent[5]->setTargetDir(wm->field->oppGoal());





}

void CPlayOff::movePositioning() {
    int sign;
    sign= wm->ball->pos.y>0 ? 1:-1;

    newRoleAgent[1]->setTarget(Vector2D(1, -1.5));
    newRoleAgent[1]->setTargetDir(wm->field->oppGoal());
    newRoleAgent[2]->setTarget(Vector2D(1, .5));
    newRoleAgent[2]->setTargetDir(wm->field->oppGoal());
    newRoleAgent[3]->setTarget(Vector2D(-.5, 1.5));
    newRoleAgent[3]->setTargetDir(wm->field->oppGoal());
    newRoleAgent[4]->setTarget(Vector2D(2.5, -2.5));
    newRoleAgent[4]->setTargetDir(wm->field->oppGoal());
    newRoleAgent[5]->setTarget(Vector2D(2, 2.5));
    newRoleAgent[5]->setTargetDir(wm->field->oppGoal());




}

void CPlayOff::donePositioning() {


    newRoleAgent[1]->setTarget(Vector2D(2, -.5));
    newRoleAgent[1]->setTargetDir(wm->field->oppGoal());
    newRoleAgent[2]->setTarget(Vector2D(3.5, -1.5));
    newRoleAgent[2]->setTargetDir(wm->field->oppGoal());
    newRoleAgent[3]->setTarget(Vector2D(0, 1.5));
    newRoleAgent[3]->setTargetDir(wm->field->oppGoal());
    newRoleAgent[4]->setTarget(Vector2D(2, -2.5));
    newRoleAgent[4]->setTargetDir(wm->field->oppGoal());
    newRoleAgent[5]->setTarget(Vector2D(2, 2.5));
    newRoleAgent[5]->setTargetDir(wm->field->oppGoal());

}



void CPlayOff::fastExecute() {
    // TODO : Write fast Execution (playoff)

}

void CPlayOff::firstExecute() {
    // TODO : Write first Execution (playoff)
    if (initial) {
        //        firstStepEnums = Stay;
        //        dynamicAssignIDNEW();
    }

    if (knowledge->getGameState() == CKnowledge::OurKickOff) {
        //        kickOffStopModePlay(masterPlan->common.currentSize);
    } else {
        firstPlayForOppCorner(agentsID.size());

    }

    // TODO : a function that calculate opponent mark streategy :D
    int shotBlocked = 0;
    int passBlocked = 0;
    Vector2D sol1,sol2;
    for (int i = 0; i < wm->opp.activeAgentsCount(); i++) {
        if (wm->opp.active(i)->id == wm->opp.data->goalieID) continue;
        for (int j = 0; j < 6; j++) {
            if (newRoleAgent[j]->getAgent() == NULL) continue;
            if (Circle2D(wm->opp.active(i)->pos, 0.25).intersection(Segment2D(newRoleAgent[j]->getAgent()->pos(), wm->field->oppGoal()), &sol1, &sol2)) {
                shotBlocked++;
            }
        }
    }

    for (int i = 0; i < wm->opp.activeAgentsCount(); i++) {
        if (wm->opp.active(i)->id == wm->opp.data->goalieID) continue;
        for (int j = 0; j < 6; j++) {
            if (newRoleAgent[j]->getAgent() == NULL) continue;
            if (Circle2D(wm->opp.active(i)->pos, 0.25).intersection(Segment2D(newRoleAgent[j]->getAgent()->pos(), wm->ball->pos), &sol1, &sol2)) {
                passBlocked++;
            }
        }
    }


    QQueue<int> oppMark = wm->opp.data->activeAgents;
    oppMark.removeOne(wm->opp.data->goalieID);
    oppMark.removeOne(knowledge->nearestOppToBall);
    double sumDist = 0.0;
    for (int i = 0; i < oppMark.size(); i++) {
        sumDist += wm->opp[oppMark.at(i)]->pos.dist(wm->field->oppGoal());
    }
    sumDist /= oppMark.size();
    if (sumDist < 2) {
        shotSpot = FarNear | FarFar | FarCenter;
    } else if (sumDist > 3) {
        shotSpot = KillSpot;
    } else {
        shotSpot = EveryWhere;
    }



    // TODO : fix tagging ;)
    debug(QString("PB : %1, SB : %2").arg(passBlocked).arg(shotBlocked), D_MAHI);
    debug(QString("ShotSpot : %1 ").arg(shotSpot), D_MAHI);

    analyze("OPP MARK SPOT", shotSpot, true);
    analyze("OPP MARK PASS", passBlocked, true);
    analyze("OPP MARK SHOT", shotBlocked, true);


    for (int i = 0; i < 6; i++) {
        newRoleAgent[i]->execute();
    }

}

void CPlayOff::mahiDebug(int limit) {
    for(size_t i = 0; i < limit;i++) {
        debug(QString("No.%1 : %2").arg(i).arg(debugs[i]),D_MAHI);
    }
}

void CPlayOff::mahiCircle(int limit) {
    for(size_t i = 0; i < limit;i++) {
        draw(circles[i],QColor(Qt::red));
    }
}

void CPlayOff::mahiVector(int limit) {
    for(size_t i = 0; i < limit;i++) {
        draw(draws[i],0,QColor(Qt::blue));
    }
}

void CPlayOff::kickOffStopModePlay(int tAgentsize) {

    for (int i = 0; i < masterPlan->common.currentSize; i++) {
        if (newRoleAgent[i]->getRoleUpdate() == false) {
            newRoleAgent[i]->setUpdated(true);
            newRoleAgent[i]->setAgent(knowledge->getAgent(masterPlan->common.matchedID.value(i)));
            newRoleAgent[i]->setRoleUpdate(true);
            newRoleAgent[i]->setAvoidBall(true);
            newRoleAgent[i]->setAvoidPenaltyArea(true);
            newRoleAgent[i]->setSelectedSkill(roleSkill::GotopointAvoid);

        }
    }

    if (tAgentsize > 0) {
        newRoleAgent[0]->setTarget(kickOffPos[0]);
        newRoleAgent[0]->setTargetDir(-newRoleAgent[0]->getAgent()->pos() + wm->ball->pos);

        for (int i = 1; i < tAgentsize; i++) {
            newRoleAgent[i]->setTarget(kickOffPos[i]);
            newRoleAgent[i]->setTargetDir(-newRoleAgent[i]->getAgent()->pos() + wm->field->oppGoal());
        }
    }

}

void CPlayOff::firstPlayForOppCorner(int _agentSize) {


    debug(QString("mode :%1").arg(firstStepEnums),D_NADIA);
    for (int i = 0; i < _agentSize; i++) {
        if (newRoleAgent[i]->getRoleUpdate() == false) {
            newRoleAgent[i]->setUpdated(true);
            newRoleAgent[i]->setAgent(knowledge->getAgent(dynamicMatch[i]));
            newRoleAgent[i]->setRoleUpdate(true);
            newRoleAgent[i]->setAvoidBall(true);
            newRoleAgent[i]->setAvoidPenaltyArea(true);
            newRoleAgent[i]->setSelectedSkill(roleSkill::GotopointAvoid);

        }
    }

    switch(firstStepEnums) {
    case Stay:
        stayPoistioning();
        break;
    case Move:
        movePositioning();
        break;
    case Done:
        donePositioning();
        break;
    default:
        break;
    }

    switch(blockerStep) {
    case S0:
        firstDegree();
        break;
    case S1:
        secondDegree();
        break;
    case S2:
        thirdDegree();
        break;
    case S3:
        doneDegree();
        break;
    }
        int finisher = 0;
        for (int i = 0; i < _agentSize; i++) {
            if (newRoleAgent[i]->getTarget().dist(newRoleAgent[i]->getAgent()->pos()) < 0.4) {
                finisher++;
            }
        }
        if(!policy()->PlayOff_UseBlockBlocker())
            blockerStep==S3;

        if (finisher == _agentSize-1) {
            if (firstStepEnums == Stay && blockerStep==S3) firstStepEnums = Done;
            else firstStepEnums = Done;
        }
        else if(newRoleAgent[0]->getTarget().dist(newRoleAgent[0]->getAgent()->pos()) < 0.1
                && newRoleAgent[0]->getTargetDir().angleWith(newRoleAgent[0]->getAgent()->dir()).degree()<5){
            if (blockerStep == S0) blockerStep = S1;
            else if (blockerStep == S1) blockerStep = S2;
            else if (blockerStep == S2) blockerStep = S3;
        }


}

void CPlayOff::oneBehindBall() {
    newRoleAgent[0]->setTarget(kickOffPos[0]);
    newRoleAgent[0]->setTargetDir(-activeAgents.at(0)->pos() + wm->ball->pos);
}

void CPlayOff::oneLeftOneCentre() {
    newRoleAgent[0]->setTarget(kickOffPos[0]);
    newRoleAgent[0]->setTargetDir(-activeAgents.at(0)->pos() + wm->ball->pos);
    newRoleAgent[1]->setTarget(kickOffPos[1]);
    newRoleAgent[1]->setTargetDir(-activeAgents.at(1)->pos() + wm->field->oppGoal());
}

void CPlayOff::oneRightOneCentre() {
    newRoleAgent[0]->setTarget(kickOffPos[0]);
    newRoleAgent[0]->setTargetDir(-activeAgents.at(0)->pos() + wm->ball->pos);
    newRoleAgent[1]->setTarget(kickOffPos[1]);
    newRoleAgent[1]->setTargetDir(-activeAgents.at(1)->pos() + wm->field->oppGoal());
}

void CPlayOff::twoSidesOneCentre() {
    newRoleAgent[0]->setTarget(kickOffPos[0]);
    newRoleAgent[0]->setTargetDir(-activeAgents.at(0)->pos() + wm->ball->pos);
    newRoleAgent[1]->setTarget(kickOffPos[1]);
    newRoleAgent[1]->setTargetDir(-activeAgents.at(1)->pos() + wm->field->oppGoal());
    newRoleAgent[2]->setTarget(kickOffPos[2]);
    newRoleAgent[2]->setTargetDir(-activeAgents.at(2)->pos() + wm->field->oppGoal());
}

void CPlayOff::twoSideOneCentreOneDef() {
    newRoleAgent[0]->setTarget(kickOffPos[0]);
    newRoleAgent[0]->setTargetDir(-newRoleAgent[0]->getAgent()->pos() + wm->ball->pos);
    newRoleAgent[1]->setTarget(kickOffPos[1]);
    newRoleAgent[1]->setTargetDir(-newRoleAgent[1]->getAgent()->pos() + wm->field->oppGoal());
    newRoleAgent[2]->setTarget(kickOffPos[2]);
    newRoleAgent[2]->setTargetDir(-newRoleAgent[2]->getAgent()->pos() + wm->field->oppGoal());
    newRoleAgent[3]->setTarget(kickOffPos[3]);
    newRoleAgent[3]->setTargetDir(-newRoleAgent[3]->getAgent()->pos() + wm->field->oppGoal());
}

void CPlayOff::twoSideOneCentreTwoDef() {
    newRoleAgent[0]->setTarget(kickOffPos[0]);
    newRoleAgent[0]->setTargetDir(-newRoleAgent[0]->getAgent()->pos() + wm->ball->pos);
    newRoleAgent[1]->setTarget(kickOffPos[1]);
    newRoleAgent[1]->setTargetDir(-newRoleAgent[1]->getAgent()->pos() + wm->field->oppGoal());
    newRoleAgent[2]->setTarget(kickOffPos[2]);
    newRoleAgent[2]->setTargetDir(-newRoleAgent[2]->getAgent()->pos() + wm->field->oppGoal());
    newRoleAgent[3]->setTarget(kickOffPos[4]);
    newRoleAgent[3]->setTargetDir(-newRoleAgent[4]->getAgent()->pos() + wm->field->oppGoal());
    newRoleAgent[4]->setTarget(kickOffPos[5]);
    newRoleAgent[4]->setTargetDir(-newRoleAgent[4]->getAgent()->pos() + wm->field->oppGoal());
}

void CPlayOff::twoSideOneCentreTwoDefAndGoalie() {
    newRoleAgent[0]->setTarget(kickOffPos[0]);
    newRoleAgent[0]->setTargetDir(-newRoleAgent[0]->getAgent()->pos() + wm->ball->pos);
    newRoleAgent[1]->setTarget(kickOffPos[1]);
    newRoleAgent[1]->setTargetDir(-newRoleAgent[1]->getAgent()->pos() + wm->field->oppGoal());
    newRoleAgent[2]->setTarget(kickOffPos[2]);
    newRoleAgent[2]->setTargetDir(-newRoleAgent[2]->getAgent()->pos() + wm->field->oppGoal());
    newRoleAgent[3]->setTarget(kickOffPos[3]);
    newRoleAgent[3]->setTargetDir(-newRoleAgent[3]->getAgent()->pos() + wm->field->oppGoal());
    newRoleAgent[4]->setTarget(kickOffPos[4]);
    newRoleAgent[4]->setTargetDir(-newRoleAgent[4]->getAgent()->pos() + wm->field->oppGoal());
    newRoleAgent[5]->setTarget(kickOffPos[5]);
    newRoleAgent[5]->setTargetDir(-newRoleAgent[5]->getAgent()->pos() + wm->field->oppGoal());
}

bool CPlayOff::isPlanEnd() {

    if (isPlanDone()) {
        debug("Plan Succeded", D_MAHI);
        return true;

    } else if (isPlanFaild()) {
        if (lastBallPos.dist(wm->ball->pos) > 0.06) {
            debug("Plan Fully Failed", D_MAHI);
            return true;
        } else {
            debug("rePlaning", D_MAHI);
            return false;
        }
    }

    return false;
}

bool CPlayOff::isPlanDone() {
    const int& tLastAgent = masterPlan->execution.theLastAgent;
    const int& tLastState = masterPlan->execution.theLastState;

    // Plan doesn't include a final shoot
    if (tLastState == -1 || tLastAgent == -1) {
        if (isAllTasksDone()) {
            debug ("Done By Fully Tasks Done", D_MAHI);
            masterPlan->common.addHistory(10); //FULL
            return true;
        }
    } else if (isFinalShotDone()) {
        debug ("Done By Final Shot !", D_MAHI);
        // TODO : IF GOAL THEN 10 ELSE 9
        masterPlan->common.addHistory(10); //FULL
        return true;
    }
    return false;
}


bool CPlayOff::isPlanFaild() {
    SFail fail = isAnyTaskFaild();
    if (isTimeOver()) {
        debug ("Faild By Time Over", D_MAHI);
        //        masterPlan->common.addHistory(); // Not Changeing History
        return true;
    } else if (isBallDirChanged()) {
        debug ("Faild By Ball Dir Changed", D_MAHI);
        masterPlan->common.addHistory(0); // MIN
        return true;
    } else if (fail = isAnyTaskFaild(), fail.fail) {
        debug(QString("Task Fail : TaskID %1").arg(fail.taskID), D_MAHI);
        debug(QString("Task Fail : AgentID %1").arg(fail.agentID), D_MAHI);
        debug(QString("Task Fail : PlanID %1").arg(fail.planID), D_MAHI);
        debug(QString("Task Fail : RoleID %1").arg(fail.roleID), D_MAHI);
        debug(QString("Task Fail : Mode %1").arg(fail.mode), D_MAHI);
        debug(QString("Task Fail : Skill %1").arg(fail.skill), D_MAHI);
        masterPlan->common.addHistory(fail.succesRate);
        return true;
    }
    return false;
}

SFail CPlayOff::isAnyTaskFaild() {
    SFail faliure;
    faliure.fail = false;
    // TODO : complete function
    // TODO : calculate succes rate
    return faliure;
}

bool CPlayOff::isAllTasksDone() {

    // if there's a agent that is not zombie
    for (size_t i = 0; i < masterPlan->common.currentSize; i++) {
        if (!positionAgent[i].zombie) {
            return false;
        }
    }

    return true;
}

bool CPlayOff::isTimeOver() {

    if (setTimer) {
        tempStart = knowledge->getCurrentTime();
    }

    if (!Circle2D(lastBallPos, 0.5).contains(wm->ball->pos)) {
        setTimer = false;
        debug(QString("Time That Left: %1").arg(knowledge->getCurrentTime() - tempStart), D_DEBUG);
        if(knowledge->getCurrentTime() - tempStart > 200*masterPlan->execution.passCount) { // 2 Second
            setTimer = true;
            return true;
        }
    }
    return false;
}

bool CPlayOff::isBallDirChanged() {

    if (masterPlan->execution.passCount != 1) return false;

    // USE PASSER FORM INITIAL LEVEL
    const int& passer = masterPlan->execution.passer.id;
    const int& recive = masterPlan->execution.reciver.id;
    Vector2D& b  = wm->ball->pos;
    if (b.dist(lastBallPos) > 0.5 && !roleAgent[passer]->getChip()) {
        Vector2D  bv = b + wm->ball->vel.norm()*_MAX_DIST;
        Circle2D  c(roleAgent[recive]->getWaitPos(), 1); // TODO : CHECK radius
        Segment2D s(b, bv);
        draw(s, QColor(Qt::blue));
        draw(c, QColor(Qt::red));
        Vector2D v1,v2;
        if (!c.intersection(s, &v1, &v2)) return true;
    } else {
        return false;
    }

}

bool CPlayOff::isFinalShotDone() {

    const int& tLastAgent = masterPlan->execution.theLastAgent;
    const int& tLastState = masterPlan->execution.theLastState;

    // Plan hasn't a final shoot
    if (tLastState == -1 || tLastAgent == -1) return false;

    CAgent* tAgent = knowledge ->
            getAgent(masterPlan -> common.matchedID[tLastAgent]);

    Circle2D cir (tAgent->pos() + tAgent->dir().norm()*0.08, 0.16);
    Circle2D cir2(tAgent->pos() + tAgent->dir().norm()*0.20, 0.40);

    draw(cir , QColor(Qt::blue));
    draw(cir2, QColor(Qt::blue));

    if (positionAgent[tLastAgent].stateNumber == tLastState) {
        if (cir.contains(wm->ball->pos)) {
            isBallIn = true;

        } else if (isBallIn && !cir2.contains(wm->ball->pos)) {
            isBallIn = false;
            return true;

        }
    }

    return false;
}

Vector2D CPlayOff::getEmptyTarget(Vector2D _position, double _radius) {
    Vector2D tempTarget,finalTarget,position;
    double escapeRad;
    int oppCnt = 0;
    bool posFound;
    escapeRad = _radius;
    position  = _position;
    finalTarget = position;
    for(double dist=0.0 ; dist<=0.5 ; dist+=0.2 ) {
        for(double ang=-180.0 ; ang<=180.0 ; ang+=60.0 ) {
            tempTarget = position + Vector2D::polar2vector(dist,ang);
            ////should check
            if(wm->field->isInOppPenaltyArea(tempTarget + (wm->field->oppGoal() - tempTarget).norm() * 0.3))
                continue;
            for(int i = 0; i < wm->opp.activeAgentsCount();i++) {
                if(Circle2D(wm->opp.active(i)->pos,0.07).contains(tempTarget)) {
                    oppCnt = 1;
                    break;
                }

            }
            if(!oppCnt) {
                finalTarget = tempTarget;
                posFound = true;
                break;
            }
        }
        if(posFound) {
            break;
        }
    }

    return finalTarget;
}
///////////////PassManager///////////////////
void CPlayOff::passManager() {
    // TODO : FOR MORE THAN ONE PASS

    const AgentPoint& p = masterPlan->execution.passer;
    const AgentPoint& r = masterPlan->execution.reciver;

    const int& i = masterPlan->common.matchedID.value(r.id);

    CAgent* c    = knowledge->getAgent(i);
    if (positionAgent[r.id].stateNumber == r.state
            ||  positionAgent[r.id].stateNumber == r.state + 1) {
        debug(QString("RC : %1, %2").arg(r.id).arg(r.state), D_MAHI);
        draw(Circle2D(positionAgent[r.id].getAbsArgs(r.state).staticPos, masterPlan->common.lastDist), QColor(Qt::darkMagenta));
        if (positionAgent[r.id].getAbsArgs(r.state).staticPos.dist(c -> pos()) >
                masterPlan->common.lastDist) {
            doPass = false;
        } else {
            doPass = true;
        }

        if (positionAgent[r.id].getAbsArgs(r.state).staticPos.dist(c -> pos()) >
                masterPlan->common.lastDist) {
            doAfterlife = false;
        } else {
            doAfterlife = true;
        }

        roleAgent[p.id]->setDoPass(doPass);

    }
}

/**
 * @brief CPlayOff::isTaskDone
 * @param _roleAgent
 * @return true if the task get done
 */
bool CPlayOff::isTaskDone(CRolePlayOff* _roleAgent){

    switch (_roleAgent->getSelectedSkill()) {
    case roleSkill::Gotopoint:
    case roleSkill::GotopointAvoid:
        return isMoveDone(_roleAgent);
        break;
    case roleSkill::Kick:
        return isKickDone(_roleAgent);
        break;
    case roleSkill::OneTouch:
        return isOneTouchDone(_roleAgent);
        break;
    case roleSkill::ReceivePass:
        return isReceiveDone(_roleAgent);
        break;
        //  Life
    case roleSkill::Mark:
    case roleSkill::Support:
    case roleSkill::Defense:
        qDebug() << "got it";
        _roleAgent->setRoleUpdate(false);
        return false;
        break;
    default:
        return false;
    }
}

void CPlayOff::posExecute() {
    for(int i = 0;i < masterPlan->common.currentSize; i++) {
        if (roleAgent[i]->getAgent() != NULL) {
            roleAgent[i]->execute();
        }
    }
}

void CPlayOff::checkEndState() {

    for(int i = 0;i < masterPlan->common.currentSize;i++) {
        if (roleAgent[i]->getAgent() == NULL) continue;
        if(isTaskDone(roleAgent[i])) {

            roleAgent[i]->setRoleUpdate(false);
            roleAgent[i]->resetTime();

            if(positionAgent[i].stateNumber + 1  < positionAgent[i].positionArg.size()) {
                if (positionAgent[i].getArgs(1).staticSkill == Defense
                        ||  positionAgent[i].getArgs(1).staticSkill == Support
                        ||  positionAgent[i].getArgs(1).staticSkill == Position
                        ||  positionAgent[i].getArgs(1).staticSkill == Goalie
                        ||  positionAgent[i].getArgs(1).staticSkill == Mark) {

                    if (doAfterlife) {
                        positionAgent[i].stateNumber++;
                    }
                    continue;
                }
                positionAgent[i].stateNumber++;
                debug(QString("ID : %1, ST : %2").arg(i).arg(positionAgent[i].stateNumber), D_MAHI);
            } else {
                //                positionAgent[i].zombie = true;
                ///Temp
                //                SPositioningArg tempPA;
                //                tempPA = positionAgent[i].getAbsArgs(positionAgent[i].positionArg.size() - 1);
                //                tempPA.staticSkill = MoveSkill;
                //                positionAgent[i].positionArg.append(tempPA);
            }
        }
    }
}

void CPlayOff::fillRoleProperties() {
    for(size_t i = 0;i < agentsID.size(); i++) {
        if (masterPlan->common.matchedID.contains(i)) {
            if (roleAgent[i]->getRoleUpdate() == false) {

                roleAgent[i]->setFirstMove((positionAgent[i].stateNumber == 0));
                roleAgent[i]->setAgent(knowledge->getAgent(masterPlan->common.matchedID.value(i)));
                // handle onetouch faster
                if (positionAgent[i].stateNumber + 1 < positionAgent[i].positionArg.size()) {
                    if (positionAgent[i].getArgs().staticSkill == MoveSkill && positionAgent[i].getArgs(1).staticSkill == OneTouchSkill) {
                        positionAgent[i].stateNumber++;
                    }
                }

                roleAgent[i]->setRoleUpdate(true);
                roleAgent[i]->resetTime();
                assignTask(roleAgent[i], positionAgent[i]);
            }

        } else {
            qWarning() << "[Warning] coach -> Match function doesn't work :( ";
            if (roleAgent[i]->getRoleUpdate() == false) {
                roleAgent[i]->setAgent(knowledge->getAgent(agentsID.at(i)));
                assignTask(roleAgent[i], positionAgent[i]);
                roleAgent[i]->setRoleUpdate(true);
            }
        }
    }
}

void CPlayOff::assignTask(CRolePlayOff* _roleAgent, const SPositioningAgent& _positionAgent) {

    switch(_positionAgent.getArgs().staticSkill) {
    case PassSkill:
        assignPass(_roleAgent, _positionAgent);
        break;
    case ReceivePassSkill:
        assignReceive(_roleAgent, _positionAgent, false);
        break;
    case ReceivePassIASkill:
        assignReceive(_roleAgent, _positionAgent, true);
        break;
    case ShotToGoalSkill:
        assignKick(_roleAgent, _positionAgent, false);
        break;
    case ChipToGoalSkill:
        assignKick(_roleAgent, _positionAgent, true);
        break;
    case OneTouchSkill:
        assignOneTouch(_roleAgent, _positionAgent);
        break;
    case MoveSkill:
        assignMove(_roleAgent, _positionAgent);
        break;
        // After Life
    case Defense:
        _roleAgent->setRoleUpdate(false);
        assignDefense(_roleAgent, _positionAgent);
        break;
    case Support:
        _roleAgent->setRoleUpdate(false);
        assignSupport(_roleAgent, _positionAgent);
        break;
    case Position:
        _roleAgent->setRoleUpdate(false);
        assignPosition(_roleAgent, _positionAgent);
        break;
    case Goalie:
        _roleAgent->setRoleUpdate(false);
        assignGoalie(_roleAgent, _positionAgent);
        break;
    case Mark:
        _roleAgent->setRoleUpdate(false);
        assignMark(_roleAgent, _positionAgent);
        break;
    case NoSkill:
        //        assignAfterLife(_roleAgent, _positionAgent);
        break;
    }
}

void CPlayOff::assignPass(CRolePlayOff* _roleAgent, const SPositioningAgent& _posAgent) {
    _roleAgent->setAvoidCenterCircle(false);
    _roleAgent->setAvoidPenaltyArea(true);
    _roleAgent->setChip(chipOrNot(_posAgent.getArgs()));
    if (_roleAgent->getChip()) {
        //        _roleAgent->setKickSpeed(_posAgent.getArgs().rightData);
        _roleAgent->setKickRealSpeed(static_cast <double> (_posAgent.getArgs().rightData)/200);
        debug(QString("VALUE : %1").arg(static_cast <double> (_posAgent.getArgs().rightData)/200), D_MAHI);
    } else {
        _roleAgent->setKickSpeed(_posAgent.getArgs().leftData);
        //        _roleAgent->setKickRealSpeed(static_cast <double> (_posAgent.getArgs().leftData)/100);

    }

    _roleAgent->setTarget(positionAgent[_posAgent.getArgs().PassToId].getAbsArgs(_posAgent.getArgs().PassToState).staticPos);
    _roleAgent->setDoPass(doPass);
    _roleAgent->setIntercept(false);
    _roleAgent->setTargetDir(_posAgent.getArgs().staticAng);
    _roleAgent->setSelectedSkill(roleSkill::Kick);
}

void CPlayOff::assignReceive(CRolePlayOff* _roleAgent, const SPositioningAgent& _posAgent, bool _ignoreAngle) {

    _roleAgent->setAvoidPenaltyArea(true);
    _roleAgent->setIgnoreAngle(_ignoreAngle);
    _roleAgent->setTarget(_posAgent.getArgs().staticPos);
    _roleAgent->setTargetDir(_posAgent.getArgs().staticAng); /** Just Matter when we use Ignore mode **/
    _roleAgent->setReceiveRadius((double)_posAgent.getArgs().leftData/100);
    _roleAgent->setSelectedSkill(roleSkill::ReceivePass);
}

void CPlayOff::assignKick(CRolePlayOff* _roleAgent,
                          const SPositioningAgent& _posAgent, bool _chip) {

    _roleAgent->setChip(_chip);
    _roleAgent->setKickRealSpeed(static_cast<double>(_posAgent.getArgs().leftData)/100);
    _roleAgent->setTarget(getGoalTarget(_posAgent.getArgs().rightData));
    _roleAgent->setIntercept(false);
    _roleAgent->setSelectedSkill(roleSkill::Kick);
}

void CPlayOff::assignOneTouch(CRolePlayOff* _roleAgent,
                              const SPositioningAgent& _posAgent) {

    _roleAgent->setAvoidPenaltyArea(true);
    _roleAgent->setWaitPos(_posAgent.getArgs().staticPos);
    _roleAgent->setKickSpeed(_posAgent.getArgs().leftData);
    _roleAgent->setTarget(getGoalTarget(_posAgent.getArgs().rightData));
    _roleAgent->setSelectedSkill(roleSkill::OneTouch);
}

void CPlayOff::assignMove(CRolePlayOff* _roleAgent,
                          const SPositioningAgent& _posAgent) {
    _roleAgent -> setAvoidPenaltyArea(true);
    _roleAgent -> setTime(_posAgent.getArgs().rightData); // ignore duration -> time is wait
    _roleAgent -> setAvoidBall(true);


    if (_posAgent.getArgs().staticPos == POBALLPOS) { // First Passer

        _roleAgent -> setTimeBased(true);
        _roleAgent -> setTarget(wm->ball->pos - Vector2D(0.20, 0));
        _roleAgent -> setTargetDir(wm->ball->pos - _roleAgent->getAgent()->pos());
        _roleAgent -> setSlow(true);
        _roleAgent -> setMaxVelocity(1);

    } else if (_roleAgent->getTime() > 10) {

        _roleAgent -> setTimeBased(true);
        _roleAgent -> setTarget(getMoveTarget(_posAgent.getArgs()));
        _roleAgent -> setTargetDir(_posAgent.getArgs().staticAng);
        _roleAgent -> setSlow(false);
        _roleAgent -> setMaxVelocity(getMaxVel(_roleAgent, _posAgent.getArgs()));

    } else {

        _roleAgent -> setTimeBased(false);
        _roleAgent -> setEventDist(_posAgent.getArgs().staticEscapeRadius);
        _roleAgent -> setTarget(getMoveTarget(_posAgent.getArgs()));
        _roleAgent -> setTargetDir(_posAgent.getArgs().staticAng);
        _roleAgent -> setSlow(false);
        _roleAgent -> setMaxVelocity(getMaxVel(_roleAgent, _posAgent.getArgs()));

    }

    _roleAgent -> setSelectedSkill(roleSkill::GotopointAvoid);
}

void CPlayOff::assignGoalie(CRolePlayOff * _roleAgent,
                            const SPositioningAgent &_posAgent) {
    _roleAgent->setAvoidPenaltyArea(true);
    _roleAgent->setAvoidBall(false);
    _roleAgent->setSlow(false);
    _roleAgent->setTargetDir(Vector2D(0, 1));
    _roleAgent->setTarget(wm->field->ourGoal() + Vector2D(0,1));
    _roleAgent->setSelectedSkill(roleSkill::GotopointAvoid);
}

void CPlayOff::assignMark(CRolePlayOff * _roleAgent,
                          const SPositioningAgent &_posAgent) {

    _roleAgent->setAvoidPenaltyArea(true);
    _roleAgent->setAvoidBall(false);
    _roleAgent->setNoAvoid(true);
    _roleAgent->setTargetDir(Vector2D(0, 1));
    _roleAgent->setSlow(false);
    CDefPos tempDefPos;
    kkDefPos tempPos = tempDefPos.getDefPositions(wm->ball->pos, 2, 2, 3);
    _roleAgent->setTarget(tempPos.pos[1]);
    _roleAgent->setSelectedSkill(roleSkill::GotopointAvoid); //GotoPointAvoid

}

void CPlayOff::assignPosition(CRolePlayOff * _roleAgent,
                              const SPositioningAgent &_posAgent) {
    assignMove(_roleAgent, _posAgent); // TODO : check this
}

void CPlayOff::assignSupport(CRolePlayOff * _roleAgent,
                             const SPositioningAgent &_posAgent) {
    Vector2D tempTarget = (lastBallPos.dist(wm->ball->pos) < 0.1)
            ? _roleAgent->getAgent()->pos()
            : (wm->ball->pos + wm->ball->vel*0.5) - Vector2D(1,0);


    _roleAgent->setAvoidPenaltyArea(true);
    _roleAgent->setAvoidBall(false);
    _roleAgent->setSlow(false);
    _roleAgent->setTargetDir(_roleAgent->getAgent()->pos() - wm->ball->pos);
    _roleAgent->setTarget(getEmptyTarget(tempTarget, .4));
    _roleAgent->setSelectedSkill(roleSkill::GotopointAvoid); //GotoPointAvoid
}

void CPlayOff::assignDefense(CRolePlayOff * _roleAgent,
                             const SPositioningAgent &_posAgent) {
    _roleAgent->setAvoidPenaltyArea(true);
    _roleAgent->setAvoidBall(false);
    _roleAgent->setNoAvoid(true);
    _roleAgent->setTargetDir(Vector2D(0, 1));
    _roleAgent->setSlow(false);
    CDefPos tempDefPos;
    kkDefPos tempPos = tempDefPos.getDefPositions(wm->ball->pos, 2, 2, 3);
    _roleAgent->setTarget(tempPos.pos[0]);
    _roleAgent->setSelectedSkill(roleSkill::GotopointAvoid); //GotoPointAvoid
}


Vector2D CPlayOff::getMoveTarget(int agentID,
                                 int agentState) {
    Vector2D tempTarget,finalTarget,position;
    double escapeRad;
    int oppCnt = 0;
    bool posFound;
    escapeRad = positionAgent[agentID].positionArg.at(agentState).staticEscapeRadius;
    position = positionAgent[agentID].positionArg.at(agentState).staticPos;
    finalTarget = position;
    for(double dist=0.0 ; dist<=escapeRad ; dist+=0.2 ) {
        for(double ang=-180.0 ; ang<=180.0 ; ang+=60.0 ) {
            tempTarget = position + Vector2D::polar2vector(dist,ang);
            for(int i = 0; i < wm->opp.activeAgentsCount();i++) {
                if(Circle2D(wm->opp.active(i)->pos,0.02).contains(tempTarget)) oppCnt++;
            }
            if(!oppCnt) {
                finalTarget = tempTarget;
                posFound = true;
                break;
            }
        }
        if(posFound) break;
    }

    return finalTarget;
}

Vector2D CPlayOff::getMoveTarget(const SPositioningArg& _posArg) {
    return getEmptyTarget(_posArg.staticPos, _posArg.staticEscapeRadius);
}

double CPlayOff::getMaxVel(int agentID,
                           int agentState) {
    double tempVel,tDist;
    Vector2D tAgentPos;
    tAgentPos = knowledge->getAgent(kkAgentsID[agentID])->pos();
    tDist = tAgentPos.dist(positionAgent[agentID].positionArg.at(positionAgent[agentID].stateNumber).staticPos);
    tempVel = tDist/positionAgent[agentID].positionArg.at(positionAgent[agentID].stateNumber).leftData;
    if(tempVel >= 4) tempVel = 4;
    else if(tempVel <= 1.5) tempVel = 1.5;
    return tempVel;
}


double CPlayOff::getMaxVel(const CRolePlayOff* _roleAgent,
                           const SPositioningArg& _posArg) {
    double tempVel,tDist;
    Vector2D tAgentPos;
    tAgentPos = _roleAgent->getAgent()->pos();
    tDist = tAgentPos.dist(_posArg.staticPos);
    tempVel = tDist/_posArg.leftData;

    if(tempVel >= 4)
        tempVel = 4;
    else if(tempVel <= 1.5)
        tempVel = 1.5;

    return tempVel;
}


Vector2D CPlayOff::getGoalTarget(int shoterID,int shoterState) {
    Vector2D tempPos;
    double tempYPos = (double)(positionAgent[shoterID].positionArg.at(shoterState).rightData)/1000 + wm->field->oppGoalR().y;
    draw(QString("%1").arg(tempYPos), Vector2D(-2,-2));
    if(tempYPos < wm->field->oppGoalR().y) tempYPos = wm->field->oppGoalR().y;
    if(tempYPos > wm->field->oppGoalL().y) tempYPos = wm->field->oppGoalL().y;
    tempPos.assign(wm->field->oppGoal().x,tempYPos);
    return tempPos;
}

Vector2D CPlayOff::getGoalTarget(const long& _y) {
    Vector2D tempPos;
    double tempYPos = (double)(_y)/1000 + wm->field->oppGoalR().y;
    draw(QString("%1").arg(tempYPos), Vector2D(-2,-2));
    if(tempYPos < wm->field->oppGoalR().y) tempYPos = wm->field->oppGoalR().y;
    if(tempYPos > wm->field->oppGoalL().y) tempYPos = wm->field->oppGoalL().y;
    tempPos.assign(wm->field->oppGoal().x,tempYPos);
    return tempPos;
}

bool CPlayOff::chipOrNot(int passerID, int ReceiverID, int ReceiverState){

    if(positionAgent[passerID].positionArg.at(positionAgent[passerID].stateNumber).leftData < 0) return true;
    else if(positionAgent[passerID].positionArg.at(positionAgent[passerID].stateNumber).rightData < 0) return false;
    else return !isPathClear(wm->ball->pos,
                             positionAgent[ReceiverID].positionArg.at(ReceiverState).staticPos,
                             0.5,
                             0.1);
}

bool CPlayOff::chipOrNot(const SPositioningArg& _posArg) {
    if (_posArg.leftData < 0) {
        return true;
    } else if(_posArg.rightData < 0) {
        return false;
    } else {
        const int& id = _posArg.PassToId;
        const int& ps = _posArg.PassToState;
        return !isPathClear(wm->ball->pos,
                            positionAgent[id].getAbsArgs(ps).staticPos,
                            0.5,   // Radius
                            0.1);  // Tereshold
    }
}

bool CPlayOff::isPathClear(Vector2D _pos1,
                           Vector2D _pos2,
                           double _radius,
                           double treshold) {

    Vector2D sol1, sol2, sol3;
    Line2D _path(_pos1, _pos2);
    Polygon2D _poly;
    Circle2D(_pos2,_radius + treshold).intersection(_path.perpendicular(_pos2),
                                                    &sol1,
                                                    &sol2);

    _poly.addVertex(sol1);
    sol3 = sol1;
    _poly.addVertex(sol2);
    Circle2D(_pos1,CRobot::robot_radius_new + treshold).intersection(_path.perpendicular(_pos1),
                                                                     &sol1,
                                                                     &sol2);

    _poly.addVertex(sol2);
    _poly.addVertex(sol1);
    _poly.addVertex(sol3);

    draw(_poly,"red");

    for(int i = 0;i < wm->opp.activeAgentsCount();i++) {
        if(_poly.contains(wm->opp.active(i)->pos)) return false;
    }
    return true;
}

void CPlayOff::assignTasks() {
    int& sym = masterPlan->execution.symmetry;
    for(size_t i = 0;i < masterPlan->common.currentSize; i++) {
        positionAgent[i].positionArg.clear();
        Q_FOREACH(playOffRobot agentPlan, masterPlan->execution.AgentPlan[i]) {
            SPositioningArg tempPosArg;
            tempPosArg.staticPos          = agentPlan.pos;
            tempPosArg.staticAng          = Vector2D::polar2vector(1, agentPlan.angle);
            tempPosArg.staticAng.assign(tempPosArg.staticAng.x, -1*sym*tempPosArg.staticAng.y);
            tempPosArg.staticPos.assign(tempPosArg.staticPos.x, sym*tempPosArg.staticPos.y);
            tempPosArg.staticEscapeRadius = agentPlan.tolerance;

            Q_FOREACH(playOffSkill skill, agentPlan.skill) {
                tempPosArg.leftData           = skill.data[0];
                tempPosArg.rightData          = skill.data[1];
                tempPosArg.staticSkill        = skill.name;
                tempPosArg.PassToId           = skill.targetAgent;
                tempPosArg.PassToState        = skill.targetIndex;

                if (skill.name == PassSkill && positionAgent[i].positionArg.back().staticSkill == MoveSkill) {
                    positionAgent[i].positionArg.back().staticPos = POBALLPOS;
                } else if (skill.name == ShotToGoalSkill
                           || skill.name == ChipToGoalSkill
                           || skill.name == OneTouchSkill) {

                    if (sym < 0) {
                        tempPosArg.rightData = 1000 - tempPosArg.rightData;
                    }
                }
                positionAgent[i].positionArg.append(tempPosArg);
            }
        }
    }
}

int CPlayOff::findReciver(int _passer, int _state) {
    if (_state == 0) {
        return 0;
    }
    for (size_t i = _state; i < positionAgent[_passer].positionArg.size(); i++) {
        if (positionAgent[_passer].getArgs(i).staticSkill == PassSkill) {
            SBallOwner temp;
            temp.id = _passer;
            temp.state = i;
            ownerList.append(temp);
            findReciver(positionAgent[_passer].getArgs(i).PassToId,
                        positionAgent[_passer].getArgs(i).PassToState);
        }
    }
}

void CPlayOff::getCostRec(double costArr[][6], int arrSize, QList<kkValue> &valueList, kkValue value, int size, int aId)
{
    if(size <= 1) {
        int tIndex = kkGetIndex(value, 0);
        value.IDs[tIndex] = aId;
        value.value += costArr[tIndex][aId];
        valueList.append(value);
        return;
    }
    for(int i = 0; i < size; i++) {
        kkValue tValue = value;
        int tIndex = kkGetIndex(tValue, i);
        tValue.IDs[tIndex] = aId;
        tValue.value += costArr[tIndex][aId];
        getCostRec(costArr, arrSize, valueList, tValue, size - 1, aId + 1);
    }
}

int CPlayOff::kkGetIndex(kkValue &value, int cIndex)
{
    int cnt = 0;
    for(int i = 0; i < value.agentSize; i++)
    {
        if(value.IDs[i] == -1)
        {
            if(cnt == cIndex)
                return i;
            cnt++;
        }
    }
}

POMODE CPlayOff::getPlayOffMode() {
    if(knowledge->getGameMode() == knowledge->OurKickOff) return KICKOFF;
    else if(knowledge->getGameState() == knowledge->OurIndirectKick || knowledge->getGameMode() == knowledge->OurIndirectKick) return INDIRECT;
    else if(knowledge->getGameState() == knowledge->OurDirectKick || knowledge->getGameMode() == knowledge->OurDirectKick) return DIRECT;
}

void CPlayOff::reset(){


    qDebug() << "Bring yourself back online playoff";

    blockerState=0;
    blockerID=-1;
    blockerStopStates=Diversion;

    decidePlan = true;
    agentSize = 1;
    for(int i = 0;i < 6;i++) {
        positionAgent[i].stateNumber = 0;
        roleAgent[i]->reset();
        newRoleAgent[i]->reset();
        positionAgent[i].zombie = false;
    }
    isBallIn = false;
    tempAgent = new CRolePlayOff();
    doAfterlife = false;
    doPass = false;
    criticalInit = true;

    setTimer = true;
    ////////////

    currentPlan = new SPlayOffPlan();
    masterPlan = NULL;

    initial    = true;
    playOnFlag = false;
    havePassInPlan = false;

    //Dynamic
    ready = pass = shot = false;
    dynamicStartTime = -1;

    executedCycles = 0;
    activeAgents.clear();
    markAgents.clear();

    firstStepEnums = Stay;
    blockerStep = S0;

    debug(QString("reset Plan"),D_MAHI);
}

void CPlayOff::init(QList<int> _agents , QMap<QString , EditData*> *_editData){
    setAgentsID(_agents);
    setEditData(_editData);
    initMaster();

}

void CPlayOff::execute_0(){
    globalExecute();
}

void CPlayOff::execute_1(){
    globalExecute();
}

void CPlayOff::execute_2(){
    globalExecute();
}

void CPlayOff::execute_3(){
    globalExecute();
}

void CPlayOff::execute_4(){
    globalExecute();
}

void CPlayOff::execute_5(){
    globalExecute();
}

void CPlayOff::execute_6(){
    globalExecute();
}

////////////////////////////////

void CPlayOff::setMasterPlan(SPlan *_thePlan) {
    masterPlan = _thePlan;
}

void CPlayOff::setMasterMode(EMode _mode) {
    masterMode = _mode;
}

EMode CPlayOff::getMasterMode() {
    return masterMode;
}
///////////////////////////////////////
/////////////Check Execution///////////

/*!
*   \brief check if ball get distance from robot,
*
*          in case that it's direct shoot
*          also check that ball is in right direction or not.
*
*
*/
bool CPlayOff::isKickDone(CRolePlayOff * _roleAgent) {

    if (Circle2D(_roleAgent->getAgent()->pos(), 0.4).contains(wm->ball->pos)) {
        _roleAgent->setBallIsNear(true);
    } else if ( !Circle2D(_roleAgent->getAgent()->pos(), 0.6).contains(wm->ball->pos)
                && _roleAgent->getBallIsNear() ) {
        _roleAgent->setBallIsNear(false);
        if (_roleAgent->getChip()) {
            debug("[playoff] chip Done", D_MAHI);
            return true;
        } else {
            /** Ball gonna touch the target point **/

            // check ball speed
            if (wm->ball->vel.length() / (_roleAgent->getAgent()->pos().dist(_roleAgent->getTarget())) > 1 || true) {
                debug("[playoff] speed is enough", D_MAHI);
                // check ball direction
                Vector2D sol1,sol2;
                if (Circle2D(_roleAgent->getTarget(), 0.5).intersection(Ray2D(wm->ball->pos, wm->ball->pos + wm->ball->vel), &sol1, &sol2)) {
                    debug("[playoff] direction is correct", D_MAHI);
                    debug("[playoff] kick is Done", D_MAHI);
                    return true;
                }
            }
        }
    }
    return false;
}

bool CPlayOff::isReceiveDone(const CRolePlayOff * _roleAgent) {
    if(Circle2D(_roleAgent->getAgent()->pos(), 0.3).contains(wm->ball->pos)) {
        if (wm->ball->vel.length() < 0.5)
            return true;
    }
    return false;
}

bool CPlayOff::isOneTouchDone(CRolePlayOff * _roleAgent) {
    if (isKickDone(_roleAgent)) {
        debug("[playoff] OneTouch is Done", D_MAHI);
        return true;
    } else {
        return false;
    }
}

bool CPlayOff::isMoveDone(const CRolePlayOff * _roleAgent) {

    if (_roleAgent->getFirstMove() && _roleAgent->getTarget() != POBALLPOS) {
        return true ;
    }

    if (_roleAgent->getTimeBased()) {
        debug(QString("EL : %1").arg(_roleAgent->getElapsed()), D_HOSSEIN);
        debug(QString("GT : %1").arg(_roleAgent->getTime()), D_HOSSEIN);
        if (_roleAgent->getElapsed() > _roleAgent->getTime()) {
            debug("D------------------", D_HOSSEIN);
            return true;
        }
    } else {
        // TODO : vartypes this
        if (_roleAgent->getAgent()->pos().dist(_roleAgent->getTarget()) < max(0.3,_roleAgent->getEventDist()/100)) {
            return true;
        }
    }
    return false;
}


///////////OverLoading Operators
QDebug operator<< (QDebug d, NGameOff::SPlan _plan) {

    QString mode;
    if (_plan.common.planMode == KICKOFF)
        mode = "KickOff";
    else if (_plan.common.planMode == DIRECT)
        mode = "Direct";
    else if (_plan.common.planMode == INDIRECT)
        mode = "InDirect";


    d << "------------------->>>";
    d << "<Common>" ;
    d << "Agent Size" << _plan.common.agentSize;
    d << "Chance" << _plan.common.chance;
    d << "Last Dist" << _plan.common.lastDist;
    d << "Plan Mode" << mode;
    d << "Succes Rate" << _plan.common.succesRate;
    d << "Tags" << _plan.common.tags;
    d << "</Common>";
    d << "<<<-------------------";
    return d;
}

void CPlayOff::setInitial(bool _init) {
    initial = _init;
}

QPair<int, int> CPlayOff::findTheLastShoot(const SExecution &_plan) {
    QPair<int, int> last;
    last.first = last.second = -1;

    QList<POffSkills> finalSkills;
    finalSkills.append(ShotToGoalSkill);
    finalSkills.append(ChipToGoalSkill);
    finalSkills.append(OneTouchSkill);

    int counter = 0;
    qDebug() << "SSS" << _plan.AgentPlan.size();
    Q_FOREACH (QList<playOffRobot> agent, _plan.AgentPlan) {
        int counter2 = 0;
        Q_FOREACH(playOffRobot node, agent) {
            Q_FOREACH(playOffSkill skill, node.skill) {
                if (finalSkills.contains(skill.name)) {
                    last.first  = counter;
                    last.second = counter2;
                    qDebug() << "MAHI " << skill.name;
                }
                counter2++;
            }
        }
        counter++;
    }

    return last;
}

void CPlayOff::analyseShoot()
{
    if (masterPlan != NULL) {
        QPair<int, int> last;
        last = findTheLastShoot(masterPlan->execution);
        masterPlan->execution.theLastAgent = last.first;
        masterPlan->execution.theLastState = last.second;
        qDebug() << "Last" << last.first << last.second;
        havePassInPlan = (last.first != -1  && last.second != -1);
    }
}

void CPlayOff::analysePass() {
    // TODO : need edit for mulitiple pass
    if (masterPlan != NULL) {
        // first : passer second : reciver
        QList<AgentPair> tPass;
        findThePasserandReciver(masterPlan->execution, tPass);
        havePassInPlan = tPass.size();
        masterPlan->execution.passCount = tPass.size();
        if (havePassInPlan) {
            masterPlan->execution.passer .id     = tPass.at(0).first.id;
            masterPlan->execution.passer .state  = tPass.at(0).first.state;
            masterPlan->execution.reciver.id     = tPass.at(0).second.id;
            masterPlan->execution.reciver.state  = tPass.at(0).second.state;
        }
    }

    qDebug() << "PI : " << masterPlan->execution.passer .id;
    qDebug() << "PS : " << masterPlan->execution.passer .state;
    qDebug() << "RI : " << masterPlan->execution.reciver.id;
    qDebug() << "RS : " << masterPlan->execution.reciver.state;
}

bool CPlayOff::criticalPlay() {

    if (criticalInit) {
        criticalInit = false;

        criticalKick->setAgent(knowledge->getAgent(masterPlan->execution.passer.id));
        criticalKick->setTarget(wm->field->oppGoal());
        criticalKick->setChip(false);
        criticalKick->setDontKick(false);
        criticalKick->setPassProfiler(false);
        criticalKick->setKickSpeed(1000);
        criticalKick->setTolerance(0.5);
    }
    criticalKick->execute();
    if (wm->ball->vel.length() > 0.5) return true;
    else return false;

}

void CPlayOff::findThePasserandReciver(const NGameOff::SExecution & _plan,
                                       QList<AgentPair>& _pairList) {

    QList<AgentPoint> passer;

    for (int i = 0; i < _plan.AgentPlan.size(); i++) {
        const QList<playOffRobot> & agent = _plan.AgentPlan.at(i);
        for (int j = 0; j < agent.size(); j++) {
            const playOffRobot& node = agent.at(j);
            for (int k = 0; k < node.skill.size(); k++) {
                const POffSkills& skill = node.skill.at(k).name;
                if (skill == PassSkill) {
                    passer.append(AgentPoint(i,j));
                }
            }
        }
    }

    for (int i = 0; i < passer.size(); i++) {
        const int &id = passer.at(i).id;
        const int &st = passer.at(i).state;

        int si = (_plan.AgentPlan[id][st].skill[1].name == PassSkill) ? 1 : 2;
        int rid = _plan.AgentPlan[id][st].skill[si].targetAgent;
        int rs  = _plan.AgentPlan[id][st].skill[si].targetIndex;
        debug(QString("PASS : %1, %2, %3, %4").arg(id).arg(st).arg(rid).arg(rs), D_MAHI);
        AgentPoint tempReciver;
        tempReciver.id    = rid;
        tempReciver.state = rs;

        AgentPair ap;
        ap.first  = passer[i];
        ap.second = tempReciver;
        _pairList.append(ap);

    }

}

Vector2D CPlayOff::getMarkTarget(const SPositioningArg &_posArg) {
    // TODO : find best point
    return _posArg.staticPos - Vector2D(1, 0);
}

Vector2D CPlayOff::getDefenseTarget(const SPositioningArg &_posArg) {
    // TODO : find best point
    return _posArg.staticPos - Vector2D(1, 0);

}

Vector2D CPlayOff::getSupportTarget(const SPositioningArg &_posArg) {
    // TODO : find best point
    return _posArg.staticPos - Vector2D(1, 0);

}
