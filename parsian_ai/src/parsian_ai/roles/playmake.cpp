#include "parsian_ai/roles/playmake.h"
#include <QDebug>
#include <ctime>
using namespace std;


CRolePlayMake::CRolePlayMake(CAgent *_agent) : CRole(_agent)
{
    qqHist = 0;
    justTurn = false;
    kickToTheirDefense = false;
    kick = new KickAction();
    wait = new NoAction();
    wait->setWaithere(true);
    gotoball = new CSkillGotoBall(_agent);
    onetouch = new CSkillKickOneTouch(_agent);
    spin = new CSkillSpinBack(_agent);
    gotopoint = new GotopointavoidAction();
    hitTheBall = new CSkillHitTheBall(_agent);
    turn = new CSkillTurn(_agent);
    chip = slow = false;
    onetouching = -1;
    lastAgent = NULL;
    shadowPass = false;
    startKicked = false;
    indirect = false;
    direct = false;
    kickoff = false;
    indirectKhafan = false;
    chipIndirect = false;
    through = false;
    gameMode = 0;
    noKick = false;
    indirectPassRecverSelectedFrame = -1;
    penaltyRand = -1;
    penaltyCounter = 0;
    penaltyTarget.invalidate();
    manualPassReceive = false;
    allowonetouch = false;
    allowonepass = false;
    indirectTiny = false;
    passReceiver = -1;
    lastPassReceiver = -1;
    ballavoidanceState = -1;
    lastDecisionTime = 0.0;
    shoot = new CBehaviourKick;
    for (int i=0;i<PASS_BEHAVS;i++)
    {
        pass[i]  = new CBehaviourPass;
        chippass[i]  = new CBehaviourChipPass;
        spinPass[i] = new CBehaviourSpinPass;
    }
    fixedPass = new CBehaviourPass;
    chipToGoal = new CBehaviourChipToGoal;
    clear = new CBehaviourKick;
    kickToDefense = new CBehaviourKickBetweenTheirDefenders;
    lastFrameCrowded  = -1;
    ballTop.setGuardMin(-0.1);
    ballTop.setGuardMax( 0.1);
    ballLeft.setGuardMin(-0.1);
    ballLeft.setGuardMax( 0.1);
    waitBeforePass = 999999;
    orderRushInPlenalty = 999999;
    orderedRush = false;
    timerStartFlag = true;

    //    spinPass = new CBehaviourSpinPass;
    initialPoint.invalidate();
    forceRedecide = false;

    lastBounceDataFile.setFileName("lastBounce");
    out.setDevice(&lastBounceDataFile);
}

CRolePlayMake::~CRolePlayMake()
{
}

