////
//// Created by parsian-ai on 9/22/17.
////

#include <parsian_ai/coach.h>


CCoach::CCoach(Agent**_agents)
{
    goalieTrappedUnderGoalNet = false;
    inited = false;
    agents = _agents;
    lastAssignCycle = -10;
    lastBallVelPM = Vector2D(0,0);
    lastBallPos = Vector2D(0,0);
    passPos = Vector2D(0,0);
    passPlayMake = false;
    first = true;

    ///////////////////////////////////
    goalieTimer.start();
    ////////////////////intentions
    playOnExecTime.start();
    intentionTimePossession.start();
    playMakeIntention.start();

    possessionIntentionInterval = 200;
    playOnTime = 2000;
    playMakeIntentionInterval = 300;
    playMakeTh = 0.3;


    // Old Plays
    ourPenalty          = new COurPenalty;
    theirDirect         = new CTheirDirect;
    theirKickOff        = new CTheirKickOff;
    theirPenalty        = new CTheirPenalty;
    theirIndirect       = new CTheirIndirect;
    //    ourBallPlacement    = new COurBallPlacement;
    //    halfTimeLineup    = new CHalftimeLineup;
    theirBallPlacement  = new CTheirBallPlacement;


    // New Plays
    ourPlayOff          = new CPlayOff;
    dynamicAttack       = new CDynamicAttack();

    //Stop
    stopPlay            = new CStopPlay();

    for( int i=0 ; i<_MAX_NUM_PLAYERS ; i++ ){
        stopRoles[i] = new CRoleStop(nullptr);
    }

    lastDefenseAgents.clear();

    defenseTimeForVisionProblem[0].start();
    defenseTimeForVisionProblem[1].start();
    transientFlag = false;
    trasientTimeOut.start();
    translationTimeOutTime = 2000;
    exeptionPlayMake = nullptr;
    exeptionPlayMakeThr = 0;

    //    m_planLoader = new CLoadPlayOffJson(QDir::currentPath() + QString("/playoff"));
    goalieAgent = nullptr;
    firstPlay = true;
    firstIsFinished = false;
    preferedDefenseCounts = 2;
    overDefThr = 0;
    selectedPlay = stopPlay;
    for (int &i : faultDetectionCounter) i = 0;
}

CCoach::~CCoach()
{
    delete stopPlay;
    //    savePostAssignment();
    //    saveLFUReapeatData(LFUList);
}

//void CCoach::saveGoalie()
//{
//     debugger->debug((QString("goalie under net timer : %1").arg(goalieTimer.elapsed())), D_MHMMD);
//
//    if(goalieTimer.elapsed() > 100 )
//    {
//        if(goalieTimer.elapsed() <200)
//        {
//            knowledge->getAgent(preferedGoalieAgent)->setRobotAbsVel(2,0,0);
//        }
//        else if(goalieTimer.elapsed() <300)
//        {
//            knowledge->getAgent(preferedGoalieAgent)->setRobotAbsVel(-2,0,0);
//
//        }
//        else if(goalieTimer.elapsed() < 400)
//        {
//            knowledge->getAgent(preferedGoalieAgent)->setRobotAbsVel(0,2,0);
//
//        }
//        else if(goalieTimer.elapsed() < 500)
//        {
//            knowledge->getAgent(preferedGoalieAgent)->setRobotAbsVel(0,-2,0);
//
//        }
//        else
//        {
//            knowledge->getAgent(preferedGoalieAgent)->setRobotAbsVel(1,1,5);
//
//        }
//    }
//    return;
//    if (goalieTrappedUnderGoalNet && knowledge->goalie != NULL)
//    {
//        knowledge->goalie->setRobotAbsVel(2, 2, 4);
//    }
//}

bool isNan(double x)
{
    return (QString("%1").arg(x).trimmed().toLower() == "nan");
}

void CCoach::checkGoalieInsight()
{
    goalieTrappedUnderGoalNet = false;
    /////////////////////////////////////////////////// new method by Don Mhmmd
    if(goalieAgent != nullptr)
    {
        //        if (goalieAgent->isVisible())
        //        {
        //            goalieTimer.restart();
        //        }
        debugger->debug("inja miad",D_MHMMD);

    }
}



