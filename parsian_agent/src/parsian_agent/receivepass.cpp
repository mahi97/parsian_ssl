#include "parsian_agent/receivepass.h"

INIT_SKILL(CSkillReceivePass,"receivepass");

CSkillReceivePass::CSkillReceivePass(Agent *_agent)
{
    agent = _agent;
    gotopointavoid = new CSkillGotoPointAvoid(_agent);
    slow = true;
    receiveRadius = 1.0;
    cirThresh = 0;
    kickCirThresh = 0;
    ignoreAngle = true;

}

double CSkillReceivePass::progress()
{
    return 0.0;
}

CSkillReceivePass::~CSkillReceivePass() = default;

kkRPMode CSkillReceivePass::decideMode()
{
    Circle2D tempCircle(target, 0.2 + cirThresh);
    Circle2D tempCircle2(target, receiveRadius);
    drawer->draw(tempCircle, QColor(Qt::cyan));

        cirThresh = 1.0;
        Circle2D tempKickCircle(kkAgentPos, 0.3 + kickCirThresh);
        Segment2D tempBallPath(kkBallPos, kkBallPos + wm->ball->vel.norm()*10);
        drawer->draw(tempBallPath,QColor(Qt::yellow));
        Vector2D sol1, sol2;
        if(tempCircle2.intersection(tempBallPath, &sol1, &sol2)  )
        {
            kickCirThresh = 0;
            return RPINTERSECT;
        }

        return RPWAITPOS;

}

void CSkillReceivePass::execute()
{
    ballRealVel = wm->ball->vel.length(); // TODO : Check //knowledge->getRealBallVel();
    gotopointavoid->setSlowmode(slow);
    gotopointavoid->setAgent(agent);
    gotopointavoid->setNoavoid(false);
    gotopointavoid->setBallobstacleradius(0.4);
    kkBallPos = wm->ball->pos;
    kkAgentPos = agent->pos();
    receivePassMode = decideMode();

    Segment2D ballPath;
    ballPath.assign(kkBallPos,kkBallPos + wm->ball->vel.norm()*(kkAgentPos.dist(kkBallPos)+1));
    drawer->draw(ballPath,"red");


    Vector2D oneTouchDir;
        oneTouchDir = (kkBallPos - kkAgentPos).norm();

    drawer->draw(Segment2D(Vector2D(0,0), Vector2D(0,0)+oneTouchDir.norm()), QColor(Qt::red));

//    Vector2D addVec = Vector2D(0.095*cos((target-kkAgentPos).th().radian()), 0.095*sin((target-kkAgentPos).th().radian()));
    Vector2D intersectPos;
    Line2D agentDirLine(kkAgentPos , kkAgentPos + oneTouchDir.norm());
    Line2D agentPerLine(kkAgentPos, kkAgentPos + oneTouchDir.norm());
    agentPerLine = agentDirLine.perpendicular(kkAgentPos);
    Line2D tempBallPath(kkBallPos, kkBallPos + wm->ball->vel.norm());
    double tempDampSpeed;

    Vector2D tempVecDamp, tempDampTarget;
    switch (receivePassMode)
    {
        case RPWAITPOS:
            if (target.valid())
                gotopointavoid->init(target, oneTouchDir);
            else
                gotopointavoid->init(agent->pos(), oneTouchDir);
            agent->setRoller(0);
            gotopointavoid->setSlowmode(false);
            gotopointavoid->execute();
            DEBUG("RPWAITPOS",D_KK);
            break;
        case  RPDAMP:
        case RPRECEIVE:
            tempVecDamp = (kkAgentPos - kkBallPos).norm();
            tempDampSpeed = (ballRealVel - agent->vel().length())*0.05;
            if(tempDampSpeed > 0.003) tempDampSpeed = 0.003;
            tempDampTarget = kkBallPos + (kkAgentPos - kkBallPos).norm()*0.10 + tempVecDamp*tempDampSpeed;
            gotopointavoid->init(tempDampTarget,oneTouchDir);
            gotopointavoid->execute();
            DEBUG("RPdamp-Back",D_KK);
            break;
        case RPINTERSECT:
            agent->setRoller(2); // TODO : Robot Command
            intersectPos = agentPerLine.intersection(tempBallPath);
            gotopointavoid->init(intersectPos,oneTouchDir);
            gotopointavoid->setSlowmode(false);
            gotopointavoid->setOnetouchmode(true);
            gotopointavoid->execute();
            DEBUG("Intercept", D_KK);

            break;
        case RPNONE:
            gotopointavoid->init(target,oneTouchDir);
            gotopointavoid->setOnetouchmode(true);
            gotopointavoid->setSlowmode(false);
            gotopointavoid->execute();
            DEBUG("RP",D_KK);
            break;
    }

}