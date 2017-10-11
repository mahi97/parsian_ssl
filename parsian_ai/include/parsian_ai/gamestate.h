// game_state.h

#ifndef __game_state_h__
#define __game_state_h__

#include <parsian_msgs/ssl_refree_wrapper.h>
#include <parsian_msgs/ssl_refree_command.h>
#include <parsian_msgs/ssl_refree_stage.h>



using namespace  parsian_msgs;

class GameState {
public:

    enum States{
        Halt ,
        PlayOff ,
        PlayOn,

        HalfTime,
        PostGame,

        OurBallPlacement, /// start of restart & our
        OurDirectKick,
        OurIndirectKick,
        OurPenaltyKick,
        OurPenaltyShootOut,
        OurKickOff,


        TheirBallPlacement,/// start of their
        TheirDirectKick,
        TheirIndirectKick,
        TheirPenaltyKick,
        TheirPenaltyShootOut,
        TheirKickOff,


    }state;

  int ourScore,theirScore;
  bool isReady;

  GameState();


  void setRefree(ssl_refree_wrapper ref_wrapper);
  bool isPlayOn();
  bool isPlayOff();
  bool restart();
  bool ourRestart();
  bool theirRestart();
  bool kickoff();
  bool ourKickoff();
  bool theirKickoff();
  bool penaltyKick();
  bool ourPenaltyKick();
  bool theirPenaltyKick();
  bool directKick();
  bool ourDirectKick();
  bool theirDirectKick();
  bool indirectKick();
  bool ourIndirectKick();
  bool theirIndirectKick();
  bool freeKick();
  bool ourFreeKick();
  bool theirFreeKick();
  bool canMove();
  bool allowedNearBall();
  bool canKickBall();
  int  getOurScore();
  int  getTheirScore();
  bool ballPlacement();
  bool ourBallPlacement();
  bool theirBallPlacement();
  bool halfTimeLineUp();
  bool penaltyShootout();
  bool ourPenaltyShootout();
  bool theirPenaltyShootout();
};
extern GameState *gameState;
#endif
