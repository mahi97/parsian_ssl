#include <parsian_agent/kick.h>
#include <parsian_agent/config.h>
///////////////////////////////////////////////////////////////////////////////////////////////////////////

//double calcBallTime(Vector2D _pos,double _dec)
//{
//    double _dist = wm->ball->pos.dist(_pos);
//    double sol1,sol2;
//    double ballVel = knowledge->getRealBallVel();
//    double ballVel2 = ballVel*ballVel;
//    double timeIndep = 2*_dec*_dist;
//    if(-1*timeIndep > (ballVel2))
//        return -1;
//    else
//    {
//        sol1 = (-ballVel + sqrt(ballVel2 + timeIndep))/2*_dec;
//        sol2 = (-ballVel - sqrt(ballVel2 + timeIndep))/2*_dec;
//        //draw(QString("s1: %1 - s2: %2").arg(sol1).arg(sol2));
//        if(0)//sol1 > 0 )
//            return sol1;
//        else if(sol2 >0)
//            return sol2;
//        else
//            return -1;
//    }
//
//}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

INIT_SKILL(CSkillKick, "kick");

CSkillKick::CSkillKick(Agent *_agent) : CSkill(_agent)
{

    kickSpeed = 15;
    avoidOppPenaltyArea = true;
    chip = false;
    spin = 0;
    slow = false;
    veryFine = false;
    avoidPenaltyArea = false;
    tolerance = 3;
    agent0 = nullptr;
    interceptMode = false;
    dontKick = false;
    chipWait = 0;
    chipSpinSpeed = 1;
    readyToChip = false;
    ballflag = false;
    ballWasFar = false;
    sagMode = false;
    penaltyKick = false;
    kickMode = KDIRECT;
    kkRadThresh = 0;
    kkAngleThresh = 0;
    kkTAngleThresh = 0;
    kkKickThresh = 0;
    kkJTurnThresh = 0;
    gpa = new CSkillGotoPointAvoid(_agent);
    gpa->setSlowmode(false);
    angPid= new _PID(2,0,0,0,0);
    speedPid = new _PID(1,0,0,0,0);
    posPid = new _PID(1,0,0,0,0);
    veryFine = false;
    interceptMode = false;
    shotEmptySpot = false;
    kickAngTol = 50;
    isJturn = false;
    passProfiler = false;
    kickWithCenterOfDribbler = false;
    goalieMode = false;
    jTurnFromBack = false;
    playMakeMode = false;
    //fastIntercept =false;
}

CSkillKick::~CSkillKick()
{

    delete gpa;
    delete angPid;
    delete speedPid;
    delete posPid;
}

void CSkillKick::resetI()
{
}

double CSkillKick::progress()
{
    return 0.0;
}

bool CSkillKick::kickable()
{

    return false;
}

kckMode CSkillKick::decideMode()
{
    /////////////////// must renew

    gpa->setAddvel(Vector2D(0,0));
    Vector2D tempVec1, tempVec2;
    Circle2D ballArea(ballPos,0.13);
    Circle2D dribblerArea(agentPos+agentDir.norm()*0.1,0.25);

    Circle2D robotArea(agentPos,1);

    if(agentPos.dist(ballPos) < 1.5) {
        robotArea.assign(agentPos,max(agentPos.dist(ballPos) - 0.1, 0.01));
    }

    if(goalieMode)
        robotArea.assign(agentPos,0.5);
    if(ballPos.dist(agentPos) < 1)
        robotArea.assign(agentPos,max(ballPos.dist(agentPos) - 0.01, 0.01));
    Segment2D robotpath(agentPos,ballPos-(target-ballPos).norm()*0.15);
    Segment2D ballpath(wm->ball->pos,wm->ball->pos+wm->ball->vel.norm()*(12));
    double tempAngle;
    tempVec1 = target - ballPos;
    tempAngle = fabs((tempVec1.th() - (ballPos - agentPos).th()).degree());

    ///////////////////////////////find center of dribbler for pass profiler
    Polygon2D robotKickArea;

    double distCoef =   (4.5/ballPos.dist(target))/100;
    distCoef = min(distCoef,0.02);
    distCoef = max(distCoef,0.01);
//    debug(QString("distCoef: %1").arg(distCoef),D_MHMMD);

    robotKickArea.addVertex(agentPos+agent->dir().norm()*0.08+agent->dir().rotate(90).norm()*distCoef);
    robotKickArea.addVertex(agentPos+agent->dir().norm()*0.35+agent->dir().rotate(90).norm()*distCoef);
    robotKickArea.addVertex(agentPos+agent->dir().norm()*0.35-agent->dir().rotate(90).norm()*distCoef);
    robotKickArea.addVertex(agentPos+agent->dir().norm()*0.08-agent->dir().rotate(90).norm()*distCoef);

    // TODO : knowledge
    if(/*(!knowledge->isOurNonPlayOnKick())&&*/(passProfiler || kickWithCenterOfDribbler)) {
        kickerOn = dribblerArea.contains(ballPos) && robotKickArea.contains(ballPos);
    }
    else kickerOn = dribblerArea.contains(ballPos);

//    draw(QString("theta : %1").arg(fabs(((target-agentPos).th().degree() - (ballPos-agentPos).th().degree() ))) , Vector2D(-1,-1));



    if(playMakeMode && wm->field->isInOurPenaltyArea(ballPos))
    {
        return KAVOIDOPPENALTY;
    }
    if((!penaltyKick) && wm->field->isInOppPenaltyArea(ballPos) && !passProfiler && avoidOppPenaltyArea && !((robotArea.intersection(ballpath,&tempVec1,&tempVec2) ==2 && ballRealVel > 1 )))
    {
        return KAVOIDOPPENALTY;
    }


    if(dontKick)
    {
        return KDONTKICK;
    }

    return KDIRECT;
// TODO : WHY ?
    gpa->setDivemode(false);

    //////////////////////

}