void CCoach::decidePreferedDefenseAgentsCountAndGoalieAgent() {

    missMatchIds.clear();
    if(first)
    {
        if(wm->our.activeAgentsCount() != 0)
        {
            robotsIdHist.clear();
            for(int i = 0 ; i < wm->our.activeAgentsCount() ; i++)
            {
                robotsIdHist.append(wm->our.active(i)->id);
            }
            first = false;
        }
    }

    if(gameState->isPlayOff() || !gameState->canMove())
    {
        if(wm->our.activeAgentsCount() != 0u)
        {
            robotsIdHist.clear();
            for(int i = 0 ; i < wm->our.activeAgentsCount() ; i++)
            {
                robotsIdHist.append(wm->our.active(i)->id);
            }
        }

    }
    if(wm->our.activeAgentsCount() > 6)
    {
        missMatchIds.clear();
        for(int i = 0 ; i < wm->our.activeAgentsCount() ; i++)
        {
            for(int k = 0 ; k < robotsIdHist.count() ; k++)
            {
                if(robotsIdHist.at(k) == wm->our.active(i)->id)
                {
                    break;
                }
                if(k == robotsIdHist.count() - 1)
                {
                    missMatchIds.append(wm->our.active(i)->id);
                }
            }
        }
    }

    int agentsCount = wm->our.data->activeAgents.count();
    if (goalieAgent != nullptr) {
        if (goalieAgent->isVisible()) {
            agentsCount--;
        }
    }

    if (conf.GoalieFromGUI) {
        preferedGoalieAgent = conf.Goalie;
    } else {
        preferedGoalieAgent = wm->our.data->goalieID;
    }

    // handle stop
    if (gameState->isPlayOff()) {
        if (wm->ball->pos.x < 0){
            preferedDefenseCounts = agentsCount - 1;

        } else if (wm->ball->pos.x > 1) {
            preferedDefenseCounts = conf.Defense;

        }

    } else if (gameState->isPlayOn()) {
        if (transientFlag) {
            if (trasientTimeOut.elapsed() > 1000 && !wm->field->isInOurPenaltyArea(wm->ball->pos)) {
                preferedDefenseCounts = static_cast<int>(max(0, agentsCount - missMatchIds.count() - 1));

            } else {
                preferedDefenseCounts = agentsCount - missMatchIds.count();

            }

        } else { // PLAYON
            bool oppsAttack = false;

            for(int i = 0 ; i < wm->opp.activeAgentsCount() ; i++) {
                if(wm->opp.active(i)->pos.x <= 0) oppsAttack = true;
            }

            if (agentsCount == 1) {
                preferedDefenseCounts = 0; // just one playmake

            } else if (agentsCount == 2) {
                preferedDefenseCounts = 1; // one playmake and one defense

            } else {
                if (!oppsAttack || checkOverdef()) {
                    preferedDefenseCounts = 1;

                } else {
                    preferedDefenseCounts = 2;

                }

            }
        }
    } else if (gameState->ourRestart()) {
        if (wm->ball->pos.x < -1)  {
            preferedDefenseCounts = (checkOverdef()) ? 1 : 2;

        } else if (wm->ball->pos.x > -.5) {
            preferedDefenseCounts = 0;
        }

    } else if (gameState->theirRestart()) {
        if (gameState->theirKickoff()) {
            preferedDefenseCounts = 2;
        } else {
            preferedDefenseCounts = std::max(agentsCount - missMatchIds.count() - 1, 0);
        }
    } else {
        debugger->debug("UNKNOWN STATE", D_ERROR, QColor(Qt::red));
    }

    //    if(policy()->Formation_StrictFormation()) {
    //        preferedDefenseCounts = policy()->Formation_Defense();
    //    }

    if(gameState->halfTimeLineUp()){
        preferedGoalieAgent = -1;
        preferedDefenseCounts = 0;
    }

    if (gameState->penaltyShootout()) {
        preferedDefenseCounts = 0;
    }
    preferedDefenseCounts = 0;
    preferedGoalieAgent = -1;
    lastPreferredDefenseCounts = preferedDefenseCounts;
}

void CCoach::calcDesiredMarkCounts()
{

    int agentsCount = wm->our.data->activeAgents.count();
    if (goalieAgent != nullptr) {
        if (goalieAgent->isVisible()) {
            agentsCount--;
        }
    }

    // used in playoff without counting goalie

    if( agentsCount == 2 ) {
        desiredDefCount = 0;
    } else if( agentsCount == 3 ) {

        if(toBeMopps.count() == 0) {
            desiredDefCount = 1;
        } else if(toBeMopps.count() <=2 ) {
            desiredDefCount = 2 - toBeMopps.count();
        } else {
            desiredDefCount = 0;
        }


    } else if( agentsCount == 4  ){

        if(toBeMopps.count() == 0)
            desiredDefCount = 2;
        else if(toBeMopps.count() <=2 )
        {
            desiredDefCount = 2 - toBeMopps.count();
        }
        else
        {
            desiredDefCount = 0;
        }

    } else if( agentsCount == 5 ){

        if(toBeMopps.count() == 0)
            desiredDefCount = 2;
        else if(toBeMopps.count() <=3 )
        {
            desiredDefCount = 3 - toBeMopps.count();
        }
        else
        {
            desiredDefCount = 0;
        }

    }

    else if( agentsCount == 6 ) {
        if(toBeMopps.count() <= 1)
            desiredDefCount = 1;
        else if(toBeMopps.count() <=3 )
        {
            desiredDefCount = 3 - toBeMopps.count();
        }
        else
        {
            desiredDefCount = 0;
        }
    }

}


