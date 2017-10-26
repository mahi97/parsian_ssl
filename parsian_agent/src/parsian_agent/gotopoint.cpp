#include <parsian_agent/gotopoint.h>




INIT_SKILL(CSkillGotoPoint, "gotopoint");
CSkillGotoPoint::CSkillGotoPoint(Agent *_agent) : CSkill(_agent)
{
    lookAt.invalidate();
    agent = _agent;
    speedPidX = new _PID(1,0,0,0,0);
    speedPidY = new _PID(1,0,0,0,0);
    posPid = new _PID(1,0,0,0,0);
    velPid = new _PID(1,0,0,0,0);
    angPid= new _PID(2,0,0,0,0);
    posXpid = new _PID(1,0,0,0,0);
    posYpid = new _PID(1,0,0,0,0);
    thPid = new _PID(1,0,0,0,0);

    maxAcceleration = 4;
    maxDeceleration = 4;


    lookAt.invalidate();

    lastGPmode = GPACC1;
    posPidDist = 0.5;

    decThr = 0.2;
    posThr = 0;
    vConstThr = 0;

    maxVelocity = 5;

    agentVDesire = 0;
    ////modes
    slowShot= false;
    slowMode = false;
    verySlow = false;
    diveMode = false;
    penaltyKick = false;
    smooth = false;
}

CSkillGotoPoint::~CSkillGotoPoint()
{
    //    delete speedPidX;
    //    delete speedPidY;
    //    delete posPid;
    //    delete angPid;
    //    delete posXpid;
    //    delete posYpid;
    //    delete thPid;
}

void CSkillGotoPoint::init(Vector2D target, Vector2D _targetDir, Vector2D _targetVel)
{
    targetPos = target;
    targetDir = _targetDir;
    targetVel = _targetVel;
}

double CSkillGotoPoint::timeNeeded()
{
    return 0 ;
}
gpMode CSkillGotoPoint::decideMode()
{
    //    posPidDist = 1/agentVc;
    //    if(posPidDist > 1)
    //        posPidDist = 1;
    if(agentDist < posPidDist + posThr) {
        decThr = 0;
        return GPPOS;
    }
    else {
        agentX3 = fabs(((posPidDist*posPid->kp)*(posPidDist*posPid->kp) - (agentVc*agentVc))/(2*maxDeceleration)) + 0.05 *agentVc;

        if(agentDist <= agentX3 + decThr) {
            if(agentVc < 0.5)
                decThr = 0;
            else
                decThr = 0.5;
            return GPDEC1;
        }
        else if(agentVc >= maxVelocity)
        {
            decThr = 0;
            return GPVCONST;
        }
        else {
            decThr = 0;
            return GPACC1;
        }

    }

}
double CSkillGotoPoint::optimalAccOrDec(double agentDir, bool dec)
{
    double Vx = cos(agentDir);
    double Vy = sin(agentDir);
    double fWheels[4];
    double biggest = 0.0;
    double optimalAcc , optimalDec;
    double Ff , Fn;
    //////////////Calculate Jacobian Matrix//////////
    fWheels[0] = -(Vx * 0.8660) + (Vy * 0.5);
    fWheels[1] = -(Vx * 0.7071) - (Vy * 0.7071);
    fWheels[2] =  (Vx * 0.7071) - (Vy * 0.7071);
    fWheels[3] =  (Vx * 0.8660) + (Vy * 0.5);
    ////////////////////////////////////////////////
    ///////////find biggest value in Jacob//////////
    for (double fWheel : fWheels) {
        if( fabs(fWheel) > biggest ) {
            biggest = fabs(fWheel);
        }
    }
    /////////////////////////////////////////////////
    //////////normalize Jacob's Value////////////////
    for (double &fWheel : fWheels) {
        fWheel = fWheel /biggest;
    }
    /////////////////////////////////////////////////
    ///////////calculate forward force vector and normal force vector////////////////////
    Ff = ((fWheels[3]-fWheels[0])*(sqrt(3)/2)) + ((fWheels[2] - fWheels[1])*(sqrt(2)/2));
    Fn = ((fWheels[3]+fWheels[0])*0.5) + (-1*(fWheels[2] + fWheels[1])*(sqrt(2)/2));
    ////////////////////////////////////////////////////////////////////////////////////
    /////////////////2.8868 is max of sum Ff and Fn and this derivation is for nomalization of Max Acc = maxAcceleration/////////
    optimalAcc = maxAcceleration * sqrt((Ff*Ff) + (Fn*Fn))/2.8868;
    optimalDec = maxDeceleration * sqrt((Ff*Ff) + (Fn*Fn))/2.8868;
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////if boll dec = true the function return optimal dec///////////
    if(dec) {
        return optimalDec;
    }
    //////////otherwise return optimal acc//////////
    return optimalAcc;
}
void CSkillGotoPoint::targetValidate()
{
    if (targetPos.x < wm->field->ourCornerL().x - 0.2) targetPos.x = wm->field->ourCornerL().x;
    if (targetPos.x > wm->field->oppCornerL().x + 0.2) targetPos.x = wm->field->oppCornerL().x;
    if (targetPos.y < wm->field->ourCornerR().y - 0.2) targetPos.y = wm->field->ourCornerR().y;
    if (targetPos.y > wm->field->ourCornerL().y + 0.2) targetPos.y = wm->field->ourCornerL().y;

//    if (false) { //conf()->LocalSettings_ParsianWorkShop()) {
//        if(conf()->LocalSettings_OurTeamSide() == "Right")
//        {
//            if(targetPos.x < 0.2)
//            {
//                targetPos.x = 0.2;
//            }
//        }
//        else
//        {
//            if(targetPos.x > 4.3)
//            {
//                targetPos.x = 4.3;
//            }
//        }
//    }

//    if (lookAt.valid())
//    {
//        targetDir = (lookAt - agentPos).norm();
//    }
}

