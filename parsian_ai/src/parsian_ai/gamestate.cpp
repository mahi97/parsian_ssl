#include <parsian_ai/gamestate.h>

GameState *gameState;

GameState::GameState()
{
    state= States::PlayOff;
    ourScore=0;
    theirScore=0;
}

int GameState::getOurScore(){return ourScore;}
int GameState::getTheirScore(){ return theirScore;}

void GameState::setRefree(ssl_refree_wrapper ref_wrapper) {

    ///////////////////// when we are ready any command means force start
    if(isReady && state!=PlayOn){
        state=PlayOn;
        return;
    }

    switch (ref_wrapper.stage.stage) {
    case ssl_refree_stage::NORMAL_HALF_TIME:
    case ssl_refree_stage::EXTRA_HALF_TIME:
        state=HalfTime;
        return;

    case ssl_refree_stage::POST_GAME:
        state=PostGame;
        return;
    }

    switch (ref_wrapper.command.command) {
    case ssl_refree_command::FORCE_START:
        state=PlayOn;
        isReady=true;
        return;
    case ssl_refree_command::HALT:
        state=Halt;
        isReady=false;
        return;

    case ssl_refree_command::STOP:
        state=PlayOff;
        isReady=false;
        return;

    case ssl_refree_command::NORMAL_START:
        isReady=true;
        return;

        //####################################################
    case ssl_refree_command::BALL_PLACEMENT_THEM:
        isReady=true;
        state=TheirBallPlacement;
        return;
    case ssl_refree_command::BALL_PLACEMENT_US:
        isReady=true;
        state=OurBallPlacement;
        return;

    case ssl_refree_command::DIRECT_FREE_THEM:
        isReady=true;
        state=TheirDirectKick;
        return;
    case ssl_refree_command::DIRECT_FREE_US:
        isReady=true;
        state=OurDirectKick;
        return;

    case ssl_refree_command::GOAL_THEM:
        theirScore++;
        return;
    case ssl_refree_command::GOAL_US:
        ourScore++;
        return;

    case ssl_refree_command::INDIRECT_FREE_THEM:
        isReady=true;
        state=TheirIndirectKick;
        return;
    case ssl_refree_command::INDIRECT_FREE_US:
        isReady=true;
        state=OurIndirectKick;
        return;

    case ssl_refree_command::PREPARE_KICKOFF_THEM:
        isReady=false;
        state=TheirKickOff;
        return;
    case ssl_refree_command::PREPARE_KICKOFF_US:
        isReady=false;
        state=OurKickOff;
        return;

    case ssl_refree_command::PREPARE_PENALTY_THEM:
        isReady=false;
        state= ((ref_wrapper.stage.stage==ssl_refree_stage::PENALTY_SHOOTOUT)?
                    TheirPenaltyShootOut:TheirPenaltyKick);
        return;
    case ssl_refree_command::PREPARE_PENALTY_US:
        isReady=false;
        state= (ref_wrapper.stage.stage==ssl_refree_stage::PENALTY_SHOOTOUT)?
                    OurPenaltyShootOut:OurPenaltyKick;
        return;
    }
}

bool GameState::isPlayOn() { return (state == PlayOn); }
bool GameState::isPlayOff(){ return state==PlayOff;}
bool GameState::canMove() { return (state != Halt); }

bool GameState::allowedNearBall() {return isPlayOn() || ourRestart(); }
bool GameState::canKickBall() {return isPlayOn() || (ourRestart() && isReady); }

bool GameState::restart() { return (state>=OurBallPlacement ); }
bool GameState::ourRestart() { return restart() &&  state< TheirBallPlacement ;}
bool GameState::theirRestart() { return state>= TheirBallPlacement; }

bool GameState::kickoff() { return (ourKickoff() || theirKickoff()); }
bool GameState::ourKickoff() { return (state==OurKickOff); }
bool GameState::theirKickoff() { return (state==TheirKickOff); }

bool GameState::penaltyKick() { return ourPenaltyKick()|| theirPenaltyKick(); }
bool GameState::ourPenaltyKick() { return (state==OurPenaltyKick); }
bool GameState::theirPenaltyKick() { return (state==TheirPenaltyKick); }

bool GameState::directKick() { return (ourDirectKick() || theirDirectKick()); }
bool GameState::ourDirectKick() { return (state == OurDirectKick); }
bool GameState::theirDirectKick() { return (state== TheirDirectKick); }

bool GameState::indirectKick() { return ourIndirectKick() || theirIndirectKick(); }
bool GameState::ourIndirectKick() { return (state == OurIndirectKick) ;}
bool GameState::theirIndirectKick() { return (state == TheirIndirectKick); }

bool GameState::freeKick() { return directKick() || indirectKick(); }
bool GameState::ourFreeKick() { return ourDirectKick() || ourIndirectKick(); }
bool GameState::theirFreeKick() { return theirDirectKick() || theirIndirectKick(); }

bool GameState::ballPlacement() { return (ourBallPlacement() || theirBallPlacement()); }
bool GameState::ourBallPlacement() { return (state == OurBallPlacement); }
bool GameState::theirBallPlacement() { return (state == TheirBallPlacement); }

bool GameState::halfTimeLineUp(){return state == HalfTime;}

bool GameState::penaltyShootout() {return ourPenaltyShootout()|| theirPenaltyShootout();}
bool GameState::ourPenaltyShootout(){return state == OurPenaltyShootOut;}
bool GameState::theirPenaltyShootout(){return state == TheirPenaltyShootOut;}