void CCoach::assignGoalieAgent( int goalieID ){
    QList<int> ids = wm->our.data->activeAgents;
    goalieAgent = nullptr;
    if( ids.contains(goalieID) ){
        goalieAgent = agents[goalieID];
    }
}
BallPossesion CCoach::isBallOurs()
{
    int oppNearestToBall = -1, ourNearestToBall = -1;
    double oppNearestToBallDist = 100000, ourNearestToBallDist = 100000;
    double oppIntersetMin       = 100000, ourIntersetMin       = 100000;
    Vector2D  ballPos = wm->ball->pos;
    Vector2D  ballVel = wm->ball->vel;
    Segment2D ballPath(ballPos, ballPos + ballVel * 2);
    Segment2D oppPath;
    Vector2D dummy1, dummy2;
    int oppIntersectAgent = -1, ourIntersectAgent = -1;
    BallPossesion decidePState;
    QList <int> ourAgents;
    QList <CRobot*> oppAgents; // TODO : What the Heck !?

    ////////////////// our
    ourAgents = wm->our.data->activeAgents;
    if(goalieAgent != nullptr) {
        ourAgents.removeOne(goalieAgent->id());
    }

    for(int i = 0 ; i<defenseAgents.count() ; i++){
        ourAgents.removeOne(defenseAgents[i]->id());
    }

    for (int i = 0 ; i < oppAgents.count() ; i++) {
        if (oppAgents.at(i)->vel.length() < 0.5) {
            double oppDist = oppAgents[i]->pos.dist(ballPos);
            if(oppDist < oppNearestToBallDist){
                oppNearestToBallDist = oppDist;
                oppNearestToBall = oppAgents[i]->id;
            }

            if(wm->ball->vel.length() > 0.5){
                if(Circle2D(oppAgents[i]->pos , 0.12).intersection(ballPath,&dummy1,&dummy2)) {
                    if(oppAgents[i]->pos.dist(ballPos) < oppIntersetMin) {
                        oppIntersetMin    = oppAgents[i]->pos.dist(ballPos);
                        oppIntersectAgent = oppAgents[i]->id;
                    }
                }
            }

        } else {
            oppPath.assign(oppAgents[i]->pos, oppAgents[i]->pos + oppAgents[i]->vel);
            double oppDist = oppPath.nearestPoint(ballPos).dist(ballPos);
            if(oppDist < oppNearestToBallDist){
                oppNearestToBall = oppAgents[i]->id;
                oppNearestToBallDist = oppDist;
            }

            if(wm->ball->vel.length() > 0.5){
                if(oppPath.intersection(ballPath).isValid()){
                    oppDist = oppAgents[i]->pos.dist(ballPos);
                    if(oppDist < oppIntersetMin){
                        oppIntersetMin = oppDist;
                        oppIntersectAgent = oppAgents[i]->id;
                    }
                }
            }
        }
    }

    ///////////////////////////
    ///////////////////////////
    ///
    ///

    if(oppNearestToBall == -1 || (wm->field->isInOurPenaltyArea(wm->ball->pos) && wm->ball->vel.length() < 0.5)) {
        decidePState =  BallPossesion ::WEHAVETHEBALL;
    } else if(oppIntersectAgent != -1 && ourIntersectAgent == -1 && wm->ball->vel.length() > 1) {
        decidePState = BallPossesion::WEDONTHAVETHEBALL;
    } else if(oppIntersectAgent != -1 && ourIntersectAgent != -1 && (ourIntersetMin > oppIntersetMin - 0.1) && wm->ball->vel.length() > 0.5) {
        decidePState = BallPossesion::WEDONTHAVETHEBALL;
    } else if(oppNearestToBallDist <= ourNearestToBallDist) {
        decidePState = BallPossesion::WEDONTHAVETHEBALL;
    } else if(ourNearestToBallDist < 0.3 && oppNearestToBallDist > ourNearestToBallDist){
        decidePState = BallPossesion::WEHAVETHEBALL;
    } else if(oppNearestToBallDist > ourNearestToBallDist + 0.5){
        decidePState = BallPossesion::WEHAVETHEBALL;
    } else if(ourNearestToBallDist < oppNearestToBallDist && ballPos.x < 0.1){
        decidePState = BallPossesion::SOSOTHEIR;
    } else if(ourNearestToBallDist < oppNearestToBallDist && ballPos.x >= 0.1){
        decidePState = BallPossesion::SOSOOUR;
    } else if(oppIntersectAgent == -1 && ourIntersectAgent != -1 && wm->ball->vel.length() > 0.7) {
        decidePState = BallPossesion::WEHAVETHEBALL;
    } else {
        decidePState = BallPossesion::SOSOTHEIR;
    }

    if (decidePState == BallPossesion::WEHAVETHEBALL) {
        playOnExecTime.restart();
    }

    if(lastBallPossesionState == BallPossesion::WEHAVETHEBALL && (decidePState == BallPossesion::WEDONTHAVETHEBALL || decidePState == BallPossesion::SOSOTHEIR || decidePState == BallPossesion::SOSOOUR) && playOnExecTime.elapsed() < playOnTime) {
        decidePState = BallPossesion::WEHAVETHEBALL;
    }



    if(oppNearestToBall >= 0) {
        Circle2D oppNearestDribblerArea(wm->opp[oppNearestToBall]->pos + wm->opp[oppNearestToBall]->dir.norm()*0.1 , 0.15);
        drawer->draw(oppNearestDribblerArea,QColor(Qt::red));
        if(oppNearestDribblerArea.contains(ballPos) && ourNearestToBallDist > 0.3) {
            decidePState = BallPossesion::WEDONTHAVETHEBALL;
        } else if( oppNearestDribblerArea.contains(ballPos) ) {
            if(wm->ball->pos.x >= 0.1) {
                decidePState = BallPossesion::SOSOOUR;
            } else {
                decidePState = BallPossesion::SOSOTHEIR;
            }
        }
    }

    if(decidePState != lastBallPossesionState) {
        if(intentionTimePossession.elapsed() > possessionIntentionInterval) {
            intentionTimePossession.restart();
        } else {
            decidePState = lastBallPossesionState;
        }
    }


    ////////////      ///////////
    ////////////      ///////////
    //// NEW BALL POSSESSION ////
    double temp = wm->ball->pos.x + wm->ball->vel.x * 1;

    if(temp > 0.5) {
        decidePState = BallPossesion::WEHAVETHEBALL;
    } else if (temp < 0.1){
        decidePState = BallPossesion::WEDONTHAVETHEBALL;
    } else {
        decidePState = lastBallPossesionState;
    }

    if (wm->field->isInOurPenaltyArea(wm->ball->pos)
            &&  wm->ball->vel.length() < 0.1) {
        decidePState = BallPossesion::SOSOTHEIR;
    }

    if (wm->field->isInOppPenaltyArea(wm->ball->pos)
            && wm->ball->vel.length() < 0.1) {
        decidePState = BallPossesion::SOSOOUR;
    }

    lastBallPossesionState = decidePState;

    return decidePState;
}