void CRolePlayMake::passShootNew()
{
    if (!gameState->ourIndirectKick()){
        knowledge->variables["passtopoint"] = "false";
        knowledge->variables["passpointdef"] = "false";
        knowledge->variables["passpointsw"] = "false";
    }
    //fixed pass
    if (knowledge->variables["passtopoint"]=="true")
    {
        fixedPass->agents.clear();
        fixedPass->agents.append(agent);
        fixedPass->setFixedTarget(knowledge->fixedPassPoint);
        fixedPass->setSlowKick(true);
        fixedPass->execute();
        for (int i=0;i<passreceivers.count();i++)
            passreceivers[i]->idle = false;
        agent->idle = false;
        return;
    }
    if (knowledge->variables["passpointdef"]=="true")
    {
        fixedPass->agents.clear();
        fixedPass->agents.append(agent);
        fixedPass->setFixedTarget(knowledge->fixedPassPointDef);
        fixedPass->setSlowKick(true);
        fixedPass->execute();
        for (int i=0;i<passreceivers.count();i++)
            passreceivers[i]->idle = false;
        agent->idle = false;
        return;
    }
    if (knowledge->variables["passpointsw"]=="true")
    {
        fixedPass->agents.clear();
        fixedPass->agents.append(agent);
        fixedPass->setFixedTarget(knowledge->fixedPassPointsw);
        fixedPass->setSlowKick(true);
        fixedPass->execute();
        for (int i=0;i<passreceivers.count();i++)
            passreceivers[i]->idle = false;
        agent->idle = false;
        return;
    }

    for (QMap<int, double>::iterator i=passProbs.begin();i!=passProbs.end();i++)
    {
        drawer->draw(QString("%1").arg(i.value(), 0, 'g', 2), knowledge->getAgent(i.key())->pos() + Vector2D(0.0, -0.15), "blue", 13);
    }
    double t = CProfiler::getTime();
    bool onetouching = false;
    if (currentBehaviour != NULL)
    {
        if (currentBehaviour->getName() == "kick")
        {
            if (static_cast<CBehaviourKick*> (currentBehaviour)->onetouching != -1)
            {
                onetouching = true;
            }
        }
    }
    bool redecide = false;
    if (onetouching)
    {
        if ((agent != lastAgent) || (t - lastDecisionTime > 3.0) || (currentBehaviour == NULL) || (knowledge->getGameStateChanged()) || ( currentBehaviour->getName() == "kick" && currentBehaviour->probability() < 0.3 ))
        {
            redecide = true;
        }
    }
    else {
        if (gameState->ourKickoff())
        {
            if ((agent != lastAgent) || (t - lastDecisionTime > 3.0) || (currentBehaviour == NULL) || (knowledge->getGameStateChanged())||( currentBehaviour->getName() == "kick" && currentBehaviour->probability() < 0.3 ))
            {
                redecide = true;
            }
        }
        else {
            if ((agent != lastAgent) || (t - lastDecisionTime > 3.0) || (currentBehaviour == NULL) || (knowledge->getGameStateChanged())||( currentBehaviour->getName() == "kick" && currentBehaviour->probability() < 0.3 ))
            {
                redecide = true;
            }
        }
    }

    if ((knowledge->getTechnicalMode().isEmpty()) &&
        (
                (gameState->isPlayOn()
                 &&
                 (wm->field->isInOurPenaltyArea( (wm->ball->pos-wm->field->ourGoal())*0.7 + wm->field->ourGoal() )
                  || wm->field->isInOurPenaltyArea( (wm->ball->predict(0.15)-wm->field->ourGoal())*0.7 + wm->field->ourGoal() )
                  || wm->field->isInOurPenaltyArea( (wm->ball->predict(0.3)-wm->field->ourGoal())*0.7 + wm->field->ourGoal() )
                  || wm->field->isInOurPenaltyArea( (wm->ball->predict(0.45)-wm->field->ourGoal())*0.7 + wm->field->ourGoal() )
                  || wm->field->isInOurPenaltyArea( (agent->pos()))
                  || wm->field->isInOurPenaltyArea( (agent->pos()+agent->vel()*0.15))
                  || wm->field->isInOurPenaltyArea( (agent->pos()+agent->vel()*0.3))
                  || wm->field->isInOurPenaltyArea( (agent->pos()+agent->vel()*0.45))
                  || wm->field->isInOurPenaltyArea( Vector2D(agent->pos()-wm->field->ourGoal())*0.7+wm->field->ourGoal())
                  || (knowledge->frameCount - knowledge->defenseClearingFrame < 9))

                ))
            )
    {
        gotopoint->setRobot_Id(agent->id());
        Vector2D qq = wm->ball->pos - wm->field->ourGoal();
        qq = qq.norm();
        bool rr = false;
        if (qqHist == 0)
        {
            if (qq.y > 0)
            {
                rr = false;
                qqHist = 1;
            }
            else
            {
                rr = true;
                qqHist = 2;
            }
        }
        else {
            if (qq.y > 0.1)
            {
                rr = false;
                qqHist = 1;
            }
            else if (qq.y < -0.1)
            {
                rr = true;
                qqHist = 2;
            }
            else {
                if (qqHist == 1)
                {
                    rr = true;
                }
                else if (qqHist == 2)
                {
                    rr = false;
                }
            }
        }
        if (rr)
        {
            qq = qq.rotatedVector(30);
        }
        else {
            qq = qq.rotatedVector(-30);
        }
        //            qq.y = -qq.y;
        qq = wm->field->ourGoal() + qq*1.5;
        gotopoint->setTargetpos(qq);
        gotopoint->setTargetdir((qq-wm->field->ourGoal()).norm());
        drawer->draw(Circle2D(qq, 0.3), "red", false);
        debugger->debug("Naro tu un sagmassab!!", D_ERROR);
        agent->action=gotopoint;
    }
    else {
        if (redecide || forceRedecide)
        {
            forceRedecide = false;
            lastDecisionTime = t;
            //create a new decision
            for (int i=0;i<PASS_BEHAVS;i++)
            {
                pass[i]->agents.clear();
                pass[i]->agents.append(agent);
                pass[i]->setSlowKick(indirect || kickoff || direct);
                pass[i]->setChipPass(false);
                pass[i]->setShadowPass(false);
                if (khers) {
                    pass[i]->setNoChip(false);
                    pass[i]->setNoKickProb(true);
                }
                else {
                    pass[i]->setNoChip(false);
                    pass[i]->setNoKickProb(false);
                }
                chippass[i]->agents.clear();
                chippass[i]->agents.append(agent);
                chippass[i]->setSlowKick(indirect || kickoff || direct);
                spinPass[i]->agents.clear();
                spinPass[i]->agents.append(agent);
            }
            passreceivers.clear();
            passreceivers.append(knowledge->roleAssignments["positioningPlan"]);
            //			for (int i=0;i<passreceivers.count();i++)
            //                if ( passreceivers.at(i)->vel().length() > 1.2)
            //				{
            //					passreceivers.removeAt(i);
            //					i--;
            //				}
            int p = 0;
            for (int i=0;i<passreceivers.count();i++)
            {
                pass[p]->agents.append(passreceivers[i]);
                chippass[p]->agents.append(passreceivers[i]);
                spinPass[p]->agents.append(passreceivers[i]);
                p++;
            }
            passProbs.clear();
            for (int i=0;i<passreceivers.count();i++)
            {
                if (!pass[i]->agents[1]->inOutState)
                {
                    passProbs.insert(pass[i]->agents[1]->id(), pass[i]->probability()*2.0);
                }
            }
            //todo: check defenders
            shoot->agents.clear();
            shoot->agents.append(agent);
            //			shoot->setSlowKick( direct || kickoff);
            shoot->setSlowKick(false);
            shoot->setChipToGoal(false);


            chipToGoal->agents.clear();
            chipToGoal->agents.append(agent);
            chipToGoal->setSlowKick( direct || kickoff);
            chipToGoal->setChipToGoal(true);

            behaviours.clear();
            for (int i=0;i<p;i++)
            {
                behaviours.append(pass[i]);
                //            behaviours.append(chippass[i]);
            }
            if ((knowledge->roleAssignments["positioningPlan"].count() == 0) && (knowledge->getGameMode()==CKnowledge::OurIndirectKick))
            {
                if (!noKick) behaviours.append(chipToGoal);
            }
            else if (knowledge->getGameMode()!=CKnowledge::OurIndirectKick)
            {
                if (!noKick)
                {
                    behaviours.append(shoot);
                    if (knowledge->getGameMode() == CKnowledge::OurKickOff)
                        behaviours.append(chipToGoal);
                }
            }
            //			selectBehaviour();

            int bestPass = -1, bestChipPass= -1, bestSpinPass = -1;
            double bPass = -1, bChip = -1, bspin = -1.0;
            for( int i = 0; i < p; i++)
            {
                if ( pass[i]->probability() > bPass)
                {
                    bPass = pass[i]->probability();
                    bestPass = i;
                }
                if ( chippass[i]->probability() > bChip)
                {
                    bChip = chippass[i]->probability();
                    bestChipPass = i;
                }
                if ( spinPass[i]->probability() > bspin)
                {
                    bspin = spinPass[i]->probability();
                    bestSpinPass = i;
                }
            }

            drawer->draw(QString("Shoot : %1 Pass : %2 ChipPass : %3 ChipGoal : %4").arg(shoot->probability()).arg(pass[bestPass]->probability()).arg(chippass[bestChipPass]->probability()).arg(chipToGoal->probability()),Vector2D(-2.1,-2.1),"magenta",12);
            if( indirect && bestPass!=-1)
                currentBehaviour = pass[bestPass];
            else if( shoot->probability() > 0.18 && !indirect)
            {
                currentBehaviour = shoot;
            }
                //			else if ( bestSpinPass != -1 && spinPass[bestPass]->probability() > 0.3)
                //			{
                //				currentBehaviour = spinPass[bestSpinPass];
                //			}
            else if ( bestPass != -1 && pass[bestPass]->probability() > 0.3)
            {
                currentBehaviour = pass[bestPass];
            }
            else if ( bestChipPass != -1 &&chippass[bestChipPass]->probability() > 0.3)
            {
                currentBehaviour = chippass[bestChipPass];
            }
            else
            {
                currentBehaviour = chipToGoal;
            }
            if (kickoff && (currentBehaviour != shoot))
            {
                if (bestPass != -1)
                {
                    if (pass[bestPass]->probability() < 0.7)
                    {
                        currentBehaviour = chipToGoal;
                    }
                }
                if (bestChipPass != -1)
                {
                    if (chippass[bestChipPass]->probability() < 0.7)
                    {
                        currentBehaviour = chipToGoal;
                    }
                }

            }

        }
        if (currentBehaviour != NULL)
        {
            for (int i=0;i<passreceivers.count();i++)
                passreceivers[i]->idle = true;
            agent->idle = true;
            currentBehaviour->updateIdleStatus(); //unidle involving agents
            currentBehaviour->execute();
            drawer->draw(QString("%1 : %2").arg(currentBehaviour->getName()).arg(currentBehaviour->probability(), 0 ,'g', 2), Vector2D(1.0,-2.2), "pink", 12);
        }
        else {
            debugger->debug("Playmaker riiiide", D_ERROR);
        }
    }
    return;
}


