#ifndef MASTERPLAY_H
#define MASTERPLAY_H

#include <queue>
#include "parsian_ai/roles/roles.h"
//#include "skills.h" TODO : Actions
#include "parsian_ai/plans/plans.h"
#include <parsian_ai/gamestate.h>
#include <parsian_ai/soccer.h>
#include <QPair>
#include <queue>
#include <regex>

enum PlaysEnum{
    OurKickOffPlay,
    OurIndirectPlay,
    OurDirectPlay,
    OurPenaltyPlay,
    OurBallPlacement,//added

    TheirKickOffPlay,
    TheirIndirectPlay,
    TheirDirectPlay,
    TheirPenaltyPlay,
    TheirBallPlacement,//added
    StartPlay,
    StopPlay,
    HaltPlay,
    HalfTimeLineUp
};

class CMasterPlay {

public:

    bool playOnFlag;
    bool lockAgents;
    CMasterPlay();
    virtual ~CMasterPlay();
    int getDefenseNum();
    virtual void execute_0() = 0;
    virtual void execute_1() = 0;
    virtual void execute_2() = 0;
    virtual void execute_3() = 0;
    virtual void execute_4() = 0;
    virtual void execute_5() = 0;
    virtual void execute_6() = 0;
    void execute();
    virtual QString whoami() {return "MasterPlay";}
    bool canScore();
    QList <CAgent *> markAgents;
    //////////////////////////////////////
    bool noPlanException;
    //////////////////////////////////////

    virtual void reset() = 0;
private:
    void execPlay();
    QString formationName;
    bool staticInited;
    QList< holdingPoints > masterStaticPoints;

protected:

    QList <int> agentsID;

    int executedCycles;
    int defenseN ;
    bool f;

    QList <CAgent *> positionAgents;

    QList <CAgent *> stopAgents;
    CAgent *playMakeAgent;
    CAgent *blockAgent;

    static PositioningPlan positioningPlan;
    static CMarkPlan markPlan;
    static DefensePlan defensePlan;

    static CRolePlayMake playMakeRole;
    static CRoleBlock blockRole;

    void initMaster();
    void setAgentsID(QList <int> _agentsID);
    void setFormation(QString _formationName);
    void setStaticPoints(QList< holdingPoints > _staticPoints);
    void resetPositioning();
    void resetPlayMaker();
    void appendRemainingsAgents(QList <CAgent *> &_list);

    void choosePlayMaker();
    void chooseBlocker();
    bool canOneTouch(QList<CAgent*> positionAgents , CAgent *playMake);
    double coveredArea( std::priority_queue < QPair< edgeMode , double > , std::vector< QPair< edgeMode , double > > , Comparar >& obstacles );
    double getOpenness(Vector2D from, Vector2D p1, Vector2D p2, QList<int> ourRelaxedIDs, QList<int> oppRelaxedIDs);
};

#endif // MASTERPLAY_H