void CCoach::assignDefenseAgents(int defenseCount){


    QList<int> ids = wm->our.data->activeAgents;
    if( goalieAgent != nullptr) {
        ids.removeOne(goalieAgent->id());
    }
    if(playmakeId != -1) {
        ids.removeOne(playmakeId);
    }

    selectedPlay->defensePlan.fillDefencePositionsTo(defenseTargets);
    double nearestDist;
    int nearestRobot = -1;

    defenseAgents.clear();
    for(int i = 0 ; i < defenseCount ; i++) {
        nearestDist = 1000000;
        for(int j = 0 ; j < ids.count() ; j++) {
            if (!agents[ids[j]]->changeIsNeeded) {
                wm->our[ids[j]]->pos;
                if (wm->our[ids[j]]->pos.dist(defenseTargets[i]) < nearestDist) {
                    nearestDist = wm->our[ids[j]]->pos.dist(defenseTargets[i]);
                    nearestRobot =  ids[j];
                }
            }
        }
        if(nearestRobot>=0) {
            defenseAgents.append(agents[nearestRobot]);
            ids.removeOne(nearestRobot);
        }

    }

    lastDefenseAgents.clear();
    lastDefenseAgents.append(defenseAgents);
    //    defenseAgents.clear();
}
bool CCoach::isBallcollide(){
    // TODO : change this :P
    Circle2D dummyCircle;
    Vector2D sol1,sol2;
    Segment2D ballPath(wm->ball->pos,wm->ball->pos+wm->ball->vel);
    debugger->debug("ball is colliding" , D_AHZ);
    for(int i = 0 ; i < wm->our.activeAgentsCount() ; i++){
        dummyCircle.assign(wm->our.active(i)->pos,0.08);
        if((dummyCircle.intersection(ballPath,&sol1,&sol2) != 0) && wm->our.active(i)->pos.dist(wm->ball->pos) < 0.14 && fabs((wm->ball->vel - lastBallVel).length()) > 0.5){
            lastBallVel = wm->ball->vel;
            return true;
        }
        if(wm->ball->vel.length() < 0.5 && wm->our.active(i)->pos.dist(wm->ball->pos) < 0.13) {
            lastBallVel = wm->ball->vel;
            return true;
        }
    }
    lastBallVel = wm->ball->vel;
    return false;
}

void CCoach::virtualTheirPlayOffState()
{
    States currentState;
    currentState = gameState->getState();
    if(lastState == States::TheirDirectKick || lastState == States::TheirIndirectKick /*|| lastState == States::TheirKickOff*/) {
        if(currentState == States::PlayOn){
            transientFlag = true;
        }
    }

    if(! transientFlag){
        trasientTimeOut.restart();
    }

    if(trasientTimeOut.elapsed() >= translationTimeOutTime) {
        transientFlag = false;
    }

    if(wm->ball->pos.x >= 1) {
        transientFlag = false;
    }

    if(isBallcollide() && 0){ // TODO : till we fix function && 0
        transientFlag = false;
    }
    PDEBUG("TS flag:", transientFlag, D_AHZ);
    lastState  = currentState;

}

void CCoach::decideDefense(){
    assignGoalieAgent(preferedGoalieAgent);
    assignDefenseAgents(preferedDefenseCounts);
    if( gameState->theirPenaltyKick() ){
        defenseAgents.clear();
        selectedPlay->defensePlan.initGoalKeeper(goalieAgent);
        selectedPlay->defensePlan.initDefense(defenseAgents);
        selectedPlay->defensePlan.execute();
    } else {
        selectedPlay->defensePlan.initGoalKeeper(goalieAgent);
        selectedPlay->defensePlan.initDefense(defenseAgents);
        selectedPlay->defensePlan.execute();
        selectedPlay->defensePlan.debugAgents("Defense");
    }
}


double CCoach::findMostPossible(Vector2D agentPos)
{
    QList<int> tempObstacles;
    QList <Circle2D> obstacles;
    obstacles.clear();
    for(int i = 0 ; i < wm->opp.activeAgentsCount() ; i++)
    {
        obstacles.append(Circle2D(wm->opp.active(i)->pos,0.1));
    }

    for(int i = 0 ; i < wm->our.activeAgentsCount() ; i++)
    {
        if(wm->our.active(i)->id != playmakeId)
            obstacles.append(Circle2D(wm->our.active(i)->pos,0.1));
    }
    double prob,angle,biggestAngle;

    CKnowledge::getEmptyAngle(*wm->field,agentPos-(wm->field->oppGoal()-agentPos).norm()*0.15,wm->field->oppGoalL(),wm->field->oppGoalR(), obstacles, prob, angle, biggestAngle);


    return prob;
}