void CRolePlayMake::executeDefault()
{
    cyclesExecuted++;
    if ( cyclesExecuted < cyclesToWait )
    {
        drawer->draw("waiting",Vector2D(0,-2));
        wait->setRobot_Id(agent->id());
        agent->action = wait;
        return;
    }

    clear = false;
    kick->setClear(clear);
    slow = false;
    kick->setSlow(indirect || direct);
    if( nostop)
        kick->setSlow(false);
    kick->setTurn(false);
    stopped = knowledge->getGameMode()==CKnowledge::Stop;
    if (nostop) stopped = false;
    if (stopped) passEval.reset();
    if (agent != lastAgent)
    {
        onetouching = -1;
    }
    oppBlockers.clear();
    target = knowledge->onetouchablity(agent->id(), w, ang, coming);

    if (((agent->distToBall().length() < 0.5) || (coming<-0.7)) && agent->id() == passEval.receivingAgent())
        passEval.receivedPass();


    if (!stopped)
    {
        if (spinBack()) {
            if( wm->field->ourGoal().dist(wm->ball->pos) > 1.5)
                knowledge->variables["spinning"] = "true";
            else
                knowledge->variables["spinning"] = "false";
        }
        else
        {
            initialPoint.invalidate();
            knowledge->variables["spinning"] = "false";
            double vis = 0;
            knowledge->goalVisiblity(agent->id(), vis, 1.0);
            passShootNew();
        }
    }
    else {
        stopBehindBall();
        knowledge->variables["spinning"] = "false";
    }

    lastAgent = agent;
}

void CRolePlayMake::stopBehindBall(bool penalty)
{
    knowledge->gameStarter = agent;
    if (passReceiver == -2)
    {
    }
    if( penalty)
    {
        if( knowledge->getGameState() == CKnowledge::Stop ){
            debugger->debug("stop, reset changeDirPenaltyStriker flag", D_FATEMEH);

        }

        //		drawer->draw("stopped !!!",Vector2D(0,0),"black",60);
        gotopoint->setRobot_Id(agent->id());
        //		gotopoint->setPlan2(true);
        if(gameState->penaltyShootout()) {
            gotopoint->setTargetpos(wm->ball->pos + (wm->ball->pos - wm->field->oppGoal()) * 0.03);
            gotopoint->setTargetdir(wm->ball->pos - agent->pos());
        }
        else{
            Vector2D direction, position;

            direction = wm->ball->pos - agent->pos();
            direction.y*=1.2;
            position = wm->ball->pos + (wm->ball->pos - wm->field->oppGoal() + Vector2D(0,0.2)).norm()*(0.13);
            gotopoint->setTargetpos(position);
            gotopoint->setTargetdir(direction);
        }

        //        gotopoint->setTargetLook( wm->ball->pos + Vector2D( wm->ball->pos - wm->field->oppGoalL()).norm()*0.15 , wm->field->oppGoalR());

        gotopoint->setSlowmode(true);
        gotopoint->setNoavoid(false);
        gotopoint->setPenaltykick(true);
        gotopoint->setAvoidpenaltyarea(false);
        gotopoint->setAvoidcentercircle(false);

        gotopoint->setBallobstacleradius(0.2);
        agent->action=gotopoint;
        gotopoint->setNoavoid(false);
        gotopoint->setSlowmode(false);

    }
    else
    {
        //		drawer->draw("stopped ?!!!",Vector2D(0,0),"orange",60);
        gotopoint->setRobot_Id(agent->id());
        //        gotopoint->setFastW(false);
        Vector2D shadowPoint = wm->ball->pos + Vector2D( wm->ball->pos - wm->field->oppGoal()).norm()*0.3;
        if ( kickoffmode || kickoffWing)
            shadowPoint = wm->ball->pos + Vector2D( wm->field->oppGoal() - wm->ball->pos ).norm()*0.3;
        gotopoint->setSlowmode(true);
        gotopoint->setNoavoid(false);
        gotopoint->setAvoidpenaltyarea(true);
        gotopoint->setAvoidcentercircle(false);
        gotopoint->setBallobstacleradius(static_cast<float>(4 * CBall::radius));
        gotopoint->setTargetpos(shadowPoint);
        gotopoint->setTargetdir(Vector2D(1.0, 0.0));
        gotopoint->setLookat(wm->ball->pos);
        agent->action = gotopoint;
    }
}

void CRolePlayMake::executeOurKickOff()
{
    slow = true;
    kickoffmode = true;

    if ( cyclesExecuted < cyclesToWait )
    {
        drawer->draw("waiting",Vector2D(0,-2));
        stopBehindBall(false);
        return;
    }

    if( kickMode == FixedShoot )
        chipToOppGoal = true;
    // todo : is true?
    if(gameState->isPlayOff() /*knowledge->getGameMode() == CKnowledge::Stop */){
        stopBehindBall(false);
    }
    else{
        if(conf.OurKickOffChipToGoal || chipToOppGoal){
            kick->setRobot_Id(agent->id());
            double w;
            kick->setTolerance(0.05);
            kick->setTarget(knowledge->goalVisiblity(agent->id(), w, policy()->PlayMaker_UnderEstimateTheirGoalie()));

            kick->setTarget(wm->field->oppGoal()*(2.0/3.0));

            int kickSpeed;
            kickSpeed = agent->chipDistanceValue(wm->ball->pos.dist(wm->field->oppCornerL()/3.0) , true);

            kick->setKickspeed(kickSpeed);
            kick->setSpin(false);
            kick->setChip(true);
            kick->setSlow(false);
            agent->action = kick;
        }
        else{
            if (wm->ball->inSight<=0 || !wm->ball->pos.valid() || !wm->field->isInField(wm->ball->pos)) {
                wait->setRobot_Id(agent->id());
                agent->action = wait;
                return;
            }

            Vector2D target = wm->field->oppGoal();
            int kickSpeed = agent->kickSpeedValue(5 , false);

            target = pointToPass;
            kickSpeed = agent->kickSpeedValue(agent->kickValueForDistance(target.dist(agent->pos()) , 3) , false); // todo wtf?

            kick->setSlow(true);
            kick->setSpin(false);
            kick->setChip(false);
            kick->setTarget(target);
            kick->setKickspeed(kickSpeed);
            kick->setTolerance(0.06);
            agent->action = kick;
        }
    }
}
bool CRolePlayMake::ShootPenalty(){
    double w;
    QList<int> relax,empty;
    relax.append(agent->id());
    penaltyTarget = knowledge->getEmptyPosOnGoal(agent->pos(), w, true, relax, empty);
    if(penaltyTarget.dist(wm->field->oppGoal()) < 0.1){
        penaltyTarget=knowledge->getEmptyPosOnGoalForPenalty(1.0/10.0, true, 0.06,agent);

    }
    debugger->debug(QString("goalie index :%1").arg(wm->opp.data->goalieID),D_NADIA);
    if(wm->opp[wm->opp.data->goalieID] == NULL)
        return false;
    return Segment2D(agent->pos(), penaltyTarget).dist(wm->opp[wm->opp.data->goalieID]->pos)
           > fabs(agent->pos().x-wm->opp[wm->opp.data->goalieID]->pos.x)/4;
}

double CRolePlayMake::lastBounce(){
    return (wm->field->oppGoal().dist(agent->pos()))-0.23;
}

