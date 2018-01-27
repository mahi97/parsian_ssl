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


void COurBallPlacement::execute_x(){
    DBUG("execute_0 is running", D_ATOUSA);
    DBUG("ballPlacement execute_0", D_ERROR);
    ROS_INFO("Executaion X");
    //ROS_INFO_STREAM(23 << "asdf" << 23.3);
    /*
    auto* gpa = new  GotopointavoidAction();
    gpa->setBallobstacleradius(0.5);
    gpa->setTargetpos(Vector2D(4,0));
    agentsID[0]->action = gpa;
    Vector2D v = Vector2D(2,2);
    Polygon2D p;
    Vector2D a,b;
    Circle2D c;
    int m = c.intersection(Segment2D(Vector2D(0,0), Vector2D(1,1)), &a, &b);
*/
    //start

    Vector2D pos;
    Vector2D ballpos = Vector2D(wm->ball->pos.x - 0.5 , wm->ball->pos.y);
    double dist  = 0;
    double mindist = 10000;
    static int minIndex = 0;
    int lastminIndex = minIndex;
    for(int i = 0 ; i < agentsID.size() ; i++) {
        if(agentsID[i]->isVisible()) {
            dist = agentsID[i]->pos().dist(ballpos);
            if (dist < mindist) {
                mindist = dist;
                minIndex = i;
            }
        }
    }
    auto* gpa = new  GotopointavoidAction();
    gpa->setBallobstacleradius(0.5);
    gpa->setTargetpos(ballpos);
    auto* pass = new  KickAction;
    pass->setTarget(Vector2D(0 , 0));
    pass->setKickspeed(300);
    pass->setChip(1);
    pass->setSpin(3);
 //   if(agentsID[minIndex]->pos().equalsWeakly(ballpos)){
        agentsID[minIndex]->action = pass;
 //   }
//    else {
//        agentsID[minIndex]->action = gpa;
//    }
    auto* gp = new  KickAction;
    //gp->setSpin(3);
    auto* passr = new  ReceivepassAction;
    passr->setTarget(Vector2D(0 , 0));
    //passr->setSlow(true);
    passr->setReceiveradius(0.1);

    if(lastminIndex != minIndex) {
        //cagentsID[lastminIndex]->action = gp;
        agentsID[lastminIndex]->action = passr;//!!!!!!!!!!!!!!!!!!!
    }
    ROS_INFO_STREAM(lastminIndex);
    ROS_INFO_STREAM(minIndex);
    /*
    dist  = 0;
    mindist = 10000;
    int minIndexToBall = 0;
    for(int i = 0 ; i < agentsID.size() ; i++){
        dist = knowledge->getAgent(agentsID.at(i))->pos.dist(ballpos);
        if(dist < mindist && i != minIndexToBall){
            mindist = dist;
            minIndexToBall = i;
         }
    }
    gotopoint->pos;//ki
    gotopoint->setTargetLook(pos , ballpos);
    currentplan.passpos = pos;//ki//resid minIndex?
    */
    //end
}