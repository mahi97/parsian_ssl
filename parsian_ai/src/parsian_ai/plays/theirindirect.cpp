#include "parsian_ai/plays/theirindirect.h"
#include "parsian_ai/roles/playmake.h"
#include "parsian_ai/soccer.h"

CTheirIndirect::CTheirIndirect() : CMasterPlay() {

}

CTheirIndirect::~CTheirIndirect(){

}

void CTheirIndirect::reset(){
	executedCycles = 0;
}

void CTheirIndirect::init(QList<CAgent*> _agents){
	setAgentsID(_agents);
	initMaster();

//	if( knowledge->getLastPlayExecuted() != TheirIndirectPlay ){
//		reset();
//	}
//	knowledge->setLastPlayExecuted(TheirIndirectPlay);
}

void CTheirIndirect::execute_x(){
	if (agentsID.empty()) return;
	if (agentsID.size() == 1) {
        executedCycles++;
        chooseBlocker();
        return;
    }
    appendRemainingsAgents(markAgents);
}