int CRolePlayMake::getPenaltychipSpeed(){
    Vector2D oppGoaliPos=wm->opp[wm->opp.data->goalieID]->pos;
    debugger->debug(QString("chipsepeed:%1").arg(knowledge->chipGoalPropability(true)),D_NADIA);
    if(knowledge->chipGoalPropability(true)>0.1){

        return (knowledge->getProfile(agent->id(),(oppGoaliPos-agent->pos()).length(),false)
                +knowledge->getProfile(agent->id()+1,lastBounce(),false))/2;
    }
    else return -1;
}

void CRolePlayMake::firstKickInShootout(bool isChip){

    double divation=0;

    debugger->debug("first : ",D_NADIA);


    penaltyTarget=wm->field->oppGoalL()+divation*Vector2D(0,wm->field->oppGoalL().y);;
    kick->setTarget(penaltyTarget);

    if(isChip){//chip first


        if(wm->getIsSimulMode())
            kick->setKickspeed(1);
        else
            kick->setKickspeed(170);
        kick->setChip(true);
        if(wm->ball->vel.length()>0.4)
            firstKick=false;

    }else{//kick first


        kick->setChip(false);
        if(wm->getIsSimulMode())
            kick->setKickspeed(1);
        else
            kick->setKickspeed(50);
        if(wm->ball->vel.length()>0.1)
            firstKick=false;
    }
}

void CRolePlayMake::kickInitialShootout(){
    kick->setRobot_Id(agent->id());
    penaltyTarget=wm->field->oppGoal();
    kick->setTarget(penaltyTarget);
    kick->setPenaltykick(false);
    kick->setInterceptmode(false);
    kick->setChip(false);
    kick->setVeryfine(false);
    kick->setTolerance(1);
    kick->setSpin(0);
}

void CRolePlayMake::ShootoutSwitching(bool isChip){

    if(wm->ball->vel.length()<0.2)
        firstKick=true;



    switch(choosePenaltyStrategy()){

        case pgoaheadShoot:
            debugger->debug("pgoahead : ",D_NADIA);
            if(agent->pos().x < 1){//agent is not ahead enough


                penaltyTarget=wm->field->oppGoalL();
                kick->setTarget(penaltyTarget);

                if(isChip){//chip first

                    kick->setKickspeed(170);
                    kick->setChip(true);

                }else{//kick first

                    kick->setChip(false);
                    kick->setKickspeed(50);
                }
            }
            else{ //shoot to goal

                penaltyTarget=knowledge->getEmptyPosOnGoalForPenalty(0.13,true, 10,agent);
                kick->setChip(false);
                kick->setKickspeed(1023);
                kick->setDontkick(false);
                kick->setTarget(penaltyTarget);

            }

            break;


        case pchipShoot:
            debugger->debug("pchipshoot",D_NADIA);
            kick->setTarget(wm->field->oppGoal());
            kick->setKickspeed(getPenaltychipSpeed());
            kick->setChip(true);
            break;

        case pshootDirect:
            debugger->debug("pdirect : ",D_NADIA);
            penaltyTarget=knowledge->getEmptyPosOnGoalForPenalty(0.13,true, 10,agent);
            kick->setTarget(penaltyTarget);
            kick->setChip(false);
            kick->setKickspeed(1000);
            kick->setAvoidopppenaltyarea(true);
            break;
    }
}

int CRolePlayMake::choosePenaltyStrategy(){
    if(ShootPenalty()) return pshootDirect;
    else if(getPenaltychipSpeed()!= -1) return pchipShoot;
    else return pgoaheadShoot;
}

void CRolePlayMake::executeOurPenaltyShootout(){

    bool chipchip=false;

    debugger->debug("penalty Shootout : ",D_NADIA);
    if (abs(wm->ball->pos.x) > 4.4)//penalty finished
        firstKick=true;

    if(wm->opp[wm->opp.data->goalieID]!= NULL )//check opp goalkeeper situation
    {
        if((wm->opp[wm->opp.data->goalieID]->pos-wm->field->oppGoal()).length()>2.5)
            goalKeeperForward=true;
    }


    if (gameState->isPlayOff()/*knowledge->getGameMode()==CKnowledge::Stop*/)
    {//stop behind ball
        cyclesExecuted--;
        srand(static_cast<unsigned int>(time(NULL)));
        stopBehindBall(true);
        penaltyCounter = 0;
        setNoKick(true);
    }
    else {      //force start

        //initial kick skill:
        kickInitialShootout();

        if(ShootPenalty())
            firstKick=false;

        if(firstKick){
            firstKickInShootout(chipchip);
        }
        else{
            ShootoutSwitching(chipchip);
        }

        kick->setShotemptyspot(true);
        kick->setAvoidopppenaltyarea(false);
        agent->action = kick;
        drawer->draw(penaltyTarget,"red"); // todo : is my change OK
        //drawer->draw(penaltyTarget,0,"red");
    }


}

void CRolePlayMake::executeOurPenalty() {
    kick->setRobot_Id(agent->id());
    gotopoint->setRobot_Id(agent->id());

    Vector2D shift;
    Vector2D position;

    if (gameState->isPlayOff()/*knowledge->getGameMode()==CKnowledge::Stop || knowledge->getGameState()==CKnowledge::Stop*/)
    {
        cyclesExecuted--;
        srand(static_cast<unsigned int>(time(NULL)));
        stopBehindBall(true);
        penaltyTarget = knowledge->getEmptyPosOnGoalForPenalty(1.0/8.0, true, 0.03);

        changeDirPenaltyStrikerTime.restart();
        timerStartFlag = true;
    }
    else {
        penaltyTarget = knowledge->getEmptyPosOnGoalForPenalty(1.0/8.0, true, 0.03);   //////// tune
        agent->setRoller(1); //todo : add roller to gotopoint

        ////////////// change robot direction before kicking //////////////
        if(timerStartFlag)
        {
            if(changeDirPenaltyStrikerTime.elapsed() < 2500){
                if(penaltyTarget.y * wm->field->oppGoalL().y < 0 && penaltyTarget.dist(wm->field->oppGoal()) > 0.25){
                    penaltyTarget.y = wm->field->oppGoalR().y*2;
                    shift = Vector2D(0,0.3);
                }
                else{
                    penaltyTarget.y = wm->field->oppGoalL().y*2;
                    shift = Vector2D(0,-0.3);
                }
                position = wm->ball->pos + (wm->ball->pos - wm->field->oppGoal() + shift).norm()*(0.13);
                gotopoint->setTargetdir(penaltyTarget);
                gotopoint->setTargetpos(position);
                gotopoint->setLookat(wm->ball->pos);
            }
            else
            {
                timerStartFlag = false;
            }

        }

        gotopoint->setDivemode(true);
        gotopoint->setSlowmode(false);

        kick->setTarget(penaltyTarget);
        // TODO : Fix This Speed
        // TODO : check this mhmmd
        //        kick->setKickSpeed(knowledge->getProfile(agent->id(),7.8 ,true, false);
        //        kick->setKickSpeed(agent->kickSpeedValue(7.8,false));
        //        kick->setPenaltyKick(true);
        kick->setInterceptmode(false);
        kick->setSpin(false);
        kick->setChip(false);
        kick->setVeryfine(false);
        kick->setWaitFrames(0);
        if(wm->getIsSimulMode())
            kick->setKickspeed(7);
        else
            kick->setKickspeed(1023);
        kick->setAvoidopppenaltyarea(false);
        kick->setTolerance(20);
        kick->setChip(false);

        if(timerStartFlag){
            agent->action = gotopoint;
        }
        else{
            agent->action = kick;
        }
    }

    //    drawer->draw(penaltyTarget, 0, "cyan");

}

