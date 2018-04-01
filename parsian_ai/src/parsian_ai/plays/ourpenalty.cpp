#include <parsian_ai/plays/ourpenalty.h>

COurPenalty::COurPenalty() = default;

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

}

void COurPenalty::execute_x() {
    if (playMakeAgent == nullptr || (playMakeAgent->id() == -1)) {
        return;
    }
    isPenaltyShootOut = gameState->ourPenaltyShootout();
    playmakeRole->execute();
    if(isPenaltyShootOut)
        executeShootoutPositioning();
    else
        executeNormalPositioning();
}

