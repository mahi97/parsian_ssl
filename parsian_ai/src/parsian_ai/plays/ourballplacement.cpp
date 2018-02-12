#include <search.h>
#include "parsian_ai/plays/ourballplacement.h"

COurBallPlacement::COurBallPlacement(){

}

COurBallPlacement::~COurBallPlacement(){

}

void COurBallPlacement::reset(){

}

void COurBallPlacement::init(const QList<Agent*>& _agents){
    setAgentsID(_agents);
    initMaster();

//    if( knowledge->getLastPlayExecuted() != OurBallPlacement ){
//        reset();
//    }
//    knowledge->setLastPlayExecuted(OurBallPlacement);
}


void COurBallPlacement::execute_x(Vector2D pos) {
    DBUG("execute_0 is running", D_ATOUSA);
    DBUG("ballPlacement execute_0", D_ERROR);
    ROS_INFO("Executaion X");
    //ROS_INFO_STREAM(23 << "asdf" << 23.3)
    Vector2D ballpos = Vector2D(wm->ball->pos.x, wm->ball->pos.y);
    double dist = 0;
    double mindist = 10000;
    static int minIndexPos = 0;
    static CAgent *ap = agents[0];
    CAgent *pap = ap;
    for (int i = 0; i < agents.size(); i++) {
        dist = agents[i]->pos().dist(pos);
        if (dist < mindist) {
            mindist = dist;
            minIndexPos = i;
            ap = agents[i];
        }
    }
    mindist = 10000;
    static int minIndex = 0;
    static CAgent *a = agents[0];
    CAgent *pa = a;
    for (int i = 0; i < agents.size(); i++) {
        dist = agents[i]->pos().dist(ballpos);
        if (dist < mindist && minIndexPos != i) {
            mindist = dist;
            minIndex = i;
            a = agents[i];
        }
    }
    auto *nothing = new NoAction;
    if(agents.size() <= 0) {
        return;
    }
    if (pa->id() != a->id())
        pa->action = nothing;
    ROS_INFO("Executaion Y");
    if (pap->id() != ap->id())
        pap->action = nothing;
    ROS_INFO("Executaion !!!!!!!!!!!");
    auto *gp = new GotopointavoidAction();
    gp->setTargetpos(pos);
    gp->setLookat(ballpos);
    gp->setSlowmode(true);
    gp->setRoller(7);
    auto *rec = new ReceivepassAction();
    rec->setReceiveradius(0.5);
    rec->setTarget(pos);
    rec->setSlow(true);
    Circle2D c{agents[minIndexPos]->pos() + (agents[minIndexPos]->dir().norm() * 0.1), 0.1};
    agents[minIndexPos]->action = rec;
    if (c.contains(ballpos))
        agents[minIndexPos]->action = gp;
    auto *pass = new KickAction();
    pass->setTarget(pos);
    int power = 100 * agents[minIndexPos]->pos().dist(ballpos);
    ROS_INFO_STREAM(power);
    pass->setKickspeed(power);
    pass->setSpin(5);
    pass->setSlow(true);
    auto *gpa = new GotopointavoidAction;
    gpa->setTargetpos(Vector2D(wm->ball->pos.x - 0.15, wm->ball->pos.y));
    gpa->setSlowmode(true);
    gpa->setBallobstacleradius(0.1);
    gpa->setLookat(ballpos);

    ROS_INFO("Executaion !");
    if (agents[minIndexPos]->pos().dist(ballpos) > 0.50 && wm->ball->vel.length() < 0.3){
        if (agents[minIndex]->pos().dist(ballpos) > 0.3)
            agents[minIndex]->action = gpa;
        agents[minIndex]->action = pass;
    }
    else
        agents[minIndex]->action = nothing;
    ROS_INFO_STREAM(minIndexPos);

    //end

/*
    switch (state) {
        case BallPlacement::GO_FOR_BALL:


            break;
        case BallPlacement::NoState:break;
        case BallPlacement::PASS:break;
        case BallPlacement::RECIVE_AND_POS:break;
        case BallPlacement::FINAL_POS:break;
        case BallPlacement::DONE:break;
    }*/
    //if

}