void CRolePlayMake::theirPenaltyPositioning(){
    debugger->debug("iiiin",D_NADIA);
    gotopoint->setRobot_Id(agent->id());
    gotopoint->setTargetpos(wm->field->oppCornerL());
    gotopoint->setTargetdir(wm->field->ourGoal());
    agent->action = gotopoint;
}

bool CRolePlayMake::canScoreGoal(){ // todo : khatarnak ehtemal eshteba check shavad
    if( gameState->ourPenaltyKick() || gameState->ourIndirectKick() ||
            (gameState->ourKickoff() && gameState->isPlayOff() ||
             ((gameState->theirPenaltyKick() || gameState->isPlayOn()) && gameState->penaltyShootout())
            )){
        return false;
    }

    double w, ang, coming;
    Vector2D targ = knowledge->onetouchablity(agent->id(), w, ang, coming);//debug(QString("id is: %1").arg(agent->id()),D_MOHAMMED);
    info()->oneToucher.removeAll(agent->self()->id);
    if(info()->oneToucher.isEmpty())
        info()->oneToucherDist2Ball = 100;
    if(agent->pos().dist(wm->ballCatchTarget(agent->self())) < info()->oneToucherDist2Ball
       &&  (agent->canOneTouch() && coming > 0.3 && ang<policy()->PlayMaker_OneTouchAngleThreshold() )
            )
    {
        Segment2D ballSeg(wm->ball->pos , wm->ball->pos+wm->ball->vel.norm());
        drawer->draw(QString("PlayMake OneTouch"),Vector2D(-2,1),"magenta",18);
        //		onetouch->setRobot_Id(agent->id());
        //		onetouch->setTarget(wm->field->oppGoal());
        //		onetouch->setKickSpeed(agent->kickSpeedValue(7.8 , false));
        //		onetouch->setWaitPos(agent->pos());
        //		onetouch->setDistToBallLine(0.1);
        //		onetouch->setVelocityToBallLine(0.3);
        //		onetouch->execute();
        //		agent->intention
        shoot->agents.clear();
        shoot->agents.append(agent);
        shoot->setSlowKick(false);
        shoot->setChipToGoal(false);
        shoot->execute();
        info()->oneToucher.append(agent->self()->id);
        info()->oneToucherDist2Ball = agent->pos().dist(wm->ballCatchTarget(agent->self()));
        return true;
    }

    double region;
    QList <int> ourRelax;
    ourRelax.clear();
    ourRelax.append(wm->our.data->activeAgents);
    if( knowledge->goalie != NULL && ourRelax.contains(knowledge->goalie->self()->id)){
        ourRelax.removeOne(knowledge->goalie->self()->id);
    }
    for( int i=0 ; i<knowledge->defenseAgents.size() ; i++ ){
        if( ourRelax.contains(knowledge->defenseAgents.at(i)->self()->id) )
            ourRelax.removeOne(knowledge->defenseAgents.at(i)->self()->id);
    }
    QList <int> oppRelax;
    Vector2D target = knowledge->getEmptyPosOnGoal(wm->ball->pos , region , true , ourRelax, oppRelax);
    if( region > 0.3 ){
        kick->setRobot_Id(agent->id());
        kick->setTarget(target);
        kick->setKickspeed(agent->kickSpeedValue(7.8,false));
        kick->setInterceptmode(true);
        kick->setSpin(false);
        kick->setChip(false);
        kick->setSlow(false);
        kick->setWaitFrames(0);
        kick->setTolerance(0.06);
        kick->setAvoidpenaltyarea(true);
        agent->action = kick;
        return true;
    }
    return false;
}

void CRolePlayMake::kickPass( int kickSpeed ){
    Vector2D behindTheBall = wm->ball->pos + Vector2D( wm->ball->pos - pointToPass ).norm()*0.2;
    if( kickPassMode == KickPassFirst && agent->pos().dist(behindTheBall) > 0.01 ){
        finalTarget = wm->ball->pos;
        gotopoint->setRobot_Id(agent->id());
        gotopoint->setTargetpos(behindTheBall);
        gotopoint->setTargetdir(Vector2D(1.0, 0.0));
        gotopoint->setLookat(pointToPass);
        gotopoint->setSlowmode(true);
        gotopoint->setNoavoid(false);
        gotopoint->setAvoidpenaltyarea(true);
        gotopoint->setAvoidcentercircle(false);
        gotopoint->setBallobstacleradius(0.2);
        agent->action = gotopoint;
    }
    else{
        kickPassMode = KickPassSecond;
        if( kickPassCyclesWait > 4 && agent->pos().dist(finalTarget) > 0.01 ){
            agent->setKick(kickSpeed+2);
            gotopoint->setRobot_Id(agent->id());
            gotopoint->setTargetpos(finalTarget);
            gotopoint->setTargetdir(Vector2D(1.0, 0.0));
            gotopoint->setLookat(pointToPass);
            gotopoint->setSlowmode(true);
            gotopoint->setNoavoid(true);
            gotopoint->setAvoidpenaltyarea(true);
            gotopoint->setAvoidcentercircle(false);
            gotopoint->setBallobstacleradius(0);
            agent->action = gotopoint;
        }
        else
            kickPassCyclesWait++;
    }
}

void CRolePlayMake::execute() {
    cyclesExecuted++;
    if( wm->ball->inSight <= 0
        || !wm->ball->pos.valid()
        || !wm->field->marginedField().contains(wm->ball->pos)){
        wait->setRobot_Id(agent->id());
        agent->action = wait;
        return;
    }

    double region;
    QList <int> ourRelax;
    ourRelax.clear();
    ourRelax.append(wm->our.data->activeAgents);
    if( knowledge->goalie != NULL && ourRelax.contains(knowledge->goalie->self()->id)){
        ourRelax.removeOne(knowledge->goalie->self()->id);
    }
    for( int i=0 ; i<knowledge->defenseAgents.size() ; i++ ){
        if( ourRelax.contains(knowledge->defenseAgents.at(i)->self()->id) )
            ourRelax.removeOne(knowledge->defenseAgents.at(i)->self()->id);
    }
    QList <int> oppRelax;
    Vector2D target = knowledge->getEmptyPosOnGoal(wm->ball->pos , region , true , ourRelax, oppRelax);
    int kickSpeed = agent->kickSpeedValue(5 , false);

    if(!noKick && canScoreGoal() ){
        return;
    }

    if(gameState->ourKickoff() ){
        executeOurKickOff();

        return;
    }
    else if(gameState->theirPenaltyShootout()){
        theirPenaltyPositioning();
        return;

    }
    else if (gameState->ourPenaltyShootout()){
        debugger->debug(QString("st:%1").arg(!gameState->ourPenaltyShootout()),D_NADIA);
        executeOurPenaltyShootout();
        return;
    }
    else if(gameState->ourPenaltyKick()){
        debugger->debug(QString("st___:%1").arg(!gameState->ourPenaltyKick()),D_NADIA);
        executeOurPenalty();
        return;

    }

    if ( cyclesExecuted < cyclesToWait )
    {
        stopBehindBall(false);
        return;
    }

    if( noKick )
        return;

    if( kickMode == FixedPass ){
        debugger->debug("HERE" , D_MASOOD);
        setThrough(false);
        target = pointToPass;
        kickSpeed = chip ? agent->chipDistanceValue(agent->pos().dist(target),false) : agent->kickValueForDistance(agent->pos().dist(target) , 1.5);
        kick->setSlow(false);
        kick->setChip(chip);
        kickPass(kickSpeed);
        return;
    }
    else if( kickMode == FixedShoot ){
        target = pointToShoot;
        setChip(false);
        kickSpeed = agent->kickValueForDistance(agent->pos().dist(target) , 6);
        kick->setSlow(false);
        kick->setRobot_Id(agent->id());
        kick->setTarget(target);
        kick->setKickspeed(kickSpeed);
        kick->setWaitFrames(0);
        kick->setTolerance(0.06);
        kick->setAvoidpenaltyarea(true);
        agent->action = kick;
        debugger->debug("HERE2" , D_MASOOD);
    }

    debugger->debug("HERE3" , D_MASOOD);
    kick->setSlow(false);
    kick->setRobot_Id(agent->id());
    kick->setTarget(target);
    kick->setKickspeed(kickSpeed);
    kick->setWaitFrames(0);
    kick->setTolerance(0.06);
    kick->setAvoidpenaltyarea(true);
    agent->action = kick;
}