void CSkillKick::kgoalie()
{
    if(shotEmptySpot)
        target = findMostPossible();
    angPid->kp = 4;

    if( ((ballPos - agentPos).th() - kickFinalDir).degree()  > 110  )
        kkMovementTheta = ((ballPos - agentPos).th().radian() + 1.2+1.5*(0.45-agentPos.dist(ballPos))- agentDir.th().radian());
    else if(((ballPos - agentPos).th() - kickFinalDir).degree() <- 110)
        kkMovementTheta = ((ballPos- agentPos).th().radian() - 1.2- 1.5*(0.45-agentPos.dist(ballPos))- agentDir.th().radian());
    else
        kkMovementTheta =((ballPos- agentPos).th().radian() - agentDir.th().radian());


    angPid->error = (kickFinalDir - agentDir.th()).radian();
    double reduce = 2;



    if(kickerOn)
    {
        reduce =0.7;
    }

    double ballx= (wm->ball->vel.x)*cos(agentDir.th().radian()) + (wm->ball->vel.y)*sin(agentDir.th().radian());
    double bally= -1*(wm->ball->vel.x)*sin(agentDir.th().radian()) + (wm->ball->vel.y)*cos(agentDir.th().radian());
    Segment2D ballpath(ballPos,ballPos+wm->ball->vel.norm()*10);
    Segment2D goalLine(wm->field->ourGoalL(),wm->field->ourGoalR());
    Vector2D targetInt;
    targetInt = goalLine.intersection(ballpath);

    agent->setRobotVel(reduce*cos(kkMovementTheta)*(1+ballx)
            ,reduce*sin(kkMovementTheta)*(1+bally) + 2*bally
            ,angPid->PID_OUT());
}

void CSkillKick::kWaitForTurn()
{
    Segment2D ballpath(wm->ball->pos,wm->ball->pos+wm->ball->vel.norm()*(10));
    Vector2D sol1;

    sol1 = ballpath.nearestPoint(agentPos);
//    agent->setRoller(3); // TODO : ROBOT Command
    gpa->init(sol1,ballPos - agentPos);
    //gpa->setADiveMode(false);
    gpa->setOnetouchmode(true);
    gpa->execute();
}

void CSkillKick::waitAndKick()
{

    Segment2D ballPath;
    double stopParam = 0.08;
    ballPath.assign(ballPos,ballPos + wm->ball->vel.norm()*(15));
    Segment2D ballLine;
    ballLine.assign(ballPos,ballPos + wm->ball->vel.norm()*(15));
//    draw(ballPath,"red");



    Vector2D oneTouchDir = Vector2D::unitVector(oneTouchAngle(agentPos, agent->vel(), wm->ball->vel,
                                                              agentPos - ballPos, target,conf->Landa, conf->Gamma));
    Vector2D kickerPoint = agentPos + agentDir.norm()*stopParam;
    Vector2D addVec = (agentPos - target).norm()*stopParam;
    Vector2D intersectPos;
    Vector2D sol1,sol2;
    double onetouchRad =2;
    double onetouchKickRad = 0.5;
    Circle2D oneTouchArea;
    Circle2D oppPenaltyArea(wm->field->oppGoal() + Vector2D(0.15,0),1.35);
    Circle2D oppPenaltyAreaWP(wm->field->oppGoal() + Vector2D(0.15,0),1.55);
//    draw(oppPenaltyAreaWP,QColor(Qt::red));
    ///temp
//    draw(oppPenaltyArea,QColor(Qt::red));

    if(ballPos.dist(agentPos) <= onetouchRad)
        onetouchRad = ballPos.dist(agentPos)-0.08;
    oneTouchArea.assign(agentPos,onetouchRad);
    gpa->setAddvel(Vector2D(0,0));

    gpa->setNoavoid(false);
    intersectPos = ballPath.nearestPoint(kickerPoint);
    //        if(wm->field->isInOppPenaltyArea(intersectPos) )
    //        {
    //            if(oppPenaltyArea.intersection(ballLine,&sol1,&sol2))
    //            {
    //                if(sol1.dist(agentPos) > sol2.dist(agentPos))
    //                {
    //                    sol1 = sol2;
    //                }
    //                intersectPos = sol1;
    //            }
    //        }


    gpa->init(intersectPos +addVec,oneTouchDir);

    gpa->setOnetouchmode(true);
    gpa->execute();
//    draw(intersectPos);
    // TODO : Robot Command
    if(agentPos.dist(ballPos) < 1)
    {
        if(chip)
        {
            agent->setChip(kickSpeed);
        }
        else
        {
            agent->setKick(kickSpeed);
        }
    }
    // agent->setRoller(3);


}



void CSkillKick::kDontKick()
{

    Vector2D finalPos;
    gpa->setSlowmode(true);
    finalPos=ballPos-(target-ballPos).norm()*0.23;
    gpa->setBallobstacleradius(0.4);
    if(fabs((kickFinalDir - agentDir.th()).degree()) < 10)
    {
        if(0 && wm->ball->pos.x < -1)
        {
            agent->setRoller(5);
            finalPos=ballPos-(target-ballPos).norm()*0.11;
        }
        else
        {
            agent->setRoller(0);
            finalPos=ballPos-(target-ballPos).norm()*0.13;
        }
        gpa->setSlowmode(true);
        gpa->setBallobstacleradius(0);
    }
    gpa->setDivemode(false);
    gpa->setNoavoid(false);
    gpa->init(finalPos, ballPos - agentPos);
    gpa->execute();
}

