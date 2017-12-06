#include "parsian_ai/plays/theirpenalty.h"
#include "parsian_ai/roles/playmake.h"
#include "parsian_ai/soccer.h"

CTheirPenalty::CTheirPenalty(){
}

CTheirPenalty::~CTheirPenalty(){

}

void CTheirPenalty::reset(){
    positioningPlan.reset();
    executedCycles = 0;
}

void CTheirPenalty::init(QList<int> _agents) {
    setAgentsID(_agents);
    initMaster();

//	if( knowledge->getLastPlayExecuted() != TheirPenaltyPlay ){
//		reset();
//	}
//	knowledge->setLastPlayExecuted(TheirPenaltyPlay);
}

void CTheirPenalty::execute_0(){

}

void CTheirPenalty::execute_1(){
    executedCycles++;
    if (gameState->penaltyShootout()) {
        choosePlayMaker();
    }
    appendRemainingsAgents(positionAgents);

    setFormation("TheirP");
}

void CTheirPenalty::execute_2(){
    executedCycles++;

    appendRemainingsAgents(positionAgents);

    setFormation("TheirP");
}

void CTheirPenalty::execute_3(){
    executedCycles++;

    appendRemainingsAgents(positionAgents);

    setFormation("TheirP");
}

void CTheirPenalty::execute_4(){
    executedCycles++;

    appendRemainingsAgents(positionAgents);

    setFormation("TheirP");
}

void CTheirPenalty::execute_5(){
    executedCycles++;

    appendRemainingsAgents(positionAgents);

    setFormation("TheirP");
}

void CTheirPenalty::execute_6(){
    executedCycles++;

    appendRemainingsAgents(positionAgents);

    setFormation("TheirP");
}