void CCoach::updateAttackState()
{
    Polygon2D robotCritArea;
    double    safeRegion= 1   ;
    double    critLenth = 0.4 ;
    double    critThrsh = 0.95;
    double    critAng   = 30  ;
    CRobot    *oppNearest;
    if(wm->opp.activeAgentsCount() > 0) {
        //        int id = CKnowledge::getNearestRobotToPoint(wm->opp, wm->ball->pos);
        //        ROS_INFO_STREAM(id);
        //        oppNearest = wm->opp[id];
        ourAttackState = SAFE;
        return;

    }
    else
    {
        ourAttackState = SAFE;
        return;
    }
    QList<int> ids;
    Segment2D oppNearestPath(oppNearest->pos,oppNearest->pos + oppNearest->vel);
    ids = wm->our.data->activeAgents;
    //    ourNearestAgent = knowledge->getAgent(knowledge->getNearestAgentToPoint(wm->ball->pos,&ids));
    if (playmakeId != -1) {
        CRobot* PMA = wm->our[playmakeId];
        if(PMA != nullptr)
        {
            if(lastASWasCritical)
            {
                robotCritArea.addVertex(PMA->pos + Vector2D(0, 0.8));
                robotCritArea.addVertex(PMA->pos + Vector2D(1.1, 0));
                robotCritArea.addVertex(PMA->pos - Vector2D(0, 0.8));
                robotCritArea.addVertex(PMA->pos - Vector2D(0.5, 0));
                //                robotCritArea.addVertex(PMA->pos + PMA->dir.norm() * critL + PMA->dir.norm().rotate(critA )* critL);
                //                robotCritArea.addVertex(PMA->pos + PMA->dir.norm() * critL + PMA->dir.norm().rotate(-critA)* critL);
            } else {
                robotCritArea.addVertex(PMA->pos);
                robotCritArea.addVertex(PMA->pos + Vector2D(0, 0.7));
                robotCritArea.addVertex(PMA->pos + Vector2D(1, 0));
                robotCritArea.addVertex(PMA->pos - Vector2D(0, 0.7));
            }
        }
    }
    ROS_INFO_STREAM("Z");


    drawer->draw(robotCritArea,QColor(Qt::cyan));

    if(robotCritArea.contains(oppNearest->pos)){
        ourAttackState = CRITICAL;
        debugger->debug(QString("Attack: critical"),D_MHMMD);
    }
    else if(oppNearestPath.nearestPoint(wm->ball->pos).dist(wm->ball->pos) >= safeRegion) {
        ourAttackState = SAFE;
        debugger->debug(QString("Attack: safe"),D_MHMMD);
    }
    else {
        ourAttackState = FAST;
        debugger->debug(QString("Attack: fast"),D_MHMMD);
    }

    lastASWasCritical = (ourAttackState == CRITICAL);

}
void CCoach::choosePlaymakeAndSupporter(bool defenseFirst)
{
    QList<int> ourPlayers = wm->our.data->activeAgents;
    if( ourPlayers.contains(preferedGoalieAgent) != nullptr) {
        ourPlayers.removeOne(preferedGoalieAgent);
    }

//    if(defenseFirst){
//        for (auto defenseAgent : defenseAgents) {
//            if ( ourPlayers.contains(defenseAgent->id()) ) {
//                ourPlayers.removeOne(defenseAgent->id());
//            } else {
//                debugger->debug("[coach] Bad Defense assigning", D_ERROR);
//            }
//        }
//    } else {
//        if (ourPlayers.size() - preferedDefenseCounts <= 0) {
//            playmakeId = -1;
//            lastPlayMake = -1;

//            return;
//        }
//    }

    if (ourPlayers.empty()) {
        playmakeId = -1;
        lastPlayMake = -1;
        return;
    }


    ////////////////////first we choose our playmake
    // third version
    double ballVel = wm->ball->vel.length();
    Vector2D ballPos = wm->ball->pos;
    if(ballVel < 0.3)
    {
        double maxD = -1000.1;
        for (int ourPlayer : ourPlayers) {
            double o = -1 * agents[ourPlayer]->pos().dist(ballPos) ;
            if(ourPlayer == lastPlayMake)
                o += playMakeTh;
            if(o > maxD)
            {
                maxD = o;
                playmakeId = ourPlayer;
            }
        }
        ROS_INFO_STREAM("op :"<<ballPos.x<<"  "<<agents[ourPlayers[0]]->pos().x);
        lastPlayMake = playmakeId;
    }
    else
    {
        if(playMakeIntention.elapsed() < playMakeIntentionInterval)
        {
            playmakeId = lastPlayMake;
            debugger->debug(QString("playmake is : %1").arg(playmakeId), D_PARSA);
            return;
        }
        playMakeIntention.restart();
        //Vector2D ballVel = wm->ball->vel;
        double nearest[10] = {};
        for (int ourPlayer : ourPlayers)
            nearest[ourPlayer] = CKnowledge::kickTimeEstimation(agents[ourPlayer], wm->field->oppGoal(), *wm->ball, 4,3,2,2); // TODO FIX
        if(lastPlayMake >= 0 && lastPlayMake <= 9)
            nearest[lastPlayMake] -= 0.2;
        double minT = 1e8;
        for (int ourPlayer : ourPlayers) {
            if(nearest[ourPlayer] < minT)
            {
                minT = nearest[ourPlayer];
                playmakeId = ourPlayer;
            }
        }
        for (int ourPlayer : ourPlayers)
            debugger->debug(QString("timeneeded of %1 is : %2 \n").arg(ourPlayer).arg(nearest[ourPlayer]), D_PARSA);
        lastPlayMake = playmakeId;
    }
    debugger->debug(QString("playmake is : %1").arg(playmakeId), D_PARSA);
}

void CCoach::decideAttack()
{
    ballPState = isBallOurs();
    updateAttackState();

    lastBallPossesionState = ballPState;

    // find unused agents!
    QList<int> ourPlayers = wm->our.data->activeAgents;
    if( goalieAgent != nullptr ){
        ourPlayers.removeOne(goalieAgent->id());
    }
    for (auto defenseAgent : defenseAgents) {
        if( ourPlayers.contains(defenseAgent->id()) ){
            ourPlayers.removeOne(defenseAgent->id());
        }
    }
    selectedPlay->defensePlan.debugAgents("DEF : ");
    QString str;
    for (int ourPlayer : ourPlayers) {
        str += QString(" %1").arg(ourPlayer);
    }
    debugger->debug(QString("%1: Size: %2 --> (%3)").arg("text: ").arg(ourPlayers.size()).arg(str) , D_ERROR , "blue");

    switch (gameState->getState()) { // GAMESTATE

    case States::Halt:
        decideHalt(ourPlayers);
        return;
        break;
    case States::PlayOff:
        decideStop(ourPlayers);
        break;

    case States::OurKickOff:
        decideOurKickOff(ourPlayers);
        break;

    case States::TheirKickOff:
        decideTheirKickOff(ourPlayers);
        break;

    case States::OurDirectKick:
        decideOurDirect(ourPlayers);
        break;

    case States::TheirDirectKick:
        decideTheirDirect(ourPlayers);
        break;

    case States::OurIndirectKick:
        decideOurIndirect(ourPlayers);
        break;

    case States::TheirIndirectKick:
        decideTheirIndirect(ourPlayers);
        break;

    case States::OurPenaltyKick:
        decideOurPenalty(ourPlayers);
        break;

    case States::TheirPenaltyKick:
        decideTheirPenalty(ourPlayers);
        break;
    case States::PlayOn:
        decideStart(ourPlayers);
        break;
    case States::OurBallPlacement:
        decideOurBallPlacement(ourPlayers);
        break;
    case States::TheirBallPlacement:
        decideStop(ourPlayers);
        break;
    case States::HalfTime:
        decideHalfTimeLineUp(ourPlayers);
        break;
    default:
        decideNull(ourPlayers);
        return;
        break;
    }
    QList<Agent*> ourAgents;
    for(auto& ourPlayer : ourPlayers) {
        ourAgents.append(agents[ourPlayer]);
    }

    selectedPlay->init(ourAgents);
    selectedPlay->execute();
    lastPlayers.clear();
    lastPlayers.append(ourPlayers);
}

