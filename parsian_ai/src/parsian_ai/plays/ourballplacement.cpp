#include <search.h>
#include "parsian_ai/plays/ourballplacement.h"

COurBallPlacement::COurBallPlacement(){

    minIndexPos = 0;
    ap = nullptr;
    minIndex = 0;
    a = nullptr;
    first = true;
    state = BallPlacement :: GO_FOR_BALL;
}

COurBallPlacement::~COurBallPlacement(){

}

void COurBallPlacement::reset(){
    flag = false;
    first = false;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

void COurBallPlacement::init(const QList<Agent*>& _agents){
    setAgentsID(_agents);
    initMaster();
    //if(knowledge->getLastPlayExecuted() != OurBallPlacement ){
    //    reset();

    //}
    // knowledge->setLastPlayExecuted(OurBallPlacement);
}


void COurBallPlacement::execute_x(){

    ROS_INFO("Executaion X");
    ROS_INFO_STREAM(flag);
    if(agents.size() <= 0)
        return;

    if (first) {
        a = ap = agents[0];
        first = false;
    }

    lockAgents = true;
    Vector2D ballpos = Vector2D(wm->ball->pos.x, wm->ball->pos.y);
    Vector2D pos = wm->ballplacementPoint();
    double dist = 0;
    double mindist = 10000;
    CAgent *pap = ap;

    for(int i = 0 ; i < agents.size()   ; i++){
        dist = agents[i]->pos().dist(pos);
        if (dist < mindist){
            mindist = dist;
            minIndexPos = i;
            ap = agents[i];
        }
    }
    mindist = 10000;
    CAgent *pa = a;
    for(int i = 0 ; i < agents.size() ; i++){
        dist = agents[i]->pos().dist(ballpos);
        if(dist < mindist && minIndexPos != i){
            mindist = dist;
            minIndex = i;
            a = agents[i];
        }
    }
    ROS_INFO("Executaion 1");
    auto *nothing = new NoAction;
    if (pap->id() != ap->id())
        pap->action = nothing;
    ROS_INFO("Executaion 1.2");
    if (pa->id() != a->id()) {
        pa->action = nothing;
        if(state == BallPlacement :: PASS){
            state = BallPlacement :: GO_FOR_BALL;
        }
    }
    ROS_INFO("Executaion 1.5");

    Vector2D behindBall = ballpos - Vector2D(pos - ballpos).norm() * 0.5;
    Circle2D cir{pos , 1 - 0.1};
    Vector2D sol1, sol2;
    drawer->draw(Segment2D(ballpos , ballpos + wm->ball->vel.norm() * 1.5) , QColor(Qt ::blue));

    if(state == BallPlacement :: GO_FOR_BALL && agents[minIndexPos]->pos().dist(pos) < 0.2 && agents[minIndex]->pos().dist(behindBall) < 0.2){
        state = BallPlacement :: PASS;
        passballpos = ballpos;
    }
    if(state == BallPlacement :: PASS &&
    (cir.contains(ballpos) || cir.intersection(Segment2D(ballpos, ballpos + wm->ball->vel.norm() * 2),&sol1,&sol2) > 0)){
        state = BallPlacement :: RECIVE_AND_POS;
    }
    if(state == BallPlacement :: RECIVE_AND_POS &&
    !(cir.contains(ballpos) || cir.intersection(Segment2D(ballpos, ballpos + wm->ball->vel.norm() * 2),&sol1,&sol2) > 0)){
        state = BallPlacement :: GO_FOR_BALL;
    }

    //GO_FOR_BALL
    auto *rec = new ReceivepassAction();
    rec->setReceiveradius(1);
    rec->setTarget(pos);
    rec->setSlow(true);
    auto *gpa = new GotopointavoidAction;
    gpa->setTargetpos(behindBall);
    gpa->setSlowmode(true);
    gpa->setBallobstacleradius(0.5);
    gpa->setLookat(pos);

    //PASS
    auto *pass = new KickAction();
    pass->setTarget(pos);
    int power = 50 * pos.dist(ballpos);
    ROS_INFO_STREAM(power);
    pass->setKickspeed(power);
    pass->setSpin(5);
    pass->setSlow(true);

    //GO_FOR_VALID_PASS
    auto *vrec = new ReceivepassAction();
    vrec->setReceiveradius(0.2);
    vrec->setTarget(pos);
    vrec->setSlow(true);

    //VALID_PASS

    //RECIVE_AND_POS
    auto *recSpin = new ReceivepassAction();
    recSpin->setReceiveradius(agents[minIndexPos]->pos().dist(passballpos));
    recSpin->setTarget(pos);
    recSpin->setSlow(true);
    //spin

    //FINAL_POS
    auto *gpas = new GotopointavoidAction();
    gpas->setTargetpos(pos);
    gpas->setLookat(ballpos);
    gpas->setSlowmode(true);
    gpas->setRoller(7);

    switch(state){
        case BallPlacement :: NoState:
            //:)
            break;
        case BallPlacement :: GO_FOR_BALL://noghtash doroste vali mikhore be top:-?
            ROS_INFO_STREAM("GO_FOR_BALL");
            agents[minIndexPos]->action = rec;
            agents[minIndex]->action = gpa;
            break;
        case BallPlacement :: PASS:
            ROS_INFO_STREAM("PASS");
            agents[minIndexPos]->action = rec;
            agents[minIndex]->action = pass;
            break;
        case BallPlacement :: GO_FOR_VALID_PASS:
            ROS_INFO_STREAM("GO_FOR_VALID_PASS");
            agents[minIndexPos]->action = vrec;
            agents[minIndex]->action = gpa;
            break;
        case BallPlacement :: VALID_PASS:
            ROS_INFO_STREAM("VALID_PASS");
            agents[minIndexPos]->action = vrec;
            agents[minIndex]->action = pass;
            break;
        case BallPlacement :: RECIVE_AND_POS:
            ROS_INFO_STREAM("RECIVE_AND_POS");
            agents[minIndexPos]->action = recSpin;
            agents[minIndex]->action = nothing;
            break;
        case BallPlacement :: FINAL_POS:
            ROS_INFO_STREAM("FINAL_POS");
            agents[minIndexPos]->action = gpas;
            agents[minIndex]->action = nothing;
            break;
        case BallPlacement :: DONE:
            ROS_INFO_STREAM("DONE");
            agents[minIndexPos]->action = nothing;
            agents[minIndex]->action = nothing;
            break;
        default:
            break;
    }
    //////////////////////////////////////////////////
    /*
    auto *gp = new GotopointavoidAction();
    gp->setTargetpos(pos);
    gp->setLookat(ballpos);
    gp->setSlowmode(true);
    gp->setRoller(7);
    ROS_INFO("Executaion 2");
    Circle2D c{agents[minIndexPos]->pos() + (agents[minIndexPos]->dir().norm() * 0.1), 0.1};
    if (c.contains(ballpos))
        agents[minIndexPos]->action = gp;
    else
        agents[minIndexPos]->action = rec;
    ROS_INFO("Executaion 3");
    auto *pass = new KickAction();
    pass->setTarget(pos);
    int power = 100 * pos.dist(ballpos);
    ROS_INFO_STREAM(power);
    pass->setKickspeed(power);
    pass->setSpin(5);
    pass->setSlow(true);

    ROS_INFO("Executaion !");
    Circle2D cir{agents[minIndexPos]->pos(), 0.5};
    Vector2D sol1, sol2;//dorost nist
    if (agents[minIndexPos]->pos().dist(ballpos) > 0.2 && cir.intersection(Ray2D(ballpos, wm->ball->dir.norm() * wm->ball->vel.length()),&sol1,&sol2) == 0
     //&& wm->ball->vel.length() < 0.3
     ){
        if(agents[minIndex]->pos().dist(ballpos - Segment2D(ballpos , pos).length() * wm->ball->dir.norm()) > 0.2)
            agents[minIndex]->action = gpa;
        else
            agents[minIndex]->action = pass;
    }
    else
        agents[minIndex]->action = nothing;
    ROS_INFO_STREAM(minIndexPos);
    */
}