void CSkillKick::direct()
{
    findPosToGo();
    //findPosToGoAlt();
}


void CSkillKick::avoidOurPenalty()
{
    gpa->setSlowmode(slow);
    Vector2D finalPos ,dummyPos1,dummyPos;
    Vector2D tempVector;
    tempVector = ballPos - wm->field->ourGoal();
    Circle2D penaltyCircle;
    Segment2D ballSeg;
    Segment2D ballPosSeg;
    ballPosSeg.assign( wm->field->ourGoal(),wm->field->ourGoal() + tempVector.norm()*2);
    ballSeg.assign(ballPos,ballPos+wm->ball->vel.norm()*10);
    penaltyCircle.assign(wm->field->ourGoal() + Vector2D(0.15,0),1.4);


    penaltyCircle.intersection(ballPosSeg,&dummyPos1,&dummyPos);

    if(wm->field->isInField(dummyPos1))
    {
        finalPos = dummyPos1;
    }
//    draw(finalPos);
    if(ballPos.dist(agentPos) > 0.2)
        finalDirVec = target - agentPos;
    gpa->setDivemode(false);
    gpa->setAvoidpenaltyarea(true);
    gpa->setNoavoid(false);

    gpa->init(finalPos, finalDirVec);
    gpa->execute();
//    agent->setKick(kickSpeed);
}

void CSkillKick::avoidOppPenalty()
{
    gpa->setSlowmode(slow);
    Vector2D finalPos ,dummyPos1,dummyPos;
    Vector2D tempVector;
    tempVector = ballPos - wm->field->oppGoal();
    Circle2D penaltyCircle;
    Segment2D ballSeg;
    Segment2D ballPosSeg;
    Segment2D penaltyStraightLine;
    penaltyStraightLine.assign(Vector2D(wm->field->_FIELD_WIDTH/2 - wm->field->_FIELD_PENALTY -0.1,0.7 ),Vector2D(wm->field->_FIELD_WIDTH/2 - wm->field->_FIELD_PENALTY-0.1 ,-0.7 ));
    ballPosSeg.assign( wm->field->oppGoal(),wm->field->oppGoal() + tempVector.norm()*2);
    ballSeg.assign(ballPos,ballPos+wm->ball->vel.norm()*10);
    penaltyCircle.assign(wm->field->oppGoal() + Vector2D(0.15,0),1.4);

    if(wm->ball->vel.length()> 0.5)
    {
        penaltyCircle.intersection(ballSeg,&dummyPos1,&dummyPos);
        if(!wm->field->isInField(dummyPos1))
        {
            penaltyCircle.intersection(ballPosSeg,&dummyPos1,&dummyPos);
        }

    }
    else
    {
        penaltyCircle.intersection(ballPosSeg,&dummyPos1,&dummyPos);
    }

    if(wm->field->isInField(dummyPos1))
    {
        finalPos = dummyPos1;
    }
//    draw(finalPos);
    if(ballPos.dist(agentPos) > 0.2)
        finalDirVec = target - agentPos;
    gpa->setDivemode(false);
    gpa->setAvoidpenaltyarea(true);
    gpa->setNoavoid(false);

    //    if(!wm->field->isInOppPenaltyArea(finalPos + (wm->field->oppGoal() - finalPos).norm()*0.1))
    //    {
    //        if(penaltyStraightLine.intersection(ballPosSeg).isValid())
    //        {
    //            finalPos = penaltyStraightLine.intersection(ballPosSeg);
    //        }
    //    }
    gpa->init(finalPos, finalDirVec);
    gpa->execute();
//    agent->setKick(kickSpeed); // TODO : Robot Command
}

void CSkillKick::indirect()
{

    if(shotEmptySpot)
        target = findMostPossible();
    Vector2D sol1,sol2;

    Circle2D ballArea(ballPos,0.15);
    ballArea.tangent(agentPos,&sol1,&sol2);
    Vector2D finalPos;
    finalPos=ballPos-(target-ballPos).norm()*0.15;

    if(sol1.dist(finalPos) >= sol2.dist(finalPos))
        sol1 = sol2;

    if(wm->ball->vel > 0.3)
    {
        for(double i = 0 ; i < 5 ; i += 0.1)
        {
            if(wm->ball->getPosInFuture(i).dist(agentPos)/vRobotTemp < i)
            {
                sol1 = wm->ball->getPosInFuture(i) - (target-wm->ball->getPosInFuture(i)).norm()*0.15;
                break;
            }
        }
    }

    gpa->setSlowmode(slow);
    gpa->setDivemode(false);
    gpa->init(sol1  , target - sol1);
    gpa->execute();
}

Vector2D CSkillKick::getTurnFunc(double ang, double _r, double _angOff)
{
    if(shotEmptySpot)
        target = findMostPossible();
    const double pi = 3.141592;
    double tempR, tempX, tempY, theta, theta2;
    double tempX2, tempY2;
    ang = pi - ang;
    theta = pi - ang;
    tempR = _r - (_r/pi)*fabs(theta);

    if(ang > 0)
    {
        theta2 = pi - ang;
        tempX = tempR*sin(theta2);
        tempY = tempR*cos(theta2);
    }
    else
    {
        theta2 = ang;
        tempX = -tempR*sin(theta2);
        tempY = tempR*cos(theta2);
    }

    if(_angOff != 0)
    {
        tempX2 = tempX*sin(_angOff) + tempY*cos(_angOff);
        tempY2 = -tempX*cos(_angOff) + tempY*sin(_angOff);
        return Vector2D(tempX2, tempY2);
    }

    return Vector2D(tempX, tempY);
}