void CRolePlayMake::parse(QStringList params) {

    setKickToTheirDefense(false);
    setJustTurn(false);
    setSlow(false);
    setChip(false);
    setIndirectKhafan(false);
    setNoStop(false);
    setKickoffWing(false);
    setIndirectGoogooli(false);
    setChipIndirect(false);
    setThrough(false);
    setManualPassReceive(false);
    setAllowOneTouch(false);
    setAllowOnePass(false);
    setFollowSequence(false);
    setKickoffmode(false);
    setIndirectTiny(false);
    setChipInPenaltyArea(false);
    setShadowPass(false);
    setKhers(false);
    setShadowyPoint(false);
    setLongChip(false);
    setPassReceiver(-1);
    setChipToOppGoal(false);
    setSafeIndirect(false);
    setLocalAgentPassTarget("");
    setPassReceiver(-1);
    kick->setInterceptmode(false);
    for (int i=0;i<params.length();i++)
    {
        bool ok = false;
        int p = params[i].toInt(&ok);
        if (params[i].toLower()=="slow") setSlow(true);
        else if (params[i].toLower()=="chip") setChip(true);
        else if (params[i].toLower()=="indirectkhafan") setIndirectKhafan(true);
        else if (params[i].toLower()=="nostop") setNoStop(true);
        else if (params[i].toLower()=="wing") setKickoffWing(true);
        else if (params[i].toLower()=="indirectgoogooli") setIndirectGoogooli(true);
        else if (params[i].toLower()=="chipindirect") setChipIndirect(true);
        else if (params[i].toLower()=="through") setThrough(true);
        else if (params[i].toLower()=="nokick") setNoKick(true);
        else if (params[i].toLower()=="allowonetouch") setAllowOneTouch(true);
        else if (params[i].toLower()=="allowonepass") setAllowOnePass(true);
        else if (params[i].toLower()=="followsequence") setFollowSequence(true);
        else if (params[i].toLower()=="kickoff") setKickoffmode(true);
        else if (params[i].toLower()=="indirecttiny") setIndirectTiny(true);
        else if (params[i].toLower()=="chipinpenalty") setChipInPenaltyArea(true);
        else if (params[i].toLower()=="shadowpass") setShadowPass(true);
        else if (params[i].toLower()=="khers") setKhers(true);
        else if (params[i].toLower()=="kicktotheirdefense") setKickToTheirDefense(true);
        else if (params[i].toLower()=="justturn") setJustTurn(true);
        else if (params[i].toLower()=="longchip") setLongChip(true);
        else if (params[i].toLower()=="chiptogoal") setChipToOppGoal(true);
        else if (params[i].toLower()=="shadowypoint") setShadowyPoint(true);
        else if (params[i].toLower()=="safe") setSafeIndirect(true);
        else if (params[i].startsWith("@"))
        {
            localAgentPassTarget = params[i].right(params[i].length()-1);
        }
        else if (params[i].toLower()=="%") {
            setManualPassReceive(true);
            setPassReceiver(-5);
        }
        else if (ok) {
            setManualPassReceive(true);
            setPassReceiver(p);
        }
    }
}

double CRolePlayMake::progress() {
    if (agent->self()->ballComingSpeed()<-0.6)
        return 1.01;
    return 0.0;
}

void CRolePlayMake::generateFromConfig(CAgent *a) {
    agent = a;
}

CRolePlayMakeInfo::CRolePlayMakeInfo(QString _roleName) : CRoleInfo(_roleName) {

}

struct DistAgent {
    CAgent* agent;
    double dist;
};

bool operator < (const DistAgent& a, const DistAgent& b) {
    return a.dist < b.dist;
}

CAgent* CRolePlayMakeInfo::passReceiver(CAgent* self, int p, QList<int> passables) {
    QList<DistAgent> s;
    if (p==-5)
    {
        for (int i=self->id()+1;i<self->id()+wm->our.activeAgentsCount();i++)
        {
            int k = i%wm->our.activeAgentsCount();
            if ((knowledge->getAgent(k)->isVisible()) && (wm->our.active(k)->id != self->id())
                    )
            {
                return knowledge->getAgent(k);
            }
        }
    }
    for (int i=0;i<wm->our.activeAgentsCount();i++)
    {
        if ((knowledge->getAgent(i)->isVisible()) && (wm->our.active(i)->id != self->id())
            && passables.contains(i)
                )
        {
            DistAgent a;
            a.agent = knowledge->getAgent(i);
            a.dist = (a.agent->pos() - self->pos()).length();
            s.append(a);
        }
    }
    qSort(s.begin(), s.end());
    if (p>=0 && p<s.count())
        return s[p].agent;
    return NULL;
}

CAgent* CRolePlayMakeInfo::bestPassReceiver(bool indirect) {
    double bestW = -1.0,w;
    CAgent* best = NULL;
    for (int i=0;i<wm->our.activeAgentsCount();i++)
    {
        if ((knowledge->getAgent(i)->isVisible()) && (knowledge->getAgent(i)->skillName != CRolePlayMake::Name)
            //&& ((knowledge->getAgent(i)->canRecvPass || indirect))
            && (knowledge->getAgent(i)->skillName=="positioningPlan"))
        {
            double ang = 0, coming = 0;
            knowledge->onetouchablity(knowledge->getAgent(i)->id(), w, ang, coming);
            //            knowledge->goalVisiblity(knowledge->getAgent(i)->id(), w, policy()->PlayMaker_UnderEstimateTheirGoalie());
            if ((knowledge->getAgent(i)->skillName=="positioningPlan") && (knowledge->getAgent(i)->skill!=NULL))
            {
                if (static_cast<CRolePosition*> (knowledge->getAgent(i)->skill)->getPenalty())
                {
                    w *= 1.6;
                }
            }
            for(int j = 0; j < wm->opp.activeAgentsCount(); j++ )
            {
                QList<int> rs;
                rs.clear();
                if (wm->opp.active(j)->pos.dist( wm->our.active(i)->pos) <  1.0  || !knowledge->isPointClear( knowledge->getAgent(i)->pos(), wm->ball->pos, 3 * CRobot::robot_radius_old, true, rs, rs))
                    w*=0.1;
            }

            if (indirect)
                w = 1.0/(1.0+fabs(ang)/180.0);

            if (w>bestW) {bestW = w;best = knowledge->getAgent(i);}
        }
    }
    return best;
}

