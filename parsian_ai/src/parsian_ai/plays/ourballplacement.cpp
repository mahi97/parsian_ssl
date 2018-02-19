#include <search.h>
#include "parsian_ai/plays/ourballplacement.h"

COurBallPlacement::COurBallPlacement(){

}

COurBallPlacement::~COurBallPlacement(){

}

void COurBallPlacement::reset(){
    flag = false;
}

void COurBallPlacement::init(const QList<Agent*>& _agents){
    setAgentsID(_agents);
    initMaster();
    //if(knowledge->getLastPlayExecuted() != OurBallPlacement ){
    //    reset();

    //}
   // knowledge->setLastPlayExecuted(OurBallPlacement);
}


void COurBallPlacement::execute_x() {
    DBUG("execute_0 is running", D_ATOUSA);
    DBUG("ballPlacement execute_0", D_ERROR);
    ROS_INFO("Executaion X");
    ROS_INFO_STREAM(flag);
    //ROS_INFO_STREAM(23 << "asdf" << 23.3)
    if(agents.size() <= 0) {
        return;
    }
    Vector2D ballpos = Vector2D(wm->ball->pos.x, wm->ball->pos.y);
    Vector2D pos = Vector2D(0 , 0);
    double dist = 0;
    double mindist = 10000;
    static int minIndexPos = 0;
    static CAgent *ap;

    for (int i = 0; i < agents.size() && flag == false ; i++) {
        dist = agents[i]->pos().dist(pos);
        if (dist < mindist) {
            mindist = dist;
            minIndexPos = i;
            ap = agents[i];
        }
    }
    flag = true;
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
    if (pa->id() != a->id())
        pa->action = nothing;
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
    if (agents[minIndexPos]->pos().dist(pos) <= 0.15 && agents[minIndexPos]->pos().dist(ballpos) <= 0.15){
        agents[minIndexPos]->action = nothing;
        reset();
    }
    else
        agents[minIndexPos]->action = rec;
    if (c.contains(ballpos)) {
        agents[minIndexPos]->action = gp;
    }
    auto *pass = new KickAction();
    pass->setTarget(pos);
    int power = 100 * pos.dist(ballpos);
    ROS_INFO_STREAM(power);
    pass->setKickspeed(power);
    pass->setSpin(5);
    pass->setSlow(true);
    auto *gpa = new GotopointavoidAction;
    gpa->setTargetpos(pos);
    gpa->setSlowmode(true);
    gpa->setBallobstacleradius(0.1);
    gpa->setLookat(ballpos);

    ROS_INFO("Executaion !");
    if (agents[minIndexPos]->pos().dist(ballpos) > 0.2 && wm->ball->vel.length() < 0.3){
        if (agents[minIndex]->pos().dist(ballpos) > 0.3)
            agents[minIndex]->action = gpa;
        agents[minIndex]->action = pass;
    }
    else
        agents[minIndex]->action = nothing;
    ROS_INFO_STREAM(minIndexPos);
}