#include "parsian_ai/plays/stopplay.h"

CStopPlay::CStopPlay() : CMasterPlay() {
    for (int i = 0; i < 6 ; i++) {
        gpa[i] = new GotopointavoidAction();
        gpa[i]->setSlowmode(true);
        gpa[i]->setNoavoid(false);
    }
}

CStopPlay::~CStopPlay(){

}

void CStopPlay::reset(){
    positioningPlan.reset();
    executedCycles = 0;
}

void CStopPlay::init(const QList<CAgent*> &_agents){
    setAgentsID(_agents);
    initMaster();

//    if( knowledge->getLastPlayExecuted() != StopPlay ){
//        reset();
//    }
//    knowledge->setLastPlayExecuted(StopPlay);
}

void CStopPlay::stopPosition() {
    executedCycles++;
    setFormation("Stop7");

    for (int i = 0;i < 6 ; i++) {
        if(conf.LineUpPosition == "OurCornerL") {
            rolePosition[i] = Vector2D(0.25*i-0.5, wm->field->_FIELD_HEIGHT/2);

        } else if(conf.LineUpPosition == "OurCornerR") {
            rolePosition[i] = Vector2D(0.25*i-0.5, - wm->field->_FIELD_HEIGHT/2);

        } else if(conf.LineUpPosition == "parsian") {
//            if (conf()->LocalSettings_OurTeamSide == "Left") {
                rolePosition[i] = Vector2D(0.25*i-1, - wm->field->_FIELD_HEIGHT/2);
//
//            } else {
//                rolePosition[i] = Vector2D((-0.25*i+1), - wm->field->_FIELD_HEIGHT/2);
//
//            }
        }

    }

    for(int i = 0; i < agentsID.size(); i++) {
        gpa[i]->setTargetpos(rolePosition[i]);
        gpa[i]->setTargetdir(wm->field->oppGoal());
        agentsID[i]->action = gpa[i];
    }

}

void CStopPlay::execute_0(){
    stopPosition();
}

void CStopPlay::execute_1(){
    stopPosition();

}

void CStopPlay::execute_2(){
    stopPosition();

}

void CStopPlay::execute_3(){
    stopPosition();

}

void CStopPlay::execute_4(){
    stopPosition();

}

void CStopPlay::execute_5(){
    stopPosition();

}

void CStopPlay::execute_6(){
    stopPosition();

}