void CSkillKick::jTurn()
{
    if(shotEmptySpot)
        target = findMostPossible();
    //    //// vars
    bool isFinalController = false;
    double posPidKp = 1;
    double speedPidKp = 1;
    Vector2D targetForJturnSpeed,targetForJturnPos;
    Vector2D idealPass ;
    Vector2D movementThSpeed,movementThPos;
    double movementDir = ((ballPos - agentPos).th() - kickFinalDir).degree();
    double shift = 0;
    double distCoef = 0.15;

    idealPass = (ballPos - agentPos).norm()*distCoef;
    /*
    if(abs(movementDir) < 30 && ballPos.dist(agentPos) < 0.25)
    {
        isFinalController =true;
    }
    else
    {
        isFinalController = false;
    }
    if(movementDir <= 70 && movementDir >= -70)
    {
        targetForJturnPos = ballPos + (ballPos - target).norm()*0.11;
        if(movementDir > 20)
        {
            if(wm->ball->vel.length() > 0.1)
                shift = 15 + (1-agentPos.dist(ballPos))*45;
            else
                shift = 0 + (1-agentPos.dist(ballPos))*10;

        }
        else if(movementDir < -20)
        {
            if(wm->ball->vel.length() > 0.1)
                shift = -15 - (1-agentPos.dist(ballPos))*45;

            else
                shift = 0 - (1-agentPos.dist(ballPos))*10;
        }
        else
        {
            agent->setRoller(spin);
            shift = 0;
        }

        idealPass.rotate(shift);
        targetForJturnSpeed = agentPos + idealPass;

    }
    else
    {
        if(movementDir > 0)
        {
            shift = 15 + (1-agentPos.dist(ballPos))*61;
        }
        else if(movementDir < 0)
        {
            shift = -15 - (1-agentPos.dist(ballPos))*61;
        }
        idealPass.rotate(shift);
        targetForJturnSpeed = agentPos + idealPass;
    }

*/

    if(movementDir < 20 && movementDir > -20)
        shift = 0;
    else if(movementDir > 50)
        shift = 15 + (1-agentPos.dist(ballPos))*61;
    else if(movementDir < -50)
        shift = -15 - (1-agentPos.dist(ballPos))*61;
    else if(movementDir > 30) {
        if(wm->ball->vel.length() < 0.1)
            shift = 5 + (1-agentPos.dist(ballPos))*10;
        else
            shift =5 + (1-agentPos.dist(ballPos))*35;
        distCoef = 0.17;
    }
    else if(movementDir < -30){
        if(wm->ball->vel.length() < 0.1)
            shift = -5 - (1-agentPos.dist(ballPos))*10;
        else
            shift = -5 - (1-agentPos.dist(ballPos))*35;

        distCoef = 0.17;
    }
    else if(movementDir > 0) {
        if(wm->ball->vel.length() < 0.1)
            shift = 5 + (1-agentPos.dist(ballPos))*10;
        else
            shift =5 + (1-agentPos.dist(ballPos))*20;
        distCoef = 0.17;
    }
    else if(movementDir < 0){
        if(wm->ball->vel.length() < 0.1)
            shift = -5 - (1-agentPos.dist(ballPos))*10;
        else
            shift = -5 - (1-agentPos.dist(ballPos))*20;

        distCoef = 0.17;
    }

    idealPass.rotate(shift);
    targetForJturnSpeed = agentPos + idealPass;

    drawer->draw(targetForJturnSpeed);

    movementThSpeed = (targetForJturnSpeed - agentPos).norm();
    drawer->draw(movementThSpeed);

//    movementThPos = (targetForJturnPos - agentPos).norm();
    double dirReduce = (fabs(movementDir)/70)*(fabs(movementDir)/70);

    speedPid->error  = targetForJturnSpeed.dist(agentPos);
    posPid->error = 0;//targetForJturnPos.dist(agentPos);

    ////////////set Active adaptive PIDs



    //    if(0 && (wm->field->isInOppPenaltyArea(wm->ball->pos + (wm->field->oppGoal() - wm->ball->pos).norm()*0.1) && agentPos.dist(ballPos) < 0.3))
    //    {
    //        if(isFinalController )
    //        {
    //            if(wm->ball->vel.length() > 0.2 || 1)
    //            {
    //                posPid->kp =3+(0.001/(agentPos.dist(targetForJturnPos)*agentPos.dist(targetForJturnPos)));
    //                speedPid->kp = 2;// +2.1*agentPos.dist(ballPos) + dirReduce;
    //            }
    //            else
    //            {
    //                posPid->kp = 3+(0.001/(agentPos.dist(targetForJturnPos)*agentPos.dist(targetForJturnPos)));
    //                speedPid->kp = 2;// +2.1*agentPos.dist(ballPos) + dirReduce;
    //            }
    //        }
    //        else
    //        {
    //            posPid->kp = 0;
    //            speedPid->kp = 6 +2.1*agentPos.dist(ballPos) + dirReduce;
    //        }
    //    }
    //    if(1)
    //    {
    posPid->kd = 0;

    //        if(isFinalController )
    //        {
    //            if(wm->ball->vel.length() > 0.2 )
    //            {
    //                posPid->kp =0;// 1+(0.001/(agentPos.dist(targetForJturnPos)*agentPos.dist(targetForJturnPos)));
    //                speedPid->kp = 4;// +2.1*agentPos.dist(ballPos) + dirReduce;
    //            }
    //            else
    //            {
    //                posPid->kd = 20;

    //                posPid->kp = 2+(0.01/(agentPos.dist(targetForJturnPos)*agentPos.dist(targetForJturnPos)));
    //                speedPid->kp = 3;// +2.1*agentPos.dist(ballPos) + dirReduce;
    //            }
    //        }
    //        else
    //        {

    dirReduce = (fabs(movementDir)/70)*(fabs(movementDir)/70);
    if(wm->field->isInOppPenaltyArea(ballPos + (wm->field->oppGoal() - ballPos).norm()*0.15) && agentPos.dist(ballPos)<0.25)
    {
        dirReduce -= 2;
    }
    // TODO : Game State Message
//    if(knowledge->isOurNonPlayOnKick())
//    {
//        dirReduce -= 1;
//    }

    if(wm->ball->vel.length() < 0.2)
        posPid->kp = 0;
    speedPid->kp = 6 +2.1*agentPos.dist(ballPos) + dirReduce;


    if(!jTurnFromBack)
    {
        dirReduce += 1;
    }


    if(penaltyKick)
    {
        angPid->kp = 7;
        speedPid->kp = 4;
    }

    angPid->kp = 5;

    double vx= movementThSpeed.x * speedPid->PID_OUT() + movementThPos.x * posPid->PID_OUT();
    double vy = movementThSpeed.y * speedPid->PID_OUT()+ movementThPos.y * posPid->PID_OUT();
    angPid->error = (kickFinalDir - agentDir.th()).radian();
    agent->setRobotAbsVel( vx + wm->ball->vel.x
            , vy + wm->ball->vel.y
            ,angPid->PID_OUT());
    speedPid->pError = speedPid->error;
    posPid->pError = posPid->error;
    agent->accelerationLimiter(0,false);

}

