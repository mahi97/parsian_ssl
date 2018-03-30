#include <parsian_ai/plays/dynamicattack.h>

CDynamicAttack::CDynamicAttack() {
    // NEW PASS
    createRegions();
    clearRobotsRegionsWeights();


    dribbleIntention.start();
    lastPMInitWasDribble = false;
    //isShotInPass = false;
    lastPassPosLoc = Vector2D(5000, 5000);
    guardSize = 3;
    for (int &lastGuard : lastGuards) {
        lastGuard = -1;
    }
    positioningIntentionInterval = 500;
    shotInPass = false;

    for (int i = 0; i  < 8; i++) {
        roleAgents[i] = new CRoleDynamic();
    }
    roleAgentPM = new CRoleDynamic();
    roleAgentPM -> setIsPlayMake(true);

    passFlag   = false;
    repeatFlag = false;
    passerID   = -1;
    counter    = 0;
    nextPlanA = new SDynamicPlan;
    nextPlanB = new SDynamicPlan;
    currentPlan.reset();
    nextPlanA->reset();
    nextPlanB->reset();

    lastPasserRoleIndex  = -1;

    isBallInOurField = wm->ball->pos.x < 0;

    guards[0] = new Rect2D();
    for (size_t i = 1; i < 7; i++) {
        guards[i] = new Rect2D[i];
        guardLocations[i] = new Vector2D* [i];
        for (size_t j = 0; j < i; j++) {
            guardLocations[i][j] = new Vector2D[3];
        }
    }



    for (bool &i : goToDynamic) {
        i = false;
    }

    assignRegions();
    assignLocations();
}

CDynamicAttack::~CDynamicAttack() {
    for (auto& roleAgent : roleAgents) {
        delete roleAgent;
    }

    delete roleAgentPM;

    delete   guards[0];
    delete   guards[1];
    delete[] guards[2];
    delete[] guards[3];
    delete[] guards[4];
    delete[] guards[5];

    for (size_t i = 1; i < 6; i++) {
        for (size_t j = 0; j < 6; j++) {
            delete[] guardLocations[i][j];
        }
    }

    for (size_t i = 0; i < 4; i++) {
        delete guardLocations[i];
    }

}

void CDynamicAttack::init(const QList<Agent*>& _agents) {
    setAgentsID(_agents);
    initMaster();

    //    if( knowledge->getLastPlayExecuted() != StartPlay){
    //        reset();
    //    }
    //    knowledge->setLastPlayExecuted(StartPlay);
}

void CDynamicAttack::reset() {
    executedCycles = 0;
    DBUG(QString("Dynamic Attack Reset"), D_MAHI);
}

void CDynamicAttack::execute_x() {
    ROS_INFO_STREAM("Dynamic Attack : " << agents.size());
    globalExecute(agents.size());
}

void CDynamicAttack::globalExecute(int agentSize) {

    isBallInOurField = wm->ball->pos.x < 0;
    dynamicPlanner(agentSize);
}

void CDynamicAttack::makePlan(int agentSize) {

    //// Initialize Plan with null values
    nextPlanA->mode = DynamicMode::NoMode;
    nextPlanA->agentSize = agentSize;
    for (auto &positionAgent : nextPlanA->positionAgents) {
        positionAgent.region = DynamicRegion::NoMatter;
        positionAgent.skill  = PositionSkill::NoSkill;
    }

    //// Initialize Plan with null values
    nextPlanB->mode = DynamicMode::NoMode;
    nextPlanB->agentSize = agentSize;
    for (auto &positionAgent : nextPlanB->positionAgents) {
        positionAgent.region = DynamicRegion::NoMatter;
        positionAgent.skill  = PositionSkill::NoSkill;
    }

    if(true)
    {
        ROS_INFO_STREAM("kian: nomode");
        nextPlanA->mode = DynamicMode::NoMode;
        nextPlanA->playmake.init(PlayMakeSkill::Pass, DynamicRegion::Best);
        for (size_t i = 0; i < agentSize; i++) {
            nextPlanA->positionAgents[i].region = DynamicRegion::Best;
            nextPlanA->positionAgents[i].skill  = PositionSkill::Ready;
        }
        currentPlan = *nextPlanA;
        return;
    }

    //// We Don't have the ball -- counter-attack, blocking, move forward
    //// And Ball is in our field
    if (wm->ball->pos.x < 0) {
        ROS_INFO_STREAM("kian: dont have the ball");
        nextPlanA->mode = DynamicMode::NotWeHaveBall;
        if (conf.ChipForward) {
            nextPlanA->playmake.init(PlayMakeSkill::Chip, DynamicRegion::Forward);
        } else {
            nextPlanA->playmake.init(PlayMakeSkill::Chip, DynamicRegion::Goal);
        }
        for (size_t i = 0; i < agentSize; i++) {
            nextPlanA->positionAgents[i].region = DynamicRegion::Near;
            nextPlanA->positionAgents[i].skill  = PositionSkill::Ready;
        }
    }
    //// we have ball and
    //// shot prob is more than 50%
    else if (directShot) {
        ROS_INFO_STREAM("kian: diret shot");
        nextPlanA->mode = DynamicMode::DirectKick;
        nextPlanA->playmake.init(PlayMakeSkill::Shot, DynamicRegion::Goal);
        for (size_t i = 0; i < agentSize; i++) {
            nextPlanA->positionAgents[i].region = DynamicRegion::Best;
            nextPlanA->positionAgents[i].skill  = PositionSkill::Ready;
        }
    }
    else if (critical) {
        ROS_INFO_STREAM("kian: critical");
        nextPlanA->mode = DynamicMode::Critical;
        {
            oppRob = wm->field->oppGoal();
            lastPMInitWasDribble = true;
            if (conf.DribbleInFast) {
                nextPlanA->playmake.init(PlayMakeSkill::Dribble, DynamicRegion::Goal);
            } else {
                nextPlanA->playmake.init(PlayMakeSkill::Shot, DynamicRegion::Goal);
            }
            for (size_t i = 0; i < agentSize; i++) {
                nextPlanA->positionAgents[i].region = DynamicRegion::Best;
                nextPlanA->positionAgents[i].skill  = PositionSkill::Ready;
            }
        }

    }
    //// if Defense isn't clearing and
    //// we have ball and
    //// shot prob isn't more than 50% and
    //// there isn't a critical situation and
    //// we don't have positioning agent
    else if (agentSize == 0) {
        ROS_INFO_STREAM("kian: no positioning");
        nextPlanA->mode = DynamicMode::NoPositionAgent;
        nextPlanA->playmake.init(PlayMakeSkill::Shot, DynamicRegion::Goal);
    }
    // we have ball and
    // shot prob isn't more than 50% and
    // there isn't a critical situation and
    // we have positioning agents
    // it's needed to be fast
    else if (fast) {
        ROS_INFO_STREAM("kian: fast");
        oppRob = wm->field->oppGoal();
        nextPlanA->mode = DynamicMode ::Fast;
        if (conf.DribbleInFast) {
            nextPlanA->playmake.init(PlayMakeSkill::Dribble, DynamicRegion ::Goal);
        } else {
            nextPlanA->playmake.init(PlayMakeSkill::Shot, DynamicRegion ::Goal);
        }
        for (size_t i = 0; i < agentSize; i++) {
            nextPlanA->positionAgents[i].region = DynamicRegion::Best;
            nextPlanA->positionAgents[i].skill  = PositionSkill::Ready;
        }
    }
    // if Defense isn't clearing and
    // we have ball and
    // shot prob isn't more than 50% and
    // there isn't a critical situation and
    // we have positioning agents
    // there's no need to be fast and
    // there is no plan for this situation
    ///the correct mode is as below but the ::choosebesrtposforpass:: giving a  bad position
    /////////TODO: fix the pos
///    else {
///        ROS_INFO_STREAM("kian: nomode");
///        nextPlanA->mode = DynamicMode::NoMode;
///        nextPlanA->playmake.init(PlayMakeSkill::Pass, DynamicRegion::Best);
///        for (size_t i = 0; i < agentSize; i++) {
///            nextPlanA->positionAgents[i].region = DynamicRegion::Best;
///            nextPlanA->positionAgents[i].skill  = PositionSkill::Ready;
///        }
    else {
        ROS_INFO_STREAM("kian: nomode");
        nextPlanA->mode = DynamicMode::NoMode;
        nextPlanA->playmake.init(PlayMakeSkill::Shot, DynamicRegion::Goal);
        for (size_t i = 0; i < agentSize; i++) {
            nextPlanA->positionAgents[i].region = DynamicRegion::Best;
            nextPlanA->positionAgents[i].skill  = PositionSkill::Ready;
        }
    }

    currentPlan = *nextPlanA;
}

Agent* CDynamicAttack::getMahiPlayMaker() {
    return mahiPlayMaker;
}

void CDynamicAttack::assignId() {

    mahiPlayMaker = nullptr;
    if (playmakeID != -1) {
        mahiPlayMaker = playmake;
    }

    int tempIndex;
    QList<int> matchedIDList;
    mahiPositionAgents.clear();
    guardIndexList.clear();

    MWBM matcher;
    int n = agents.size();
    matcher.create(n, n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matcher.setWeight(i, j, -1 * semiDynamicPosition[i].dist(agents.at(j)->pos()));
        }
    }
    matcher.findMaxMinMatching();
    //matcher.findMatching();
    for (int i = 0; i < n; i++) {
        tempIndex = matcher.getMatch(i);
        matchedIDList.append(tempIndex);
        guardIndexList.append(i);
        mahiAgentsID[i] = tempIndex;
        mahiPositionAgents.append(agents.at(tempIndex));
    }
    for (auto mahiPoisitionAgent : mahiPositionAgents) {
        DBUG(QString("1 : %2").arg(mahiPoisitionAgent->id()), D_MAHI);
    }
}

