#include <parsian_ai/soccer.h>

CSoccer* soccer;

CSoccer::CSoccer()
{
    wm = new CWorldModel;
    agents = new CAgent*[_MAX_NUM_PLAYERS];
    for(int i = 0; i < wm->our.activeAgentsCount(); i++ )
    {
        agents[i] = new CAgent(wm->our.activeAgentID(i));// todo noOne says: is myChange is correct;
        agents[i]->self = *wm->our.active(i);
    }

    coach = new CCoach(agents);
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
    auto* kick = new KickAction;
    kick->setKickspeed(1023);
    kick->setTarget(wm->field->oppGoal());
    kick->setSlow(true);
    kick->setShotemptyspot(true);

    auto* gtp = new GotopointavoidAction;
    Circle2D aroundBall = Circle2D(wm->ball->pos, 0.5);
    Vector2D vec1, vec2;
    aroundBall.intersection(Line2D(Vector2D(0,0), wm->ball->pos), &vec1, &vec2);
    gtp->setTargetpos(vec1.absX() < vec2.absX() ? vec1 : vec2);


    if(!gameState->canMove()) {    //HALT
        agents[0]->action = gtp;
    }
    else {
        agents[0]->action = kick;
    }
}

void CSoccer::customControl(bool &custom) {
    custom = true;
    updateTask();
    custom = false;
    return;
}