void CSkillKick::turnForKick()
{
    if(shotEmptySpot)
        target = findMostPossible();
    agent->setRoller(0);
    double angReduce = 1;

    if(false) //knowledge->isOurNonPlayOnKick()) TODO : Command
    {
        if(fabs((agentDir.th() - kickFinalDir).degree()) < 80)
            angReduce = 0.5;
        if ((agentDir.th() - kickFinalDir).degree()  <- 10 )
        {
            angPid->kp = 4*angReduce;

            angPid->error = ((ballPos - agentPos).th() - agent->dir().th()).radian();
            agent->setRobotVel( (-0.12 + agentPos.dist(ballPos))*4 ,-1*angReduce,angPid->PID_OUT() +4*angReduce);


        }
        else if ((agentDir.th() - kickFinalDir).degree()  > 10 )
        {
            angPid->kp = 4*angReduce;

            angPid->error = ((ballPos - agentPos).th() - agent->dir().th()).radian();
            agent->setRobotVel( (-0.12 + agentPos.dist(ballPos))*4,1*angReduce,angPid->PID_OUT() - 4*angReduce) ;
        }


    } else {
        angReduce = 1;
        if(fabs((agentDir.th() - kickFinalDir).degree()) < 80)
            angReduce = 0.6;

        agent->setRoller(1);
        if ((agentDir.th() - kickFinalDir).degree()  <- 10 )
        {
            angPid->kp = 4*angReduce;

            angPid->error = ((ballPos - agentPos).th() - agent->dir().th()).radian();
            agent->setRobotVel(0.5, -1*angReduce, angPid->PID_OUT() +4*angReduce);

        } else if ((agentDir.th() - kickFinalDir).degree()  > 10 ) {
            angPid->kp = 4*angReduce;

            angPid->error = ((ballPos - agentPos).th() - agent->dir().th()).radian();
            agent->setRobotVel(0.5, 1*angReduce, angPid->PID_OUT() - 4*angReduce) ;
        }
    }
}

void CSkillKick::kkKick()
{

}

void CSkillKick::kkIntercept()
{

    Vector2D stopTarget, finalDir;
    Segment2D ballPath;
    Segment2D fieldBottomline,fieldTopline,fieldLeftline,fieldRightline;
    fieldBottomline.assign(Vector2D(-1*wm->field->_FIELD_WIDTH/2,-1*wm->field->_FIELD_HEIGHT/2),Vector2D(wm->field->_FIELD_WIDTH/2,-1*wm->field->_FIELD_HEIGHT/2));
    fieldTopline.assign(Vector2D(-1*wm->field->_FIELD_WIDTH/2,1*wm->field->_FIELD_HEIGHT/2),Vector2D(wm->field->_FIELD_WIDTH/2,1*wm->field->_FIELD_HEIGHT/2));
    fieldLeftline.assign(Vector2D(-1*wm->field->_FIELD_WIDTH/2,1*wm->field->_FIELD_HEIGHT/2),Vector2D(-1*wm->field->_FIELD_WIDTH/2,-1*wm->field->_FIELD_HEIGHT/2));
    fieldRightline.assign(Vector2D(wm->field->_FIELD_WIDTH/2,1*wm->field->_FIELD_HEIGHT/2),Vector2D(wm->field->_FIELD_WIDTH/2,-1*wm->field->_FIELD_HEIGHT/2));

    Circle2D robotArea;
    Vector2D sol1,sol2;
    Vector2D bestPos;
    double ballVel = ballRealVel;
    double bTime;
    double difTime[100] = {0};
    int minNum=0;


    Circle2D nowTurn(agentPos + agentDir.norm()*0.2,0.2);
//    draw(nowTurn,QColor(Qt::red));


    if(nowTurn.contains(ballPos) && ballRealVel < 0.5)
    {
        if(fabs((kickFinalDir - agentDir.th()).degree()) > 35)
            return turnForKick();

        return jTurn();
    }


}

