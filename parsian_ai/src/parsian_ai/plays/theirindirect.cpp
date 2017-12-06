#include "parsian_ai/plays/theirindirect.h"
#include "parsian_ai/roles/playmake.h"
#include "parsian_ai/soccer.h"

CTheirIndirect::CTheirIndirect(){

}

CTheirIndirect::~CTheirIndirect(){

}

void CTheirIndirect::reset(){
	executedCycles = 0;
}

void CTheirIndirect::init(QList<int> _agents){
	setAgentsID(_agents);
	initMaster();

//	if( knowledge->getLastPlayExecuted() != TheirIndirectPlay ){
//		reset();
//	}
//	knowledge->setLastPlayExecuted(TheirIndirectPlay);
}

void CTheirIndirect::execute_0(){

}

void CTheirIndirect::execute_1(){
	executedCycles++;

	chooseBlocker();
}

void CTheirIndirect::execute_2(){
	executedCycles++;

	chooseBlocker();

	appendRemainingsAgents(markAgents);
}

void CTheirIndirect::execute_3(){
	executedCycles++;

	chooseBlocker();

	appendRemainingsAgents(markAgents);
}

void CTheirIndirect::execute_4(){
	executedCycles++;

	chooseBlocker();

	appendRemainingsAgents(markAgents);
}

void CTheirIndirect::execute_5(){
	executedCycles++;

	chooseBlocker();

	appendRemainingsAgents(markAgents);
}

void CTheirIndirect::execute_6(){
	executedCycles++;

	chooseBlocker();

	appendRemainingsAgents(markAgents);
}