void CSkillGotoPoint::trajectoryPlanner()
{
    agentMovementTh = (targetPos -agentPos ).th();
    //////////////////acc dec
    agentBestAcc = optimalAccOrDec(Vector2D::angleBetween(targetPos-agentPos,agent->dir()).radian(),false);
    agentBestDec = optimalAccOrDec(Vector2D::angleBetween(targetPos-agentPos,agent->dir()).radian(),true);
    appliedAcc =1.5 * maxAcceleration;

    if(smooth)
    {
        if((agentMovementTh - lastPath).degree() > 20 && (agentMovementTh - lastPath).degree() < 100 && agentVc > 1) {
            agentMovementTh = lastPath + 60;
            maxVelocity = 1;
            appliedAcc = 0;

        }
        else if((agentMovementTh - lastPath).degree() < -20 && (agentMovementTh - lastPath).degree() > -100 && agentVc > 1) {
            agentMovementTh = lastPath - 60;
            maxVelocity = 1;
            appliedAcc = 0;

        }
        else if((agentMovementTh - lastPath).degree() >= 100 && agentVc > 1) {
            agentMovementTh = lastPath + 80;
            maxVelocity = 0.5;
            appliedAcc = 0;

        }
        else if((agentMovementTh - lastPath).degree() <= -100 && agentVc > 1) {
            agentMovementTh = lastPath - 80;
            maxVelocity = 0.5;
            appliedAcc = 0;

        }
    }
    ///////////////////////////////////////////// th pid
    thPid->kp =0;
    thPid->error = (agentMovementTh - agent->vel().norm().th()).radian();
    if((fabs(thPid->error) > 1)
       || agentVc < 0.5
       || agentDist > 3
       || fabs((agentMovementTh - agent->dir().th()).degree()) > 80 && fabs((agentMovementTh - agent->dir().th()).degree()) < 100)
        thPid->error =0;

    appliedTh = agentMovementTh.radian() +thPid->PID_OUT();


}

