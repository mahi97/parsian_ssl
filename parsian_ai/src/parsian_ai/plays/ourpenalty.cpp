#include <parsian_ai/plays/ourpenalty.h>

COurPenalty::COurPenalty() = default;

COurPenalty::~COurPenalty() = default;

void COurPenalty::reset(){
	positioningPlan.reset();
	executedCycles = 0;
}

void COurPenalty::init(QList<int> _agents){
	setAgentsID(_agents);
	initMaster();

    // TODO : Last Play seems important!
//	if( knowledge->getLastPlayExecuted() != OurPenaltyPlay ){
//		reset();
//	}
//	knowledge->setLastPlayExecuted(OurPenaltyPlay);
}

void COurPenalty::penaltyKick(){
    executedCycles++;
    if (gameState->penaltyShootout()) {
        playOnFlag = false;
    }
    choosePlayMaker();

    appendRemainingsAgents(positionAgents);

    setFormation("OurP");

}

void COurPenalty::execute_0(){
    penaltyKick();
}

void COurPenalty::execute_1(){
    penaltyKick();
}

void COurPenalty::execute_2(){
    penaltyKick();
}

void COurPenalty::execute_3(){
    penaltyKick();
}

void COurPenalty::execute_4(){
    penaltyKick();
}

void COurPenalty::execute_5(){
    penaltyKick();
}

void COurPenalty::execute_6(){
    penaltyKick();
}