void CSkillKick::kkPenalty()
{
    angPid->kp =4;
    if( ((ballPos - agentPos).th() - kickFinalDir).degree()  > 90  )
        kkMovementTheta = ((ballPos - agentPos).th().radian() + 0.8+1.5*(0.4-agentPos.dist(ballPos))- agentDir.th().radian());
    else if(((ballPos - agentPos).th() - kickFinalDir).degree() <- 90)
        kkMovementTheta = ((ballPos- agentPos).th().radian() - 0.8- 1.5*(0.4-agentPos.dist(ballPos))- agentDir.th().radian());
    else if(((ballPos - agentPos).th() - kickFinalDir).degree() >45)
        kkMovementTheta = ((ballPos- agentPos).th().radian() +0.7 + 1.5*(0.4-agentPos.dist(ballPos)) - agentDir.th().radian());
    else if(((ballPos - agentPos).th() - kickFinalDir).degree() < -45)
        kkMovementTheta = ((ballPos- agentPos).th().radian() - 0.7 - 1.5*(0.4-agentPos.dist(ballPos))- agentDir.th().radian());
    else if(((ballPos - agentPos).th() - kickFinalDir).degree() >3)
        kkMovementTheta = ((ballPos- agentPos).th().radian() +0.6 + 1.5*(0.2-agentPos.dist(ballPos)) - agentDir.th().radian());
    else if(((ballPos - agentPos).th() - kickFinalDir).degree() < -3)
        kkMovementTheta = ((ballPos- agentPos).th().radian() - 0.6 - 1.5*(0.2-agentPos.dist(ballPos))- agentDir.th().radian());
    else
        kkMovementTheta =((ballPos- agentPos).th().radian() - agentDir.th().radian());
    /////////////////////////////////////////////////////////////////////////////////
    double ballx= (wm->ball->vel.x)*cos(agentDir.th().radian()) + (wm->ball->vel.y)*sin(agentDir.th().radian());
    double bally= -1*(wm->ball->vel.x)*sin(agentDir.th().radian()) + (wm->ball->vel.y)*cos(agentDir.th().radian());
    angPid->error = (kickFinalDir - agentDir.th()).radian();
    double reduce = 0.6;
    // TODO : Command
//    if(interceptMode)
//        agent->setRobotVel(reduce*cos(kkMovementTheta)  + 0.4*ballx,reduce*sin(kkMovementTheta)+0.4*bally ,angPid->PID_OUT());
//    else
//        agent->setRobotVel(reduce*cos(kkMovementTheta) + 0.2*ballx,reduce*sin(kkMovementTheta) + 0.2*bally,angPid->PID_OUT());

}

Vector2D CSkillKick::findMostPossible()
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
        if(wm->our.active(i)->id != agent->id())
            obstacles.append(Circle2D(wm->our.active(i)->pos,0.1));
    }
    double prob,angle,biggestAngle;

    CKnowledge::getEmptyAngle(wm->ball->pos-(wm->field->oppGoal()-ballPos).norm()*0.15,wm->field->oppGoalL(),wm->field->oppGoalR(), obstacles, prob, angle, biggestAngle);
    //debug(QString("prob: %1 , angle :%2, biggest:%3").arg(prob).arg(angle).arg(biggestAngle),D_MHMMD);

    Segment2D goalSeg(wm->field->oppGoalL(),wm->field->oppGoalR());
    Vector2D sol1,sol2;
    //    debug(QString("ang %1").arg(angle),D_MHMMD);
//    draw(Segment2D(wm->ball->pos , wm->ball->pos + Vector2D(cos(_PI*(angle)/180),sin(_PI*(angle)/180)).norm()*12));

    return  goalSeg.intersection(Segment2D(wm->ball->pos , wm->ball->pos + Vector2D(cos(_PI*(angle)/180),sin(_PI*(angle)/180)).norm()*12));
}


double CSkillKick::oneTouchAngle(Vector2D pos,
                                 Vector2D vel,
                                 Vector2D ballVel,
                                 Vector2D ballDir,
                                 Vector2D goal,
                                 double landa,
                                 double gamma)
{
    float ang1 = (-ballDir).th().degree();
    float ang2 = (goal - pos).th().degree();
    float theta = AngleDeg::normalize_angle(ang2 - ang1);
    float th = fabs(theta)*_DEG2RAD;
    float vkick = 8; // agent->self()->kickValueSpeed(kickSpeed, false);// + Vector2D::unitVector(self().pos.d).innerProduct(self().vel);
    float v = (ballVel - vel).length();
    float th1 = th*0.5;
    float f,fmin=1e10;
    float th1best;
    for (int k=0;k<6000;k++)
    {
        th1 = ((float)k/6000.0)*th;
        f  = gamma*v*(1.0/tan(th-th1))*sin(th1)-landa*v*cos(th1)-vkick;
        if (fabs(f)<fmin)
        {
            fmin = fabs(f);
            th1best = th1;
        }
    }
    th1 = th1best;
    th1 *= _RAD2DEG;
    AngleDeg::normalize_angle(th1);
    th  *= _RAD2DEG;
    float ang = 0;
    if (theta>0) ang = ang1 + th1;
    else ang = ang1 - th1;

    return ang;
}