void CDynamicAttack::assignTasks() {
    if (mahiPlayMaker != nullptr) {
        playMake();
    }
    if (currentPlan.agentSize > 0) {
        ROS_INFO_STREAM("kian: too if positioning : currentPlan.agentSize:" << currentPlan.agentSize);
        positioning(semiDynamicPosition);
    }
}

/**
 * @brief CDynamicAttack::dynamicPlanner
 * @param agentSize number of positioning Agents
 */
void CDynamicAttack::dynamicPlanner(int agentSize) {
    //HAMID HERE

    for (size_t i = 0; i < 8; i++) {
        mahiAgentsID[i] = -1;
    }

    makePlan(agentSize);

    for (int i = 0; i < agents.size(); i++) {
        if (agents[i]->isVisible() && agents[i]->action != nullptr) {
            Action *mahi = agents[i]->action;
            ROS_INFO_STREAM(i << ": " << mahi->getActionName().toStdString().c_str());
        }
    }
    if (agentSize > 0) {
        chooseBestPositons_new();
    }
    assignId_new();
    if (agentSize > 0) {
        //    chooseMarkPos();
        chooseBestPosForPass(semiDynamicPosition);
        ROS_INFO_STREAM("hamid after chooseBestPosForPass");
    }
    ROS_INFO_STREAM("kian: ghable assign task");
    assignTasks();

//    DBUG(QString("MODE : %1").arg(getString(currentPlan.mode)),D_MAHI);
//    ROS_INFO_STREAM("MODE : " << getString(currentPlan.mode).toStdString());
//    DBUG(QString("BALL : %1").arg(isBallInOurField),D_MAHI);
    for(size_t i = 0;i < currentPlan.agentSize;i++) {
        if(mahiAgentsID[i] >= 0) {
            roleAgents[i]->execute();
        } else {
            DBUG(QString("[dynamicAttack - %1] mahiAgentID buged").arg(__LINE__), D_MAHI);
        }
    }
    if (playmakeID != -1) {
        roleAgentPM->execute();
    }
//    for (auto i : semiDynamicPosition) {
//        drawer->draw(i, QColor(Qt::black));
//    }
//
//    for (auto i : dynamicPosition) {
//        drawer->draw(Circle2D(i, 0.2), QColor(Qt::red), false);
//    }
//
//    showRegions(static_cast<unsigned int>(currentPlan.agentSize), QColor(Qt::gray));
//    showLocations(static_cast<unsigned int>(currentPlan.agentSize), QColor(Qt::red));

    // TODO : remove this
    if (isPlayMakeChanged()) {
        for (bool &i : goToDynamic) {
            i = false;
        }
    }
    hamidDebug();
}

void CDynamicAttack::playMake() {

    drawer->draw(Circle2D(mahiPlayMaker->pos(), 0.1), QColor(Qt::red), true);
    if (teamConfig.color == teamConfig.BLUE) {
        drawer->draw(Circle2D(mahiPlayMaker->pos() + mahiPlayMaker->dir() * 0.08, 0.06), QColor(Qt::blue), true);
    } else {
        drawer->draw(Circle2D(mahiPlayMaker->pos() + mahiPlayMaker->dir() * 0.08, 0.06), QColor(Qt::yellow), true);
    }

    roleAgentPM->setAgent(mahiPlayMaker);
    roleAgentPM->setAvoidPenaltyArea(true);

    Vector2D og = wm->ball->pos - wm->field->ourGoal();
    switch (currentPlan.playmake.skill) {
    case PlayMakeSkill ::Dribble:
        ROS_INFO_STREAM("kian: drrible");//<< currentPlan.playmake.skill);
        ROS_INFO_STREAM("kian: passpos: " << currentPlan.passPos.x << ", " << currentPlan.passPos.y << "//////////////////");
        roleAgentPM -> setTargetDir(currentPlan.passPos);
        roleAgentPM -> setTarget(oppRob);
        roleAgentPM -> setChip(false);
        roleAgentPM -> setNoKick(true);
        roleAgentPM -> setSelectedPlayMakeSkill(PlayMakeSkill ::Dribble); // skill Dribble
        break;
    case PlayMakeSkill ::Pass:
        ROS_INFO_STREAM("kian: pass");
        ROS_INFO_STREAM("kian: passpos: " << currentPlan.passPos.x << ", " << currentPlan.passPos.y << "//////////////////");
        roleAgentPM -> setChip(chipOrNot(currentPlan.passPos, 0.5, 0.1));
        roleAgentPM -> setTarget(currentPlan.passPos);
        roleAgentPM -> setEmptySpot(false);
        roleAgentPM -> setNoKick(false);
        if (roleAgentPM->getChip()) {
            roleAgentPM->setKickSpeed(appropriateChipSpeed());
        } else {
            roleAgentPM->setKickSpeed(appropriatePassSpeed());
        }

        roleAgentPM -> setSelectedPlayMakeSkill(PlayMakeSkill ::Pass);// Skill Kick

        /////////////////////////////// I HATE PARSA
        //        for(int i = 0; i < wm->opp.activeAgentsCount(); i++)
        //            if(ballPos.dist(wm->field->oppGoal()) < 0.7 && Circle2D(wm->opp[wm->opp.activeAgentID(i)]->pos + wm->opp[wm->opp.activeAgentID(i)]->dir.norm()*0.08,0.07).contains(ballPos))
        //                roleAgentPM -> setSelectedSkill(DynamicEnums::Dribble); // skill Dribble
        //            else
        //                roleAgentPM -> setSelectedSkill(DynamicEnums::Pass);// Skill Kick
        // TODO : fix this dastan
        /*if(isRightTimeToPass()) {
                roleAgentPM->setNoKick(false); //TEST
            } else {
                roleAgentPM->setNoKick(true);
            }*/
        break;

    case PlayMakeSkill ::Chip:
        ROS_INFO_STREAM("chip");
        roleAgentPM->setNoKick(false);
        if (currentPlan.playmake.region == DynamicRegion ::Goal) {
            roleAgentPM ->setTarget(wm->field->oppGoal());
            if (wm->ball->pos.x < -2) {
                roleAgentPM ->setKickSpeed(conf.HighSpeedChip);
            } else {
                roleAgentPM ->setKickSpeed(conf.MediumSpeedChip);

            }
        } else if (currentPlan.playmake.region == DynamicRegion ::Forward) {
            roleAgentPM->setTarget(Vector2D(1000, 0));
            roleAgentPM->setKickSpeed(conf.LowSpeedChip);
        } else {
            roleAgentPM->setTarget(wm->field->oppGoal());
            roleAgentPM->setKickSpeed(conf.LowSpeedChip);
        }
        roleAgentPM->setChip(true);
        roleAgentPM->setSelectedPlayMakeSkill(PlayMakeSkill ::Chip);// Skill Chip
        break;
    case PlayMakeSkill ::Shot: {
        roleAgentPM->setEmptySpot(true);
        roleAgentPM->setChip(false);
        roleAgentPM->setNoKick(false);
        roleAgentPM->setTarget(wm->field->oppGoal());
        roleAgentPM->setKickSpeed(6); // TODO : 8m/s by profiller
        roleAgentPM->setSelectedPlayMakeSkill(PlayMakeSkill ::Shot); // Skill Kick
        break;
    }
    default:
        ROS_INFO_STREAM("default");
        roleAgentPM->setEmptySpot(true);
        roleAgentPM->setChip(false);
        roleAgentPM->setNoKick(false);
        roleAgentPM->setTarget(wm->field->oppGoal());
        // Parsa : ino hamintory avaz kardam kar kard...
        roleAgentPM->setKickSpeed(6); // TODO : 8m/s by profiller
        roleAgentPM->setSelectedPlayMakeSkill(PlayMakeSkill ::Shot); // Skill Kick
        break;
    }
}

void CDynamicAttack::positioning(QList<Vector2D> _points) {
    ROS_INFO_STREAM("hamid inside positioning");
    bool check = false;
    int index = 0;
    for (int i = 0 ; i < currentPlan.agentSize; i++) {
        if (mahiAgentsID[i] >= 0) {
            ROS_INFO_STREAM("kian: too if set shodan : ID:" << agents.at(mahiAgentsID[i])->id() << ", agentID: " << mahiPositionAgents.at(i)->id());
            roleAgents[i]->setAgent(mahiPositionAgents.at(i));
            roleAgents[i]->setAvoidPenaltyArea(true);
            if (i < _points.size()) {

                switch (currentPlan.positionAgents[i].skill) {
                case PositionSkill ::Ready: // Ready For Pass
                    ROS_INFO_STREAM("kian: too switch set : skill: ready");
                    roleAgents[i]->setTarget(_points.at(index++));
                    roleAgents[i]->setReceiveRadius(
                                std::max(0.5, 2 - roleAgents[i]->getAgent()->pos()
                                         .dist(roleAgents[i]->getTarget())));
                    roleAgents[i]->setSelectedPositionSkill(PositionSkill ::Ready);// Receive Skill

                    break;
                case PositionSkill ::OneTouch: // OneTouch Reflects
                    ROS_INFO_STREAM("kian: too switch set : skill: onetouch");
                    roleAgents[i]->setWaitPos(_points.at(index++));
                    roleAgents[i]->setReceiveRadius(
                                std::max(0.5, 2 - roleAgents[i]->getAgent()->pos()
                                         .dist(roleAgents[i]->getTarget())));

                    // TODO : fix the target
                    roleAgents[i]->setTarget(wm->field->oppGoal());
                    roleAgents[i]->setSelectedPositionSkill(PositionSkill ::OneTouch);// Receive Skill

                    break;
                case PositionSkill ::Move:
                    ROS_INFO_STREAM("kian: too switch set : skill: move");
                    roleAgents[i]->setReceiveRadius(
                                std::max(0.5, 2 - roleAgents[i]->getAgent()->pos()
                                         .dist(roleAgents[i]->getTarget())));
                    roleAgents[i]->setTarget(_points.at(index));
                    roleAgents[i]->setTargetDir(wm->ball->pos - roleAgents[i]->getAgent()->pos());
                    roleAgents[i]->setSelectedPositionSkill(PositionSkill ::Move);

                    break;
                case PositionSkill ::NoSkill:
                    ROS_INFO_STREAM("kian: too switch set : skill: noskill");
                    roleAgents[i]->setSelectedPositionSkill(PositionSkill ::Ready);// Receive Skill

                    break;
                    //                    case PositionSkill::Pass:break;
                    //                    case PositionSkill::CatchBall:break;
                    //                    case PositionSkilll::Shot:break;
                    //                    case PositionSkill::Keep:break;
                    //                    case PositionSkill::Chip:break;
                    //                    case PositionSkill::Dribble:break;
                }

                if (roleAgents[i]->getTarget() == currentPlan.passPos) {
                    check = true;
                }
                //debug(QString("pos positions : %1 %2").arg(roleAgents[i]->getTarget().x).arg(roleAgents[i]->getTarget().y), D_PARSA);
            }
        } else {
            DBUG("[dynamicAttack] mahiagent ha eshtebahe chera ?", D_MAHI);
        }
    }
    //assert(check);
}