void CCoach::decidePlayOff(QList<int>& _ourPlayers, POMODE _mode) {

    //Decide Plan
    firstIsFinished = ourPlayOff->isFirstFinished();
    if (firstTime) {
        NGameOff::EMode tempMode;
        selectPlayOffMode(_ourPlayers.size(), tempMode);
        initPlayOffMode(tempMode, _mode, _ourPlayers);
        ourPlayOff->setMasterMode(tempMode);
        if ( tempMode == NGameOff::FirstPlay ) {
            if (firstPlay && !firstIsFinished) {
                firstTime = true;

            } else if (firstPlay && firstIsFinished) {
                firstTime = true;
                firstPlay = false;

            } else if (!firstPlay && firstIsFinished) {
                firstTime = false;
                firstPlay = true;
                firstIsFinished = false;
                preferedShotSpot = ourPlayOff->getShotSpot();
                ourPlayOff->resetFirstPlayFinishedFlag();
            }

        } else {
            firstTime = false;

        }

    } else {
        setPlayOff( ourPlayOff->getMasterMode() );
    }
}
void CCoach::decidePlayOn(QList<int>& ourPlayers, QList<int>& lastPlayers) {

    BallPossesion ballPState = isBallOurs();

    if(playmakeId > -1 && playmakeId < 12) {
        dynamicAttack->setPlayMake(agents[playmakeId]);
        ourPlayers.removeOne(playmakeId);
        debugger->debug(QString("playMake : %1").arg(playmakeId), D_MHMMD);
    }

    dynamicAttack->setDefenseClear(false);

    if(wm->our[playmakeId] != nullptr)
    {
        bool goodForKick = ((wm->ball->pos.dist(wm->field->oppGoal()) < 1.5) || (findMostPossible(wm->our[playmakeId]->pos) > (conf.DirectTrsh - shotToGoalthr)));
        if(goodForKick)
        {
            dynamicAttack->setDirectShot(true);
            if((findMostPossible(wm->our[playmakeId]->pos) > (conf.DirectTrsh - shotToGoalthr)))
                shotToGoalthr = max(0, conf.DirectTrsh - 0.2);
        } else {
            dynamicAttack->setDirectShot(false);
            shotToGoalthr = 0;
        }

    }
    /////////////////////////////////////////////////////////////////////////

    dynamicAttack->setWeHaveBall(ballPState   == BallPossesion::WEHAVETHEBALL);
    dynamicAttack->setFast(ourAttackState     == FAST);
    dynamicAttack->setCritical(ourAttackState == CRITICAL);

    //////////////////////////////////////////////assign agents
    bool overdef;
    overdef = checkOverdef();
    int MarkNum = 0;
    if(ballPState == BallPossesion::WEHAVETHEBALL) {
        MarkNum = 0;
    } else if(ballPState == BallPossesion::WEDONTHAVETHEBALL) {
        MarkNum = (overdef) ? 3 : 2;

    } else if(ballPState == BallPossesion::SOSOOUR) {
        MarkNum = 2;

    } else if(ballPState == BallPossesion::SOSOTHEIR) {
        MarkNum = 2;

    }

    MarkNum = std::min(MarkNum, ourPlayers.count());

    selectedPlay->markAgents.clear();
    if(wm->ball->pos.x >= 0
            && selectedPlay->lockAgents
            && lastPlayers.count() == ourPlayers.count()) {
        ourPlayers.clear();
        ourPlayers = lastPlayers;

    } else {
        qSort(ourPlayers.begin(), ourPlayers.end());
        for (int i = 0; i < MarkNum; i++) {
            selectedPlay->markAgents.append(agents[ourPlayers.front()]);
            ourPlayers.removeFirst();

        }

    }
}


void CCoach::selectPlayOffMode(int agentSize, NGameOff::EMode &_mode) {
    if (agentSize < 2) {
        _mode = NGameOff::DynamicPlay;
    } else if (isFastPlay() && false) { // TODO : fastPlay should be completed!
        _mode = NGameOff::FastPlay;

    } else if (gameState->ourKickoff()) {
        _mode = NGameOff::StaticPlay;
    } else if (wm->ball->pos.x < -1) {
        _mode = NGameOff::DynamicPlay;

    } else if (!firstIsFinished && conf.UseFirstPlay) {
        _mode = NGameOff::FirstPlay;

    } else if (wm->ball->pos.x > -1) {
        _mode = NGameOff::StaticPlay;

    } else {
        _mode = NGameOff::DynamicPlay;

    }
}

void CCoach::initPlayOffMode(const NGameOff::EMode _mode,
                             const POMODE _gameMode,
                             const QList<int>& _ourplayers) {
    switch(_mode) {
    case NGameOff::StaticPlay:
        initStaticPlay(_gameMode, _ourplayers);
        break;
    case NGameOff::DynamicPlay:
        initDynamicPlay(_ourplayers);
        break;
    case NGameOff::FastPlay:
        initFastPlay(_ourplayers);
        break;
    case NGameOff::FirstPlay:
        initFirstPlay(_ourplayers);
        break;
    default:
        initStaticPlay(_gameMode, _ourplayers);
    }
}

void CCoach::setPlayOff(NGameOff::EMode _mode) {
    switch(_mode) {
    case NGameOff::StaticPlay:
        setStaticPlay();
        break;
    case NGameOff::DynamicPlay:
        setDynamicPlay();
        break;
    case NGameOff::FastPlay:
        setFastPlay();
        break;
    case NGameOff::FirstPlay:
        setFirstPlay();
        break;
    default:
        setStaticPlay();
    }
}

