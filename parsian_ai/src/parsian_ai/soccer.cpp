#include <parsian_ai/soccer.h>

CSoccer* soccer;

CSoccer::CSoccer()
{
    agents = new Agent*[_MAX_NUM_PLAYERS];
    for(int i = 0; i < _MAX_NUM_PLAYERS; i++ )
    {
        agents[i] = new Agent(i);
    }
    coach = new CCoach(agents);
    kick = new KickAction;
    gtp = new GotopointavoidAction;

}

CSoccer::~CSoccer()
{
    for(int i = _MAX_NUM_PLAYERS-1; i >= 0; i-- ){
        delete agents[i];
    }
    delete agents;


}


void CSoccer::execute() {
    bool custom = false;
    customControl(custom);
    if (! custom) {
        coach->execute();
    }

}

void CSoccer::updateTask(){
    kick->setKickspeed(5);
    kick->setTarget(wm->field->oppGoal());
    kick->setDontkick(false);
    kick->setVeryfine(false);
    kick->setTolerance(0.0);
    kick->setSlow(false);
    kick->setShotemptyspot(false);
    kick->setPlaymakemode(false);
    kick->setPenaltykick(false);
    kick->setKickwithcenterofdribbler(false);
    kick->setKickangtol(0.0);
    kick->setInterceptmode(false);
    kick->setGoaliemode(false);
    kick->setChip(false);
    kick->setAvoidpenaltyarea(false);
    kick->setAvoidopppenaltyarea(false);
    kick->setPassprofiler(false);
    kick->setSpin(0);

    Circle2D aroundBall = Circle2D(wm->ball->pos, 0.5);
    Vector2D vec1, vec2;
    ROS_INFO_STREAM(wm->ball->pos.x << "  " << wm->ball->pos.y);
    aroundBall.intersection(Line2D(Vector2D(0,0), wm->ball->pos), &vec1, &vec2);
    gtp->setTargetpos(vec1.absX() < vec2.absX() ? vec1 : vec2);
    gtp->setMaxvelocity(3);
    gtp->setMaxacceleration(3);
    gtp->setMaxdeceleration(3);

    if(!gameState->canMove()) {    //HALT
        ROS_INFO("GOTO");
        agents[0]->action = gtp;
    }
    else {
        ROS_INFO("KICK");
        agents[0]->action = kick;
    }
}

void CSoccer::customControl(bool &custom) {
    custom = false;
    custom = true;
    updateTask();
    return;
}