inline bool CDynamicAttack::chipOrNot(Vector2D target,
                                      double _radius, double _treshold) {
    return !isPathClear(wm->ball->pos, target, _radius, _treshold);
}

bool CDynamicAttack::keepOrNot() {
    return true;
}

bool CDynamicAttack::isPathClear(Vector2D _pos1, Vector2D _pos2,
                                 double _radius, double treshold) {
    Vector2D sol1, sol2, sol3;
    Line2D _path(_pos1, _pos2);
    Polygon2D _poly;
    Circle2D(_pos2, _radius + treshold).
            intersection(_path.perpendicular(_pos2), &sol1, &sol2);

    _poly.addVertex(sol1);
    sol3 = sol1;
    _poly.addVertex(sol2);
    Circle2D(_pos1, Robot::robot_radius_new + treshold).
            intersection(_path.perpendicular(_pos1), &sol1, &sol2);

    _poly.addVertex(sol2);
    _poly.addVertex(sol1);
    _poly.addVertex(sol3);

    drawer->draw(_poly, "red");

    for (int i = 0; i < wm->opp.activeAgentsCount(); i++) {
        if (_poly.contains(wm->opp.active(i)->pos)) {
            return false;
        }
    }
    return true;
}

bool CDynamicAttack::isPlayMakeChanged() {

    if (mahiPlayMaker != nullptr) {
        if (mahiPlayMaker->id() != lastPlayMakerId) {
            lastPlayMakerId = mahiPlayMaker->id();
            return true;
        }
    }
    return false;
}

int CDynamicAttack::appropriatePassSpeed() {

    double tempDistance = 0;
    double speed = 0;
    if (mahiPlayMaker != nullptr) {
        tempDistance = mahiPlayMaker->pos().dist(currentPlan.passPos);

        if (false) { // dynamic pass Speed // FALSED IN ROS
            //            if (tempDistance < 2) {
            //                speed = knowledge->getProfile(mahiPlayMaker->id(), tempDistance) + policy()->DynamicPlay_LowSpeedPass();
            //
            //            } else if(tempDistance > 4) {
            //                speed = knowledge->getProfile(mahiPlayMaker->id(), tempDistance) + policy()->DynamicPlay_HighSpeedPass();
            //
            //            } else {
            //                speed = knowledge->getProfile(mahiPlayMaker->id(), tempDistance) + policy()->DynamicPlay_MediumSpeedPass();
            //
            //            }

        } else {
            if (tempDistance < 2) {
                speed = conf.LowSpeedPass;

            } else if (tempDistance > 4) {
                speed = conf.HighSpeedPass;

            } else {
                speed = conf.MediumSpeedPass;

            }

        }
    } else {
        speed = conf.MediumSpeedPass;
    }
    return static_cast<int>(speed);
}


int CDynamicAttack::appropriateChipSpeed() {

    double tempDistance = 0;
    double speed = 0;
    if (mahiPlayMaker != nullptr) {
        tempDistance = mahiPlayMaker->pos().dist(currentPlan.passPos);

        if (false) { // dynamic chip Speed
            //            if (tempDistance < 2) {
            //                speed = knowledge->getProfile(mahiPlayMaker->id(), tempDistance, false) + policy()->DynamicPlay_LowSpeedChip();
            //
            //            } else if(tempDistance > 4) {
            //                speed = knowledge->getProfile(mahiPlayMaker->id(), tempDistance, false) + policy()->DynamicPlay_HighSpeedChip();
            //
            //            } else {
            //                speed = knowledge->getProfile(mahiPlayMaker->id(), tempDistance, false) + policy()->DynamicPlay_MediumSpeedChip();
            //
            //            }

        } else {
            if (tempDistance < 2) {
                speed = conf.LowSpeedChip;

            } else if (tempDistance > 4) {
                speed = conf.HighSpeedChip;

            } else {
                speed = conf.MediumSpeedChip;

            }

        }
    } else {
        speed = conf.MediumSpeedPass;
    }
    return static_cast<int>(speed);
}

void CDynamicAttack::chooseBestPositons() {

    /*
     *
     *
     *
     *
     * OLD CODE*/
    //it has three code of choosing best position that only one of them must be uncommented
    int agentSize = agents.size();
    int cntD = 0;
    Vector2D ans;
    bool haveSupporter = false;
    Vector2D passPos;
    guardIndexList.clear();
    for (int i = 0; i < currentPlan.agentSize; i++) {
        guardIndexList.append(i);
    }

    semiDynamicPosition.clear();

    if (mahiPlayMaker != nullptr && wm->ball->pos.dist(mahiPlayMaker->pos()) < 0.15) {
        passPos        = currentPlan.passPos;
        lastPassPosLoc = currentPlan.passPos;
    } else {
        passPos = lastPassPosLoc;
    }
    //first type of choosing and probably the best one
    //this choose the points with maximum x that has a good one touch angle
    for (int i = 0; i < agentSize; i++) {
        //if it wants to get a dribble position:
        if (currentPlan.positionAgents[i].region == DynamicRegion ::Supporter ||
                (i <= 1 && dribbleIntention.elapsed() < 3000)) {
            cntD++;
            double x, y;
            if (cntD == 1) {
                if (wm->ball->pos.x < 3.3 - thrshDribble) {
                    thrshDribble = 0;
                    if (lastYDrib == 10) {
                        if (wm->ball->pos.y > 0) {
                            y = wm->ball->pos.y - 1.5;
                        } else {
                            y = wm->ball->pos.y + 1.5;
                        }
                    } else if (wm->ball->pos.y > 0.2) {
                        y = wm->ball->pos.y - 1.5;
                    } else if (wm->ball->pos.y < -0.2) {
                        y = wm->ball->pos.y + 1.5;
                    } else {
                        y = lastYDrib;
                    }
                    x = min(wm->ball->pos.x - 0.1, 3.3);
                } else {
                    thrshDribble = 0.2;
                    if (lastYDrib == 10) {
                        if (wm->ball->pos.y > 0) {
                            y = -1.35;
                        } else {
                            y = 1.35;
                        }
                    } else if (wm->ball->pos.y > 0.2) {
                        y = -1.35;
                    } else if (wm->ball->pos.y < -0.2) {
                        y = 1.35;
                    } else {
                        y = lastYDrib;
                    }
                    x = 3.65;
                }
                lastYDrib = y;
            }
            if (cntD == 2) {
                x = wm->ball->pos.x - 1;
                y = wm->ball->pos.y;
            }
            semiDynamicPosition.append(Vector2D(x, y));
            if (i == 1) {
                haveSupporter = true;
                i = -1;
                agentSize -= 2;
                continue;
            }
        }

        //if it wants to get the BEST position :
        //first if we can have supporter :
        if (!haveSupporter && conf.SupportPriority <= agentSize) {
            haveSupporter = true;
            semiDynamicPosition.append(Vector2D(wm->ball->pos.x - 1, wm->ball->pos.y));
            i--;
            agentSize--;
            continue;
        }
        //second if we have only one positioning
        if (agentSize == 1) {
            if (wm->ball->pos.y > 0.3) {
                semiDynamicPosition.append(guardLocations[agentSize][0][2]);
                lastGuards[0] = 2;
            } else if (wm->ball->pos.y < -0.3) {
                semiDynamicPosition.append(guardLocations[agentSize][0][0]);
                lastGuards[0] = 0;
            } else {
                semiDynamicPosition.append(guardLocations[agentSize][0][lastGuards[0]]);
            }
        }
        //third if we have more than one positioning :
        else {
            lastYDrib = 10;
            int best = -1;
            Vector2D  points  [guardSize];
            double tempAngle  [guardSize];
            bool tooNearToBall[guardSize];
            bool farChoice = false;
            bool tooFarToBall [guardSize];
            double ballVelCoef = 0.3;
            double maxAng = -1;

            for (int j = 0; j < guardSize; j++) {
                points[j]    = guardLocations[agentSize][i][j];
                tooNearToBall[j] = ((wm->ball->pos + wm->ball->vel * ballVelCoef).
                                    dist(points[j]) <= 1.2);
                tooFarToBall[j] = ((wm->ball->pos + wm->ball->vel * ballVelCoef).
                                   dist(points[j]) > 4.7);
                tempAngle[j] = Vector2D::angleOf(wm->field->oppGoal(),
                                                 points[j], wm->ball->pos + 0.2 * wm->ball->vel).degree();
                //            debug(QString("angle   is  : %1").arg(tempAngle    [j]), D_PARSA);
                //            debug(QString("toofar  is  : %1").arg(tooFarToBall [j]), D_PARSA);
                //            debug(QString("toonear is  : %1").arg(tooNearToBall[j]), D_PARSA);
                maxAng = max(maxAng, tempAngle[j]);
            }
            for (int j = 0; j < guardSize; j++)
                if (tempAngle[j] < 70 || (j == guardSize - 1 && tempAngle[j] < 80))
                    if (best == -1 || points[best].x < points[j].x)
                        if (!tooNearToBall[j])
                            if (!tooFarToBall[j] || agentSize != 3) { //exception for 3 positionings
                                best = j;
                            }
            //        debug(QString("best is : %1").arg(best), D_PARSA);
            for (int j = 0; j < guardSize; j++)
                if (points[j].dist(passPos) < 0.3) {
                    best = j;
                }
            double   coef = 1.3;
            Vector2D nextPos;
            do {
                coef -= 0.1;
                nextPos = wm->ball->pos + wm->ball->vel * coef;
            } while (!(wm->field->isInField(nextPos)) &&
                     (wm->field->isInField(wm->ball->pos)));
            if (guards[currentPlan.agentSize][i]
                    .contains(nextPos)) {
                best = farGuardFromPoint(i, nextPos);
                farChoice = true;
            }
            //        debug(QString("best is : %1").arg(best), D_PARSA);
            if (agentSize == 3 && best == guardSize - 1 && i == 1
                    && points[best].x > wm->ball->pos.x + 1) { //exception for 3 positionings
                Line2D l = Line2D(wm->ball->pos, points[best]);
                Vector2D dummy;
                Segment2D s = Segment2D(wm->field->oppGoalR() - Vector2D(0, 0.3),
                                        wm->field->oppGoalL() + Vector2D(0, 0.3));
                dummy = s.intersection(l);
                if (dummy.x < 10) {
                    if (wm->ball->pos.y < 0) {
                        ans = points[best] + Vector2D(0, 0.6);
                    } else {
                        ans = points[best] - Vector2D(0, 0.6);
                    }
                    best = -2;
                }
            }
            if (best == -2);
            else if (best == -1) {
                if ((i == guardSize - 1 || i == 0) && (agentSize == 3)) {
                    if (wm->ball->pos.dist(points[guardSize / 2]) > 0.6) {
                        ans = points[guardSize / 2];
                    } else if (wm->ball->pos.dist(points[0]) > 0.6) {
                        ans = points[0];
                    } else {
                        ans = Vector2D(0, points[0].y);
                    }
                } else {
                    ans = Vector2D(0, points[0].y);
                }
            } else if (i < currentPlan.agentSize) {
                ans = points[best];
            } else if (currentPlan.mode == DynamicMode ::DefenseClear) {
                ans = Vector2D(0, 0);
            } else
                ans = guardLocations[currentPlan.agentSize]
                        [currentPlan.agentSize - 1]
                        [best];

            int matchId = -1;
            for (int j = 0; j < guardSize; j++)
                if (points[j] == ans) {
                    matchId = j;
                }
            //        debug(QString("ans is matchId: %1 %2 %3").arg(ans.x).arg(ans.y).arg(matchId), D_PARSA);
            //        debug(QString("last guard is : %1").arg(lastGuards[i]), D_PARSA);
            //        debug(QString("%1").arg(positioningIntention.msec()), D_PARSA);
            if (matchId == -1) {
                semiDynamicPosition.append(ans);
                positioningIntention.restart();
                lastGuards[i] = -1;
            } else if (matchId != lastGuards[i]) {
                if (lastGuards[i] == -1 || farChoice ||
                        positioningIntention.msec() > positioningIntentionInterval) {
                    semiDynamicPosition.append(ans);
                    positioningIntention.restart();
                    lastGuards[i] = matchId;
                } else {
                    semiDynamicPosition.append(points[lastGuards[i]]);
                }
            } else {
                semiDynamicPosition.append(points[matchId]);
                positioningIntention.restart();
            }
            //        debug(QString(""), D_PARSA);
        }
    }

   /*
   *
   *
   *
   *
   *
   *
   */
}

