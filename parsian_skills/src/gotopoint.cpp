#include "parsian_skills/gotopoint.h"




INIT_SKILL(CSkillGotoPoint, "gotopoint");
CSkillGotoPoint::CSkillGotoPoint(CAgent *_agent) : CSkill(_agent)
{
    lookat.invalidate();
    agent = _agent;
    speedPidX = new _PID(1,0,0,0,0);
    speedPidY = new _PID(1,0,0,0,0);
    posPid = new _PID(1,0,0,0,0);
    velPid = new _PID(1,0,0,0,0);
    angPid= new _PID(2,0,0,0,0);
    posXpid = new _PID(1,0,0,0,0);
    posYpid = new _PID(1,0,0,0,0);
    thPid = new _PID(1,0,0,0,0);

    _Acc = 4;
    _Dec = 4;


    lookat.invalidate();

    lastGPmode = GPACC1;
    posPidDist = 0.5;

    decThr = 0.2;
    posThr = 0;
    vConstThr = 0;
    _VmDesire = 5;

    agentVDesire = 0;
    ////modes
    slow = false;
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

void CSkillGotoPoint::init(Vector2D target, Vector2D _targetDir, Vector2D _targetVel, bool dynamicStart)
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
        agentX3 = fabs(((posPidDist*posPid->kp)*(posPidDist*posPid->kp) - (agentVc*agentVc))/(2*_Dec)) + 0.05 *agentVc;

        if(agentDist <= agentX3 + decThr) {
            if(agentVc < 0.5)
                decThr = 0;
            else
                decThr = 0.5;
            return GPDEC1;
        }
        else if(agentVc >= _VmDesire)
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
    /////////////////2.8868 is max of sum Ff and Fn and this derivation is for nomalization of Max Acc = _Acc/////////
    optimalAcc = _Acc * sqrt((Ff*Ff) + (Fn*Fn))/2.8868;
    optimalDec = _Dec * sqrt((Ff*Ff) + (Fn*Fn))/2.8868;
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

    if (false) { //conf()->LocalSettings_ParsianWorkShop()) {
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
    }

    if (lookat.valid())
    {
        targetDir = (lookat - agentPos).norm();
    }
}

void CSkillGotoPoint::trajectoryPlanner()
{
    agentMovementTh = (targetPos -agentPos ).th();
    //////////////////acc dec
    agentBestAcc = optimalAccOrDec(Vector2D::angleBetween(targetPos-agentPos,agent->self.dir).radian(),false);
    agentBestDec = optimalAccOrDec(Vector2D::angleBetween(targetPos-agentPos,agent->self.dir).radian(),true);
    appliedAcc =1.5* _Acc;

    if(smooth)
    {
        if((agentMovementTh - lastPath).degree() > 20 && (agentMovementTh - lastPath).degree() < 100 && agentVc > 1) {
            agentMovementTh = lastPath + 60;
            _VmDesire = 1;
            appliedAcc = 0;

        }
        else if((agentMovementTh - lastPath).degree() < -20 && (agentMovementTh - lastPath).degree() > -100 && agentVc > 1) {
            agentMovementTh = lastPath - 60;
            _VmDesire = 1;
            appliedAcc = 0;

        }
        else if((agentMovementTh - lastPath).degree() >= 100 && agentVc > 1) {
            agentMovementTh = lastPath + 80;
            _VmDesire = 0.5;
            appliedAcc = 0;

        }
        else if((agentMovementTh - lastPath).degree() <= -100 && agentVc > 1) {
            agentMovementTh = lastPath - 80;
            _VmDesire = 0.5;
            appliedAcc = 0;

        }
    }
    ///////////////////////////////////////////// th pid
    thPid->kp =0;
    thPid->error = (agentMovementTh - agent->self.vel.norm().th()).radian();
    if(fabs(thPid->error > 1) || agentVc < 0.5 || agentDist >3 ||( fabs((agentMovementTh - agent->dir().th()).degree()) > 80 && fabs((agentMovementTh - agent->dir().th()).degree()) < 100 )   )
        thPid->error =0;

    appliedTh = agentMovementTh.radian() +thPid->PID_OUT();


}