void CSkillKick::findPosToGo()
{
    Circle2D  agentNearArea(agentPos,0.15);
    Vector2D sol1,sol2;
    QList<int> ourRelax,oppRelax;
    double agentTime = 0;
    Vector2D finalDir;
    Segment2D ballPath(ballPos,ballPos + wm->ball->vel.norm()*10);
    Circle2D dribblerArea(agentPos+agentDir.norm()*0.1,0.25);
    Circle2D robotArea(agentPos,1);

    gpa->setAddvel(Vector2D(0,0));
    if(agentPos.dist(ballPos) < 1.5) {
        robotArea.assign(agentPos,max(agentPos.dist(ballPos) - 0.1, 0.01));
    }

    if(agentPos.dist(ballPos) < 0.15)
    {
        agentNearArea.assign(agentPos,agentPos.dist(ballPos) - 0.01);
    }

    if(wm->ball->vel.length() > 0.2 )
    {
        for(double i = 0 ; i < 5 ; i += 0.1)
        {

            finalPos = wm->ball->getPosInFuture(i);// - (target-wm->ball->getPosInFuture(i)).norm()*0.15;
            agentTime = CSkillGotoPointAvoid::timeNeeded(agent,finalPos,conf->VelMax,ourRelax,oppRelax,!goalieMode,0.2,true);

            if(agentTime < i )
            {
                break;
            }
        }
        if((fabs(((ballPos - agentPos).th() - kickFinalDir).degree()) < 60) )
        {
            finalDir = Vector2D(cos(kickFinalDir.radian()),sin(kickFinalDir.radian()));
        }
        else
        {
            finalDir = Vector2D(cos(kickFinalDir.radian()),sin(kickFinalDir.radian()));
        }
    }

    else
    {
        finalPos = ballPos - (target-finalPos).norm() * 0.15;
        finalDir = Vector2D(cos(kickFinalDir.radian()),sin(kickFinalDir.radian()));
    }
    Vector2D oneTouchPos = ballPath.nearestPoint(agentPos);
    Segment2D kickerSeg(agentPos+agent->dir().norm()*0.08+agent->dir().rotate(90).norm()*0.02 ,agentPos+agent->dir().norm()*0.08-agent->dir().rotate(90).norm()*0.02 );
    bool canOneTouch = false;
    if(robotArea.intersection(ballPath,&sol1,&sol2) > 1 && wm->ball->vel.length() > 0.5 )
    {
        for(double i = 0 ; i < 5 ; i+=0.1)
        {
            if(wm->ball->getPosInFuture(i).dist(oneTouchPos) <= 0.2)
            {
                if(CSkillGotoPointAvoid::timeNeeded(agent,oneTouchPos,conf->VelMax,ourRelax,oppRelax,!goalieMode,0,true) <= i+0.1)
                {
                    canOneTouch =true;
                    break;
                }
            }
        }

        if(agentPos.dist(ballPos) < 0.5)
        {
            if((canOneTouch || kickerSeg.intersection(ballPath).isValid()) && !sagMode )
            {
//                debug("oneTOUCH",D_MHMMD);
                if( ( fabs(((target-agentPos).th().degree() - (ballPos-agentPos).th().degree() )) < 60 ))
                    waitAndKick();
                else
                    kWaitForTurn();


                return;
            }
        }
    }

    Circle2D oppPenalty(wm->field->oppGoal() + Vector2D(0.2 , 0),1.4);
    gpa->setOnetouchmode(false);
//
    if(oppPenalty.contains(ballPos))
    {
        finalPos = finalPos - (target-finalPos).norm() * 0.15;
    }
    if(finalPos.x > wm->field->_FIELD_WIDTH)
    {
        finalPos = CKnowledge::getReflectPos(wm->field->oppGoal(), 3, wm->ball->pos);
    }

    if((fabs(((ballPos - agentPos).th() - kickFinalDir).degree()) < 60))
    {
        finalPos = finalPos - (target-finalPos).norm() * 0.1;
    }
    Vector2D s1,s2;
    Circle2D finalPosArea;
//    draw(finalPosArea,QColor(Qt::blue));

    Segment2D directPath(agentPos,finalPos);
//    draw(directPath);
    finalPosArea.assign(ballPos ,0.145);
//    if(finalPosArea.intersection(directPath,&s1,&s2))
//    {
//        gpa->setOnetouchmode(false);
//
//        finalPosArea.assign(ballPos ,0.245);
//        finalPosArea.tangent(agentPos,&s1,&s2);
//        if(s2.dist(target) >= s1.dist(target))
//            s1 = s2;
//        s1 = s1 + (s1 - agentPos).norm()*(finalPos.dist(ballPos))*1.5;
//        finalPos = s1;
//        //gpa->setAddVel(wm->ball->vel);
//    }
//    else
//    {
//        gpa->setAddvel(Vector2D(0,0));
//    }

    gpa->init(finalPos,finalDir);
    gpa->setNoavoid(false);

    if(wm->ball->vel.length() > 0.3)
        gpa->setBallobstacleradius(0);
    else
        gpa->setBallobstacleradius(0);
    if(((fabs(((ballPos - agentPos).th() - kickFinalDir).degree()) < 60) && (agentPos.dist(ballPos) < 1) && (wm->ball->vel.length() > 0.2)) || (agentPos.dist(ballPos) < 0.4)) {
        if(fabs((kickFinalDir - agentDir.th()).degree()) > 30 && dribblerArea.contains(ballPos))
        {
            turnForKick();
        }
        else
        {
            jTurn();
        }
        return;
    }

    gpa->setSlowmode(slow);
    gpa->setDivemode(false);
    gpa->setAvoidpenaltyarea(true);
    gpa->execute();


}

