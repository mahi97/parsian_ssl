#include <parsian_ai/plays/ourpenalty.h>

COurPenalty::COurPenalty() : CMasterPlay()
{
    initMaster();
    playmakeRole = new CRolePlayMake(nullptr);
}

COurPenalty::~COurPenalty() = default;

void COurPenalty::reset() {
    positioningPlan.reset();
    executedCycles = 0;
}

void COurPenalty::init(const QList<Agent*>& _agents) {
    setAgentsID(_agents);
    ROS_INFO_STREAM("penalty: init our agents");
}

void COurPenalty::setPlaymake(Agent* _playmakeAgent)
{
    ROS_INFO_STREAM("penalty:  before set playmake to: ");
    if(_playmakeAgent != nullptr)
    {
        playMakeAgent = _playmakeAgent;
        playmakeRole->assign(playMakeAgent);
    }
    ROS_INFO_STREAM("penalty: set playmake to: " << playMakeAgent->id());
}

void COurPenalty::executeShootoutPositioning()
{

}

void COurPenalty::executeNormalPositioning()
{
    ROS_INFO_STREAM("penalty: normal positioning");
    if(agents.isEmpty())
        return;
    generatePositions();
    assignSkills();
}

void COurPenalty::execute_x() {
    ROS_INFO_STREAM("penalty: execute_x");
    if (playMakeAgent == nullptr || (playMakeAgent->id() == -1)) {
        ROS_INFO_STREAM("penalty: playmakeagent is null");
        return;
    }
    isPenaltyShootOut = gameState->ourPenaltyShootout();
    //playmakeRole->execute();
    if(isPenaltyShootOut)
    {
        ROS_INFO_STREAM("penalty: shootout");
        executeShootoutPositioning();
    }
    else
    {
        ROS_INFO_STREAM("penalty: norma");
        executeNormalPositioning();
    }
}

void COurPenalty::generatePositions()
{
    ROS_INFO_STREAM("penalty: generate positions");
    positions.clear();
    double penaltyPositioningOffset = 0.4;
    double penaltyRuleOffset = 0.4;
    double maximum_x_width = wm->field->oppGoalL().x - (wm->field->_PENALTY_DEPTH + penaltyRuleOffset + penaltyPositioningOffset);
    for(int i{}; i < agents.size(); i++)
    {
        positions.append(getEmptyTarget(Vector2D{maximum_x_width, pow(-1, i)* i/2}, penaltyPositioningOffset));
    }
}


Vector2D COurPenalty::getEmptyTarget(Vector2D _position, double _radius)
{
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

void COurPenalty::assignSkills()
{
    ROS_INFO_STREAM("penalty: assign skills");
    moveSkills.clear();
    for(int i{0}; i<agents.count(); i++)
    {
        moveSkills.append(new GotopointavoidAction());
        moveSkills[i]->setTargetpos(positions[i]);
        moveSkills[i]->setTargetdir(wm->field->oppGoal());
        agents[i]->action = moveSkills[i];
    }
}