void CDynamicAttack::chooseBestPosForPass(QList<Vector2D> _points) {
    //the new one:
    QList <Vector2D> temp;
    int ans = 0;
    double points[10] = {};

    for (auto _point : _points) {
        temp.append(_point);
    }

    //    debug(QString("DIntention %3").arg(dribbleIntention.elapsed()), D_PARSA);
    //if we are dribbling
    int a = temp.size();
    ROS_INFO("build shode 2");
    if (dribbleIntention.elapsed() < 3000) {
        currentPlan.passPos =  temp[0];
        return;
    }
    //else

    for (int i = 0; i < temp.size(); i++) {
        if (lastPassPosLoc == temp[i]) {
            points[i] += 2;
        }
        DBUG(QString("%1 %2 point is %3").arg(temp.at(i).x).arg(temp.at(i).y).arg(points[i]), D_PARSA);
        double M = 100;
        if (mahiPlayMaker != nullptr) {
            for (int j = 0; j < wm->opp.activeAgentsCount(); j++) {
                M = min(M, wm->opp.active(j)->pos.dist(mahiPlayMaker->pos()));
                M = min(M, (wm->opp.active(j)->pos +
                            wm->opp.active(j)->vel * 0.5).dist(mahiPlayMaker->pos()));
            }
            //if(M < 2)
            {
                double e = mahiPlayMaker->dir().angleOf(temp[i], mahiPlayMaker->pos(),
                                                        mahiPlayMaker->dir().norm() * 1 + mahiPlayMaker->pos()).degree();
                double p = (e / 30) / (M + 0.001);
                points[i] -= p;
                DBUG(QString("angle is %1").arg(mahiPlayMaker->dir().angleOf(temp[i], mahiPlayMaker->pos(),
                                                                             mahiPlayMaker->dir().norm() * 1 + mahiPlayMaker->pos()).degree()), D_PARSA);
            }
        }
        DBUG(QString("%1 %2 near opp %3").arg(temp.at(i).x).arg(temp.at(i).y).arg(points[i]), D_PARSA);
        M = 100;
        for (int j = 0; j < wm->opp.activeAgentsCount(); j++) {
            M = min(M, wm->opp.active(j)->pos.dist(temp[i]));
        }
        if (M < 2) {
            points[i] += M;
        } else {
            points[i] += 4;
        }
        DBUG(QString("%1 %2 opptotemp %3").arg(temp.at(i).x).arg(temp.at(i).y).arg(points[i]), D_PARSA);
        //        points[i] -= ballPos.dist(temp[i]) / 2;
        M = 100;
        for (int j = 0; j < wm->our.activeAgentsCount(); j++) {
            M = min(M, wm->our.active(j)->pos.dist(temp[i]));
        }
        if (M > 1) {
            points[i] -= 5;
        }
        if (points[i] > points[ans]) {
            ans = i;
        }
        DBUG(QString("%1 %2 ourtopoint %3").arg(temp.at(i).x).arg(temp.at(i).y).arg(points[i]), D_PARSA);
        DBUG(QString("end"), D_PARSA);
    }
    currentPlan.passPos = temp[ans];
    lastPassPosLoc = currentPlan.passPos;
    DBUG(QString("pass Pos %1 %2").arg(currentPlan.passPos.x).arg(currentPlan.passPos.y), D_PARSA);
}

double CDynamicAttack::getDynamicValue(const Vector2D &_dynamicPos) const {
    double defMoveAngle, openaAngle;
    defMoveAngle = Vector2D::angleOf(wm->ball->pos, wm->field->oppGoal(), _dynamicPos).degree();
    return defMoveAngle;
}

Vector2D CDynamicAttack::neaerstGuardToPoint(const Vector2D &startVec) const {
    int regionIndex = 0;
    double tempDist, minDist = 10000;
    const int positionAgentCnt = currentPlan.agentSize - 1;
    int index = 0;
    for (int i = 0; i < positionAgentCnt; i++) {
        if (guards[positionAgentCnt][i].contains(startVec)) {
            regionIndex = i;
            break;
        }
    }

    for (int i = 0; i < 3; i++) {
        tempDist = guardLocations[positionAgentCnt][regionIndex][i].dist(startVec);
        if (tempDist < minDist) {
            minDist = tempDist;
            index = i;
        }
    }

    return guardLocations[positionAgentCnt][regionIndex][index];
}

bool CDynamicAttack::isRightTimeToPass() {
    double minDist = 99999, tempDist;
    int tempDefIndex = 0;

    for (int i = 0; i < mahiPositionAgents.size(); i++) {
        tempDist = mahiPositionAgents.at(i)->pos().dist(currentPlan.passPos);
        if (tempDist < minDist) {
            minDist      = tempDist;
            tempDefIndex = i;
        }
    }
    if (semiDynamicPosition.size() > tempDefIndex) {
        if (mahiPositionAgents.at(tempDefIndex)->pos()
                .dist(semiDynamicPosition.at(tempDefIndex)) < conf.Area) {
            return true;
        }
    }
    return false;
}

void CDynamicAttack::managePasser() {

    //    Vector2D sol1,sol2;

    //    if(!repeatFlag) {
    //        if(currentPlan.agents[0].role == DynamicEnums::PlayMaker) {
    //            if(currentPlan.agents[0].skill == DynamicEnums::Pass) {
    //                if(Circle2D(mahiPlayMaker->pos() + mahiPlayMaker->dir()*0.08,
    //                            0.08).contains(ballPos)) {
    //                    passFlag = true;
    //                }
    //                else if(!Circle2D(mahiPlayMaker->pos(),0.3).contains(ballPos)) {
    //                    if(Circle2D(currentPlan.passPos,1)
    //                            .intersection(Ray2D(ballPos,
    //                                                ballPos + ballVel),
    //                                          &sol1, &sol2)) {
    //                        if(passFlag) {
    //                            passFlag   = false;
    //                            repeatFlag = true;
    //                            passerID   = mahiPlayMaker->id();
    //                        }
    //                    }
    //                }
    //            }
    //        }
    //    }        /////////ISPASSNOTDONE///////////
    //    if(repeatFlag && (counter < 100)) {
    //        counter++;
    //        repeatFlag = true;
    //        for(size_t i = 0; i  < agents.size();i++) {
    //            if(roleAgents[i]->getAgent()->id() == passerID)
    //                lastPasserRoleIndex = i;
    //        }

    //        roleAgents[lastPasserRoleIndex]->setAgent(knowledge->getAgent(passerID));
    //        roleAgents[lastPasserRoleIndex]->setAgentID(passerID);
    //        roleAgents[lastPasserRoleIndex]->setAvoidPenaltyArea(true);
    //        roleAgents[lastPasserRoleIndex]->setTarget(Vector2D(0,0));
    //        roleAgents[lastPasserRoleIndex]->setReceiveRadius(0.1);
    //        roleAgents[lastPasserRoleIndex]->setSelectedSkill(DynamicEnums::Mark);

    ////        counter    = 0;
    ////        repeatFlag = false;
    ////        passerID   = -1;
    //    }
    //    else {
    //        passerID = -1;
    //    }

    //    debug(QString("PasserID %1 , Counter : %2").arg(passerID).arg(counter),D_MAHI);
    //    debug(QString("playMake %1 , skill : %2"  ).arg(mahiPlayMaker->id())
    //          .arg(roleAgentPM->getSelectedSkill()),D_MAHI);
    //    //    debug(QString("LPRI %1 , skill : %2").arg(mahiPlayMaker->id()).arg(roleAgentPM->getSelectedSkill()),D_MAHI);

}

