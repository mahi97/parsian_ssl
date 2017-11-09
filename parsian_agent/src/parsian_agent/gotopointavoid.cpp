//
// Created by parsian-ai on 9/29/17.
//

#include <parsian_agent/gotopointavoid.h>
#include <parsian_agent/config.h>


INIT_SKILL(CSkillGotoPointAvoid, "gotopointavoid");


CSkillGotoPointAvoid::CSkillGotoPointAvoid(Agent *_agent) : CSkill(_agent)
{
    counter = 0;
    avoidPenaltyArea = static_cast<unsigned char>(true);
    inited = false;
    keeplooking = static_cast<unsigned char>(false);
    extendStep = static_cast<float>(-1.0);
    gotopoint = new CSkillGotoPoint(_agent);
    bangBang = new CNewBangBang();
    dynamicStart = static_cast<unsigned char>(true);
    plan2 = static_cast<unsigned char>(false);
    noAvoid = false;
    avoidCenterCircle = false;
    avoidGoalPosts = true;
    stucked = -1;
    counting = 0; // SGP
    averageDir.assign(0, 0);
    addVel.assign(0, 0);
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

    //drawer->draw(Circle2D(Vector2D(1,0),0.1),QColor(Qt::red),true);
    if(agent == nullptr)
        return;
    agentPos = agent->pos();
    agentVel = agent->vel();
    double dVx,dVy,dW;
    bangBang->setDecMax(conf.groups.bang_bang.DecMax);
    bangBang->setOneTouch(oneTouchMode);
    bangBang->setDiveMode(diveMode);
    if(slowMode || slowShot)
    {
        bangBang->setVelMax(1.4);
        bangBang->setSlow(true);
    }
    else
    {
        bangBang->setSlow(false);
        bangBang->setVelMax(conf.groups.bang_bang.VelMax);
    }
    if (!Vector2D(targetPos).valid())
    {
        agent->waitHere();
        return;
    }
    if (!targetVel.valid())
        targetVel.assign(0,0);

    if(drawPath)
    {
        if(agentVel.length() < 0.1)
        {
            pathPoints.clear();
        }
        else
        {
            pathPoints.append(agentPos);
            for (auto pathPoint : pathPoints) {
            //    drawer->draw(Circle2D(pathPoint ,0.02),QColor(Qt::blue),true); //todo shoule uncommented
            }
        }
    }
    else
    {
        pathPoints.clear();
    }


    /////////////////
//    if (targetPos.x < wm->field->ourCornerL().x - 0.2) targetPos.x = wm->field->ourCornerL().x;
//    if (targetPos.x > wm->field->oppCornerL().x + 0.2) targetPos.x = wm->field->oppCornerL().x;
//    if (targetPos.y < wm->field->ourCornerR().y - 0.2) targetPos.y = wm->field->ourCornerR().y;
//    if (targetPos.y > wm->field->ourCornerL().y + 0.2) targetPos.y = wm->field->ourCornerL().y;


    if (lookAt.valid())
    {
        targetDir = (lookAt - agentPos).norm();
    }

//    knowledge->plotWidgetCustom[1] = agentVel.length();
    //    debug(QString("speed: %1").arg(agentVel.length()),D_MHMMD);
    ///////////

//    if (targetPos.x < wm->field->ourCornerL().x - 0.2) targetPos.x = wm->field->ourCornerL().x;
//    if (targetPos.x > wm->field->oppCornerL().x + 0.2) targetPos.x = wm->field->oppCornerL().x;
//    if (targetPos.y < wm->field->ourCornerR().y - 0.2) targetPos.y = wm->field->ourCornerR().y;
//    if (targetPos.y > wm->field->ourCornerL().y + 0.2) targetPos.y = wm->field->ourCornerL().y;

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

    if (lookAt.valid())
    {
        targetDir = (lookAt - agentPos).norm();
    }

    if(noAvoid )
    {
        result.clear();
    }
    else
    {

        ROS_INFO_STREAM("Target: " << targetPos.x << targetPos.y);
        ROS_INFO_STREAM("ourRel: " << ourRelaxList.size());
        ROS_INFO_STREAM("oppRel: " << oppRelaxList.size());
        ROS_INFO_STREAM("apa: " << avoidPenaltyArea);
        ROS_INFO_STREAM("acc: " << avoidCenterCircle);
        ROS_INFO_STREAM("bor: " << ballObstacleRadius);
        ROS_INFO_STREAM("ff: " << wm->field->_FIELD_WIDTH);
        ROS_INFO_STREAM("dd: " << wm->field->_FIELD_HEIGHT);
        agent->initPlanner(targetPos , ourRelaxList , oppRelaxList , avoidPenaltyArea , avoidCenterCircle ,ballObstacleRadius);
        result.clear();
        for(long i = agent->pathPlannerResult.size()-1 ; i >= 0 ; i-- )
        {
            ROS_INFO_STREAM("POS : " << agent->pathPlannerResult[i].x << " , " << agent->pathPlannerResult[i].y);
            result.append(agent->pathPlannerResult[i]);
//            drawer->draw(Circle2D(agent->pathPlannerResult[i],0.01),QColor(Qt::red));
             }
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



//        for( int i=1 ; i<result.size() ; i++ ){
//            alpha = 0;
//            if(fabs(Vector2D::angleBetween(result[i] - result[0] , result[i+1] - result[i]).degree()) > 3 ){
//                if( i+1 == result.size() ){
//                    vf = 0;
//                    alpha = 0;
//                }
//                else{
//                    alpha = fabs(Vector2D::angleBetween(result[i] - result[0] , result[i+1] - result[i]).degree());
//                    vf = -1.0259280143 * log(alpha) + 4.570475303;
//                    vf = max(vf , 0.5);
//                }
//                D = result[i].dist(result[0]);
//                lllll = result[i];
//                d = dist - D;
//
//
//                flag = false;
//                break;
//            }
//        }

    if(result.size() >= 3)
    {
        alpha = fabs(Vector2D::angleBetween(result[1] - result[0] , result[2] - result[1]).degree());
        DEBUG(QString("alpha : %1").arg(alpha),D_MHMMD);
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
    agent->setRobotAbsVel(dVx + addVel.x,dVy + addVel.y,dW);
//    agent->setRobotVel(2,2,10);
        agent->accelerationLimiter(vf,oneTouchMode);
    ROS_INFO_STREAM("vx: "<<dVx<<"vy: "<<dVy<<"w: "<< dW);
    ROS_INFO_STREAM("x: "<<agentPos.x<<"y: "<<agentPos.y<<"w: "<< dW);

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
    setTargetpos(finalPos);
    setTargetdir(Vector2D{1.0, 0.0});
    setTargetvel(Vector2D(0.0, 0.0));
    setLookat(lookAtPoint);
    setKeeplooking(true);
    return this;
}

CSkillGotoPointAvoid* CSkillGotoPointAvoid::setTarget(Vector2D finalPos, Vector2D finalDir)
{
    setTargetpos(finalPos);
    setTargetdir(finalDir);
    setTargetvel(Vector2D(0.0, 0.0));
    setLookat(Vector2D::INVALIDATED);
    setKeeplooking(false);
    return this;
}

void CSkillGotoPointAvoid::init(Vector2D target, Vector2D _targetDir, Vector2D _targetVel)
{
    targetPos = target;
    targetDir = _targetDir;
    targetVel = _targetVel;
}

double CSkillGotoPointAvoid::timeNeeded(Agent *_agentT,Vector2D posT,double vMax,QList <int> _ourRelax,QList <int> _oppRelax ,bool avoidPenalty,double ballObstacleReduce,bool _noAvoid)
{

    double _x3;
    double acc = conf.groups.bang_bang.AccMaxForward;
    double dec = conf.groups.bang_bang.DecMax;
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

        _agentT->initPlanner(posT,_ourRelax,_oppRelax,avoidPenalty,false,ballObstacleReduce);
        _result.clear();
        for(unsigned long i = _agentT->pathPlannerResult.size()-1 ; i>=0 ; i-- )
        {
            _result.append(_agentT->pathPlannerResult[i]);
        }
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
        distEffect = std::max(1.0, distEffect);
    }

    if(tAgentVel.length() < 0.2)
    {
        acc = (conf.groups.bang_bang.AccMaxForward + conf.groups.bang_bang.AccMaxNormal)/2;

    }
    else
    {
        acc =conf.groups.bang_bang.AccMaxForward*(fabs(veltan)/tAgentVel.length()) + conf.groups.bang_bang.AccMaxNormal*(fabs(velnorm)/tAgentVel.length());
    }



    double vMaxReal = sqrt(((_agentT->pos().dist(posT) + (tAgentVel.length()*tAgentVel.length()/2*acc))*2*acc*dec)/(acc+dec));
    vMaxReal = min(vMaxReal, 4);
    vMax = min(vMax, vMaxReal);
    xSat = sqrt(((vMax*vMax)-(tAgentVel.length()*tAgentVel.length()))/acc) + sqrt((vMax*vMax)/dec);
    _x3 = ( -1* tAgentVel.length()*tAgentVel.length()) / (-2 * fabs(conf.groups.bang_bang.DecMax)) ;

    if(_agentT->pos().dist(posT) < _x3 ) {
        return std::max(0.0,(tAgentVel.length()/ conf.groups.bang_bang.DecMax - offset) * distEffect);
    }
    if(tAgentVel.length() < (vMax)){
        if(_agentT->pos().dist(posT) > xSat)
        {
            return std::max(0.0, (-1*offset + vMax/dec + (vMax-tAgentVel.length())/acc + (_agentT->pos().dist(posT) - ((vMax*vMax/(2*dec)) + ((vMax+tAgentVel.length())*(vMax-tAgentVel.length())/acc))/2)/vMax) * distEffect);
        }
        return std::max(0.0, (vMax/dec + (vMax-tAgentVel.length())/acc - offset)*distEffect);

    }
    else
    {
        return std::max(0.0, (vMax/dec + (_agentT->pos().dist(posT) - ((vMax*vMax/(2*dec)) ))/vMax - offset) * distEffect);
    }

}