void CCoach::initDynamicPlay(const QList<int> &_ourplayers) {

    for (int i = 0; i < 6; i++) {
        if (i >= _ourplayers.size()) {
            ourPlayOff->dynamicMatch[i] = -1;
        } else {
            ourPlayOff->dynamicMatch[i] = _ourplayers.at(i);
        }
    }
    if (_ourplayers.size() < 2) {
        ourPlayOff->dynamicSelect = CHIP;
    } else {
        ourPlayOff->dynamicSelect = KHAFAN;
    }


    double dis = 1000000;
    int id = 0;
    int swapID = 0;
    for (int i = 0; i < _ourplayers.size(); i++){
        double tempDis = agents[_ourplayers.at(i)]->pos().dist(wm->ball->pos) ;
        if (tempDis < dis) {
            dis = tempDis;
            id = _ourplayers.at(i);
            swapID = i;
        }
    }

    int tempID = ourPlayOff->dynamicMatch[0];
    ourPlayOff->dynamicMatch[0] = id;
    ourPlayOff->dynamicMatch[swapID] = tempID;


    ourPlayOff->setInitial(true);
    ourPlayOff->lockAgents = true;

}

void CCoach::initFastPlay(const QList<int> &_ourplayers) {
    // TODO : Initial Fast Play
}

void CCoach::initFirstPlay(const QList<int> &_ourplayers) {

    double minDist = wm->field->_MAX_DIST;
    int minID = -1;
    int minOwner = 0;
    for (int i = 0; i < _ourplayers.size(); i++) {

        int tempID = _ourplayers.at(i);
        double tempDist = agents[tempID]->pos().dist(wm->ball->pos);
        if (tempDist < minDist) {
            minDist = tempDist;
            minID = tempID;
            minOwner = i;
        }

        if (i >= _ourplayers.size()) {
            ourPlayOff->dynamicMatch[i] = -1;
        } else {
            ourPlayOff->dynamicMatch[i] = _ourplayers.at(i);
        }
    }
    // fix passer :)
    int tempID = ourPlayOff -> dynamicMatch[0];
    ourPlayOff -> dynamicMatch[0] = minID;
    ourPlayOff -> dynamicMatch[minOwner] = tempID;

    ourPlayOff -> setInitial(true);
    ourPlayOff -> lockAgents = true;

    // TODO : Initial First Play
}

void CCoach::setStaticPlay() {
    // TODO : Complete staticPlay checker
    ourPlayOff->setInitial(false);
}

void CCoach::setDynamicPlay() {
    // TODO : Write Dynamic Play checker
    ourPlayOff->setInitial(false);
}

void CCoach::setFirstPlay() {
    // TODO : Write First Play checker
    ourPlayOff->setInitial(false);
    firstIsFinished = false;
}

void CCoach::execute()
{
    gameState->setState(States::PlayOn);

    // place your reset codes about knowledge vars in this function
    virtualTheirPlayOffState();
    decidePreferedDefenseAgentsCountAndGoalieAgent();
    /////////////////////////////////////// choose play maker
    double critAreaRadius = 1.6;
    Circle2D critArea(wm->field->ourGoal(), critAreaRadius);
    playmakeId = -1;
    if((critArea.contains(wm->ball->pos) && wm->field->isInField(wm->ball->pos)) || (transientFlag && stateForMark != "BlockPass")) {
        decideDefense();
        choosePlaymakeAndSupporter(true);
    } else {
        choosePlaymakeAndSupporter(false);
        decideDefense();
    }
    ROS_INFO_STREAM("M : " << preferedDefenseCounts);
    ROS_INFO_STREAM("PM :" << playmakeId);
    ROS_INFO_STREAM("GAMESTATE : " << static_cast<int>(gameState->getState()));
    ////////////////////////////////////////////
    decideAttack();
    for(int i = 0 ; i < 8 ; i ++)
    {
        if(agents[i]->action != NULL)
            ROS_INFO_STREAM("robot ID decide: "<< i << "task : " << agents[i]->action->getActionName().toStdString());
    }

    checkSensorShootFault();
    // checks whether the goalie is under the net or not if it is moves out
    checkGoalieInsight();
    // Old Role Base Execution -- used for block, old_playmaker
    checkRoleAssignments();
    for(int i = 0 ; i < 8 ; i ++)
    {
        if(agents[i]->action != NULL)
            ROS_INFO_STREAM("robot ID decide 33333: "<< i << "task : " << agents[i]->action->getActionName().toStdString());
    }

    //// Handle Roles Here
    for (auto &stopRole : stopRoles) {
        if (stopRole->agent != nullptr) {
            stopRole->execute();
            ROS_INFO_STREAM("DD " << stopRole->agent->id());
        }
    }
    for(int i = 0 ; i < 8 ; i ++)
    {
        if(agents[i]->action != NULL)
            ROS_INFO_STREAM("robot ID decide 2222: "<< i << "task : " << agents[i]->action->getActionName().toStdString());
    }

    //    saveGoalie(); //if goalie is trapped under goal net , move it forward to be seen by the vision again
}

void CCoach::setFastPlay() {
    // TODO : Write Fast Play checker

}

void CCoach::checkRoleAssignments()
{
    //    knowledge->roleAssignments.clear();

    for_visible_agents(agents, i)
    {
        //        knowledge->roleAssignments[agents[i]->skillName].append(agents[i]);
    }

    //////////////// Matching for marker agents to mark better! /////////////////
    //    CRoleMarkInfo *markInfo = (CRoleMarkInfo*) CSkills::getInfo("mark");
    //    markInfo->matching();
    /////////////////////////////////////////////////////////////////////////////
}

DefensePlan& CCoach::getDefense() {return selectedPlay->defensePlan;}

void CCoach::decideHalt(QList<int>& _ourPlayers) {
    firstTime = true;
    cyclesWaitAfterballMoved = 0;
    _ourPlayers.clear();
    _ourPlayers.append(wm->our.data->activeAgents);
    for( int i = 0 ; i < _ourPlayers.count() ; i++ )
    {
        //        agents[_ourPlayers[i]]->waitHere(); // TODO : Halt Role or No Action
    }

    if(!ourPlayOff->deleted)
    {
        ourPlayOff->reset();
        ourPlayOff->deleted = true;
    }

}