void CRolePlayMake::executeOurDirect() {

    kick->setSlow(true);


    double goal_p;
    QList<int> ourrelaxed;
    QList<int> opprelaxed;
    ourrelaxed.append(agent->self()->id);
    Vector2D tar = knowledge->getEmptyPosOnGoal(wm->ball->pos, goal_p, true, ourrelaxed, opprelaxed, 0.9, 1.0); ///calculate empty angle     //p*empty_width + (1-p)*goalwidth
    drawer->draw(Circle2D(Vector2D(0,0),1.2),0,360,"pink",true);
    drawer->draw(QString("prob : %1").arg(goal_p));
    if(goal_p > 0.3)
    {
        kick->setRobot_Id(agent->id());
        kick->setTarget(tar);
        kick->setChip(false);
        kick->setKickspeed(8.0);
        //        kick->setTolerance(0.3);
        agent->action = kick;
    }
    else if ( chipToOppGoal)
    {
        kick->setRobot_Id(agent->id());
        kick->setTarget(wm->field->oppGoal());
        kick->setChip(true);
        kick->setKickspeed(agent->chipDistanceValue(wm->ball->pos.dist(wm->field->oppGoal())-0.3 , false));
        kick->setTolerance( 0.3);
        agent->action = kick;
        return;
    }
    else if( chipPenaltyArea)
    {
        kick->setRobot_Id(agent->id());
        kick->setTarget(wm->field->oppGoal());//Vector2D(wm->field->oppPenalty().x + 0.3, wm->field->oppPenalty().y));//(wm->field->oppGoal() + wm->field->oppPenalty())/2.0);
        kick->setChip( true);
        if ( wm->ball->pos.x < 0 )  // comment in telecomp
        {
            debugger->debug("heyyyyyyyyyaaaaaaaaaaaaaaaaaaa",D_ERROR,"red");
            kick->setKickspeed(agent->chipDistanceValue(wm->ball->pos.dist(wm->field->oppGoal())-0.3 , false));
        }
        else
            kick->setKickspeed(agent->chipDistanceValue(wm->ball->pos.dist((wm->field->oppGoal()+wm->field->oppPenalty())*0.5) , false));
        kick->setKickspeed(agent->chipDistanceValue(wm->ball->pos.dist((wm->field->oppGoal()+wm->field->oppPenalty())*0.5) , false));
        kick->setSlow(true);
        kick->setInterceptmode(false);
        kick->setTolerance( 0.1);
        agent->action = kick;
        return;
    }
    else slow = true;
    double w;
    if (conf.OurDirectChipToGoal ||
        (conf.OurDirectChipToGoalInOurField
         && CGameConditions::check("ballinside",QStringList() << "field1stquarter")))
    {
        drawer->draw("Chip it: direct", Vector2D(-1,-1), "black");
        kick->setRobot_Id(agent->id());
        kick->setTolerance(0.2);
        kick->setTarget(knowledge->goalVisiblity(agent->id(), w, 1.0));
        kick->setKickspeed(agent->chipDistanceValue(wm->ball->pos.dist((wm->field->oppGoal()+wm->field->oppPenalty())*0.5) , true));
        kick->setSpin(true);
        kick->setChip(true);
        kick->setAutoChipSpeed(false);
        kick->setSlow(true);
        agent->action = kick;
        /*TEMP*/gotoball->resetI();
        return;
    }
    else {
        executeDefault();
    }
}