int CDynamicAttack::farGuardFromPoint(const int &_guardIndex,
                                      const Vector2D &_point) {
    double tempDist, tempMax = 0;
    int tempIndex = -1;
    for (int i = 0; i < 3; i++) {
        tempDist = guardLocations[currentPlan.agentSize]
                [guardIndexList.at(_guardIndex)]
                [i].dist(_point);
        if (i == lastGuards[_guardIndex]) {
            tempDist += 0.3;
        }
        if (tempDist  > tempMax) {
            tempMax = tempDist;
            tempIndex = i;
        }
    }
    return tempIndex;
}

void CDynamicAttack::checkPoints(QList<Vector2D>& _points) {

    Rect2D validField(-wm->field->_FIELD_WIDTH / 2,
                      wm->field->_FIELD_HEIGHT / 2,
                      wm->field->_FIELD_WIDTH,
                      wm->field->_FIELD_HEIGHT);

    for (int i = 0; i < _points.size() - 1; i++) {
        if (!validField.contains(_points.at(i))) {
            _points.removeAt(i);
        }
    }
}

int CDynamicAttack::minHorizontalDistID(const QList<Vector2D> &_points) {
    double tempDist, minDist = 100000;
    int tempIndex = -1;

    for (int i = 0; i < _points.size(); i++) {
        tempDist = fabs(wm->ball->pos.y - _points.at(i).y);
        if (lastPassPos == i) {
            tempDist -= 2;
        }
        if (tempDist < minDist && fabs(wm->ball->pos.y - _points.at(i).y) > 0.2) {
            minDist = tempDist;
            tempIndex = i;
        }
    }
    lastPassPos = tempIndex;
    return tempIndex;
}

int CDynamicAttack::maxHorizontalDistID(const QList<Vector2D> &_points) {
    double tempDist, maxDist = -1;
    int tempIndex = -1;

    for (int i = 0; i < _points.size(); i++) {
        tempDist = fabs(wm->ball->pos.y - _points.at(i).y);
        if (lastPassPos == i) {
            tempDist += 2;
        }
        if (tempDist > maxDist && fabs(wm->ball->pos.y - _points.at(i).y) > 0.2) {
            maxDist = tempDist;
            tempIndex = i;
        }
    }
    lastPassPos = tempIndex;
    return tempIndex;
}

void CDynamicAttack::assignLocations() {
    // order of call function is important
    assignLocations_0();
    assignLocations_1();
    assignLocations_2();
    assignLocations_3();
    assignLocations_4();
    assignLocations_5();
    assignLocations_6();
    ////////////////////////////////////////

}

void CDynamicAttack::assignRegions() {
    // order of call function is important
    assignRegion_0();
    assignRegion_1();
    assignRegion_2();
    assignRegion_3();
    assignRegion_4();
    assignRegion_5();
    assignRegion_6();
    ////////////////////////////////////////
}

void CDynamicAttack::assignRegion_0() {
    //Opp Field
    guards[0]->assign(-wm->field->_CENTER_CIRCLE_RAD,                 //top_X
                      wm->field->_FIELD_HEIGHT / 2,                   //top_Y
                      wm->field->_FIELD_WIDTH / 2 + wm->field->_CENTER_CIRCLE_RAD, //X_Length
                      wm->field->_FIELD_HEIGHT);                      //Y_Length
}


void CDynamicAttack::assignRegion_1() {
    //Opp Field
    guards[1][0].assign(-wm->field->_CENTER_CIRCLE_RAD,                 //top_X
                        wm->field->_FIELD_HEIGHT / 2,                   //top_Y
                        wm->field->_FIELD_WIDTH / 2 + wm->field->_CENTER_CIRCLE_RAD, //X_Length
                        wm->field->_FIELD_HEIGHT);                      //Y_Length

}


void CDynamicAttack::assignRegion_2() {
    //Top Opp Half
    guards[2][0].assign(-wm->field->_CENTER_CIRCLE_RAD,                //top_X
                        wm->field->_FIELD_HEIGHT / 2,                  //top_Y
                        wm->field->_FIELD_WIDTH / 2 + wm->field->_CENTER_CIRCLE_RAD, //X_Length
                        wm->field->_FIELD_HEIGHT / 2);                 //Y_Length

    //Bottom Opp Half
    guards[2][1].assign(-wm->field->_CENTER_CIRCLE_RAD,                 //top_X
                        0,                                   //top_Y
                        wm->field->_FIELD_WIDTH / 2 + wm->field->_CENTER_CIRCLE_RAD, //X_Length
                        wm->field->_FIELD_HEIGHT / 2);                  //Y_Length

}


void CDynamicAttack::assignRegion_3() {
    //Top Opp Tertium
    guards[3][0].assign(-wm->field->_CENTER_CIRCLE_RAD,                 //top_X
                        wm->field->_FIELD_HEIGHT / 2,                   //top_Y
                        wm->field->_FIELD_WIDTH / 2 + wm->field->_CENTER_CIRCLE_RAD, //X_Length
                        wm->field->_FIELD_HEIGHT / 3);                  //Y_Length

    //Middle Opp Tertium
    guards[3][1].assign(-wm->field->_CENTER_CIRCLE_RAD,                 //top_X
                        wm->field->_FIELD_HEIGHT / 2 - wm->field->_FIELD_HEIGHT / 3, //top_Y
                        wm->field->_FIELD_WIDTH / 2 + wm->field->_CENTER_CIRCLE_RAD, //X_Length
                        wm->field->_FIELD_HEIGHT / 3);                  //Y_Length

    //Bottom Opp Tertium
    guards[3][2].assign(-wm->field->_CENTER_CIRCLE_RAD,                 //top_X
                        wm->field->_FIELD_HEIGHT / 2 - 2 * wm->field->_FIELD_HEIGHT / 3, //top_Y
                        wm->field->_FIELD_WIDTH / 2 + wm->field->_CENTER_CIRCLE_RAD, //X_Length
                        wm->field->_FIELD_HEIGHT / 3);                  //Y_Length

}


void CDynamicAttack::assignRegion_4() {
    //Top Opp 1/4
    guards[4][0].assign(-wm->field->_CENTER_CIRCLE_RAD,                 //top_X
                        wm->field->_FIELD_HEIGHT / 2,                   //top_Y
                        wm->field->_FIELD_WIDTH / 2 + wm->field->_CENTER_CIRCLE_RAD, //X_Length
                        wm->field->_FIELD_HEIGHT / 4);                  //Y_Length

    //Mid-Top Opp 1/4
    guards[4][1].assign(-wm->field->_CENTER_CIRCLE_RAD,                 //top_X
                        wm->field->_FIELD_HEIGHT / 4,                   //top_Y
                        wm->field->_FIELD_WIDTH / 2 + wm->field->_CENTER_CIRCLE_RAD, //X_Length
                        wm->field->_FIELD_HEIGHT / 4);                  //Y_Length

    //Mid-Bottom Opp 1/4
    guards[4][2].assign(-wm->field->_CENTER_CIRCLE_RAD,                 //top_X
                        0,                                   //top_Y
                        wm->field->_FIELD_WIDTH / 2 + wm->field->_CENTER_CIRCLE_RAD, //X_Length
                        wm->field->_FIELD_HEIGHT / 4);                  //Y_Length                    //Y_Length
    //Bottom Opp 1/4
    guards[4][3].assign(-wm->field->_CENTER_CIRCLE_RAD,                 //top_X
                        -wm->field->_FIELD_HEIGHT / 4,                  //top_Y
                        wm->field->_FIELD_WIDTH / 2 + wm->field->_CENTER_CIRCLE_RAD, //X_Length
                        wm->field->_FIELD_HEIGHT / 4);                  //Y_Length

}


void CDynamicAttack::assignRegion_5() {
    for (int i = 0; i < 4; i++) {
        guards[5][i].assign(guards[4][i].topLeft(), guards[4][i].size());
    }

    //BackUp Line
    guards[5][4].assign(-wm->field->_CENTER_CIRCLE_RAD - 1,             //top_X
                        wm->field->_FIELD_HEIGHT / 2,                   //top_Y
                        1,                                   //X_Length
                        wm->field->_FIELD_HEIGHT);                      //Y_Length

}


void CDynamicAttack::assignRegion_6() {
    for (int i = 0; i < 5; i++) {
        guards[6][i].assign(guards[5][i].topLeft(), guards[5][i].size());
    }

    //BackUp Line
    guards[6][5].assign(-wm->field->_FIELD_WIDTH / 2 + wm->field->_FIELD_PENALTY_POINT,      //top_X
                        wm->field->_PENALTY_WIDTH / 2,                      //top_Y
                        wm->field->_FIELD_WIDTH / 2 - 1 - wm->field->_CENTER_CIRCLE_RAD, //X_Length
                        wm->field->_PENALTY_WIDTH);                         //Y_Length

}

