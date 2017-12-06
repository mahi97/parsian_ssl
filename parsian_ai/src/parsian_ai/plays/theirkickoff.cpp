#include "parsian_ai/plays/theirkickoff.h"
#include "parsian_ai/roles/playmake.h"
#include "parsian_ai/soccer.h"

CTheirKickOff::CTheirKickOff(){
}

CTheirKickOff::~CTheirKickOff(){

}

void CTheirKickOff::reset(){
    executedCycles = 0;
}

void CTheirKickOff::init(const QList<CAgent*>& _agents){
    setAgentsID(_agents);
    initMaster();

//	if( knowledge->getLastPlayExecuted() != TheirKickOffPlay ){
//		reset();
//	}
//	knowledge->setLastPlayExecuted(TheirKickOffPlay);
}

void CTheirKickOff::execute_x(){
    if (agentsID.empty()) return;
    executedCycles++;
    chooseBlocker();
    if (agentsID.size() > 1) appendRemainingsAgents(markAgents);
}