void CRolePlayMake::executeOurIndirect() {

    if (indirectTiny) {
        if( !knowledge->executingPlays.isEmpty()) {
            if ( knowledge->executingPlays.at(0) == "ourindirecttinyplus") {
                Rect2D checkRectTiny(Vector2D( wm->ball->pos.x, wm->ball->pos.y + 1.5), Vector2D( wm->ball->pos.x + 0.6, wm->ball->pos.y));
                for ( int i = 0; i < wm->opp.activeAgentsCount(); i++)
                {
                    if( checkRectTiny.contains(wm->opp[i]->pos))
                    {
                        indirectTiny = false;
                        indirect = true;
                        return;
                    }
                }
            }
        }
        debugger->debug("tinying",D_SEPEHR);
        kick->setRobot_Id(agent->id());
        if ( wm->ball->pos.y > 0 )
            kick->setTarget( Vector2D(wm->ball->pos.x - 0.4, wm->ball->pos.y - 0.2));
        else
            kick->setTarget( Vector2D(wm->ball->pos.x - 0.4, wm->ball->pos.y + 0.2));

        for( int i =0; i < wm->our.activeAgentsCount();i++)
        {
            if (knowledge->getAgent(i)->skill != NULL)
            {
                if (knowledge->getAgent(i)->skill->getName() == "positioningPlan")
                {
                    if( static_cast<CRolePosition*>( knowledge->getAgent(i)->skill)->getIndirectTiny())
                        kick->setTarget(knowledge->getAgent(i)->self()->getKickerPos(0.01));
                }
            }
        }
        kick->setSlow(true);
        kick->setKickspeed(agent->kickSpeedValue(2.7, true));
        kick->setChip(false);
        kick->setTolerance(0.1);
        kick->setSpin(true);
        agent->action = kick;
        /*TEMP*/gotoball->resetI();
        if ((agent->self()->ballComingSpeed() < -0.45))/// ||
            //fabs(knowledge->currentTime()-knowledge->getLastTimeGameStateChanged()) > 6.0 )
        {
            //            CKnowledge::State s = CKnowledge::Start;
            //            knowledge->setgameMode(s);
            //            debug("toop raft ! ",D_SEPEHR,"orange");
            indirect = false;
            indirectTiny = false;
            slow = false;
            kick->setInterceptmode(true);
            executeDefault();
        }
        return;
    }
    else if (indirectKhafan)
    {
        kick->setRobot_Id(agent->id());
        kick->setSlow(true);
        kick->setTolerance(0.1);
        kick->setChip(true);
        kick->setSpin(true);
        bool chipIt=false;
        if ((conf.OurIndirectChipToGoalInOurField && gameState->ourIndirectKick())
            ||
            (conf.OurDirectChipToGoalInOurField && gameState->ourIndirectKick())
                )
        {
            if (CGameConditions::check("ballinside",QStringList() << "field1stquarter"))
            {
                chipIt = true;
            }
        }
        kick->setRobot_Id(agent->id());
        kick->setTarget((wm->field->oppGoal()-agent->pos()).norm()*1.5 + agent->pos());
        kick->setTarget(wm->field->oppPenalty());
        kick->setKickspeed(agent->chipDistanceValue(wm->ball->pos.dist(wm->field->oppPenalty()) , true));
        kick->setSlow(true);
        kick->setChip(true);
        kick->setSpin(false);
        agent->action = kick;
        if ((agent->self()->ballComingSpeed() < -0.45))// ||
            //                fabs(knowledge->currentTime()-knowledge->getLastTimeGameStateChanged()) > 6.0 )
        {
            //            CKnowledge::State s = CKnowledge::Start;
            //            knowledge->setgameMode(s);
            //            debug("toop parid ! ",D_SEPEHR,"orange");
            indirect = false;
            indirectKhafan = false;
            slow = false;
            kick->setInterceptmode(true);
            executeDefault();
        }
        return;
    }
    else if(chipPenaltyArea)
    {
        kick->setRobot_Id(agent->id());
        double ofset;
        if ( wm->ball->pos.y < 0)
            ofset = 0.6;
        else
            ofset = -0.6;
        kick->setTarget(wm->field->oppPenalty());//Vector2D(Vector2D((wm->field->oppGoal() + wm->field->oppPenalty())/2.0).x,Vector2D((wm->field->oppGoal() + wm->field->oppPenalty())/2.0).y+ofset));
        kick->setChip( true);
        kick->setKickspeed(agent->chipDistanceValue(wm->ball->pos.dist(wm->field->oppPenalty())-0.3 , false));
        if ( wm->ball->pos.x < 0 )  // comment in telecomp
        {
            debugger->debug("heyyyyyyyyyaaaaaaaaaaaaaaaaaaa",D_ERROR,"red");
            kick->setKickspeed(agent->chipDistanceValue(wm->ball->pos.dist(wm->field->oppPenalty())-0.3 , false));
        }
            //kick->setKickSpeed( 22);
        else
            kick->setKickspeed(agent->chipDistanceValue(wm->ball->pos.dist(wm->field->oppPenalty()) , false));
        kick->setTolerance( 0.1);
        agent->action = kick;
        if ((agent->self()->ballComingSpeed() < -0.45))// ||
            //                fabs(knowledge->currentTime()-knowledge->getLastTimeGameStateChanged()) > 6.0 )
        {
            //            CKnowledge::State s = CKnowledge::Start;
            //            knowledge->setgameMode(s);
            //            debug("toop parid ! ",D_SEPEHR,"orange");
            indirect = false;
            chipPenaltyArea = false;
            slow = false;
            kick->setInterceptmode(true);
            executeDefault();
        }
        return;
    }
    else if (longchip)
    {
        kick->setRobot_Id(agent->id());
        if (wm->ball->pos.y > 0)
            kick->setTarget(wm->field->oppGoalR());
            //			kick->setTarget( wm->field->OppPenalty);
        else
            kick->setTarget(wm->field->oppGoalL());
        //        kick->setTarget( wm->field->oppGoal());
        kick->setChip( true);
        kick->setSlow(true);
        // long chip dist
        kick->setKickspeed( agent->chipDistanceValue(wm->ball->pos.dist(wm->field->oppPenalty()),true));
        kick->setSpin(true);
        kick->setTolerance( 0.3);
        agent->action = kick;
        if ( wm->ball->vel.length() > 0.3)
            knowledge->setRushInPenaltyArea( true);
        return;
    }
    else if (chipToOppGoal)
    {
        kick->setRobot_Id(agent->id());
        kick->setTarget(wm->field->oppGoal());
        kick->setSlow(true);
        kick->setChip( true);
        kick->setKickspeed(agent->chipDistanceValue(wm->ball->pos.dist(wm->field->oppGoal()) , false));
        kick->setAutoChipSpeed(false);
        kick->setTolerance( 0.3);
        agent->action = kick;
        return;
    }
    else if (kickToTheirDefense)
    {
        kickToDefense->agents.clear();
        kickToDefense->agents.append(agent);
        kickToDefense->execute();
        return;
    }
    else if (justTurn)
    {
        gotoball->setRobot_Id(agent->id());
        gotoball->setSlow(false);
        gotoball->setRotate(true);
        gotoball->setThroughMode(false);
        gotoball->setFinalVel(0);
        gotoball->setGoal(wm->field->oppGoal());
        for (int i=0;i<wm->our.activeAgentsCount();i++)
        {
            if (knowledge->getAgent(i)->localName == localAgentPassTarget)
            {
                if (knowledge->getAgent(i)->isVisible())
                {
                    gotoball->setGoal(knowledge->getAgent(i)->pos());
                }
            }
        }
        gotoball->execute();
        return;
    }


    debugger->debug("indirect ! ",D_SEPEHR,"orange");
    indirect = true;
    slow = true;
    if (policy()->OurIndirect_NoPass() || conf.OurIndirectChipToGoal || chipIndirect)
    {

        kick->setRobot_Id(agent->id());
        double w;
        kick->setTolerance(0.2);
        kick->setTarget(knowledge->goalVisiblity(agent->id(), w, 1.0));
        Vector2D hoyBechipInja;
        if ( chipIndirect){
            //			if ( wm->ball->pos.y > 0)
            hoyBechipInja = Vector2D(wm->field->oppGoal().x - 0.30, 0);
            kick->setTarget(hoyBechipInja) ;

        }
        kick->setKickspeed(agent->chipDistanceValue(wm->ball->pos.dist(hoyBechipInja) - 0.3, true));
        kick->setSpin(false);
        kick->setChip(conf.OurIndirectChipToGoal || chipIndirect);
        kick->setAutoChipSpeed(false);
        //        kick->setTurn(true);
        kick->setSlow(true);
        bool waitForPositioners = chipIndirect;
        //		if ( chipIndirect && (knowledge->cornerChipPhase & 1 || knowledge->cornerChipPhase & 2))
        //			waitForPositioners = false;
        if ( chipIndirect && (knowledge->cornerChipPhase & 8 || knowledge->cornerChipPhase & 4))
            waitForPositioners = false;
        if (!waitForPositioners)
            agent->action = kick;
        else
            stopBehindBall();
        /*TEMP*/gotoball->resetI();
        knowledge->chipPassSender = agent->id();
        knowledge->sentChipPass = true;
    }
    else if ( safeIndirect && knowledge->currentTime() - knowledge->getLastTimeGameStateChanged()< 3.6)
    {
        bool waitforP2 = false;
        if ( safeIndirect && knowledge->currentTime() - knowledge->getLastTimeGameStateChanged()< 3.6)
            waitforP2 = true;
        if( waitforP2) {
            wait->setRobot_Id(agent->id());
            agent->action = wait;
        }
    }
    else {
        executeDefault();
    }
}

void CRolePlayMake::resetOffPlays() {
    indirect = false;
    setIndirectTiny( false);
    setIndirectGoogooli(false);
    setIndirectKhafan(false);
    setChipInPenaltyArea(false);
    setChipIndirect(false);
    direct = false;
    setChipInPenaltyArea(false);
}

void CRolePlayMake::resetPlayMake() {
    cyclesToWait = 0;
    cyclesExecuted = 0;
    kickPassMode = KickPassFirst;
    kickPassCyclesWait = 0;
}