void CDynamicAttack::showRegions(unsigned int agentSize,
                                 QColor color) {

    for (size_t i = 0; i < agentSize; i++) {
        drawer->draw(guards[agentSize][i], color);
    }
}

void CDynamicAttack::showLocations(unsigned int agentSize,
                                   QColor color) {
    for (size_t i = 0; i < agentSize; i++) { //RegionsCount
        for (size_t j = 0; j < 3; j++) {
            drawer->draw(guardLocations[agentSize][i][j], color);
        }
    }
}

void CDynamicAttack::assignLocations_0() {
    //    guardLocations[0][0][0].assign(0,0);
    //    guardLocations[0][0][1].assign(0,0);
    //    guardLocations[0][0][2].assign(0,0);
}


void CDynamicAttack::assignLocations_1() {
    //Opp Feild
    guardLocations[1][0][0].assign(wm->field->_FIELD_WIDTH / 2 - 1,  wm->field->_FIELD_HEIGHT / 4 + 0.5);
    guardLocations[1][0][1].assign(wm->field->_FIELD_WIDTH / 4, 0);
    guardLocations[1][0][2].assign(wm->field->_FIELD_WIDTH / 2 - 1, -wm->field->_FIELD_HEIGHT / 4 - 0.5);
}

void CDynamicAttack::assignLocations_2() {
    //Top Opp Half
    const int offset = 1;
    guardLocations[2][0][0].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 1 + offset, wm->field->_FIELD_HEIGHT / 4);
    guardLocations[2][0][1].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 2 + offset, wm->field->_FIELD_HEIGHT / 4);
    guardLocations[2][0][2].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 3 + offset, wm->field->_FIELD_HEIGHT / 4);

    //Bottom Opp Half
    guardLocations[2][1][0].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 1 + offset, -wm->field->_FIELD_HEIGHT / 4);
    guardLocations[2][1][1].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 2 + offset, -wm->field->_FIELD_HEIGHT / 4);
    guardLocations[2][1][2].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 3 + offset, -wm->field->_FIELD_HEIGHT / 4);

}

void CDynamicAttack::assignLocations_3() {
    //Top Opp Tertium
    const int offset = 1;
    guardLocations[3][0][0].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 1 + offset, wm->field->_FIELD_HEIGHT / 3);
    guardLocations[3][0][1].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 2 + offset, wm->field->_FIELD_HEIGHT / 3);
    guardLocations[3][0][2].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 3 + offset, wm->field->_FIELD_HEIGHT / 3);
    //Middle Opp Tertium
    guardLocations[3][1][0].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 1 + offset, 0);
    guardLocations[3][1][1].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 2 + offset, 0);
    guardLocations[3][1][2].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 3 + offset, 0);
    //Bottom Opp Tertium
    guardLocations[3][2][0].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 1 + offset, -wm->field->_FIELD_HEIGHT / 3);
    guardLocations[3][2][1].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 2 + offset, -wm->field->_FIELD_HEIGHT / 3);
    guardLocations[3][2][2].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 3 + offset, -wm->field->_FIELD_HEIGHT / 3);
}

void CDynamicAttack::assignLocations_4() {
    const int offset = 1;
    // Top Opp 1/4
    guardLocations[4][0][0].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 1 + offset, 3 * wm->field->_FIELD_HEIGHT / 8);
    guardLocations[4][0][1].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 2 + offset, 3 * wm->field->_FIELD_HEIGHT / 8);
    guardLocations[4][0][2].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 3 + offset, 3 * wm->field->_FIELD_HEIGHT / 8);
    // Mid-Top Opp 1/4

    guardLocations[4][1][0].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 1 + offset, wm->field->_FIELD_HEIGHT / 8);
    guardLocations[4][1][1].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 2 + offset, wm->field->_FIELD_HEIGHT / 8);
    guardLocations[4][1][2].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 3 + offset, wm->field->_FIELD_HEIGHT / 8);
    // Mid-Bottom Opp 1/4

    guardLocations[4][2][0].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 1 + offset, -wm->field->_FIELD_HEIGHT / 8);
    guardLocations[4][2][1].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 2 + offset, -wm->field->_FIELD_HEIGHT / 8);
    guardLocations[4][2][2].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 3 + offset, -wm->field->_FIELD_HEIGHT / 8);

    // Bottom Opp 1/4
    guardLocations[4][3][0].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 1 + offset, -3 * wm->field->_FIELD_HEIGHT / 8);
    guardLocations[4][3][1].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 2 + offset, -3 * wm->field->_FIELD_HEIGHT / 8);
    guardLocations[4][3][2].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 3 + offset, -3 * wm->field->_FIELD_HEIGHT / 8);
}

void CDynamicAttack::assignLocations_5() {
    const int offset = 1;
    // Top Opp 1/4
    guardLocations[5][0][0].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 1 + offset, 3 * wm->field->_FIELD_HEIGHT / 8);
    guardLocations[5][0][1].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 2 + offset, 3 * wm->field->_FIELD_HEIGHT / 8);
    guardLocations[5][0][2].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 3 + offset, 3 * wm->field->_FIELD_HEIGHT / 8);
    // Mid-Top Opp 1/4

    guardLocations[5][1][0].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 1 + offset, wm->field->_FIELD_HEIGHT / 8);
    guardLocations[5][1][1].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 2 + offset, wm->field->_FIELD_HEIGHT / 8);
    guardLocations[5][1][2].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 3 + offset, wm->field->_FIELD_HEIGHT / 8);
    // Mid-Bottom Opp 1/4

    guardLocations[5][2][0].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 1 + offset, -wm->field->_FIELD_HEIGHT / 8);
    guardLocations[5][2][1].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 2 + offset, -wm->field->_FIELD_HEIGHT / 8);
    guardLocations[5][2][2].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 3 + offset, -wm->field->_FIELD_HEIGHT / 8);

    // Bottom Opp 1/4
    guardLocations[5][3][0].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 1 + offset, -3 * wm->field->_FIELD_HEIGHT / 8);
    guardLocations[5][3][1].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 2 + offset, -3 * wm->field->_FIELD_HEIGHT / 8);
    guardLocations[5][3][2].assign((wm->field->_FIELD_WIDTH - offset) / 3 * 3 + offset, -3 * wm->field->_FIELD_HEIGHT / 8);

    // BackUp-Line
    guardLocations[5][4][0].assign(0, +wm->field->_FIELD_HEIGHT / 3);
    guardLocations[5][4][1].assign(0, 0);
    guardLocations[5][4][2].assign(0, -wm->field->_FIELD_HEIGHT / 3);

}

void CDynamicAttack::assignLocations_6() {
    // Top Opp 1/4
    guardLocations[6][0][0].assign(1  , 2.5);
    guardLocations[6][0][1].assign(2.5, 2.5);
    guardLocations[6][0][2].assign(3.5, 2.5);
    // Mid-Top Opp 1/4

    guardLocations[6][1][0].assign(1  , 1);
    guardLocations[6][1][1].assign(2.5, 1);
    guardLocations[6][1][2].assign(3.5, 1);
    // Mid-Bottom Opp 1/4

    guardLocations[6][2][0].assign(1  , -1);
    guardLocations[6][2][1].assign(2.5, -1);
    guardLocations[6][2][2].assign(3.5, -1);

    // Bottom Opp 1/4
    guardLocations[6][3][0].assign(1  , -2.5);
    guardLocations[6][3][1].assign(2.5, -2.5);
    guardLocations[6][3][2].assign(3.5, -2.5);

    // BackUp-Line
    guardLocations[6][4][0].assign(-1 ,  2);
    guardLocations[6][4][1].assign(-1 ,  0);
    guardLocations[6][4][2].assign(-1 , -2);

    // Goal Area
    guardLocations[6][5][0].assign(-3.5 ,  0);
    guardLocations[6][5][1].assign(-2.5 ,  0);
    guardLocations[6][5][2].assign(-1.5 ,  0);


}

QString CDynamicAttack::getString(const DynamicMode &_mode) const {
    switch (_mode) {
    default:
    case DynamicMode::NoMode:
        return QString("NoMode");
    case DynamicMode::DefenseClear:
        return QString("DefenseClear");
    case DynamicMode::NotWeHaveBall:
        return QString("NotWeHaveBall");
    case DynamicMode::DirectKick:
        return QString("DirectKick");
    case DynamicMode::Fast:
        return QString("Fast");
    case DynamicMode::Critical:
        return QString("Critical");
    case DynamicMode::Plan:
        return QString("NewPlan");
    case DynamicMode::Forward:
        return QString("Ball In Our Field");
    case DynamicMode::NoPositionAgent:
        return QString("No Agent");
    }
}


void CDynamicAttack::setDefenseClear(bool _isDefenseClearing) {
    isDefenseClearing = _isDefenseClearing;
}

void CDynamicAttack::setDirectShot(bool _directShot) {
    directShot = _directShot;
}

void CDynamicAttack::setPositions(QList<int> _positioningRegion) {
    regionsList.clear();
    dynamicPosition.clear();
    for (int i : _positioningRegion) {
        regionsList.append(i);
        //        dynamicPosition.append(knowledge->getStaticPoses
        //                               (_positioningRegion.at(i))); // TODO : Static Pos
    }
}

void CDynamicAttack::setPlayMake(Agent* _playMake) {
    playmakeID = _playMake->id();
    playmake = _playMake;
}

void CDynamicAttack::setWeHaveBall(bool _ballPoss) {
    isWeHaveBall = _ballPoss;
}

void CDynamicAttack::setNoPlanException(bool _noPlanException) {
    noPlanException = _noPlanException;
}

void CDynamicAttack::setCritical(bool _critical) {
    critical = _critical;
}

void CDynamicAttack::setBallInOppJaw(bool _ballInOppJaw) {
    ballInOppJaw = _ballInOppJaw;
}

void CDynamicAttack::setFast(bool _fast) {
    fast = _fast;
}