void CSkillGotoPoint::execute()
{
    _VmDesire = 4;
    if(slow || slowMode || penaltyKick)
    {
        _VmDesire = 1.5;
    }
    if(diveMode)
    {
        _VmDesire = 4;
    }
//    if(wm->getIsSimulMode()) {
//        _VmDesire = 2;
//    } //TODO : CHECK
    targetValidate();

    /////////////////decide and exec

    agentPos = agent->self.pos;
    agentVel = agent->self.vel;
    agentDist = agentPos.dist(targetPos);
    currentGPmode = decideMode();
    angPid->error = (targetDir.th() - agent->self.dir.th()).radian();
    agentVc = agentVel.length();
    //////////////// set params
    posPid->kd = 3;
    if(startingPoint.dist(agentPos) < 0.05)
    {
        posPid->kp = 4;
        posPid->kd = 0;
    }
    else if(startingPoint.dist(agentPos) < 0.3)
    {
        posPid->kp = 0.37/agentDist;
        if(posPid->kp > 3)
            posPid->kp = 3;

        posPid->kd = 10;
    }
    else
        posPid->kp = 1.9;


    if(slow || slowMode || penaltyKick)
    {
        posPid->kp = 1.6;
    }
    if(diveMode)
    {
        posPid->kp = 1.8/agentDist;
        posPid->kd = 10;
        if(posPid->kp > 4)
            posPid->kp = 4;posPidDist = 1;
    }
    else
    {
        posPidDist = 0.5;
    }
    posPid->kd = 1;

    diveMode = false;



    //    debug(QString("dive %1").arg(diveMode),D_MHMMD);

    angPid->kp = 3;
    angPid->kd = 1;



    trajectoryPlanner();

    //////////////////////// dec calculations
    double vp =(posPidDist*posPid->kp);
    double moreDec = 0.65;
    double decOffset = 0.8;
    if(agentVc < 0.2)
        startingPoint = agentPos;


    ////////////////////////////
    if(currentGPmode == GPPOS) {
        ////////////////ACC + DEC
//        agent->_ACC = 0; // TODO : skill config
//        agent->_DEC = 0; // TODO : skill config
        ////////////////
        posPid->error = agentDist;
        _Vx = posPid->PID_OUT()*cos(agentMovementTh.radian());
        _Vy = posPid->PID_OUT()*sin(agentMovementTh.radian());
        if(agentDist  < 0.015){
            _Vx = 0;
            _Vy = 0;
        }
        ///////////////////////////////////////////////PID Previous error
        posPid->pError = agentDist;
        velPid->_I = 0;
    }
    else if(currentGPmode == GPVCONST) {
        /////////////////ACC + DEC
//        agent->_ACC = 0;
//        agent->_DEC = 0;
        agentVDesire = _VmDesire;
        velPid->_I = 0;
        ////////////////
        _Vx = _VmDesire*cos(appliedTh);
        _Vy = _VmDesire*sin(appliedTh);

    }
    else if(currentGPmode == GPDEC1) {
//        agent->_ACC = 0;
//        agent->_DEC = 0;
        agentVDesire = sqrt(fabs(2*_Dec*agentDist*moreDec) + vp *vp) - decOffset;
        _Vx =  agentVDesire*cos(appliedTh) ;
        _Vy =  agentVDesire*sin(appliedTh) ;

    }
    else if(currentGPmode == GPACC1) {
        if(agentVc > 0.3)
        {
//            agent->_ACC = appliedAcc;
//            agent->_DEC = 0;
            agentVDesire = _VmDesire;
        }
        else if(!slow && !slowMode && !penaltyKick)
        {
//            agent->_ACC = 0;
//            agent->_DEC = 0;
            agentVDesire = 0.7;
        }
        else
        {
//            agent->_ACC = 0;
//            agent->_DEC = 0;
            agentVDesire = 0.5;
        }
        ////////////////
        _Vx = agentVDesire*cos(appliedTh);
        _Vy = agentVDesire*sin(appliedTh);
        ///////////////////////////////////////////////PID Previous error
    }
    else
    {
//        agent->waitHere();
        velPid->_I = 0;
    }
//    agent->setRobotAbsVel(_Vx,_Vy,angPid->PID_OUT());
    angPid->pError = angPid->error;


    lastPath = agentVel.th();

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

//--------------------------------------------------
INIT_SKILL(CSkillGotoPointAvoid, "gotopointavoid");

CSkillGotoPointAvoid::CSkillGotoPointAvoid(CAgent *_agent) : CSkillGotoPoint(_agent)
{
    counter = 0;
    avoidPenaltyArea = true;
    inited = false;
    keeplooking = false;
    extendStep = -1.0;
    gotopoint = new CSkillGotoPoint(_agent);
    bangBang = new CNewBangBang();
    dynamicStart = true;
    plan2 = false;
    noAvoid = false;
    avoidCenterCircle = false;
    avoidGoalPosts = true;
    stucked = -1;
    counting = 0;
    averageDir.assign(0 , 0);
    addVel.assign(0,0);
    nextPos.invalidate();
    diveMode = false;
    oneTouchMode = false;
    drawPath = false;
}

CSkillGotoPointAvoid::~CSkillGotoPointAvoid()
{
    delete gotopoint;
}

CSkillGotoPointAvoid* CSkillGotoPointAvoid::noRelax()
{
    ourRelaxList.clear();
    oppRelaxList.clear();
    return this;
}

CSkillGotoPointAvoid* CSkillGotoPointAvoid::ourRelax(int element)
{
    if (!ourRelaxList.contains(element)) ourRelaxList.append(element);
    return this;
}

CSkillGotoPointAvoid* CSkillGotoPointAvoid::oppRelax(int element)
{
    if (!oppRelaxList.contains(element)) oppRelaxList.append(element);
    return this;
}

void CSkillGotoPointAvoid::execute()
{
    if(agent == nullptr)
        return;
    agentPos = agent->pos();
    agentVel = agent->vel();
    double dVx,dVy,dW;
    bangBang->setDecMax(1);//conf()->BangBang_DecMax()); // TODO : skill config
    bangBang->setOneTouch(oneTouchMode );
    bangBang->setDiveMode(diveMode);
    if(slowMode || slow)
    {
        bangBang->setVelMax(1.4);
        bangBang->setSlow(true);
    }
    else
    {
        bangBang->setSlow(false);
        bangBang->setVelMax(1);//conf()->BangBang_VelMax()); // TODO : skill Config
    }
    if (!targetPos.valid())
    {
//        agent->waitHere();
        return;
    }
    if (!targetVel.valid())
        vel2.assign(0,0);

    if(drawPath)
    {
        if(agentVel.length() < 0.1)
        {
            pathPoints.clear();
        }
        else
        {
            pathPoints.append(agentPos);
            for(int i = 0 ; i < pathPoints.size() ; i++)
            {
//                draw(Circle2D(pathPoints[i],0.02),QColor(Qt::blue),true);
            }
        }
    }
    else
    {
        pathPoints.clear();
    }

    /////////////////
    if (targetPos.x < wm->field->ourCornerL().x - 0.2) targetPos.x = wm->field->ourCornerL().x;
    if (targetPos.x > wm->field->oppCornerL().x + 0.2) targetPos.x = wm->field->oppCornerL().x;
    if (targetPos.y < wm->field->ourCornerR().y - 0.2) targetPos.y = wm->field->ourCornerR().y;
    if (targetPos.y > wm->field->ourCornerL().y + 0.2) targetPos.y = wm->field->ourCornerL().y;

    if (false) { //conf()->LocalSettings_ParsianWorkShop()) { // TODO : Config
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
    }

    if (lookat.valid())
    {
        targetDir = (lookat - agentPos).norm();
    }

//    knowledge->plotWidgetCustom[1] = agentVel.length();
    //    debug(QString("speed: %1").arg(agentVel.length()),D_MHMMD);
    ///////////
    targetValidate();
    if(noAvoid)
    {
        result.clear();
    }
    else
    {
        // TODO : Avoidance service call
//        agent->initPlanner(agent->id() , targetPos , ourRelaxList , oppRelaxList , avoidPenaltyArea , avoidCenterCircle ,ballObstacleRadius);
//        result.clear();
//        for( int i=agent->pathPlannerResult.size()-1 ; i>=0 ; i-- )
//        {
//            result.append(agent->pathPlannerResult[i]);
//        }
    }

    double dist = 0.0;
    bool flag = false;
    Vector2D dir(0,0);

    if( result.size() > 1 )
        dir = (result[1] - result[0]).norm();

    double D = 0 , alpha = 0 , d = 0 , vf = 0;
    Vector2D lllll ;
    if(!result.empty())
    {
        lllll= result.last();
    }



    //    for( int i=1 ; i<result.size() ; i++ ){
    //        alpha = 0;
    //        if(fabs(Vector2D::angleBetween(result[i] - result[0] , result[i+1] - result[i]).degree()) > 3 ){
    //            if( i+1 == result.size() ){
    //                vf = 0;
    //                alpha = 0;
    //            }
    //            else{
    //                alpha = fabs(Vector2D::angleBetween(result[i] - result[0] , result[i+1] - result[i]).degree());
    //                vf = -1.0259280143 * log(alpha) + 4.570475303;
    //                vf = max(vf , 0.5);
    //            }
    //            D = result[i].dist(result[0]);
    //            lllll = result[i];
    //            d = dist - D;


    //            flag = false;
    //            break;
    //        }
    //    }

    if(result.size() >= 3)
    {
        alpha = fabs(Vector2D::angleBetween(result[1] - result[0] , result[2] - result[1]).degree());
//        debug(QString("alpha : %1").arg(alpha),D_MHMMD);
        lllll = result[1];
        vf = -2 * log(alpha) + 9;
        vf = max(vf , 0.5);
        vf = min (vf,4);
    }
    else
    {
        vf = 0;
        lllll = targetPos;
    }

    ////////////////////// avoid goal posts
    Segment2D goalPostL,goalPostR;
    goalPostL.assign(wm->field->ourGoalL() - Vector2D(0.2,0),wm->field->ourGoalL() + Vector2D(0.1,0));
    goalPostR.assign(wm->field->ourGoalR() - Vector2D(0.2,0),wm->field->ourGoalR() + Vector2D(0.1,0));
    avoidGoalPosts = true;
    Segment2D agenPath(agent->pos(),lllll);
    if(avoidGoalPosts)
    {
        if(agenPath.intersection(goalPostL).isValid())
        {
            lllll = wm->field->ourGoalL() + Vector2D(0.12,0);
        }
        else if(agenPath.intersection(goalPostR).isValid())
        {
            lllll = wm->field->ourGoalR() + Vector2D(0.12,0);
        }

    }
    /////////////////////


    if( noAvoid || result.size() < 3){
        lllll = targetPos;
        vf = 0;
    }

    bangBang->setSmooth(true);// = false;
    bangBang->bangBangSpeed(agentPos,agentVel,agent->dir(),lllll,targetDir,vf,0.016,dVx,dVy,dW);
//    agent->setRobotAbsVel(dVx + addVel.x,dVy + addVel.y,dW); // TODO : Robot Command
//    agent->accelerationLimiter(vf,oneTouchMode);



    counter ++;
}

double CSkillGotoPointAvoid::progress()
{
    if(agentPos.dist(targetPos) < 0.05)
    {
        return 1;
    }
    return 0;
}

CSkillGotoPointAvoid* CSkillGotoPointAvoid::setTargetLook(Vector2D finalPos, Vector2D lookAtPoint)
{
    init(finalPos, Vector2D(1.0, 0.0));
    setFinalVel(Vector2D(0.0, 0.0));
    setLookAt(lookAtPoint);
    setKeepLooking(true);
    return this;
}

CSkillGotoPointAvoid* CSkillGotoPointAvoid::setTarget(Vector2D finalPos, Vector2D finalDir)
{
    init(finalPos, finalDir);
    setFinalVel(Vector2D(0.0, 0.0));
    setLookAt(Vector2D::INVALIDATED);
    setKeepLooking(false);
    return this;
}

double CSkillGotoPointAvoid::timeNeeded(CAgent *_agentT,Vector2D posT,double vMax,QList <int> _ourRelax,QList <int> _oppRelax ,bool avoidPenalty,double ballObstacleReduce,bool _noAvoid)
{

    double _x3;
    double acc = 1;//conf()->BangBang_AccMaxForward(); // TODO :config
    double dec = 1;//conf()->BangBang_DecMax();
    double xSat;
    Vector2D tAgentVel = _agentT->vel();
    Vector2D tAgentDir = _agentT->dir();
    double veltan= (tAgentVel.x)*cos(tAgentDir.th().radian()) + (tAgentVel.y)*sin(tAgentDir.th().radian());
    double offset = 0;
    double velnorm= -1 * (tAgentVel.x)*sin(tAgentDir.th().radian()) + (tAgentVel.y)*cos(tAgentDir.th().radian());
    double distCoef = 1, distEffect = 1, angCoef = 0.003;
    double dist = 0;
    double rrtAngSum = 0;
    QList <Vector2D> _result;
    Vector2D _target;

    if(_noAvoid)
    {
        _result.clear();
    }
    else
    {
        // TODO : Avoidance Service
//        _agentT->initPlanner(_agentT->id(),posT,_ourRelax,_oppRelax,avoidPenalty,false,ballObstacleReduce);
//        _result.clear();
//        for( int i = _agentT->pathPlannerResult.size()-1 ; i>=0 ; i-- )
//        {
//            _result.append(_agentT->pathPlannerResult[i]);
//        }
    }

    if( _result.size() >= 3) {
        for(int i = 0 ; i < _result.size() - 1; i++)
        {
            dist += _result[i].dist(_result[i+1]);
        }
        for(int i = 1 ; i < _result.size() - 1; i++)
        {
            rrtAngSum += fabs(Vector2D::angleBetween(_result[i] - _result[i-1] , _result[i+1] - _result[i]).degree());
        }
        distEffect = dist / _agentT->pos().dist(posT);
        distEffect += rrtAngSum*angCoef;
        distEffect = max(1,distEffect);
    }

    if(tAgentVel.length() < 0.2)
    {
        acc = 1;//(conf()->BangBang_AccMaxForward() + conf()->BangBang_AccMaxNormal())/2; // TODO : Skill Config
    }
    else
    {
        // TODO : Skill Config
        acc = 1;//conf()->BangBang_AccMaxForward()*(fabs(veltan)/tAgentVel.length()) + conf()->BangBang_AccMaxNormal()*(fabs(velnorm)/tAgentVel.length());
    }



    double vMaxReal = sqrt(((_agentT->pos().dist(posT) + (tAgentVel.length()*tAgentVel.length()/2*acc))*2*acc*dec)/(acc+dec));
    vMaxReal = min(vMaxReal, 4);
    vMax = min(vMax, vMaxReal);
    xSat = sqrt(((vMax*vMax)-(tAgentVel.length()*tAgentVel.length()))/acc) + sqrt((vMax*vMax)/dec);
//    _x3 = ( -1* tAgentVel.length()*tAgentVel.length()) / (-2 * fabs(conf()->BangBang_DecMax())) ;
    _x3 = ( -1* tAgentVel.length()*tAgentVel.length()) / (-2 * fabs(1)) ; // TODO : Skill Config

    if(_agentT->pos().dist(posT) < _x3 ) {
//        return max(0,(tAgentVel.length()/conf()->BangBang_DecMax() - offset) * distEffect);
        return max(0,(tAgentVel.length()/1 - offset) * distEffect); // TODO : Skill Config
    }
    else if(tAgentVel.length() < (vMax)){
        if(_agentT->pos().dist(posT) > xSat)
        {
            return max(0, (-1*offset + vMax/dec + (vMax-tAgentVel.length())/acc + (_agentT->pos().dist(posT) - ((vMax*vMax/(2*dec)) + ((vMax+tAgentVel.length())*(vMax-tAgentVel.length())/acc))/2)/vMax) * distEffect);
        }
        return max(0, (vMax/dec + (vMax-tAgentVel.length())/acc - offset)*distEffect);

    }
    else
    {
        return max(0,(vMax/dec + (_agentT->pos().dist(posT) - ((vMax*vMax/(2*dec)) ))/vMax - offset) * distEffect);
    }

}