void CSkillKick::findPosToGoAlt()
{
    Circle2D ballArea(ballPos,0.2);
    Vector2D sol1,sol2;
    if(wm->ball->vel.length() > 0.2)
    {
        for(double i = 0 ; i < 5 ; i += 0.1)
        {
            if(wm->ball->getPosInFuture(i).dist(agentPos)/vRobotTemp < i)
            {
                finalPos = wm->ball->getPosInFuture(i) - (target-wm->ball->getPosInFuture(i)).norm()*0.16;
                break;
            }
        }
    }

    else
    {
        finalPos = ballPos - (target-ballPos).norm() * 0.16;
    }

    if(finalPos.x > wm->field->_FIELD_WIDTH)
    {
        finalPos = CKnowledge::getReflectPos(wm->field->oppGoal(), 3, wm->ball->pos);
    }

    Vector2D finalDir;
    Circle2D dribblerArea(agentPos+agentDir.norm()*0.1,0.1);

    if(agentPos.dist(ballPos) > 1)
    {
        finalDir = wm->ball->pos - agentPos;
    }
    else if(!dribblerArea.contains(ballPos))
    {
        finalDir = (agentPos.dist(ballPos)) * (wm->ball->pos - agentPos) + (1 - agentPos.dist(ballPos))*(target-agentPos) ;
    }
    else
    {
        finalDir = target-agentPos;
    }
    gpa->init(finalPos,finalDir);

    gpa->setBallobstacleradius(1);
    if((fabs(((ballPos - agentPos).th() - kickFinalDir).degree()) < 60) && (agentPos.dist(ballPos) < 1) && (wm->ball->vel.length() > 0.2)) {
        jTurn();
        return;
    }

    gpa->setAvoidpenaltyarea(false);
    gpa->execute();


}

void CSkillKick::execute()
{

    ballRealVel = wm->ball->vel.length();
    agentPos = agent->pos();
    kkDist = agentPos.dist(target);
    ballPos = wm->ball->pos;
    kkBallDist = agentPos.dist(ballPos);
    AngleDeg maxAngP,maxAngN;
    AngleDeg kickTargetDir ;

//    ROS_INFO_STREAM("ball speed x: "<<wm->ball->vel.x << "  "<< wm->ball->vel.y);
//    draw(Segment2D(agentPos,agentPos+agentDir*10));
    if(shotEmptySpot)
        target = findMostPossible();
    ///////////////dir correction
    /// ballSpeed
    double ballVelCoef = 1,robotVelCoef = 1;
    double robotVelDif = 0,ballVelDif = 0;
    double finalVelDif = 0;
    kickTargetDir= (target - ballPos).th();
    if(wm->ball->vel.length() > 0.5)
    {
        ballVelDif = ballVelCoef * (Vector2D::angleBetween((target -ballPos).norm(),wm->ball->vel.norm()).degree())*wm->ball->vel.length();
        robotVelDif = robotVelCoef*(Vector2D::angleBetween((target -ballPos).norm(),agent->vel().norm()).degree())*agent->vel().length();
        finalVelDif = ballVelDif + robotVelDif;
    }
    agentDir = agent->dir();
    maxAngP = (target - ballPos).th() + kickAngTol;
    maxAngN = (target - ballPos).th() - kickAngTol;




    kickFinalDir = kickTargetDir;
    finalDirVec = target - ballPos;
    oneTouchDir=Vector2D::unitVector(oneTouchAngle(agentPos, agent->vel(), wm->ball->vel, agentPos - ballPos, target, conf->Landa, conf->Gamma));

    gpa->setAgent(agent);
    kickMode = decideMode();

    if(kickMode != KJTURN)
    {
        isJturn = false;
    }


    if(dontKick)
    {
        agent->setKick(0);
        agent->setChip(0);
    }
    else if(kickMode == KWAITANDKICK)
    {
        if(Circle2D(agentPos,1).contains(ballPos) && fabs((agentDir.th() - oneTouchDir.th()).degree()) < 2)
        {
            agent->setRoller(spin);
            if(chip)
                agent->setChip(kickSpeed);
            else
                agent->setKick(kickSpeed);
        }
        else
        {

            agent->setKick(0);
            agent->setChip(0);
        }
    }
    else if(veryFine)
    {
        if(kickerOn && fabs((agentDir.th() - kickTargetDir).degree()) < 2)
        {
            if(chip)
                agent->setChip(kickSpeed);
            else
                agent->setKick(kickSpeed);
        }
        else
        {
            agent->setKick(0);
            agent->setChip(0);
        }
    }

    else
    {
        if(kickerOn && fabs((agentDir.th() - kickTargetDir).degree()) < tolerance || kickerOn && fabs((agentDir.th() - kickTargetDir).degree()) < 3)
        {

            if(chip)
                agent->setChip(kickSpeed);
            else
                agent->setKick(kickSpeed);
        }
        else
        {
            agent->setKick(0);
            agent->setChip(0);
        }
    }
//TODO: run over one meter
    //    if(agentPos.dist(jTurnStartPos) > 0.5 && !slow && !penaltyKick )
    //    {
    //        agent->setChip(1);
    //        jTurnStartPos = agentPos;
    //    }


    if(shotEmptySpot)
        target = findMostPossible();
    switch(kickMode)
    {
        case KDIRECT:
            direct();
//        debug("DIRECT",D_KK);
            break;
        case KINDIRECCT:
            indirect();
//        debug("INDIRECT",D_KK);
            break;
        case KJTURN:
            jTurn();
//        debug("JTURN",D_KK);
            break;
        case KTURN:
            turnForKick();
//        debug("TURN",D_KK);
            break;
        case KINTERCEPT:
            kkIntercept();
//        debug("INTERCEPT",D_KK);
            break;
        case KPENALTY:
            kkPenalty();
//        debug("PENALTY",D_KK);
            break;
        case KAVOIDOPPENALTY:
            if(wm->ball->pos.x  >0)
            {
                avoidOppPenalty();
            }
            else
            {
                avoidOurPenalty();
            }
            break;
        case KWAITANDKICK:
            waitAndKick();
            break;
        case KWAITFORTURN:
            kWaitForTurn();
            break;
        case KDONTKICK:
            kDontKick();
            break;
        case KGOALIE:
            kgoalie();
            break;


        case KKICK:break;
    }
}