void CDynamicAttack::choosePlan() {
    if (isPlanDone()) {
        currentPlan = *nextPlanA;
    } else if (isPlanFailed()) {
        currentPlan = *nextPlanB;
    }

}

bool CDynamicAttack::isPlanFailed() {
    switch (currentPlan.mode) {

    case DynamicMode::NoMode:
        return false;
        break;
    case DynamicMode::CounterAttack:break;
    case DynamicMode::DefenseClear:break;
    case DynamicMode::DirectKick:break;
    case DynamicMode::Fast:break;
    case DynamicMode::Critical:break;
    case DynamicMode::NotWeHaveBall:break;
    case DynamicMode::Plan:break;
    case DynamicMode::Forward:break;
    case DynamicMode::NoPositionAgent:break;
    case DynamicMode::BallInOppJaw:break;
    }
    return false;
}

bool CDynamicAttack::isPlanDone() {
    switch (currentPlan.mode) {

    case DynamicMode::NoMode:
        return true;
        break;
    case DynamicMode::CounterAttack:break;
    case DynamicMode::DefenseClear:break;
    case DynamicMode::DirectKick:break;
    case DynamicMode::Fast:break;
    case DynamicMode::Critical:break;
    case DynamicMode::NotWeHaveBall:break;
    case DynamicMode::Plan:break;
    case DynamicMode::Forward:break;
    case DynamicMode::NoPositionAgent:break;
    case DynamicMode::BallInOppJaw:break;
    }
}


// NEW PASS
double f1(long double &tot, long double a, long double b)
{
    tot += b;
    if (a>1.0) a=1.0;
    if (a<0.0) a=0.0;
    return a*b;
}

double f1(long double a, long double b)
{
    if (a>1.0) a=1.0;
    if (a<0.0) a=0.0;
    return a*b;
}

double fm1(long double a, long double b)
{
    long double r = a/b;
    if (r>1.0) r = 1.0;
    if (r<0.0) r = 0.0;
    return r;
}
void CDynamicAttack::createRegions()
{
    // <make rectangles>
    QList<QList<Rect2D>> rectangles;
    QList<Rect2D> row0;
    QList<Rect2D> row1;
    QList<Rect2D> row2;
    for(int i{0}; i<3; i++)
    {
        row0.push_back(Rect2D(Vector2D(2*i,4.5), 2, 2.8));
        row1.push_back(Rect2D(Vector2D(2*i,1.7), 2, 3.4));
        row2.push_back(Rect2D(Vector2D(2*i,-1.7), 2, 2.8));
    }
    rectangles.push_back(row0);
    rectangles.push_back(row1);
    rectangles.push_back(row2);
    // </make rectangles>
    // <make eval points>
    QList<Vector2D> points[3][3];
    for(int i{0}; i<3; i++)
    {
        for(int j{0}; j<3; j++)
        {
            points[i][j].push_back(rectangles[i][j].center());
        }
    }
    // </make eval points>
    // <fill the regions>
    int _id = 0;
    for(int i{0}; i<3; i++)
    {
        for(int j{0}; j<3; j++)
        {
            regions[i][j] = FieldRegion(rectangles[i][j], points[i][j]);
            regions[i][j].id = _id++;
        }
    }
    regions[1][2].points[0] += Vector2D(-0.6, 0);
    // </fill the regions>
}

void CDynamicAttack::chooseBestPositons_new()
{
    clearRobotsRegionsWeights();

    // get the search regions
    QList<Rect2D> searchRegions;
    for(int i{0}; i<3; i++)
    {
        for(int j{0}; j<3; j++)
        {
            searchRegions.append(regions[i][j].rectangle);
        }
    }
    QList<Rect2D> avoidRects;
    avoidRects.append(wm->field->oppPenaltyRect());

    // get the pass sender
    int passSenderID{playmakeID};
    ROS_INFO_STREAM("hamid playkame ID: " << passSenderID);
    Vector2D passSenderPos;
    Vector2D passSenderDir;
    if(passSenderID != -1)
    {
        passSenderPos = wm->our[passSenderID]->pos;
        passSenderDir  = wm->our[passSenderID]->dir;
        ourRelaxedIDs.append(passSenderID);
    }
    else
    {
        passSenderPos.invalidate();
    }

    // this section determines the factors and calculates the probability of those factors
    for (auto& passReciever : agents)
    {
        auto passRecieverID = passReciever->id();
        if(passRecieverID != passSenderID)
        {
            // getting the pass receiver
            Vector2D passRecieverPos;
            Vector2D passRecieverDir;
            if(passRecieverID != -1)
            {
                passRecieverPos = wm->our[passRecieverID]->pos;
                passRecieverDir = wm->our[passRecieverID]->dir;
                ourRelaxedIDs.append(passRecieverID);
            }
            else
            {
                passRecieverPos.invalidate();
            }
            ROS_INFO_STREAM("hamid pass receiverpos: (" << passRecieverPos.x << ", " << passRecieverPos.y);
            for(int region_id{0}; region_id<9; region_id++)
            {
                ROS_INFO_STREAM("hamid said hi ");
//                Vector2D bestPoint(Vector2D::ERROR_VALUE, Vector2D::ERROR_VALUE);
                Vector2D bestPoint(regions[region_id/3][region_id%3].rectangle.center());
                double maxProbability = 0;
                for(auto& point : regions[region_id/3][region_id%3].points)
                {
                    for(int i{0}; i<avoidRects.count(); i++)
                    {
                        if (avoidRects[i].contains(point))
                            continue;
                    }

                    double prob = 0.0;
                    //factors for pass
                    double receiverDistanceFactor = 0; // be the closer than opp robots (temp), could change based on timing
                    double senderDistanceFactor = 0; // being within a specified intervval (temp), ccould change based on timing
                    double clearPathFactor = 0; // if the path to the point is clear for receiving robot
                    double widenessFactor = 0;

                    // factors for shoot
                    double oneTouchAngleFactor = 0; // if the angle to the opp goal is whitin a desird interval
                    double shootFactor = 0;

                    getBestPosToShootToGoal(point, shootFactor, true);
                    receiverDistanceFactor = calcReceiverDistanceFactor(point, passRecieverID);
                    senderDistanceFactor = calcSenderDistanceFactor(passSenderPos, point);
                    clearPathFactor = caclClearPathFactor(point, passSenderPos, ROBOT_RADIUS);
                    oneTouchAngleFactor = calcOneTouchAngleFactor(point, passSenderPos);
                    widenessFactor = calcWidenessFactor(passSenderPos, point);

                    double f = 1.0;
                    prob += f1(shootFactor,2.0*f);
                    prob += f1(widenessFactor,0.5*f);
                    prob += f1(receiverDistanceFactor,2.0*f);
                    prob += f1(senderDistanceFactor,0.1*f);
                    prob += f1(clearPathFactor,1.0*f);
                    prob += f1(oneTouchAngleFactor,0.1*f);
                    prob  = fm1(prob,5.2*f);

                    if( prob > maxProbability )
                    {
                    maxProbability = prob;
                    bestPoint = point;
                    }
                }
                robotRegionsWeights[passRecieverID][region_id] = maxProbability;
                ROS_INFO_STREAM("hamid between best assignmets");
                bestPointForRobotsInRegions[passRecieverID][region_id] = bestPoint;
                ROS_INFO_STREAM("hamid end of best assignmets");

            }
        }
    }
}

void CDynamicAttack::assignId_new()
{
    mahiPlayMaker = nullptr;
    if (playmakeID != -1) {
        mahiPlayMaker = playmake;
    }
    QList<int> robotIDs;
    MWBM matcher;
    for(int k{0}; k<11; k++)
    {
        if(robotRegionsWeights[k][0] != -1.0)
            robotIDs.append(k);
    }
    for(int i{0}; i<robotIDs.count(); i++)
        ROS_INFO_STREAM("hamid robot ids: " << robotIDs[i] << " ");
    if(robotIDs.count() != agents.count())
        ROS_INFO_STREAM("hamid counts are not equal");
    matcher.create(robotIDs.count(), 9);
    for(int i{0}; i< robotIDs.count(); i++)
    {
        for(int j{0}; j<9; j++)
        {
            matcher.setWeight(i, j, robotRegionsWeights[robotIDs[i]][j]);
        }
    }
    matcher.findMatching();
    mahiPositionAgents.clear();
    semiDynamicPosition.clear();
    for(int i{0}; i<8; i++)
    {mahiAgentsID[i] = -1;}
    //HAMID THERE
    for(int v = 0; v<robotIDs.count(); v++)
    {
        mahiAgentsID[v] = matcher.getMatch(v);
        //matchingIDs.append(robotIDs.at(v)); matchingRegions.append(matcher.getMatch(v));
        semiDynamicPosition.append(bestPointForRobotsInRegions[robotIDs.at(v)][matcher.getMatch(v)]);
        for(auto& agent : agents)
        {
            if(agent->id() == robotIDs.at(v))
            {
                mahiPositionAgents.append(agent);
            }
        }
    }
    ROS_INFO_STREAM("hamid after matching semidynamic points");
    hamidDebug();
}


void CDynamicAttack::chooseBestPosForPass_new(QList<Vector2D> semiDynamicPosition)
{

}

void CDynamicAttack::assignTasks_new()
{

}