void CSkillGotoPoint::execute()
{

    maxVelocity = 4;
    if(slowShot|| slowMode || penaltyKick)
    {
        maxVelocity = 1.5;
    }
    if(diveMode)
    {
        maxVelocity = 4;
    }

    targetValidate();

    /////////////////decide and exec

    agentPos = agent->pos();
    agentVel = agent->vel();
    agentDist = agentPos.dist(targetPos);

    if (!agentPos.valid() || !agentVel.valid() || !agent->dir().valid()) return;

    DBUG(QString("target : %1, %2").arg(targetDir.th().degree()), D_MAHI);
    DBUG(QString("agent : %1, %2").arg(agent->dir().th().degree()), D_MAHI);

//    currentGPmode = decideMode();
    angPid->error = (targetDir.th() - agent->dir().th()).radian();

    agentVc = agentVel.length();
    ////////////// set params
//    posPid->kd = 3;
//    if(startingPoint.dist(agentPos) < 0.05)
//    {
//        posPid->kp = 4;
//        posPid->kd = 0;
//    }
//    else if(startingPoint.dist(agentPos) < 0.3)
//    {
//        posPid->kp = 0.37/agentDist;
//        if(posPid->kp > 3)
//            posPid->kp = 3;
//
//        posPid->kd = 10;
//    }
//    else
//        posPid->kp = 1.9;
//
//
//    if(slowShot|| slowMode || penaltyKick)
//    {
//        posPid->kp = 1.6;
//    }
//    if(diveMode)
//    {
//        posPid->kp = 1.8/agentDist;
//        posPid->kd = 10;
//        if(posPid->kp > 4)
//            posPid->kp = 4;posPidDist = 1;
//    }
//    else
//    {
//        posPidDist = 0.5;
//    }
//    posPid->kd = 1;
//
//    diveMode = false;
//
    angPid->kp = 3;
    angPid->kd = 1;
//
//
//    trajectoryPlanner();
//
    ////////////////////// dec calculations
//    double vp =(posPidDist*posPid->kp);
//    double moreDec = 0.65;
//    double decOffset = 0.8;
//    if(agentVc < 0.2)
//        startingPoint = agentPos;

    PDEBUG("mode :", currentGPmode, D_MAHI);
    PDEBUG("dist :", angPid->error, D_MAHI);
//    PDEBUG("mode :", currentGPmode, D_MAHI);

    ////////////////////////////
//    if(currentGPmode == GPPOS) {
        ////////////////ACC + DEC
//        agent->_ACC = 0; // TODO : skill config
//        agent->_DEC = 0; // TODO : skill config
        //////////////
//        posPid->error = agentDist;
//        _Vx = posPid->PID_OUT()*cos(agentMovementTh.radian());
//        _Vy = posPid->PID_OUT()*sin(agentMovementTh.radian());
//        if(agentDist  < 0.015){
//            _Vx = 0;
//            _Vy = 0;
//        }
        /////////////////////////////////////////////PID Previous error
//        posPid->pError = agentDist;
//        velPid->_I = 0;
//    }
//    else if(currentGPmode == GPVCONST) {
//        /////////////////ACC + DEC
//        agent->_ACC = 0;
//        agent->_DEC = 0;
//        agentVDesire = maxVelocity;
//        velPid->_I = 0;
        //////////////
//        _Vx = maxVelocity*cos(appliedTh);
//        _Vy = maxVelocity*sin(appliedTh);
//
//    }
//    else if(currentGPmode == GPDEC1) {
//        agent->_ACC = 0;
//        agent->_DEC = 0;
//        agentVDesire = sqrt(fabs(2*maxDeceleration*agentDist*moreDec) + vp *vp) - decOffset;
//        _Vx =  agentVDesire*cos(appliedTh) ;
//        _Vy =  agentVDesire*sin(appliedTh) ;
//
//    }
//    else if(currentGPmode == GPACC1) {
//        if(agentVc > 0.3)
//        {
//            agent->_ACC = appliedAcc;
//            agent->_DEC = 0;
//            agentVDesire = maxVelocity ;
//        }
//        else if(!slowShot&& !slowMode && !penaltyKick)
//        {
//            agent->_ACC = 0;
//            agent->_DEC = 0;
//            agentVDesire = 0.7;
//        }
//        else
//        {
//            agent->_ACC = 0;
//            agent->_DEC = 0;
//            agentVDesire = 0.5;
//        }
//        ////////////////
//        _Vx = agentVDesire*cos(appliedTh);
//        _Vy = agentVDesire*sin(appliedTh);
//        ///////////////////////////////////////////////PID Previous error
//    }
//    else
//    {
//        agent->waitHere();
//        velPid->_I = 0;
//    }

    agent->setRobotAbsVel(10, 0, 10);
    angPid->pError = angPid->error;


//    lastPath = agentVel.th();

}

double CSkillGotoPoint::progress()
{
    if(agent == nullptr) return 0;
    double d = (agentPos - targetPos).length();
    if (d<0.04) return 1.0;
    if (d<0.05) return 0.8;
    if (d<0.10) return 0.7;
    if (d<0.20) return 0.6;
    return 0;
}