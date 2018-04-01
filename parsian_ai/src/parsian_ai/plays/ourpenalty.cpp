#include <parsian_ai/plays/ourpenalty.h>

COurPenalty::COurPenalty() : CMasterPlay()
{
    for(int i{0}; i<8; i++)
        moveSkill[i] = new GotopointavoidAction();
}

COurPenalty::~COurPenalty() = default;

void COurPenalty::reset() {
    positioningPlan.reset();
    executedCycles = 0;
}

void COurPenalty::init(const QList<Agent*>& _agents) {
    setAgentsID(_agents);
    initMaster();
}

void COurPenalty::setPlaymake(Agent* _playmakeAgent)
{
    playMakeAgent = _playmakeAgent;
    playmakeRole->assign(playMakeAgent);
}

void COurPenalty::executeShootoutPositioning()
{

}

void COurPenalty::executeNormalPositioning()
{
//    generatePositions();
//    assignSkills();
    if(agents.isEmpty())
        return;
    moveSkill[0]->setTargetpos(Vector2D(0, 0));
    agents[0]->action = moveSkill[0];
}

void COurPenalty::execute_x() {
    if (playMakeAgent == nullptr || (playMakeAgent->id() == -1)) {
        return;
    }
    isPenaltyShootOut = gameState->ourPenaltyShootout();
    //playmakeRole->execute();
    if(isPenaltyShootOut)
        executeShootoutPositioning();
    else
        executeNormalPositioning();
}

Vector2D COurPenalty::getEmptyTarget(Vector2D _position, double _radius) {
    Vector2D tempTarget, finalTarget, position;
    double escapeRad;
    int oppCnt = 0;
    bool posFound;
    escapeRad = _radius;
    position  = _position;
    finalTarget = position;
    for (double dist = 0.0 ; dist <= 0.5 ; dist += 0.2) {
        for (double ang = -180.0 ; ang <= 180.0 ; ang += 60.0) {
            tempTarget = position + Vector2D::polar2vector(dist, ang);
            ////should check
            if (wm->field->isInOppPenaltyArea(tempTarget + (wm->field->oppGoal() - tempTarget).norm() * 0.3)) {
                continue;
            }
            for (int i = 0; i < wm->opp.activeAgentsCount(); i++) {
                if (Circle2D(wm->opp.active(i)->pos, 0.07).contains(tempTarget)) {
                    oppCnt = 1;
                    break;
                }

            }
            if (!oppCnt) {
                finalTarget = tempTarget;
                posFound = true;
                break;
            }
        }
        if (posFound) {
            break;
        }
    }

    return finalTarget;
}