Vector2D CDynamicAttack::getBestPosToShootToGoal(Vector2D from, double &regionWidth, bool oppGaol )
{
    Rect2D playingField(wm->field->ourCornerL(), wm->field->oppCornerR());
    if( ! playingField.contains(from) )
    {
        regionWidth = 0.0;
        double goalProbablity = 0.0;
        auto shootPos = Vector2D(Vector2D::ERROR_VALUE, Vector2D::ERROR_VALUE);
        return shootPos;
    }
    Vector2D goal;
    Vector2D goalL;
    Vector2D goalR;
    if( oppGaol )
    {
        goal = wm->field->oppGoal();
        goalL = wm->field->oppGoalL();
        goalR = wm->field->oppGoalR();
    }
    else
    {
        goal = wm->field->ourGoal();
        goalL = wm->field->ourGoalL();
        goalR = wm->field->ourGoalR();
    }
    double StepOnGoal = _GOAL_WIDTH / _GOAL_STEP;
    double MaxRegionWidth = 0 , MaxRegionTemp = 0;
    double BeginPos = 0 , EndPos = 0;
    Vector2D MaxRegionCenter(Vector2D::ERROR_VALUE , Vector2D::ERROR_VALUE) , RegionCenterTemp;
    bool WasLastPosClear = false;

    for( double y = goalR.y ; y <= goalL.y ; y += StepOnGoal ){
        Vector2D pos(goal.x , y);
        if( WasLastPosClear == false )
            BeginPos = y;

        if( this->isPathClear(pos, from, (ROBOT_RADIUS + 2*CBall::radius), true) ){
            WasLastPosClear = true;
            //            draw(Segment2D(from,pos) , "white");
        }
        else
            WasLastPosClear = false;
        EndPos = y;
        if( WasLastPosClear ){
            RegionCenterTemp = Segment2D(goalL , goalR).intersection(Line2D(from,(from+Vector2D(Vector2D(goal.x,BeginPos)-from).rotate(Vector2D::angleBetween( Vector2D(goal.x,BeginPos)-from , Vector2D(goal.x,EndPos)-from).degree()/2))));
            if( RegionCenterTemp.x == Vector2D::ERROR_VALUE || RegionCenterTemp.y == Vector2D::ERROR_VALUE )
                RegionCenterTemp = Vector2D(goal.x , (EndPos - BeginPos)/2);
            MaxRegionTemp = (EndPos - BeginPos + 0.001)*from.dist(Line2D(goalL , goalR).projection(from))/from.dist(RegionCenterTemp);
            if( MaxRegionWidth < MaxRegionTemp ){
                MaxRegionWidth = MaxRegionTemp;
                MaxRegionCenter = RegionCenterTemp;
            }
        }
    }
    //    if( MaxRegionCenter.x != Vector2D::ERROR_VALUE  )
    //    {
    //        if( oppGaol )
    //        {
    //            regionWidth = (MaxRegionWidth / _GOAL_WIDTH) * 0.7 +
    //                          ((Vector2D::dirTo_deg(from,goalL) - Vector2D::dirTo_deg(from,goalR)) / 180.0) * 0.3;
    //            goalProbablity = regionWidth;
    //        }
    //        else
    //        {
    //            double dirL = Vector2D::dirTo_deg(from,goalL);
    //            dirL = dirL < 0.0? dirL + 360.0 : dirL;
    //            double dirR = Vector2D::dirTo_deg(from,goalR);
    //            dirR = dirR < 0.0? dirR + 360.0 : dirR;

    //            regionWidth = (MaxRegionWidth / _GOAL_WIDTH) * 0.7 +
    //                          ((dirR - dirL) / 180.0) * 0.3;
    //            goalProbablity = regionWidth;
    //        }
    //        shootPos = MaxRegionCenter;
    //        draw(Circle2D(shootPos, 0.05), 0, 360, "blue", true);
    //        return shootPos;
    //    }
    double goalProbablity;
    Vector2D shootPos;
    if( MaxRegionCenter.x != Vector2D::ERROR_VALUE && MaxRegionCenter.y != Vector2D::ERROR_VALUE ){
        regionWidth = MaxRegionWidth / _GOAL_WIDTH ;
        goalProbablity = regionWidth;
        Vector2D shootPos = MaxRegionCenter;
        //        draw(Segment2D(from,shootPos) , "red");
        return shootPos;
    }
    regionWidth = 0.0;
    goalProbablity = 0.0;
    shootPos = Vector2D(Vector2D::ERROR_VALUE, Vector2D::ERROR_VALUE);
    return shootPos;
}

bool CDynamicAttack::isPathClear(Vector2D point, Vector2D from, double rad, bool considerRelaxedIDs){
    Vector2D posIntersect1(Vector2D::ERROR_VALUE, Vector2D::ERROR_VALUE);
    Vector2D posIntersect2(Vector2D::ERROR_VALUE, Vector2D::ERROR_VALUE);
    Segment2D l(from, point);
    for(int i = 0; i < wm->opp.activeAgentsCount(); i++){
        if((wm->opp.active(i)->inSight > 0.0)){
            if(considerRelaxedIDs && oppRelaxedIDs.contains(wm->opp.activeAgentID(i))){
                continue;
            }
            Circle2D c(wm->opp.active(i)->pos, rad);
            if(c.intersection(l,&posIntersect1, &posIntersect2) != 0){
                return false;
            }
        }
    }
    for(int i = 0; i < wm->our.activeAgentsCount(); i++){
        if (wm->our.active(i)->inSight > 0.0){
            if(considerRelaxedIDs && ourRelaxedIDs.contains(wm->our.activeAgentID(i))){
                continue;
            }
            Circle2D c(wm->our.active(i)->pos, rad);
            if(c.intersection(l,&posIntersect1, &posIntersect2) != 0){
                return false;
            }
        }
    }
    return true;
}

int CDynamicAttack::getNearestOppToPoint(Vector2D point) {
    double minDist = 10000.0;
    int nearest = -1;
    for (int i = 0; i < wm->opp.activeAgentsCount(); i++) {
        if (wm->opp.active(i)->inSight <= 0) {
            continue;
        }
        double dist = (wm->opp.active(i)->pos - point).length();
        if (dist < minDist) {
            minDist = dist;
            nearest = wm->opp.active(i)->id;
        }
    }
    return nearest;
}

void CDynamicAttack::clearRobotsRegionsWeights()
{
    for(int i{0}; i<11; i++)
    {
        for(int j{0}; j<9; j++)
        {
            robotRegionsWeights[i][j] = -1.0;
            bestPointForRobotsInRegions[i][j] = Vector2D(Vector2D::ERROR_VALUE, Vector2D::ERROR_VALUE);
        }
    }
}

double CDynamicAttack::calcReceiverDistanceFactor(Vector2D point, int passReceiverID)
{
    int nearestOppID = getNearestOppToPoint(point);
    if(nearestOppID == -1)
        return 1;
    double nearestOppDist = (wm->opp[nearestOppID]->pos - point).length();
    double receiverDist = (wm->our[passReceiverID]->pos - point).length();
    if (fabs(receiverDist) < 0.0005 )
        return 1;
    double res = nearestOppDist/receiverDist;
    if(res > 1 )
        return 0;
    else if (res < 0.05)
        return 1;
    else
        return 1.0 - res;
}

double CDynamicAttack::calcSenderDistanceFactor(Vector2D passSenderPos, Vector2D point)
{
    auto passSenderDist = (passSenderPos - point).length();
    if(passSenderDist > 10.0)
        return 0;
    else if (passSenderDist < 0.5)
        return 0;
    else
        return 1.0 - passSenderDist/5;
}

double CDynamicAttack::caclClearPathFactor(Vector2D point, Vector2D passSenderPos, double robot_raduis)
{
    if(isPathClear(point, passSenderPos, robot_raduis, true))
        return 1.0;
    else
        return 0.0;
}

double CDynamicAttack::calcOneTouchAngleFactor(Vector2D point, Vector2D passSenderPos)
{
    double oneTouchAngleFactor;
    double oneTouchAng = fabs((passSenderPos - point).th().degree() - (wm->field->oppGoal() - point).th().degree());
    if(oneTouchAng <= 45)
    {
        oneTouchAngleFactor = 0.9*(oneTouchAng/45.0);
    }
    else if(oneTouchAng <= 90)
    {
        oneTouchAngleFactor = 0.9 + 0.1*(90-oneTouchAng)/45;
    }
    else
    {
        oneTouchAngleFactor = 0;
    }
    return oneTouchAngleFactor;
}

double CDynamicAttack::calcWidenessFactor(Vector2D passSenderPos, Vector2D point)
{
    double widenessAngle = fabs((passSenderPos - wm->field->oppGoal()).th().degree() - (wm->field->oppGoal() - point).th().degree());
    if (widenessAngle < 0.005)
        return 0.0;
    if (widenessAngle > 170)
        return 1.0;
    return widenessAngle/180.0;
}

void CDynamicAttack::hamidDebug()
{
    for(int i{0}; i<11; i++)
    {
        ROS_INFO_STREAM("hamid weights of row " << i << " : " << robotRegionsWeights[i][0]
                                                << " " << robotRegionsWeights[i][1] << " " << robotRegionsWeights[i][2] << " "
                                                << robotRegionsWeights[i][3] << " " << robotRegionsWeights[i][4] << " "
                                                << robotRegionsWeights[i][5] << " " << robotRegionsWeights[i][6] << " "
                                                << robotRegionsWeights[i][7] << " " << robotRegionsWeights[i][8]);
    }

    for(int i{0}; i<11; i++)
    {
        ROS_INFO_STREAM("hamid points of row " << i << " : " << bestPointForRobotsInRegions[i][0]
                                                << " " << bestPointForRobotsInRegions[i][1] << " " << bestPointForRobotsInRegions[i][2] << " "
                                                << bestPointForRobotsInRegions[i][3] << " " << bestPointForRobotsInRegions[i][4] << " "
                                                << bestPointForRobotsInRegions[i][5] << " " << bestPointForRobotsInRegions[i][6] << " "
                                                << bestPointForRobotsInRegions[i][7] << " " << bestPointForRobotsInRegions[i][8]);
    }

    for(int i{0}; i<semiDynamicPosition.count(); i++)
    {
        drawer->draw(Circle2D(semiDynamicPosition[i], 0.1), QColor("yellow"), true);
    }
    for(int i{0}; i<3; i++)
    {
        for(int j{0}; j<3; j++)
        {
            drawer->draw(regions[i][j].rectangle, QColor("red"));
            for(int k{0}; k<regions[i][j].points.count(); k++)
            {
                drawer->draw(Circle2D(regions[i][j].points[k], 0.1), QColor("red"));
            }
        }
    }
}