void CCoach::decideStop(QList<int> & _ourPlayers) {
    firstTime = true;
    firstPlay = true;
    cyclesWaitAfterballMoved = 0;
    lastPlayMake = -1;
    if(!ourPlayOff->deleted)
    {
        ourPlayOff->reset();
        ourPlayOff->deleted = true;
    }

    QList<int> tempAgents;

    for (int i = 0; i < _ourPlayers.size(); i++) {
        stopRoles[i]->assign(nullptr);
        Agent* tempAgent = agents[_ourPlayers.at(i)];
        if (!tempAgent->changeIsNeeded) {
            ROS_INFO_STREAM("D " << i);
            stopRoles[i]->assign(agents[_ourPlayers.at(i)]);
        } else {
            tempAgents.append(tempAgent->id());
        }
    }

    _ourPlayers.clear();
    _ourPlayers.append(tempAgents);
    selectedPlay = stopPlay;
    selectedPlay->positioningPlan.reset();
}

void CCoach::decideOurKickOff(QList<int> &_ourPlayers) {
    if(ourPlayOff->deleted)
    {
        ourPlayOff->deleted = false;
    }
    selectedPlay = ourPlayOff;
    decidePlayOff(_ourPlayers, KICKOFF);
    PDEBUG("ourplayers", _ourPlayers.size(),D_MAHI);

}

void CCoach::decideTheirKickOff(QList<int> &_ourPlayers) {
    selectedPlay = theirKickOff;
    firstTime = true;
}

void CCoach::decideOurDirect(QList<int> &_ourPlayers) {
    if(ourPlayOff->deleted)
    {
        ourPlayOff->deleted = false;
    }
    selectedPlay = ourPlayOff;
    decidePlayOff(_ourPlayers, DIRECT);
    PDEBUG("ourplayers", _ourPlayers.size(),D_MAHI);

}

void CCoach::decideTheirDirect(QList<int> &_ourPlayers) {
    selectedPlay = theirDirect;
    firstTime = true;
}

void CCoach::decideOurIndirect(QList<int> &_ourPlayers) {
    if(ourPlayOff->deleted)
    {
        ourPlayOff->deleted = false;
    }
    selectedPlay = ourPlayOff;
    decidePlayOff(_ourPlayers, INDIRECT);
    PDEBUG("ourplayers", _ourPlayers.size(),D_MAHI);

}

void CCoach::decideTheirIndirect(QList<int> &_ourPlayers) {
    selectedPlay = theirIndirect;
    firstTime = true;
}

void CCoach::decideOurPenalty(QList<int> &_ourPlayers) {
    selectedPlay = ourPenalty;
    DBUG("penalty",D_MHMMD);
    firstTime = true;
}

void CCoach::decideTheirPenalty(QList<int> &_ourPlayers) {
    selectedPlay = theirPenalty;
    firstTime = true;
}

void CCoach::decideStart(QList<int> &_ourPlayers) {
    if(gameState->penaltyShootout()){
        selectedPlay=theirPenalty;
        return;
    }
    selectedPlay = dynamicAttack;
    decidePlayOn(_ourPlayers, lastPlayers);
}

void CCoach::decideOurBallPlacement(QList<int> &_ourPlayers) {
    //    selectedPlay = ourBallPlacement;
}

void CCoach::decideTheirBallPlacement(QList<int> &_ourPlayers) {
    selectedPlay = theirBallPlacement;
}

void CCoach::decideHalfTimeLineUp(QList<int> &_ourPlayers) {
    //    selectedPlay = halfTimeLineup;
}


void CCoach::decideNull(QList<int> &_ourPlayers) {
    selectedPlay->markAgents.clear();
    firstTime = true;
    if(!ourPlayOff->deleted)
    {
        ourPlayOff->reset();
        ourPlayOff->deleted = true;
    }
}

bool CCoach::isFastPlay() {
    if (conf.UseFastPlay) {
        return true; // TODO : fix this by considering that opp agents
    }
}

///HMD
bool CCoach::checkOverdef(){
    if((Vector2D::angleOf(wm->ball->pos,wm->field->ourGoal(),wm->field->ourCornerL()).abs() < 20 + overDefThr
        ||Vector2D::angleOf(wm->ball->pos,wm->field->ourGoal(),wm->field->ourCornerR()).abs() < 20 + overDefThr)
            && !Circle2D((wm->field->ourGoal() - Vector2D(0.2,0)),1.60).contains(wm->ball->pos)) {
        overDefThr = 5;
        return true;
    }
    overDefThr = 0;
    return false;

}

void CCoach::checkSensorShootFault() {
    QList<int> ourPlayers = wm->our.data->activeAgents;
    for (int i = 0; i < 12; i++) {
        if (ourPlayers.contains(i) != nullptr) {
            Agent* tempAgent = agents[i];
            if (tempAgent->shootSensor
                    &&  wm->ball->pos.dist(tempAgent->pos() + tempAgent->dir().norm()*0.08) > 0.2) {
                faultDetectionCounter[i]++;

            } else {
                faultDetectionCounter[i] = 0;
            }
        } else {
            faultDetectionCounter[i] = 0;
        }
    }

    for (size_t i = 0; i < 12; i++) {
        if ( faultDetectionCounter[i] > 300 || agents[i]->changeIsNeeded) {
            agents[i]->changeIsNeeded = true;
        }
    }
    QString s;
    for (int ourPlayer : ourPlayers) {
        s += QString(" %1 ").arg(ourPlayer);
        if (agents[ourPlayer]->changeIsNeeded) {
            debugger->debug(QString("[ROBOT FAULT] %1").arg(ourPlayer), D_ERROR);

        }

    }

}

