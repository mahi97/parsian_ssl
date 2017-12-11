//
// Created by parsian-ai on 9/22/17.
//

#ifndef PARSIAN_AI_COACH_H
#define PARSIAN_AI_COACH_H

#include <QStringList>

#include <algorithm>

#include <QtCore/QMap>
#include <parsian_util/core/agent.h>
#include <QtCore/QTime>
#include <QtCore/QFile>
#include <QPair>
#include <QTextStream>
#include "parsian_util/base.h"
#include <parsian_ai/util/worldmodel.h>
#include <parsian_ai/plans/plans.h>
#include <parsian_ai/plays/plays.h>
#include <parsian_ai/roles/stop.h>

enum class BallPossesion {
    WEDONTHAVETHEBALL = 0,
    WEHAVETHEBALL = 1,
    SOSOOUR = 2,
    SOSOTHEIR = 3
};

class CCoach {

public:

    bool inited;
    double playOnTime;
    explicit CCoach(Agent** _agents);
    ~CCoach();
    void execute();
    void saveGoalie(); // TODO : Move To roles/Agent
    DefensePlan& getDefense();
    BallPossesion lastBallPossesionState;
    BallPossesion isBallOurs();
    BallPossesion ballPState;

private:
    bool lastASWasCritical;
    Vector2D passPos;
    bool passPlayMake;
    Vector2D lastBallVelPM;
    Vector2D lastBallPos;
    double findMostPossible(Vector2D agentPos);
    States lastState;
    Agent *goalieAgent;
    Agent *exeptionPlayMake;
    double exeptionPlayMakeThr;
    QList<Agent*> defenseAgents;
    int preferedDefenseCounts ,lastPreferredDefenseCounts;
    int preferedGoalieAgent;
    Vector2D defenseTargets[12];
    QTime intentionTimePossession;
    QTime playMakeIntention;
    QTime playOnExecTime;
    double playMakeIntentionInterval;
    double possessionIntentionInterval;

    CMasterPlay          *selectedPlay;

    CPlayOff             *ourPlayOff;
    COurPenalty          *ourPenalty;
//    COurBallPlacement    *ourBallPlacement;

    CTheirDirect         *theirDirect;
    CTheirPenalty        *theirPenalty;
    CTheirKickOff        *theirKickOff;
    CTheirIndirect       *theirIndirect;
    CTheirBallPlacement  *theirBallPlacement;

    CDynamicAttack       *dynamicAttack;

    CStopPlay            *stopPlay;


    CRoleStop *stopRoles[_MAX_NUM_PLAYERS];
    QTime goalieTimer;
    bool goalieTrappedUnderGoalNet;

    Agent** agents;
    int lastAssignCycle;

    void checkRoleAssignments();
    ///////manage over number of agents
    ///
    ///
    QList <int> robotsIdHist;
    bool first;
    QList <int> missMatchIds;
    ///////////////////////////////////////
    int cyclesWaitAfterballMoved;
    QList <Agent*> lastDefenseAgents;

    void assignGoalieAgent(int goalieID);
    void assignDefenseAgents(int defenseCount);
    void checkGoalieInsight();
    void decidePreferedDefenseAgentsCountAndGoalieAgent();
    void decideAttack();
    void decideDefense();
    void decidePlayOff(QList<int>& _ourPlayers, POMODE _mode = INDIRECT);
    void decidePlayOn(QList<int>& ourPlayers, QList<int>& lastPlayers);
    QTime defenseTimeForVisionProblem[2];
    double shotToGoalthr ;
    void virtualTheirPlayOffState();
    bool transientFlag;
    QTime trasientTimeOut;
    int translationTimeOutTime;
    bool isBallcollide();
    void calcDesiredMarkCounts(); // not used at all
    ///////////////////////new play make and supporter chooser
    int playmakeId;
    double playMakeTh;
    int lastPlayMake;
    void choosePlaymakeAndSupporter(bool defenseFirst);

    ///////////////////////////////////////////////

    enum attackState
    {
        SAFE     = 0,
        FAST     = 1,
        CRITICAL = 2
    };
    attackState ourAttackState;
    void updateAttackState();


    ///////////////////////New Play Off
    void selectPlayOffMode(int agentSize, NGameOff::EMode& _mode);
    void initPlayOffMode(NGameOff::EMode _mode,
                         POMODE _gameMode,
                         const QList<int>& _agentSize);
    void setPlayOff(NGameOff::EMode _mode);
    void initStaticPlay(POMODE _mode, const QList<int>& _agentSize);
    void initDynamicPlay(const QList<int> &_ourplayers);
    void initFastPlay(const QList<int> &_ourplayers);
    void initFirstPlay(const QList<int> &_ourplayers);
    void setStaticPlay();
    void setDynamicPlay();
    void setFirstPlay();
    void setFastPlay();
    bool firstTime, firstPlay, firstIsFinished;


    int preferedShotSpot;

    QList<int> lastPlayers;
    Vector2D lastBallVel;
    //////////////Decide Attack functions
    void decideHalt               (QList<int>&);
    void decideStop               (QList<int>&);
    void decideOurKickOff         (QList<int>&);
    void decideTheirKickOff       (QList<int>&);
    void decideOurIndirect        (QList<int>&);
    void decideTheirIndirect      (QList<int>&);
    void decideOurDirect          (QList<int>&);
    void decideTheirDirect        (QList<int>&);
    void decideOurPenalty         (QList<int>&);
    void decideTheirPenalty       (QList<int>&);
    void decideStart              (QList<int>&);
    void decideOurBallPlacement   (QList<int>&);
    void decideTheirBallPlacement (QList<int>&);
    void decideHalfTimeLineUp   (QList<int>&);
    void decideNull               (QList<int>&);
    /////////////////////////////////////
    QTextStream out;
    bool isFastPlay();
    ///HMD
    bool checkOverdef();
    double overDefThr;

    // inter change
    void checkSensorShootFault();
    int  faultDetectionCounter[12];


    // MAHI ADD IN ROS
    QList <CRobot*> toBeMopps;
    int desiredDefCount;
    QString stateForMark;
};

#endif //PARSIAN_AI_COACH_H
